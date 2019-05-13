#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(2); //framerate
    ofBackground(0,0,0); //black background
    
    // OSC Setup
    receiver.setup(SEND_PORT); //we are listening to the port 5000 defined in ofApp.h
    
    //new file called data:
    //dataFile.open("dataFile.txt",ofFile::WriteOnly);
    
    delta = theta = beta = alpha = gamma = 0.0; //all these variables are of float 0
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //reading messages from OSC
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
        
        if(leftEar == 1 && leftForehead == 1 && rightForehead == 1 && rightEar == 1){
            mockdata = false;
            if(msg.getAddress() == "/muse/elements/delta_relative") { //raw_fft0
                cout << "delta is: " << endl;
                cout << msg << endl;
                delta = getAverageFromChannels(msg);
                //delta = ofMap(msg.getNumArgs(),0, 1682.815, 0, 1024);
                
            } else if(msg.getAddress() == "/muse/elements/theta_relative") {
                cout << "theta is: " << endl;
                cout << msg << endl;
                theta = getAverageFromChannels(msg);
                
            } else if(msg.getAddress() == "/muse/elements/alpha_relative") {
                cout << "alpha is: " << endl;
                cout << msg << endl;
                alpha = getAverageFromChannels(msg);
                
            } else if(msg.getAddress() == "/muse/elements/beta_relative") {
                cout << "beta is: " << endl;
                cout << msg << endl;
                beta = getAverageFromChannels(msg);
                
            } else if(msg.getAddress() == "/muse/elements/gamma_relative") {
                cout << "gamma is: " << endl;
                cout << msg << endl;
                gamma = getAverageFromChannels(msg);
                
            }
            signalGood = true;
            
            /* getting data saved if the signal is good:
            dataFile.open("dataFile.txt",ofFile::Append);
            //delta, theta, alpha, beta, gamma
            dataFile << ofToString(delta) + "," + ofToString(theta) + "," + ofToString(alpha) + "," + ofToString(beta) + "," + ofToString(gamma) + ", signal: "+ ofToString(signalGood) +"\n";
            */
        }else{
            //we write mock data:
            mockdata = true;
            delta = (float)ofRandom(0,1);
            theta = (float)ofRandom(0,1);
            alpha = (float)ofRandom(0,1);
            beta = (float)ofRandom(0,1);
            gamma = (float)ofRandom(0,1);
        }
        
        //reading battery data:
        if(msg.getAddress() == "/muse/batt"){ //if we have a bettery message
            batteryPercentage = msg.getArgAsFloat(0)/100;
        }
        //reading signal quality
        if(msg.getAddress() == "/muse/elements/is_good"){
            //left ear(0), left forehead(1), right forehead(2), right ear(3)
            leftEar = msg.getArgAsInt(0);
            leftForehead = msg.getArgAsInt(1);
            rightForehead = msg.getArgAsInt(2);
            rightEar = msg.getArgAsInt(3);
        }
        offset = ofRandom(-1, 1)*ofRandom(hSize);
    }
    
}

//--------------------------------------------------------------
float ofApp::getAverageFromChannels(ofxOscMessage& msg) {
    
    float avg = 0;
    int channels = msg.getNumArgs();
    for(int i = 0; i < channels; i++) { //takes all channels
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
    
    ofFill();
    
    //------------------ READING FILE DATA ------------------//
    //reading data from file:
    vector < string > linesOfTheFile;
    ofBuffer buffer = ofBufferFromFile("dataFile.txt");
    for (auto line : buffer.getLines()){
        linesOfTheFile.push_back(line);
    }
    /* trying to rewrite a file:
    if(linesOfTheFile.size()>50){
        //keep only 40
        dataFile.open("dataFile.txt",ofFile::WriteOnly);
        for(int i=linesOfTheFile.size()-1; i>=linesOfTheFile.size()-40; i--){
            vector <string> splitItems = ofSplitString(linesOfTheFile[i], ",");
            //delta, theta, alpha, beta, gamma, signal: (0 or 1)
            dataFile << ofToString(splitItems[0]) + "," + ofToString(splitItems[1]) + "," + ofToString(splitItems[2]) + "," + ofToString(splitItems[3]) + "," + ofToString(splitItems[4]) + ", signal: "+ ofToString(splitItems[5]) +"\n";
        }
    }*/
    //for debugging (how many data points in file):
    //cout << linesOfTheFile.size() << endl;
    
    ofEnableAlphaBlending();    // turn on alpha blending
    int color = 255;
    
    for(int i=linesOfTheFile.size()-1; i>=0; i--){
        vector <string> splitItems = ofSplitString(linesOfTheFile[i], ",");
            ofSetColor(241-i, 226-i, 119-i, color/1.05);
        /*
        0.159698,0.0806554,0.165869,0.372284,0.221493, signal: 1
        0.132792,0.0836684,0.154343,0.39236,0.236837, signal: 1
        0.0838018,0.0574778,0.139654,0.451473,0.267593, signal: 1
         delta, theta, alpha, beta, gamma, signal (0 or 1)
         + (offset*2)*/
        
        int offset2 = ofToFloat(splitItems[5]);
            linePoints[0].set(ofMap( ofToFloat(splitItems[0]),0,1,0,1024) + offset2,ofMap( ofToFloat(splitItems[1]),0,1,0,768) + offset2);
            linePoints[1].set(ofMap( ofToFloat(splitItems[1]),0,1,0,1024) + offset2,ofMap( ofToFloat(splitItems[2]),0,1,0,768) + offset2);
            linePoints[2].set(ofMap( ofToFloat(splitItems[2]),0,1,0,1024) + offset2,ofMap( ofToFloat(splitItems[3]),0,1,0,768) + offset2);
            linePoints[3].set(ofMap( ofToFloat(splitItems[3]),0,1,0,1024) + offset2,ofMap( ofToFloat(splitItems[4]),0,1,0,768) + offset2);
            linePoints[4].set(ofMap( ofToFloat(splitItems[4]),0,1,0,1024) + offset2,ofMap( ofToFloat(splitItems[0]),0,1,0,768) + offset2);
            
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2, GL_FLOAT, sizeof(ofVec2f), &linePoints[0].x);
            glDrawArrays(GL_POLYGON, 0, 5);
        color = color/1.05;
    }
    
    ofDisableAlphaBlending();   // turn it back off, if you don't need it
    
    //------------------ YOUR CURRENT DATA ------------------//
    ofSetColor(150, 150, 150);
    ofDrawRectangle(wSize-wSize/4, 0, wSize/4, hSize/4);
    // mockup data:
    ofSetColor(255, 0, 0);
    linePoints[0].set(ofMap(delta,0,1,wSize-wSize/4,wSize),ofMap(theta,0,1,0,hSize/4));
    linePoints[1].set(ofMap(theta,0,1,wSize-wSize/4,wSize),ofMap(alpha,0,1,0,hSize/4));
    linePoints[2].set(ofMap(alpha,0,1,wSize-wSize/4,wSize),ofMap(beta,0,1,0,hSize/4));
    linePoints[3].set(ofMap(beta,0,1,wSize-wSize/4,wSize),ofMap(gamma,0,1,0,hSize/4));
    linePoints[4].set(ofMap(gamma,0,1,wSize-wSize/4,wSize),ofMap(delta,0,1,0,hSize/4));
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, sizeof(ofVec2f), &linePoints[0].x);
    glDrawArrays(GL_POLYGON, 0, 5);
    
    //------------------ DEBUGGING TEXTS (LEFT CORNER) ------------------//
    // colours for debug text
    ofSetColor(255,255,255);
    //debug messages:
    ofDrawBitmapString("Your current data:", ofGetWidth()-(ofGetWidth()/4)+10, 20);
    ofDrawBitmapString("Headband battery at: " + ofToString(batteryPercentage) + "%", 50, 20);
    ofDrawBitmapString("leftEar: " + ofToString(leftEar) + ", leftForehead: " + ofToString(leftForehead) + ", rightForehead: " + ofToString(rightForehead) + ", rightEar: " + ofToString(rightEar), 50, 35);
    ofDrawBitmapString("Mock data: " + ofToString(mockdata), 50, 50);
    ofDrawBitmapString( ofToString(linesOfTheFile[linesOfTheFile.size()-1]) , 50, hSize-40 );

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //with s key press save to file the current data points
    if(key == 's'){
        dataFile.open("dataFile.txt",ofFile::Append);
        //delta, theta, alpha, beta, gamma, offset, signal (0 or 1)
        dataFile << ofToString(delta) + "," + ofToString(theta) + "," + ofToString(alpha) + "," + ofToString(beta) + "," + ofToString(gamma) + "," + ofToString(offset) +", signal: "+ ofToString(signalGood) +"\n";
    }
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
