#include <gst/gst.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <termios.h>
#include <unistd.h>
#include "KeyBoard_Handler.h"

// Load playlist from file
std::vector<std::string> load_playlist(const std::string& path) {
    std::vector<std::string> playlist;
    std::ifstream file(path);
    std::string line;

    while (std::getline(file, line)) {
        if (!line.empty()) {
            if (!g_str_has_prefix(line.c_str(), "file://") &&
                !g_str_has_prefix(line.c_str(), "http://") &&
                !g_str_has_prefix(line.c_str(), "https://") &&
                !g_str_has_prefix(line.c_str(), "rtsp://")) {
                char* abs_path = realpath(line.c_str(), nullptr);
                if (abs_path) {
                    playlist.push_back("file://" + std::string(abs_path));
                    free(abs_path);
                }
            } else {
                playlist.push_back(line);
            }
        }
    }
    return playlist;
}

int main(int argc, char* argv[]) {
    gst_init(&argc, &argv);

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <playlist.txt or media path>" << std::endl;
        return -1;
    }

    std::vector<std::string> playlist;
    std::string first_arg = argv[1];

    if (first_arg.size() >= 4 && first_arg.substr(first_arg.size() - 4) == ".txt") {
        playlist = load_playlist(first_arg);
    } else {
        char* abs = realpath(first_arg.c_str(), nullptr);
        if (!abs) {
            std::cerr << "Error: Invalid media path." << std::endl;
            return -1;
        }
        playlist.push_back("file://" + std::string(abs));
        free(abs);
    }

    if (playlist.empty()) {
        std::cerr << "Playlist is empty or failed to load." << std::endl;
        return -1;
    }

    GstElement* playbin = gst_element_factory_make("playbin", "player");
    if (!playbin) {
        std::cerr << "Failed to create playbin element." << std::endl;
        return -1;
    }

    // // Create and attach visualizer- Phase 3
    // GstElement *visualizer = gst_element_factory_make("goom", "visualizer");
    // if (!visualizer) {
    //     std::cerr << "Failed to create visualizer element." << std::endl;
    //     return -1;
    // }
    // g_object_set(playbin, "video-sink", visualizer, NULL);

    // /*Phase 3 ends*/

    size_t current_index = 0;
    g_object_set(playbin, "uri", playlist[current_index].c_str(), NULL);

    if (gst_element_set_state(playbin, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "Failed to start playback." << std::endl;
        gst_object_unref(playbin);
        return -1;
    }

    std::atomic<bool> running(true);
    std::thread kb_thread(KeyBoard_Handler::keyboard_handler, playbin, std::ref(running), std::ref(playlist), std::ref(current_index));

    GstBus* bus = gst_element_get_bus(playbin);

    while (running) {
        GstMessage* msg = gst_bus_timed_pop_filtered(bus, 100 * GST_MSECOND,
            (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

        if (msg != nullptr) {
            switch (GST_MESSAGE_TYPE(msg)) {
                case GST_MESSAGE_ERROR: {
                    GError* err;
                    gchar* debug;
                    gst_message_parse_error(msg, &err, &debug);
                    std::cerr << "ERROR: " << err->message << "\n";
                    g_clear_error(&err);
                    g_free(debug);
                    running = false;
                    break;
                }
                case GST_MESSAGE_EOS:
                    std::cout << "End of stream." << std::endl;
                    if (++current_index < playlist.size()) {
                        g_object_set(playbin, "uri", playlist[current_index].c_str(), NULL);
                        gst_element_set_state(playbin, GST_STATE_PLAYING);
                    } else {
                        running = false;
                    }
                    break;
                default: break;
            }
            gst_message_unref(msg);
        }
    }

    kb_thread.join();
    gst_element_set_state(playbin, GST_STATE_NULL);
    gst_object_unref(bus);
    gst_object_unref(playbin);

    return 0;
}