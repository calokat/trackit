#include <libuvc/libuvc.h>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/stitching.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <vector>
 
static const std::string WINDOW_NAME = "Hello Trackit!";

static cv::Ptr<cv::FeatureDetector> orbFinder;

/* This callback function runs once per frame. Use it to perform any
 * quick processing you need, or have it put the frame into your application's
 * input queue. If this function takes too long, you'll start losing frames. */
void cb(uvc_frame_t *frame, void *ptr) {
  uvc_frame_t *bgr;
  uvc_error_t ret;
  enum uvc_frame_format *frame_format = (enum uvc_frame_format *)ptr;
 
  /* We'll convert the image from YUV/JPEG to BGR, so allocate space */
  bgr = uvc_allocate_frame(frame->width * frame->height * 3);
  if (!bgr) {
    printf("unable to allocate bgr frame!\n");
    return;
  }
  
  uint8_t* typedBuf = (uint8_t*)frame->data;
  std::vector<uint8_t> imgVec(typedBuf, typedBuf + frame->data_bytes);
  cv::Mat cvImg = cv::imdecode(imgVec, cv::ImreadModes::IMREAD_UNCHANGED);
  
  cv::detail::ImageFeatures features;
  std::vector<cv::KeyPoint> keypoints;
  cv::Mat _;
  orbFinder->detectAndCompute(cvImg, _, keypoints, _);
  std::cout << "Keypoints: " << keypoints.size() << std::endl;
  std::cout << "Image cols: " << cvImg.cols << std::endl;
  cv::drawKeypoints(cvImg, keypoints, cvImg, cv::Scalar(0, 255, 0, 255));
  
  imshow(WINDOW_NAME, cvImg);
  cv::waitKey(1);
  /* Call a user function:
   *
   * my_type *my_obj = (*my_type) ptr;
   * my_user_function(ptr, bgr);
   * my_other_function(ptr, bgr->data, bgr->width, bgr->height);
   */
 
  /* Call a C++ method:
   *
   * my_type *my_obj = (*my_type) ptr;
   * my_obj->my_func(bgr);
   */
 
  /* Use opencv.highgui to display the image:
   * 
   * cvImg = cvCreateImageHeader(
   *     cvSize(bgr->width, bgr->height),
   *     IPL_DEPTH_8U,
   *     3);
   *
   * cvSetData(cvImg, bgr->data, bgr->width * 3); 
   *
   * cvNamedWindow("Test", CV_WINDOW_AUTOSIZE);
   * cvShowImage("Test", cvImg);
   * cvWaitKey(10);
   *
   * cvReleaseImageHeader(&cvImg);
   */
 
  uvc_free_frame(bgr);
}
 
int main(int argc, char **argv) {
  uvc_context_t *ctx;
  uvc_device_t *dev;
  uvc_device_handle_t *devh;
  uvc_stream_ctrl_t ctrl;
  uvc_error_t res;
  
  orbFinder = cv::ORB::create();
  
  cv::namedWindow(WINDOW_NAME);
 
  /* Initialize a UVC service context. Libuvc will set up its own libusb
   * context. Replace NULL with a libusb_context pointer to run libuvc
   * from an existing libusb context. */
  res = uvc_init(&ctx, NULL);
 
  if (res < 0) {
    uvc_perror(res, "uvc_init");
    return res;
  }
 
  puts("UVC initialized");
 
  /* Locates the first attached UVC device, stores in dev */
  res = uvc_find_device(
      ctx, &dev,
      0, 0, NULL); /* filter devices: vendor_id, product_id, "serial_num" */
 
  if (res < 0) {
    uvc_perror(res, "uvc_find_device"); /* no devices found */
  } else {
    puts("Device found");
 
    /* Try to open the device: requires exclusive access */
    res = uvc_open(dev, &devh);
 
    if (res < 0) {
      uvc_perror(res, "uvc_open"); /* unable to open device */
    } else {
      puts("Device opened");
 
      /* Print out a message containing all the information that libuvc
       * knows about the device */
      uvc_print_diag(devh, stderr);
 
      const uvc_format_desc_t *format_desc = uvc_get_format_descs(devh);
      const uvc_frame_desc_t *frame_desc = format_desc->frame_descs;
      enum uvc_frame_format frame_format;
      int width = 640;
      int height = 480;
      int fps = 30;
 
      switch (format_desc->bDescriptorSubtype) {
      case UVC_VS_FORMAT_MJPEG:
        frame_format = UVC_COLOR_FORMAT_MJPEG;
        break;
      default:
        frame_format = UVC_FRAME_FORMAT_YUYV;
        break;
      }
 
      if (frame_desc) {
        width = frame_desc->wWidth;
        height = frame_desc->wHeight;
        fps = 10000000 / frame_desc->dwDefaultFrameInterval;
      }
 
      printf("\nFirst format: (%4s) %dx%d %dfps\n", format_desc->fourccFormat, width, height, fps);
 
      /* Try to negotiate first stream profile */
      res = uvc_get_stream_ctrl_format_size(
          devh, &ctrl, /* result stored in ctrl */
          frame_format,
          width, height, fps /* width, height, fps */
      );
 
      /* Print out the result */
      uvc_print_stream_ctrl(&ctrl, stderr);
 
      if (res < 0) {
        uvc_perror(res, "get_mode"); /* device doesn't provide a matching stream */
      } else {
        /* Start the video stream. The library will call user function cb:
         *   cb(frame, (void *) 12345)
         */
        res = uvc_start_streaming(devh, &ctrl, cb, (void *) 12345, 0);
 
        if (res < 0) {
          uvc_perror(res, "start_streaming"); /* unable to start stream */
        } else {
          puts("Streaming...");
 
          /* enable auto exposure - see uvc_set_ae_mode documentation */
          puts("Enabling auto exposure ...");
          const uint8_t UVC_AUTO_EXPOSURE_MODE_AUTO = 2;
          res = uvc_set_ae_mode(devh, UVC_AUTO_EXPOSURE_MODE_AUTO);
          if (res == UVC_SUCCESS) {
            puts(" ... enabled auto exposure");
          } else if (res == UVC_ERROR_PIPE) {
            /* this error indicates that the camera does not support the full AE mode;
             * try again, using aperture priority mode (fixed aperture, variable exposure time) */
            puts(" ... full AE not supported, trying aperture priority mode");
            const uint8_t UVC_AUTO_EXPOSURE_MODE_APERTURE_PRIORITY = 8;
            res = uvc_set_ae_mode(devh, UVC_AUTO_EXPOSURE_MODE_APERTURE_PRIORITY);
            if (res < 0) {
              uvc_perror(res, " ... uvc_set_ae_mode failed to enable aperture priority mode");
            } else {
              puts(" ... enabled aperture priority auto exposure mode");
            }
          } else {
            uvc_perror(res, " ... uvc_set_ae_mode failed to enable auto exposure mode");
          }
 
          sleep(10); /* stream for 10 seconds */
 
          /* End the stream. Blocks until last callback is serviced */
          uvc_stop_streaming(devh);
          puts("Done streaming.");
        }
      }
 
      /* Release our handle on the device */
      uvc_close(devh);
      puts("Device closed");
    }
 
    /* Release the device descriptor */
    uvc_unref_device(dev);
  }
 
  /* Close the UVC context. This closes and cleans up any existing device handles,
   * and it closes the libusb context if one was not provided. */
  uvc_exit(ctx);
  puts("UVC exited");
 
  return 0;
}
