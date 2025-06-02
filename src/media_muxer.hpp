#ifndef MEDIA_MUXER_HPP
#define MEDIA_MUXER_HPP

#include <string>
#include <gst/gst.h>

class MediaMuxer {
public:
    MediaMuxer(const std::string& videoPath, const std::string& audioPath, const std::string& outputPath);
    ~MediaMuxer();
    bool run();

private:
    std::string videoFile;
    std::string audioFile;
    std::string outputFile;
};

#endif // MEDIA_MUXER_HPP
