#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

#define SEND_PORT 5000

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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
    //just need a receiver to read osc messages from port that muse is sending to
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
    
    // processed values (these are pretty much the same as the brainband's esense values)
    bool signalGood = false;
    
    //an array with 2 points in it
    ofVec2f linePoints [5];
};
