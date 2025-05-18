#include <gst/gst.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <termios.h>
#include <unistd.h>

// Get character input non-blocking
char get_char_non_blocking() {
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

// Keyboard input handler
void keyboard_handler(GstElement* playbin, std::atomic<bool>& running,
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

    size_t current_index = 0;
    g_object_set(playbin, "uri", playlist[current_index].c_str(), NULL);

    if (gst_element_set_state(playbin, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "Failed to start playback." << std::endl;
        gst_object_unref(playbin);
        return -1;
    }

    std::atomic<bool> running(true);
    std::thread kb_thread(keyboard_handler, playbin, std::ref(running), std::ref(playlist), std::ref(current_index));

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

