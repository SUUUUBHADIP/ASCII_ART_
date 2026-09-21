#include "Arguments.hpp"
#include <iostream>
#include <stdexcept>
bool ArgumentParser::isInteger(const std::string& v){
    if(v.empty()) return false;
    size_t s=(v[0]=='-'||v[0]=='+')?1:0;
    if(s>=v.size()) return false;
    for(size_t i=s;i<v.size();++i) if(v[i]<'0'||v[i]>'9') return false;
    return true;
}
Arguments ArgumentParser::parse(int argc,char* argv[]){
    Arguments a;
    for(int i=1;i<argc;++i){
        std::string x=argv[i];
        if(x=="-h"||x=="--help"){a.help=true;return a;}
        if(x=="-i"||x=="--input"){if(++i>=argc)throw std::runtime_error("Missing value for --input.");a.inputPath=argv[i];}
        else if(x=="-o"||x=="--output"){if(++i>=argc)throw std::runtime_error("Missing value for --output.");a.outputPath=argv[i];}
        else if(x=="-c"||x=="--color") a.color=true;
        else if(x=="-w"||x=="--width"){
            if(++i>=argc)throw std::runtime_error("Missing value for --width.");
            if(!isInteger(argv[i]))throw std::runtime_error("Width must be an integer.");
            a.width=std::stoi(argv[i]); if(a.width<=0)throw std::runtime_error("Width must be greater than zero.");
        } else throw std::runtime_error("Unknown argument: "+x);
    }
    if(a.inputPath.empty())throw std::runtime_error("Input path is required. Use -i or --input.");
    return a;
}
void ArgumentParser::printHelp(const char* p){
    std::cout<<"\nASCII Art Converter v2.0\n========================\n\n"
             <<"Usage:\n  "<<p<<" -i <input> [options]\n\n"
             <<"Options:\n"
             <<"  -i, --input <path>     Required input image/video\n"
             <<"  -o, --output <path>    Image: .txt | Video: .mp4\n"
             <<"  -c, --color            24-bit RGB ANSI terminal mode\n"
             <<"  -w, --width <int>      ASCII width. Default: 100\n"
             <<"  -h, --help             Show this help\n\n"
             <<"v2 performance: multi-threaded conversion, buffered ANSI output,\n"
             <<"reduced color escapes, and real-time frame scheduling.\n\n";
}
