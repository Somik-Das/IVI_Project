#include <gst/gst.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <termios.h>
#include <unistd.h>

// Keyboard input handler
class KeyBoard_Handler{
public:
    static void keyboard_handler(GstElement* playbin, std::atomic<bool>& running,
                      std::vector<std::string>& playlist, size_t& current_index);
    static char get_char_non_blocking();                    
};