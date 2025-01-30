#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    libcamera.setup();
    // Select camera
}

//--------------------------------------------------------------
void ofApp::update(){
    libcamera.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Draw selected camera
    libcamera.draw(); //TODO: Refactor
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
