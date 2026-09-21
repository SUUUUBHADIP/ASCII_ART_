#include "Arguments.hpp"
#include "AsciiConverter.hpp"
#include "Renderer.hpp"
#include "VideoRenderer.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace {
std::string lower(std::string s){std::transform(s.begin(),s.end(),s.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});return s;}
bool ext(const std::string& p,std::initializer_list<const char*> xs){
    std::string e=lower(std::filesystem::path(p).extension().string());
    for(auto x:xs)if(e==x)return true;return false;
}
enum class Type{Image,Video,Unknown};
Type detect(const std::string& p){
    if(ext(p,{".jpg",".jpeg",".png",".bmp",".tif",".tiff",".webp",".ppm",".pgm",".pbm",".jp2"}))return Type::Image;
    if(ext(p,{".mp4",".avi",".mov",".mkv",".webm",".wmv",".m4v",".mpeg",".mpg",".3gp"}))return Type::Video;
    cv::Mat m=cv::imread(p,cv::IMREAD_COLOR);if(!m.empty())return Type::Image;
    cv::VideoCapture c(p);if(c.isOpened()){c.release();return Type::Video;}return Type::Unknown;
}
bool image(const Arguments& a){
    cv::Mat m=cv::imread(a.inputPath,cv::IMREAD_COLOR);
    if(m.empty()){std::cerr<<"Error: unable to read image: "<<a.inputPath<<"\n";return false;}
    auto f=AsciiConverter::convert(m,a.width);if(f.width<=0)return false;
    if(!a.outputPath.empty()){
        if(lower(std::filesystem::path(a.outputPath).extension().string())!=".txt"){std::cerr<<"Error: image output must use .txt.\n";return false;}
        if(!Renderer::renderToFile(f,a.outputPath,a.color)){std::cerr<<"Error: cannot write "<<a.outputPath<<"\n";return false;}
        std::cout<<"ASCII image saved to: "<<a.outputPath<<"\n";return true;
    }
    Renderer::renderToTerminal(f,a.color);return true;
}
bool video(const Arguments& a){
    if(!a.outputPath.empty()&&lower(std::filesystem::path(a.outputPath).extension().string())!=".mp4"){std::cerr<<"Error: video output must use .mp4.\n";return false;}
    return VideoRenderer(a.width,a.color,a.outputPath).process(a.inputPath);
}
}
int main(int argc,char* argv[]){
    try{
        auto a=ArgumentParser::parse(argc,argv);
        if(a.help){ArgumentParser::printHelp(argv[0]);return 0;}
        if(!std::filesystem::exists(a.inputPath)){std::cerr<<"Error: input file does not exist: "<<a.inputPath<<"\n";return 1;}
        Type t=detect(a.inputPath);
        if(t==Type::Unknown){std::cerr<<"Error: unsupported or undecodable input.\n";return 1;}
        return (t==Type::Image?image(a):video(a))?0:1;
    }catch(const std::exception& e){
        std::cerr<<"Error: "<<e.what()<<"\n\n";ArgumentParser::printHelp(argv[0]);return 1;
    }
}
