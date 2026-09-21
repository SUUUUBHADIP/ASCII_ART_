#pragma once
#include <opencv2/core.hpp>
#include <string>
#include <vector>

struct AsciiPixel {
    char character=' ';
    unsigned char r=0,g=0,b=0;
};
struct AsciiFrame {
    int width=0,height=0;
    std::vector<AsciiPixel> pixels;
    const AsciiPixel& at(int x,int y) const {
        return pixels[static_cast<size_t>(y)*static_cast<size_t>(width)+static_cast<size_t>(x)];
    }
};
class AsciiConverter {
public:
    static AsciiFrame convert(const cv::Mat&, int targetWidth);
    static std::string toText(const AsciiFrame&, bool color);
    static cv::Mat toImage(const AsciiFrame&, bool color);
private:
    static constexpr const char* CHARACTER_MAP =
        " $@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrft/"
        "\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
    static char luminanceToCharacter(unsigned char);
    static unsigned char calculateLuminance(unsigned char,unsigned char,unsigned char);
};
