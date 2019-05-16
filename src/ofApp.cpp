#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(2); //framerate
    ofBackground(0,0,0); //black background
    
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
    cout << tick << endl;
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
        offset = ofRandom(-1, 1)*ofRandom(wSize);
        randomColor = ofRandom(255);
        vertexOfShape = ofRandom(0,4);
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
    
    //for debugging (how many lines of data in file):
    //cout << linesOfTheFile.size() << endl;
    
    ofEnableAlphaBlending();    // turn on alpha blending
    int opacity = 50;
    float vertexForNext = 0.0,
          x1  = 0.0, y1 = 0.0,
          x2 = 0.0, y2 = 0.0,
          x3 = 0.0, y3 = 0.0,
          x4 = 0.0, y4 = 0.0,
          x5 = 0.0, y5 = 0.0;
    float wMapped = wSize/4, hMapped = hSize/4;
    if(linesOfTheFile.size()>1 && linesOfTheFile[0] != ""){
        for(int i=0; i<linesOfTheFile.size(); i++){
            vector <string> splitItems = ofSplitString(linesOfTheFile[i], ",");
            int offset2 = ofToFloat(splitItems[5]);
            int randomC = ofToInt(splitItems[6]);
            
            //with solid color
            //ofSetColor(241-i, 226-i*2, 119-i*2, color/1.05);
            ofSetColor(randomC, 226, 119);
            
            // Drawing sharp shapes:
            if(i != 0){
                x1 = ofMap( vertexForNext,0,1,0, wMapped );
                y1 = ofMap( ofToFloat(splitItems[1]),0,1,0, hMapped );
                x2 = ofMap( ofToFloat(splitItems[1]),0,1,0, wMapped );
                y2 = ofMap( ofToFloat(splitItems[2]),0,1,0, hMapped );
                x3 = ofMap( ofToFloat(splitItems[2]),0,1,0, wMapped );
                y3 = ofMap( ofToFloat(splitItems[3]),0,1,0, hMapped );
                x4 = ofMap( ofToFloat(splitItems[3]),0,1,0, wMapped );
                y4 = ofMap( ofToFloat(splitItems[4]),0,1,0, hMapped );
                x5 = ofMap( ofToFloat(splitItems[4]),0,1,0, wMapped );
                y5 = ofMap( vertexForNext,0,1,0, hMapped );
            } else {
                x1 = ofMap( ofToFloat(splitItems[0]),0,1,0, wMapped );
                y1 = ofMap( ofToFloat(splitItems[1]),0,1,0, hMapped );
                x2 = ofMap( ofToFloat(splitItems[1]),0,1,0, wMapped );
                y2 = ofMap( ofToFloat(splitItems[2]),0,1,0, hMapped );
                x3 = ofMap( ofToFloat(splitItems[2]),0,1,0, wMapped );
                y3 = ofMap( ofToFloat(splitItems[3]),0,1,0, hMapped );
                x4 = ofMap( ofToFloat(splitItems[3]),0,1,0, wMapped );
                y4 = ofMap( ofToFloat(splitItems[4]),0,1,0, hMapped );
                x5 = ofMap( ofToFloat(splitItems[4]),0,1,0, wMapped );
                y5 = ofMap( ofToFloat(splitItems[0]),0,1,0, hMapped );
                
                //the random vertex point (0-4) is stored in 7th value of the split string and we get that value for next round:
                vertexForNext = ofToFloat(splitItems[ ofToFloat(splitItems[7]) ]);
            }
            //setting the x,y points
            linePoints[0].set( x1, y1 );
            linePoints[1].set( x2, y2 );
            linePoints[2].set( x3, y3 );
            linePoints[3].set( x4, y4 );
            linePoints[4].set( x5, y5 );
            
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2, GL_FLOAT, sizeof(ofVec2f), &linePoints[0].x);
            glDrawArrays(GL_POLYGON, 0, 5);
            
            /* Last shape that's the last one added to file or vector
            if(!linesOfTheFile.empty() && i == linesOfTheFile.size()-1){
                //with stroke
                ofNoFill();
                ofSetColor(255, 255, 255);
                ofSetLineWidth(2);
                linePoints[0].set(ofMap( ofToFloat(splitItems[0]),0,1,0,hMapped) + offset2,ofMap( ofToFloat(splitItems[1]),0,1,0,hMapped) + offset2);
                linePoints[1].set(ofMap( ofToFloat(splitItems[1]),0,1,0,hMapped) + offset2,ofMap( ofToFloat(splitItems[2]),0,1,0,hMapped) + offset2);
                linePoints[2].set(ofMap( ofToFloat(splitItems[2]),0,1,0,hMapped) + offset2,ofMap( ofToFloat(splitItems[3]),0,1,0,hMapped) + offset2);
                linePoints[3].set(ofMap( ofToFloat(splitItems[3]),0,1,0,hMapped) + offset2,ofMap( ofToFloat(splitItems[4]),0,1,0,hMapped) + offset2);
                linePoints[4].set(ofMap( ofToFloat(splitItems[4]),0,1,0,hMapped) + offset2,ofMap( ofToFloat(splitItems[0]),0,1,0,hMapped) + offset2);
                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(2, GL_FLOAT, sizeof(ofVec2f), &linePoints[0].x);
                glDrawArrays(GL_POLYGON, 0, 5);
                
                
                ofDrawBitmapString("delta", ofMap( ofToFloat(splitItems[0]),0,1,0,1024) + offset2,ofMap( ofToFloat(splitItems[1]),0,1,0,768) + offset2);
                ofDrawBitmapString("theta", ofMap( ofToFloat(splitItems[1]),0,1,0,1024) + offset2,ofMap( ofToFloat(splitItems[2]),0,1,0,768) + offset2);
                ofDrawBitmapString("alpha", ofMap( ofToFloat(splitItems[2]),0,1,0,1024) + offset2,ofMap( ofToFloat(splitItems[3]),0,1,0,768) + offset2);
                ofDrawBitmapString("beta", ofMap( ofToFloat(splitItems[3]),0,1,0,1024) + offset2,ofMap( ofToFloat(splitItems[4]),0,1,0,768) + offset2);
                ofDrawBitmapString("gamma", ofMap( ofToFloat(splitItems[4]),0,1,0,1024) + offset2,ofMap( ofToFloat(splitItems[0]),0,1,0,768) + offset2);
            }*/
            
            opacity = opacity+3;
        }
    }
    
    ofDisableAlphaBlending();   // turn alphaBlending off when not used
    
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
    ofDrawBitmapString("Headband battery at: " + ofToString(batteryPercentage) + "%", 50, 20);
    ofDrawBitmapString("leftEar: " + ofToString(leftEar) + ", leftForehead: " + ofToString(leftForehead) + ", rightForehead: " + ofToString(rightForehead) + ", rightEar: " + ofToString(rightEar), 50, 35);
    ofDrawBitmapString("delta: " + ofToString(delta) + ", theta: " + ofToString(theta) + ", alpha: " + ofToString(alpha) + ", beta: " + ofToString(beta) + + ", gamma: " + ofToString(gamma), 50, 50);
    ofDrawBitmapString("Mock data: " + ofToString(mockdata), 50, 65);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //with s key press save to file the current data points:
    if(key == 's'){
        if(linesOfTheFile.size() > 49){
            linesOfTheFile.erase(linesOfTheFile.begin());
        }
        //delta, theta, alpha, beta, gamma, offset, randomColor, vertexOfShape, signalGood
        linesOfTheFile.push_back(ofToString(delta) + "," + ofToString(theta) + "," + ofToString(alpha) + "," + ofToString(beta) + "," + ofToString(gamma) + "," + ofToString(offset) + "," + ofToString(randomColor) + "," + ofToString(vertexOfShape) + ", signal: "+ ofToString(signalGood));
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
void ofApp::exit(){
    dataFile.close();
}
