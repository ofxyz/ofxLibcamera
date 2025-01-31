#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    // Selects first available camera
    libcamera.setup(800,600);
}

//--------------------------------------------------------------
void ofApp::update()
{
    // Grabs a new frame
    libcamera.update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    // Draw the internal texture
    libcamera.draw( 0, 0, libcamera.getWidth(), libcamera.getHeight() );
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
