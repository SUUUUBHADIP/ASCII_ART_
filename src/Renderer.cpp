#include "Renderer.hpp"
#include <cstdio>
#include <fstream>
void Renderer::renderToTerminal(const AsciiFrame& f,bool color){
    const std::string s=AsciiConverter::toText(f,color);
    if(!s.empty()){std::fwrite(s.data(),1,s.size(),stdout);std::fflush(stdout);}
}
bool Renderer::renderToFile(const AsciiFrame& f,const std::string& p,bool color){
    std::ofstream file(p,std::ios::binary|std::ios::trunc);
    if(!file)return false;
    const std::string s=AsciiConverter::toText(f,color);
    file.write(s.data(),static_cast<std::streamsize>(s.size()));
    return file.good();
}
