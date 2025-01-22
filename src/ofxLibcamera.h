#pragma once

#include <libcamera/libcamera.h>

class ofxLibcamera {
public:
    ofxLibcamera();
    ~ofxLibcamera();

    void setup();

protected:
    std::string cameraName(libcamera::Camera *camera);

private:

    static std::shared_ptr<libcamera::Camera> camera;
    std::unique_ptr<libcamera::CameraManager> cm;
};
