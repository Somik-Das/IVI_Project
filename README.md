### 🖼️ **Overview**

A feature-rich, modular **In-Vehicle Infotainment (IVI) Media Player** application designed to handle a wide range of **local and streaming media**, with support for **advanced GStreamer processing**, **multimedia controls**, **custom pipelines**, and an **optional touch-friendly UI**.

---

### 💡 **User Perspective**

* You can play **local files** (`.mp4`, `.mp3`, `.avi`, etc.) or **remote streams** (`RTSP`, `HTTP`).
* Use a **GUI** (GTKmm-based) or a **Terminal UI** depending on the system’s interface.
* Control playback using **buttons or key presses**: Play, Pause, Next, Previous, Seek, Volume.
* Visual feedback via **audio visualizer (goom, wavescope)**.
* **Dynamic stream control** like switching between camera feeds or live streams.
* Real-time **composition** and **blending** of multiple video/audio inputs.

---

### 🧰 **Core Functionalities (Fully Integrated)**

| Feature                         | Description                                                        |
| ------------------------------- | ------------------------------------------------------------------ |
| ✅ **Audio/Video Playback**      | Using `playbin`, `decodebin`, and custom pipelines                 |
| ✅ **Playlist Support**          | `.txt` based playlist navigation with keyboard or GUI              |
| ✅ **Playback Controls**         | Play, Pause, Next, Previous, Seek, Volume                          |
| ✅ **FFmpeg Fallback**           | For unsupported formats, ensuring reliability                      |
| ✅ **RTSP Streaming**            | Live feed or network stream handling with low latency              |
| ✅ **Muxing / Demuxing**         | Combine or extract streams (e.g., record audio+video)              |
| ✅ **Mixing / Blending**         | Use `compositor`, `audiomixer` to blend sources                    |
| ✅ **Stream Compositing**        | Overlay or grid-view multiple video feeds (e.g., 360° dashcam)     |
| ✅ **Padding & Layout**          | Use `videobox` to add margins, align streams                       |
| ✅ **Multiple Sink Handling**    | Play to screen, record to file, or broadcast simultaneously        |
| ✅ **Audio Visualizer**          | Real-time effects using `goom`, `wavescope`                        |
| ✅ **Modular Pipeline Building** | Easy plug-and-play of components                                   |
| ✅ **Optional GUI (GTKmm)**      | Touchscreen-friendly frontend with volume bar, buttons, visualizer |
| ✅ **Headless Terminal UI**      | Lightweight version for CLI playback & debug                       |
| ✅ **Cross-input Handling**      | Stream switching or input rerouting (live → local fallback)        |

---

### 🖥️ **Application Layout (if GUI enabled)**

```
 ---------------------------------------------------------
| [Play] [Pause] [<< Prev] [Next >>]  Volume: [|||---]     |
|---------------------------------------------------------|
|                📺 Video Playback Window                 |
|---------------------------------------------------------|
| 🔊 Visualizer: Wavescope / Goom                        |
| Now Playing: song3.mp3                                 |
 ---------------------------------------------------------
```

Or, in **headless terminal UI**, you might see:

```
> Playing: assets/song3.mp3
[▶️] Press [P] to Pause, [N] for Next, [←/→] to Seek
Visualizer: Goom running
```

---

### 🔧 **Architecture Summary**

* **Backend (C++)**: Controls all media logic via GStreamer and FFmpeg
* **Pipeline Orchestration**: Dynamic graph construction for different features
* **GUI Frontend (GTKmm)**: Optional interactive media control
* **Modular Structure**: Components like `player`, `playlist`, `stream_handler`, `pipeline_builder`


### 🚘 **IVI Use Cases Enabled**

| Use Case                     | Functionality                                            |
| ---------------------------- | -------------------------------------------------------- |
| 🎥 **Dashcam Integration**   | Composite live feeds from cameras into one stream        |
| 📶 **Live Media Feed**       | Play RTSP or HTTP-based streams from traffic sources     |
| 📁 **Offline Media Library** | Play from USB/SD using playlist system                   |
| 🎚️ **Media Mixing**         | Blend or overlay infotainment sources (e.g., news + map) |
| 📡 **Telematics Recording**  | Muxing and recording camera + mic feed to file           |
| 🧩 **Modular Upgrades**      | Add new features without breaking existing ones          |


### ✅ Final Status

* 💯 Production-ready media engine with advanced GStreamer capabilities
* 💡 Easily extensible and portable (can run on Raspberry Pi, Jetson, etc.)
* 🔌 Plugin-friendly architecture (e.g., add speech control or GPS overlay)
* 📦 Fully open-source or license-cleared (based on used libs)
