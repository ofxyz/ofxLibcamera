#pragma once

#include <libcamera/libcamera.h>

class ofxLibcamera {
public:
    ofxLibcamera();
    ~ofxLibcamera();

    void setup();

    static std::shared_ptr<libcamera::Camera> camera;
    std::unique_ptr<libcamera::CameraManager> cm;
};
