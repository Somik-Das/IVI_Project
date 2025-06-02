#ifndef CAM_RECORDER_HPP
#define CAM_RECORDER_HPP

#include <gst/gst.h>

extern GstElement* camera_pipeline;
extern bool is_recording;

void init_camera_pipeline();
void start_recording();
void stop_recording();
void destroy_camera_pipeline();

#endif // CAM_RECORDER_HPP
