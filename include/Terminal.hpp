#pragma once
class Terminal {
public:
    Terminal();
    ~Terminal();
    void initialize();
    void resetCursor();
    void hideCursor();
    void showCursor();
    void clear();
private:
    bool initialized_=false, cursorHidden_=false;
};
