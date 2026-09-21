# ASCII Art Converter

C++17 command-line image/video to ASCII art converter using OpenCV.

## Features

- JPG, PNG, BMP, TIFF, WebP and other OpenCV-supported images
- MP4, AVI, MOV, MKV, WebM and other OpenCV-supported videos
- Automatic image/video detection
- Configurable ASCII width
- Grayscale or 24-bit ANSI RGB terminal output
- FPS-matched video playback
- ANSI cursor positioning to reduce flicker
- Ctrl+C cleanup
- Optional `.txt` image output
- Optional `.mp4` ASCII video output
- CMake build
- Built purely to play badapple in terminal as a joke.
# ASCII Art Converter current version-v2.0 — Performance Build


This version adds:

- Multi-threaded ASCII conversion using `std::thread`
- Integer luminance calculation
- One buffered stdout write per frame
- Reduced ANSI color escape traffic
- No per-row ANSI reset
- Real-time video scheduling that does not accumulate lag
- Release-build optimization flags

## Build

From the project root:

```powershell
cd build
cmake ..
cmake --build . --config Release
```

For MinGW:

```powershell
cmake -G "MinGW Makefiles" ..
cmake --build . --config Release
```

MSVC:
```powershell
.\Release\ascii-art.exe -i ..\video.mp4 -w 120
```

MinGW:
```powershell
.\ascii-art.exe -i ..\video.mp4 -w 120
```

Color:
```powershell
.\Release\ascii-art.exe -i ..\video.mp4 -w 120 -c
```

## Performance tips

The terminal can become the bottleneck before the CPU does.

Try:
```text
-w 80
-w 100
-w 120
-w 150
```

At very high widths, every frame contains a large amount of terminal data. Color mode is considerably heavier because ANSI RGB escape sequences must be transmitted.

The scheduler keeps playback close to real time rather than allowing processing delays to accumulate.

## MP4 output

MP4 output requires an OpenCV build with a compatible encoder, commonly FFmpeg.
