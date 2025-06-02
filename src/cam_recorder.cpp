#include <gst/gst.h>
#include <iostream>
#include <thread>
#include <chrono>

GstElement* camera_pipeline = nullptr;
bool is_recording = false;

void init_camera_pipeline() {
    camera_pipeline = gst_parse_launch(
        "videotestsrc is-live=true pattern=ball ! "
        "video/x-raw,width=640,height=480,framerate=30/1 ! "
        "x264enc tune=zerolatency bitrate=500 ! "
        "mp4mux ! filesink location=output.mp4",
        NULL
    );
}

void start_recording() {
    if (!camera_pipeline) return;

    gst_element_set_state(camera_pipeline, GST_STATE_PLAYING);
    is_recording = true;
}

void stop_recording() {
    if (!camera_pipeline) return;

    // Send EOS to finalize the MP4 file properly
    gst_element_send_event(camera_pipeline, gst_event_new_eos());

    // Wait for EOS or ERROR to ensure the file is properly closed
    GstBus *bus = gst_element_get_bus(camera_pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, 5 * GST_SECOND,
                        (GstMessageType)(GST_MESSAGE_EOS | GST_MESSAGE_ERROR));
    if (msg) {
        gst_message_unref(msg);
    }
    gst_object_unref(bus);

    // Stop and cleanup
    gst_element_set_state(camera_pipeline, GST_STATE_NULL);
    gst_object_unref(camera_pipeline);
    camera_pipeline = nullptr;
    is_recording = false;
}


void destroy_camera_pipeline() {
    if (camera_pipeline) {
        gst_object_unref(camera_pipeline);
        camera_pipeline = nullptr;
    }
}

