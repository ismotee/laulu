#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup() {
    Ohjain::setup();
    ofSetFrameRate(60);
    ofHideCursor();
}

//--------------------------------------------------------------

void ofApp::setupMonitor() {
    ofBackground(ofColor::black);
    monitori2.setup();
}

//--------------------------------------------------------------

void ofApp::update() {
    Ohjain::update();
    Kyna::update();   
}

//--------------------------------------------------------------

void ofApp::draw() {
    monitori1.draw();

    Ohjain::draw();
}

//--------------------------------------------------------------

void ofApp::drawMonitor(ofEventArgs& args) {
    Ohjain::updateMonitori();
    monitori2.draw();
}

//--------------------------------------------------------------

void ofApp::keyPressed(int key) {
    Ohjain::keyPressed(key);
}

//--------------------------------------------------------------

void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------

void ofApp::mouseMoved(int x, int y) {
    //Kyna::moved(x, y);
}

//--------------------------------------------------------------

void ofApp::mouseDragged(int x, int y, int button) {
    //Kyna::moved(x, y);
}

//--------------------------------------------------------------

void ofApp::mousePressed(int x, int y, int button) {
    //Kyna::pressed(x, y);
}

//--------------------------------------------------------------

void ofApp::mouseReleased(int x, int y, int button) {
    //Kyna::released(x, y);
}
//--------------------------------------------------------------

void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------

void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------

void ofApp::windowResized(int w, int h) {
    windowWidth = w;
    windowHeight = h;
}
//--------------------------------------------------------------

void ofApp::monitorResized(ofResizeEventArgs& args) {
    monitorWidth = args.width;
    monitorHeight = args.height;
}

void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------

void ofApp::dragEvent(ofDragInfo dragInfo) {

}