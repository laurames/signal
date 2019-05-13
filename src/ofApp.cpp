#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(2);
    ofBackground(0,0,0);
    
    // OSC Setup
    receiver.setup(SEND_PORT);
    
    delta = theta = beta = alpha = gamma = 0.0;
}

//--------------------------------------------------------------
void ofApp::update(){
    /*for(int i=0; i<5; i++){
        linePoints[i].set(ofRandom(ofGetWidth()),ofRandom(ofGetHeight()));
    }
    for(int i=0; i<5; i++){
        linePoints[i].set(ofRandom(1028),ofRandom(768));
    }*/
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage msg;
        receiver.getNextMessage(msg);
        
        
        // all available messages (a lot with muse): https://sites.google.com/a/interaxon.ca/muse-developer-site/museio/osc-paths/3-4-0
        //ofLog() << msg.getAddress() << " : " << msg.getNumArgs();// << " : " << msg.getArgAsFloat(0);
        
        // Getting the 5 main EEG bands (can get relative or absolute values)
        // filter the messages - 4 channels per data point (left ear(0), left forehead(1), right forehead(2), right ear(3))
        // averaging the 4 channels for simplicity - change: keep all 4 in float array instead
        // if it NANS - it's because one of the args in the channel is NAN due to head band not being attached correctly. should add checks for this.
        // checking for "/muse/elements/is_good" for each of the 4 channels (1 then were ok).

            if(msg.getAddress() == "/muse/elements/raw_fft0") {
                cout << "raw_fft0 is: " << endl;
                cout << msg << endl;
                //delta = getAverageFromChannels(msg);
                delta = ofMap(msg.getNumArgs(),0, 1682.815, 0, 1024);
                
            } else if(msg.getAddress() == "/muse/elements/raw_fft1") {
                cout << "raw_fft1 is: " << endl;
                cout << msg << endl;
                theta = getAverageFromChannels(msg);
                
            } else if(msg.getAddress() == "/muse/elements/raw_fft2") {
                cout << "alpha is: " << endl;
                cout << msg << endl;
                alpha = getAverageFromChannels(msg);
                
            } else if(msg.getAddress() == "/muse/elements/raw_fft3") {
                cout << "beta is: " << endl;
                cout << msg << endl;
                beta = getAverageFromChannels(msg);
                
            }
    }
    
    linePoints[0].set(ofMap(delta,0,1,0,1024),ofMap(delta,0,1,0,768));
    linePoints[1].set(ofMap(theta,0,1,0,1024),ofMap(theta,0,1,0,768));
    linePoints[2].set(ofMap(alpha,0,1,0,1024),ofMap(theta,0,1,0,768));
    linePoints[3].set(ofMap(beta,0,1,0,1024),ofMap(beta,0,1,0,768));
    linePoints[4].set(ofMap(gamma,0,1,0,1024),ofMap(gamma,0,1,0,768));
}

//--------------------------------------------------------------
float ofApp::getAverageFromChannels(ofxOscMessage& msg) {
    
    float avg = 0;
    int channels = msg.getNumArgs();
    for(int i = 0; i < channels; i++) {
        avg += msg.getArgAsFloat(i);
    }
    avg /= channels;
    return avg;
}

float ofApp::getHighestFromChannels(ofxOscMessage& msg) {
    
    float highest = 0;
    int channels = msg.getNumArgs();
    for(int i = 0; i < channels; i++) {
        float channelValue = msg.getArgAsFloat(i);
        if(channelValue > highest) highest = channelValue;
    }
    return highest;
}

float ofApp::getFromEarChannels(ofxOscMessage& msg) {
    
    float avg = 0;
    avg += msg.getArgAsFloat(0);
    avg += msg.getArgAsFloat(3);
    avg /= 2;
    return avg;
}

float ofApp::getFromForeheadChannels(ofxOscMessage& msg) {
    
    float avg = 0;
    avg += msg.getArgAsFloat(1);
    avg += msg.getArgAsFloat(2);
    avg /= 2;
    return avg;
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(241, 226, 119);
    ofFill();
    
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, sizeof(ofVec2f), &linePoints[0].x);
    glDrawArrays(GL_POLYGON, 0, 5);
    
    /*
    // colours for debug text
    ofColor clearClr(0,0);
    ofColor textClr(0);
    
    
    ofPushMatrix();
    ofTranslate(0,ofGetHeight()*.5);
    
    // draw circles for each of the 5 eeg bands
    int offsetX = ofGetWidth() / 6;
    
    // delta yellow
    ofSetColor(241, 226, 119);
    float deltaScaled = ofMap(delta, 0, 1.0, 0, 150);
    ofCircle(offsetX, 0, deltaScaled);
    ofDrawBitmapStringHighlight("Delta:\n"+ofToString(delta,2), offsetX, 0, clearClr, textClr);
    
    // theta green
    ofSetColor(128, 188, 177);
    float thetaScaled = ofMap(theta, 0, 1.0, 0, 150);
    ofCircle(offsetX*2, 0, thetaScaled);
    ofDrawBitmapStringHighlight("Theta:\n"+ofToString(theta,2), offsetX*2, 0, clearClr, textClr);
    
    // alpha blue
    ofSetColor(0, 174, 240);
    float alphaScaled = ofMap(alpha, 0, 1.0, 0, 150);
    ofCircle(offsetX*3, 0, alphaScaled);
    ofDrawBitmapStringHighlight("Alpha:\n"+ofToString(alpha,2), offsetX*3, 0, clearClr, textClr);
    
    // beta red
    ofSetColor(255, 107, 130);
    float betaScaled = ofMap(beta, 0, 1.0, 0, 150);
    ofCircle(offsetX*4, 0, betaScaled);
    ofDrawBitmapStringHighlight("Beta:\n"+ofToString(beta,2), offsetX*4, 0, clearClr, textClr);
    
    // gamma orange
    ofSetColor(255, 165, 106);
    float gammaScaled = ofMap(gamma, 0, 1.0, 0, 150);
    ofCircle(offsetX*5, 0, gammaScaled);
    ofDrawBitmapStringHighlight("Gamma:\n"+ofToString(gamma,2), offsetX*5, 0, clearClr, textClr);
    
    ofPopMatrix();
    */
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
