#include "AsciiConverter.hpp"
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <thread>

unsigned char AsciiConverter::calculateLuminance(unsigned char r,unsigned char g,unsigned char b){
    return static_cast<unsigned char>((299u*r+587u*g+114u*b+500u)/1000u);
}
char AsciiConverter::luminanceToCharacter(unsigned char l){
    constexpr size_t n=sizeof(CHARACTER_MAP)-1;
    return CHARACTER_MAP[(static_cast<size_t>(l)*(n-1))/255u];
}
AsciiFrame AsciiConverter::convert(const cv::Mat& frame,int targetWidth){
    if(frame.empty()||targetWidth<=0)return {};
    const double ratio=static_cast<double>(frame.rows)/frame.cols;
    const int h=std::max(1,static_cast<int>(targetWidth*ratio*0.5));
    cv::Mat resized;
    cv::resize(frame,resized,cv::Size(targetWidth,h),0,0,cv::INTER_AREA);

    AsciiFrame out;
    out.width=targetWidth; out.height=h;
    out.pixels.resize(static_cast<size_t>(targetWidth)*h);

    const unsigned int cores=std::max(1u,std::thread::hardware_concurrency());
    const unsigned int n=std::min<unsigned int>(cores,static_cast<unsigned int>(h));
    const int rowsPer=(h+static_cast<int>(n)-1)/static_cast<int>(n);

    auto work=[&](int first,int last){
        for(int y=first;y<last;++y){
            const cv::Vec3b* src=resized.ptr<cv::Vec3b>(y);
            AsciiPixel* dst=out.pixels.data()+static_cast<size_t>(y)*targetWidth;
            for(int x=0;x<targetWidth;++x){
                const auto& p=src[x];
                const unsigned char b=p[0],g=p[1],r=p[2];
                dst[x].character=luminanceToCharacter(calculateLuminance(r,g,b));
                dst[x].r=r;dst[x].g=g;dst[x].b=b;
            }
        }
    };
    if(n==1){work(0,h);return out;}
    std::vector<std::thread> threads;
    threads.reserve(n);
    for(unsigned int i=0;i<n;++i){
        int first=static_cast<int>(i)*rowsPer,last=std::min(h,first+rowsPer);
        if(first<last)threads.emplace_back(work,first,last);
    }
    for(auto& t:threads)t.join();
    return out;
}
namespace {
inline void appendNumber(std::string& s,unsigned int v){
    char b[3];int n=0;
    do{b[n++]=static_cast<char>('0'+v%10u);v/=10u;}while(v);
    while(n--)s.push_back(b[n]);
}
}
std::string AsciiConverter::toText(const AsciiFrame& f,bool color){
    if(f.width<=0||f.height<=0)return {};
    std::string s;
    s.reserve(static_cast<size_t>(f.width+1)*f.height);
    unsigned int lr=256,lg=256,lb=256;
    for(int y=0;y<f.height;++y){
        const AsciiPixel* row=f.pixels.data()+static_cast<size_t>(y)*f.width;
        for(int x=0;x<f.width;++x){
            const auto& p=row[x];
            if(color&&(p.r!=lr||p.g!=lg||p.b!=lb)){
                s.append("\033[38;2;");appendNumber(s,p.r);s.push_back(';');
                appendNumber(s,p.g);s.push_back(';');appendNumber(s,p.b);s.push_back('m');
                lr=p.r;lg=p.g;lb=p.b;
            }
            s.push_back(p.character);
        }
        s.push_back('\n');
    }
    if(color)s.append("\033[0m");
    return s;
}
cv::Mat AsciiConverter::toImage(const AsciiFrame& f,bool color){
    constexpr int CW=10,CH=18;
    cv::Mat img(f.height*CH,f.width*CW,CV_8UC3,cv::Scalar(0,0,0));
    for(int y=0;y<f.height;++y)for(int x=0;x<f.width;++x){
        const auto& p=f.at(x,y);
        cv::Scalar c;
        if(color)c=cv::Scalar(p.b,p.g,p.r);
        else {auto l=calculateLuminance(p.r,p.g,p.b);c=cv::Scalar(l,l,l);}
        cv::putText(img,std::string(1,p.character),cv::Point(x*CW,y*CH+14),
                    cv::FONT_HERSHEY_SIMPLEX,0.50,c,1,cv::LINE_AA);
    }
    return img;
}
