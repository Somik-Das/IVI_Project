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


// Keyboard input handler
std::atomic<bool> running(true);

// Get character input non-blocking
char KeyBoard_Handler::get_char_non_blocking() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// char KeyBoard_Handler::get_char_non_blocking() {   // This will block until the user types something
//     char ch;
//     std::cin >> ch;
//     return ch;
// }

void KeyBoard_Handler::keyboard_handler(GstElement* playbin, std::atomic<bool>& running,
                      std::vector<std::string>& playlist, size_t& current_index) {
    while (running) {
        char c = get_char_non_blocking();
        switch (c) {
            case 'p': {
                GstState state;
                gst_element_get_state(playbin, &state, nullptr, GST_CLOCK_TIME_NONE);
                gst_element_set_state(playbin, state == GST_STATE_PLAYING ? GST_STATE_PAUSED : GST_STATE_PLAYING);
                std::cout << "Play/Pause toggled.\n";
                break;
            }
            case '+': {
                gdouble vol;
                g_object_get(playbin, "volume", &vol, nullptr);
                vol = std::min(vol + 0.1, 1.0);
                g_object_set(playbin, "volume", vol, nullptr);
                std::cout << "Volume increased: " << vol << "\n";
                break;
            }
            case '-': {
                gdouble vol;
                g_object_get(playbin, "volume", &vol, nullptr);
                vol = std::max(vol - 0.1, 0.0);
                g_object_set(playbin, "volume", vol, nullptr);
                std::cout << "Volume decreased: " << vol << "\n";
                break;
            }
            case 'f': {
                gint64 pos;
                if (gst_element_query_position(playbin, GST_FORMAT_TIME, &pos)) {
                    gst_element_seek_simple(playbin, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, pos + 10 * GST_SECOND);
                    std::cout << "Seeked forward.\n";
                }
                break;
            }
            case 'r': {
                gint64 pos;
                if (gst_element_query_position(playbin, GST_FORMAT_TIME, &pos)) {
                    gint64 new_pos = std::max(pos - 10 * GST_SECOND, (gint64)0);
                    gst_element_seek_simple(playbin, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, new_pos);
                    std::cout << "Seeked backward.\n";
                }
                break;
            }
            case 'n': {
                if (current_index + 1 < playlist.size()) {
                    current_index++;
                    g_object_set(playbin, "uri", playlist[current_index].c_str(), NULL);
                    gst_element_set_state(playbin, GST_STATE_PLAYING);
                    std::cout << "Next track.\n";
                }
                break;
            }
            case 'b': {
                if (current_index > 0) {
                    current_index--;
                    g_object_set(playbin, "uri", playlist[current_index].c_str(), NULL);
                    gst_element_set_state(playbin, GST_STATE_PLAYING);
                    std::cout << "Previous track.\n";
                }
                break;
            }
            case 'q': {
                running = false;
                std::cout << "Quitting...\n";
                break;
            }
            default: break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}