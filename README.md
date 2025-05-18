# IVI_Project
📁 1. Project Overview 
Build an extensible, modular media player that supports:
  Local and RTSP streaming playback
  Playlist navigation
  Audio visualization
  Muxing/demuxing streams
  Stream compositing
  UI (GTKmm optional)
  Format fallback via FFmpeg

🛠️ 2. Initial Environment Setup
✅ 2.1. Update and Upgrade Packages
sudo apt update && sudo apt upgrade -y

✅ 2.2. Install Required Development Tools
sudo apt install build-essential cmake g++ pkg-config git -y

✅ 2.3. Install GStreamer and Plugins
sudo apt install \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-tools \
    gstreamer1.0-libav \
    gstreamer1.0-alsa \
    gstreamer1.0-plugins-base-apps \
    -y
✅ 2.4. Install GTKmm (Optional GUI)
sudo apt install libgtkmm-3.0-dev -y

✅ 2.5. Install FFmpeg
sudo apt install ffmpeg libavcodec-dev libavformat-dev libavutil-dev -y

📁 3. Project Directory Structure
IVI_Project/
├── src/
│   ├── main.cpp
│   ├── player.cpp / .h
│   ├── playlist.cpp / .h
│   ├── ffmpeg_helper.cpp / .h
│   ├── pipeline_composer.cpp / .h
│   └── utils.cpp / .h
├── assets/            # media files
├── include/
├── Makefile

🔧 4. Core Modules to Implement
✅ 4.1. main.cpp
Initializes the GTKmm or CLI\
Parses input args (e.g., playlist file or stream URL)
Starts GStreamer pipeline

✅ 4.2. player.cpp/h
Create and manage GStreamer elements (playbin, rtspsrc, etc.)
Manage states: PLAYING, PAUSED, STOPPED
Integrate audio visualizer

✅ 4.3. ffmpeg_helper.cpp/h
Provide a fallback decoder/converter using FFmpeg for unsupported formats

✅ 4.4. playlist.cpp/h
Load playlist file and manage navigation between tracks

✅ 4.5. pipeline_composer.cpp/h
Build custom pipelines for:
RTSP input + overlay
Compositing multiple videos
Padding and alignment
Mux/demux streams dynamically

⚙️ 5. GStreamer Functionalities
✅ Audio/Video Playback
Use playbin or manual decodebin pipelines.

✅ Playlist Navigation
Implement simple .txt playlist loader + Next/Prev logic.

✅ Audio Visualizer
g_object_set(playbin, "vis-plugin", "goom", NULL);

✅ FFmpeg Fallback
Use ffmpeg or libav* to transcode unsupported formats and feed to GStreamer.

✅ RTSP Streaming
GstElement* rtspsrc = gst_element_factory_make("rtspsrc", "source");
g_object_set(rtspsrc, "location", "rtsp://...", NULL);

✅ Muxing/Demuxing
GstElement* muxer = gst_element_factory_make("mp4mux", NULL);
GstElement* demuxer = gst_element_factory_make("qtdemux", NULL);

✅ Mixing / Blending
GstElement* mixer = gst_element_factory_make("compositor", NULL); // for video
GstElement* amixer = gst_element_factory_make("audiomixer", NULL); // for audio

✅ Padding
GstElement* videobox = gst_element_factory_make("videobox", NULL);
g_object_set(videobox, "top", 10, "left", 20, NULL);

✅ Sink Options
autovideosink, xvimagesink, filesink, appsink, rtmpsink

🧪 6. Build System (Makefile Example)
CXX = g++
CXXFLAGS = -Wall -std=c++17 `pkg-config --cflags gstreamer-1.0 gtkmm-3.0`

LIBS = `pkg-config --libs gstreamer-1.0 gtkmm-3.0 gstreamer-video-1.0 gstreamer-audio-1.0`
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)
TARGET = ivi_player

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)

🧪 7. Run the Application
# Run with local file
./ivi_player assets/test.mp4

# Run with RTSP stream
./ivi_player rtsp://example.com/live.stream

🧱 8. Phased Development Plan
Phase	Feature	Notes
1	Basic player with playbin	CLI or simple UI
2	Playlist, controls, volume, seek	Text + keyboard support
3	Audio visualizer + FFmpeg fallback	goom + ffmpeg
4	RTSP + mux/demux	Network + recording ready
5	Stream composition, padding, sink mgmt	Advanced layout & routing
6	GTKmm GUI (optional)	Touch-compatible frontend
