#pragma once

#include <libcamera/libcamera.h>
#include <mutex>
#include <queue>
#include "ofTexture.h"

class ofxLibcamera {
public:
    ofxLibcamera();
    ~ofxLibcamera();

    void setup();
    void update();
    void draw(); //TODO REMOVE 
    void exit();

protected:
    std::string getCameraName(libcamera::Camera *camera);

private:
    std::unique_ptr<libcamera::CameraManager> cm;
    std::vector< std::shared_ptr< libcamera::Camera > > cameras;

    // TODO Create camera class
    std::shared_ptr<libcamera::Camera> camera;
    std::unique_ptr<libcamera::CameraConfiguration> config;
    libcamera::StreamConfiguration streamConfig;
    std::vector<std::unique_ptr<libcamera::Request>> requests;
    libcamera::Stream *stream;
    libcamera::FrameBufferAllocator *allocator;
    ofTexture tex;

    std::map<int, std::pair<void *, unsigned int>> mappedBuffers_;

    void requestComplete(libcamera::Request *request);

    std::queue<libcamera::Request *> requestQueue;

    std::mutex control_mutex_;
    std::mutex camera_stop_mutex_;
    std::mutex free_requests_mutex_;
};
