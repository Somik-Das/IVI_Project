#include "media_muxer.hpp"
#include <gst/gst.h>
#include <iostream>

MediaMuxer::MediaMuxer(const std::string& video, const std::string& audio, const std::string& output)
    : videoFile(video), audioFile(audio), outputFile(output) {
}

MediaMuxer::~MediaMuxer() {
    // Optional cleanup if needed
}

bool MediaMuxer::run() {
    std::cout << "[DEBUG] Video path: " << videoFile << "\n";
    std::cout << "[DEBUG] Audio path: " << audioFile << "\n";
    std::cout << "[DEBUG] Output path: " << outputFile << "\n";

    // Check if files exist
    if (!g_file_test(videoFile.c_str(), G_FILE_TEST_EXISTS) ||
        !g_file_test(audioFile.c_str(), G_FILE_TEST_EXISTS)) {
        std::cerr << "Error: Resource not found.\n";
        return false;
    }

    std::cout << "[INFO] Merging video and audio into " << outputFile << "...\n";

    // Prepare the GStreamer pipeline
    std::string pipeline_description =
        "filesrc location=" + videoFile + " ! decodebin ! videoconvert ! x264enc bitrate=512 ! queue ! mux. "
        "filesrc location=" + audioFile + " ! decodebin ! audioconvert ! voaacenc bitrate=128000 ! queue ! mux. "
        "mp4mux name=mux ! filesink location=" + outputFile;

    GError* error = nullptr;
    GstElement* pipeline = gst_parse_launch(pipeline_description.c_str(), &error);

    if (!pipeline) {
        std::cerr << "Failed to create pipeline: " << error->message << std::endl;
        g_error_free(error);
        return false;
    }

    // Set the pipeline to playing
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "Failed to set pipeline to PLAYING state." << std::endl;
        gst_object_unref(pipeline);
        return false;
    }

    // Wait for the pipeline to complete
    GstBus* bus = gst_element_get_bus(pipeline);
    bool terminate = false;
    while (!terminate) {
        GstMessage* msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
            static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

        if (msg != nullptr) {
            switch (GST_MESSAGE_TYPE(msg)) {
                case GST_MESSAGE_ERROR: {
                    GError* err;
                    gchar* debug_info;
                    gst_message_parse_error(msg, &err, &debug_info);
                    std::cerr << "Error received from element: " << err->message << std::endl;
                    g_clear_error(&err);
                    g_free(debug_info);
                    terminate = true;
                    break;
                }
                case GST_MESSAGE_EOS:
                    std::cout << "Muxing completed successfully!\n";
                    terminate = true;
                    break;

                default:
                    break;
            }
            gst_message_unref(msg);
        }
    }

    // Clean up
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(bus);
    gst_object_unref(pipeline);

    return true;
}


