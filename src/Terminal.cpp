#include "Terminal.hpp"
#include <cstdio>
#ifdef _WIN32
#include <windows.h>
#endif
Terminal::Terminal()=default;
Terminal::~Terminal(){showCursor();std::fputs("\033[0m",stdout);std::fflush(stdout);}
void Terminal::initialize(){
    if(initialized_)return;initialized_=true;
#ifdef _WIN32
    HANDLE h=GetStdHandle(STD_OUTPUT_HANDLE);DWORD mode=0;
    if(h!=INVALID_HANDLE_VALUE&&GetConsoleMode(h,&mode)){
        mode|=ENABLE_VIRTUAL_TERMINAL_PROCESSING;SetConsoleMode(h,mode);
    }
#endif
    hideCursor();
}
void Terminal::resetCursor(){std::fputs("\033[H",stdout);}
void Terminal::hideCursor(){if(!cursorHidden_){std::fputs("\033[?25l",stdout);std::fflush(stdout);cursorHidden_=true;}}
void Terminal::showCursor(){if(cursorHidden_){std::fputs("\033[?25h",stdout);std::fflush(stdout);cursorHidden_=false;}}
void Terminal::clear(){std::fputs("\033[2J\033[H",stdout);std::fflush(stdout);}
