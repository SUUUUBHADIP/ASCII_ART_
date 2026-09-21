#pragma once
#include <string>
struct Arguments {
    std::string inputPath, outputPath;
    bool color=false, help=false;
    int width=100;
};
class ArgumentParser {
public:
    static Arguments parse(int argc, char* argv[]);
    static void printHelp(const char* programName);
private:
    static bool isInteger(const std::string&);
};
