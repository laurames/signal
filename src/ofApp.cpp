#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(2); //framerate
    ofBackground(0,0,0); //black background
    ofHideCursor();
    
    // OSC Setup
    receiver.setup(SEND_PORT); //we are listening to the port 5000 defined in ofApp.h
    
    //new file called dataFile.txt:
    //dataFile.open("dataFile.txt",ofFile::WriteOnly);
    
    delta = theta = beta = alpha = gamma = 0.0; //all these variables are of float 0
    
    //reading data from file:
    ofBuffer buffer = ofBufferFromFile("dataFile.txt");
    for (auto line : buffer.getLines()){
        linesOfTheFile.push_back(line);
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){
    receiving = false;
    //counter for saving data into file:
    if(tick > 30){
        dataFile.open("dataFile.txt",ofFile::WriteOnly);
        if(linesOfTheFile.size()>1){
            for(int i=0; i<linesOfTheFile.size(); i++){
                dataFile << linesOfTheFile[i] << "\n";
            }
        }
        dataFile.close();
        tick = 0;
    }
    //cout << tick << endl;
    tick++;
    
    //reading messages from OSC
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage msg;
        receiver.getNextMessage(msg);
        receiving = true;
        
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
                /* cout << "delta is: " << endl;
                cout << msg << endl; */
                delta = getAverageFromChannels(msg);
                
            } else if(msg.getAddress() == "/muse/elements/theta_relative") {
                /* cout << "theta is: " << endl;
                cout << msg << endl; */
                theta = getAverageFromChannels(msg);
                
            } else if(msg.getAddress() == "/muse/elements/alpha_relative") {
                /* cout << "alpha is: " << endl;
                cout << msg << endl; */
                alpha = getAverageFromChannels(msg);
                
            } else if(msg.getAddress() == "/muse/elements/beta_relative") {
                /* cout << "beta is: " << endl;
                cout << msg << endl; */
                beta = getAverageFromChannels(msg);
                
            } else if(msg.getAddress() == "/muse/elements/gamma_relative") {
                /* cout << "gamma is: " << endl;
                cout << msg << endl; */
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
            signalGood = false;
            delta = (float)ofRandom(0,1);
            theta = (float)ofRandom(0,1);
            alpha = (float)ofRandom(0,1);
            beta = (float)ofRandom(0,1);
            gamma = (float)ofRandom(0,1);
        }
        
        //reading blink data:
        if(msg.getAddress() == "/muse/elements/blink"){
            blink = msg.getArgAsFloat(0);
            cout << "blink: " << endl;
            cout << msg << endl;
        }
        
        //reading jaw clenching:
        if(msg.getAddress() == "/muse/elements/jaw_clench" ){
            jawClench = msg.getArgAsFloat(0);
        }
        
        //headband touching forhead
        if(msg.getAddress() == "/muse/elements/touching_forehead" ){
            onForehead = msg.getArgAsFloat(0);
        }
        
        /*read Accelerometer Data:
        if(msg.getAddress() == "/muse/acc" ){
            if(change == 1){
                accX = msg.getArgAsFloat(0);
                accY = msg.getArgAsFloat(1);
                accZ = msg.getArgAsFloat(2);
                change = 0;
            } else {
                accXchange = msg.getArgAsFloat(0);
                accYchange = msg.getArgAsFloat(1);
                accZchange = msg.getArgAsFloat(2);
                change = 1;
            }
        }*/
        
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
    
    //------------------ READING FILE DATA ------------------//
    
    //for debugging (how many lines of data in file):
    //cout << linesOfTheFile.size() << endl;
    
    //blink detected save a data point:
    if(blink == 1){
        if(linesOfTheFile.size() > 9){
            linesOfTheFile.erase(linesOfTheFile.begin());
        }
        //delta, theta, alpha, beta, gamma, offset, randomColor, vertexOfShape, signalGood
        linesOfTheFile.push_back(ofToString(delta) + "," + ofToString(theta) + "," + ofToString(alpha) + "," + ofToString(beta) + "," + ofToString(gamma) + "," + ofToString(ofRandom(255)) + ", signal: "+ ofToString(signalGood));
    }
    
    if(linesOfTheFile.size()>1 && linesOfTheFile[0] != ""){
        
        vector <string> splitItems;
        
        /*if(onForehead == 1){
            cout << "accX, accXchange" << endl;
            cout << accX << endl;
            cout << accXchange << endl;
            //cout << accZ << endl;
            
            if( accX != 0.0 && accX+20 < accXchange ){
                //LEFT
                hSize + 100;
                wSize + 100;
            }
        }*/
        
        if(jawClench == 1){
            
            ofSetColor(113, 178, 223); //blue
            //------(f)--------------------------------------
            //
            //         ofCurveVertex
            //
            //         because it uses catmul rom splines, we need to repeat the first and last
            //         items so the curve actually goes through those points
            //
            
            ofSetHexColor(0x2bdbe6);
            ofBeginShape();
            //runs through all the lines in the file:
            for(int i=0; i<linesOfTheFile.size(); i++){
                //we split it into a vector
                splitItems = ofSplitString(linesOfTheFile[i], ",");
                
                for (int y=0; y<5; y++) {
                    if(y < 4){
                        ofCurveVertex( ofMap( ofToFloat(splitItems[y]),0,1,0, wSize), ofMap( ofToFloat(splitItems[y+1]),0,1,50, hSize) );
                    }else{
                        ofCurveVertex( ofMap( ofToFloat(splitItems[y]),0,1,0, wSize), ofMap( ofToFloat(splitItems[0]),0,1,50, hSize) );
                    }
                }
                
            }//end of for loop for i
            ofEndShape();
        } else {
            //------(d)--------------------------------------
            //
            //         poylgon of eeg points ()
            //
            //         lots of self intersection, 500 pts is a good stress test
            //
            //
            ofSetPolyMode(OF_POLY_WINDING_ODD);
            
            ofEnableAlphaBlending();
            int opasity = 50;
            //runs through all the lines in the file:
            for(int i=0; i<linesOfTheFile.size(); i++){
                
                //we split it into a vector
                splitItems = ofSplitString(linesOfTheFile[i], ",");
                int randomC = ofToInt(splitItems[5]);
                ofSetColor(randomC, 226, 119, opasity);
                
                ofBeginShape();
                for (int y=0; y<5; y++) {
                    if(y < 4){
                        ofVertex( ofMap( ofToFloat(splitItems[y]),0,1,0, wSize), ofMap( ofToFloat(splitItems[y+1]),0,1,50, hSize) );
                    }else{
                        ofVertex( ofMap( ofToFloat(splitItems[y]),0,1,0, wSize), ofMap( ofToFloat(splitItems[0]),0,1,50, hSize) );
                    }
                }ofEndShape();
                
                opasity += 10;
            }//end of for loop for i
            ofDisableAlphaBlending();
        }
        
        ofNoFill();
        ofSetColor(255, 255, 255);
        ofSetLineWidth(2);
        linePoints[0].set( ofMap( ofToFloat(splitItems[0]),0,1,0, wSize), ofMap( ofToFloat(splitItems[1]),0,1,50, hSize) );
        linePoints[1].set( ofMap( ofToFloat(splitItems[1]),0,1,0, wSize), ofMap( ofToFloat(splitItems[2]),0,1,50, hSize) );
        linePoints[2].set( ofMap( ofToFloat(splitItems[2]),0,1,0, wSize), ofMap( ofToFloat(splitItems[3]),0,1,50, hSize) );
        linePoints[3].set( ofMap( ofToFloat(splitItems[3]),0,1,0, wSize), ofMap( ofToFloat(splitItems[4]),0,1,50, hSize) );
        linePoints[4].set( ofMap( ofToFloat(splitItems[4]),0,1,0, wSize), ofMap( ofToFloat(splitItems[0]),0,1,50, hSize) );
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, sizeof(ofVec2f), &linePoints[0].x);
        glDrawArrays(GL_POLYGON, 0, 5);
        
        
        ofDrawBitmapString("delta", ofMap( ofToFloat(splitItems[0]),0,1,0, wSize), ofMap( ofToFloat(splitItems[1]),0,1,0, hSize) );
        ofDrawBitmapString("theta", ofMap( ofToFloat(splitItems[1]),0,1,0, wSize), ofMap( ofToFloat(splitItems[2]),0,1,0, hSize) );
        ofDrawBitmapString("alpha", ofMap( ofToFloat(splitItems[2]),0,1,0, wSize), ofMap( ofToFloat(splitItems[3]),0,1,0, hSize) );
        ofDrawBitmapString("beta", ofMap( ofToFloat(splitItems[3]),0,1,0, wSize), ofMap( ofToFloat(splitItems[4]),0,1,0, hSize) );
        ofDrawBitmapString("gamma", ofMap( ofToFloat(splitItems[4]),0,1,0, wSize), ofMap( ofToFloat(splitItems[0]),0,1,0, hSize));
        
    }//end of path
    
    //------------------ YOUR CURRENT DATA ------------------//
    ofFill();
    ofSetColor(150, 150, 150);
    float recMinW = (wSize-wSize/4)-20, recMaxW = wSize/4, recMinH = 30, recMaxH = (hSize/4)+30, recMaxWposition = wSize-20;
    //x,y (upper right hand corner), width, height
    ofDrawRectangle(recMinW, recMinH, wSize/4, hSize/4);
    // mockup data:
    ofSetColor(241, 150, 150);
    //float ofMap(float value, float inputMin, float inputMax, float outputMin, float outputMax)
    linePoints[0].set(ofMap(delta,0,1,recMinW,recMaxWposition),ofMap(theta,0,1,recMinH,recMaxH));
    linePoints[1].set(ofMap(theta,0,1,recMinW,recMaxWposition),ofMap(alpha,0,1,recMinH,recMaxH));
    linePoints[2].set(ofMap(alpha,0,1,recMinW,recMaxWposition),ofMap(beta,0,1,recMinH,recMaxH));
    linePoints[3].set(ofMap(beta,0,1,recMinW,recMaxWposition),ofMap(gamma,0,1,recMinH,recMaxH));
    linePoints[4].set(ofMap(gamma,0,1,recMinW,recMaxWposition),ofMap(delta,0,1,recMinH,recMaxH));
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, sizeof(ofVec2f), &linePoints[0].x);
    glDrawArrays(GL_POLYGON, 0, 5);
    
    //------------------ DEBUGGING TEXTS (LEFT CORNER & RIGHT BOX) ------------------//
    
    //and the labels & colour for debug text:
    ofSetColor(255,255,255);
    if(receiving){
        int pointR = 2;
        ofDrawCircle(ofMap(delta,0,1,recMinW,recMaxWposition),ofMap(theta,0,1,recMinH,recMaxH), pointR);
        ofDrawCircle(ofMap(theta,0,1,recMinW,recMaxWposition),ofMap(alpha,0,1,recMinH,recMaxH), pointR);
        ofDrawCircle(ofMap(alpha,0,1,recMinW,recMaxWposition),ofMap(beta,0,1,recMinH,recMaxH), pointR);
        ofDrawCircle(ofMap(beta,0,1,recMinW,recMaxWposition),ofMap(gamma,0,1,recMinH,recMaxH), pointR);
        ofDrawCircle(ofMap(gamma,0,1,recMinW,recMaxWposition),ofMap(delta,0,1,recMinH,recMaxH), pointR);
        ofDrawBitmapString("delta", ofMap(delta,0,1,recMinW,recMaxWposition),ofMap(theta,0,1,recMinH,recMaxH));
        ofDrawBitmapString("theta", ofMap(theta,0,1,recMinW,recMaxWposition),ofMap(alpha,0,1,recMinH,recMaxH));
        ofDrawBitmapString("alpha", ofMap(alpha,0,1,recMinW,recMaxWposition),ofMap(beta,0,1,recMinH,recMaxH));
        ofDrawBitmapString("beta", ofMap(beta,0,1,recMinW,recMaxWposition),ofMap(gamma,0,1,recMinH,recMaxH));
        ofDrawBitmapString("gamma", ofMap(gamma,0,1,recMinW,recMaxWposition),ofMap(delta,0,1,recMinH,recMaxH));
    }
    
    if(!receiving){
        ofDrawBitmapString("HEADBAND NOT CONNECTED!", wSize-( (wSize/4)-10 ), (hSize/4)/2 );
    }
    ofDrawBitmapString("Your current data:", ofGetWidth()-(ofGetWidth()/4)+5, 20);
    ofDrawBitmapStringHighlight("Headband battery at: " + ofToString(batteryPercentage) + "%", 50, 20);
    ofDrawBitmapStringHighlight("leftEar: " + ofToString(leftEar) + ", leftForehead: " + ofToString(leftForehead) + ", rightForehead: " + ofToString(rightForehead) + ", rightEar: " + ofToString(rightEar), 50, 35);
    ofDrawBitmapStringHighlight("delta: " + ofToString(delta) + ", theta: " + ofToString(theta) + ", alpha: " + ofToString(alpha) + ", beta: " + ofToString(beta) + + ", gamma: " + ofToString(gamma), 50, 50);
    ofDrawBitmapStringHighlight("Mock data: " + ofToString(mockdata), 50, 65);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    /*with s key press save to file the current data points:
    if(key == 's'){
        if(linesOfTheFile.size() > 9){
            linesOfTheFile.erase(linesOfTheFile.begin());
        }
        //delta, theta, alpha, beta, gamma, offset, randomColor, vertexOfShape, signalGood
        linesOfTheFile.push_back(ofToString(delta) + "," + ofToString(theta) + "," + ofToString(alpha) + "," + ofToString(beta) + "," + ofToString(gamma) + "," + ofToString(ofRandom(255)) + ", signal: "+ ofToString(signalGood));
    }*/
    if(key == 'c'){
        linesOfTheFile.clear();
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
    if(linesOfTheFile.size() > 9){
        linesOfTheFile.erase(linesOfTheFile.begin());
    }
    //delta, theta, alpha, beta, gamma, offset, randomColor, vertexOfShape, signalGood
    linesOfTheFile.push_back(ofToString(delta) + "," + ofToString(theta) + "," + ofToString(alpha) + "," + ofToString(beta) + "," + ofToString(gamma) + "," + ofToString(ofRandom(255)) + ", signal: "+ ofToString(signalGood));
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
void ofApp::exit(){
    dataFile.close();
}
