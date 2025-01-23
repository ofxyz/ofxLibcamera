#pragma once

#include <libcamera/libcamera.h>

class ofxLibcamera {
public:
    ofxLibcamera();
    ~ofxLibcamera();

    void setup();
    void update();
    void exit();

protected:
    std::string getCameraName(libcamera::Camera *camera);

private:
    std::unique_ptr<libcamera::CameraManager> cm;
    std::vector< std::shared_ptr< libcamera::Camera > > cameras;

    // TODO
    std::shared_ptr<libcamera::Camera> camera;
    std::unique_ptr<libcamera::CameraConfiguration> config;
    libcamera::StreamConfiguration streamConfig;
    std::vector<std::unique_ptr<libcamera::Request>> requests;
    libcamera::Stream *stream;
    libcamera::FrameBufferAllocator *allocator;

    void requestComplete(libcamera::Request *request);
};