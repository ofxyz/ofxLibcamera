#include "ofxLibcamera.h"
#include "ofLog.h"

ofxLibcamera::ofxLibcamera() {
    
};

ofxLibcamera::~ofxLibcamera(){
    
};

void ofxLibcamera::setup() {
    cm = std::make_unique<libcamera::CameraManager>();
    cm->start();

    for (auto const &camera : cm->cameras()){
        ofLog(OF_LOG_VERBOSE) << camera->id();
    }
};
