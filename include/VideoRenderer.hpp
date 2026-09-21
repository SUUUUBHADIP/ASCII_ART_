#pragma once
#include "AsciiConverter.hpp"
#include <opencv2/videoio.hpp>
#include <atomic>
#include <string>
class VideoRenderer {
public:
    VideoRenderer(int,bool,const std::string&);
    bool process(const std::string&);
    static void requestStop();
private:
    bool initializeWriter(const cv::VideoCapture&,const AsciiFrame&);
    bool writeFrame(const AsciiFrame&);
    void renderFrame(const AsciiFrame&);
    int targetWidth_;
    bool color_;
    std::string outputPath_;
    cv::VideoWriter writer_;
    double fps_=30.0;
    static std::atomic<bool> stopRequested_;
};
