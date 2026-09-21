#include "VideoRenderer.hpp"
#include "Renderer.hpp"
#include "Terminal.hpp"
#include <csignal>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib> // <--- Added for std::system

std::atomic<bool> VideoRenderer::stopRequested_{false};
namespace { void signalHandler(int){VideoRenderer::requestStop();} }

VideoRenderer::VideoRenderer(int w,bool c,const std::string& o)
    :targetWidth_(w),color_(c),outputPath_(o){}

void VideoRenderer::requestStop(){stopRequested_.store(true,std::memory_order_relaxed);}

bool VideoRenderer::initializeWriter(const cv::VideoCapture& cap,const AsciiFrame& f){
    if(outputPath_.empty())return true;
    double sourceFps=cap.get(cv::CAP_PROP_FPS);
    if(sourceFps>0&&sourceFps<1000)fps_=sourceFps;
    const cv::Size size(f.width*10,f.height*18);
    const int fourcc=cv::VideoWriter::fourcc('m','p','4','v');
    if(!writer_.open(outputPath_,fourcc,fps_,size,true)){
        std::cerr<<"Error: could not open MP4 writer. Your OpenCV build may lack a compatible encoder.\n";
        return false;
    }
    return true;
}

bool VideoRenderer::writeFrame(const AsciiFrame& f){
    if(!writer_.isOpened())return true;
    cv::Mat img=AsciiConverter::toImage(f,color_);
    if(img.empty())return false;
    writer_.write(img);return true;
}

void VideoRenderer::renderFrame(const AsciiFrame& f){
    std::fputs("\033[H",stdout);
    Renderer::renderToTerminal(f,color_);
}

bool VideoRenderer::process(const std::string& input){
    stopRequested_.store(false,std::memory_order_relaxed);
    std::signal(SIGINT,signalHandler);
    cv::VideoCapture cap(input);
    if(!cap.isOpened()){
        std::cerr<<"Error: could not open video: "<<input<<"\n"
                 <<"The file may be missing, corrupted, or use an unsupported codec.\n";
        return false;
    }
    fps_=cap.get(cv::CAP_PROP_FPS);
    if(fps_<=0||fps_>1000)fps_=30.0;
    const auto interval=std::chrono::duration<double>(1.0/fps_);
    Terminal terminal;terminal.initialize();terminal.clear();
    cv::Mat frame;bool writerReady=false;

    // --- AUDIO BACKEND PROCESS (ONLY FOR TERMINAL PLAYBACK) ---
    if (outputPath_.empty()) {
#if defined(_WIN32)
        std::string audioCmd = "start /b ffplay -nodisp -autoexit -loglevel quiet \"" + input + "\"";
#else
        std::string audioCmd = "ffplay -nodisp -autoexit -loglevel quiet \"" + input + "\" &";
#endif
        std::system(audioCmd.c_str());
    }
    // ---------------------------------------------------------

    auto next=std::chrono::steady_clock::now();

    while(!stopRequested_.load(std::memory_order_relaxed)){
        if(!cap.read(frame)||frame.empty())break;
        AsciiFrame ascii=AsciiConverter::convert(frame,targetWidth_);
        if(ascii.width<=0){std::cerr<<"Error: frame conversion failed.\n";return false;}
        if(!writerReady){if(!initializeWriter(cap,ascii))return false;writerReady=true;}
        renderFrame(ascii);
        if(!writeFrame(ascii)){std::cerr<<"\nError: failed writing video frame.\n";return false;}

        next+=std::chrono::duration_cast<std::chrono::steady_clock::duration>(interval);
        auto now=std::chrono::steady_clock::now();
        if(now<next)std::this_thread::sleep_until(next);
        else next=now; // Never accumulate playback lag.
    }

    // Stop background audio process if user hits Ctrl+C
    if (outputPath_.empty()) {
#if defined(_WIN32)
        std::system("taskkill /f /im ffplay.exe >nul 2>&1");
#else
        std::system("killall ffplay >/dev/null 2>&1");
#endif
    }

    cap.release();if(writer_.isOpened())writer_.release();
    terminal.showCursor();std::fputs("\033[0m\n",stdout);std::fflush(stdout);
    if(stopRequested_.load(std::memory_order_relaxed))std::cout<<"Playback interrupted.\n";
    return true;
}