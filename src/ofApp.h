#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

#define SEND_PORT 5000

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	
    /* Muse program variables */
    // muse is running in terminal with: muse-io --device Muse-623E --osc osc.udp://localhost:5000
    ofxOscReceiver receiver;
    
    //from: https://github.com/trentbrooks/EEGWorkshop/blob/master/03_muse_osc/src/ofApp.h
    // 5 eeg bands
    float delta;
    float theta;
    float alpha;
    float beta;
    float gamma;
    
    // helper methods to get individual channel data
    float getAverageFromChannels(ofxOscMessage& msg); //this is mostly used.
    float getHighestFromChannels(ofxOscMessage& msg);
    float getFromEarChannels(ofxOscMessage& msg);
    float getFromForeheadChannels(ofxOscMessage& msg);
    
    //blink data
    float blink = 0;
    
    //debugging variables:
    bool signalGood = false;
    float batteryPercentage = 0.0;
    float leftEar = 0, leftForehead = 0, rightForehead = 0, rightEar = 0;
    bool mockdata = false;
    bool receiving = false;
    
    //screen sizes:
    float wSize = ofGetWidth();
    float hSize = ofGetHeight();
    
    //class for storing a two dimensional vector
    ofVec2f linePoints [5];
    
    //for saving data:
    ofFile dataFile;
    vector < string > linesOfTheFile;
    
    //timer for saving file:
    int tick = 0;
};
