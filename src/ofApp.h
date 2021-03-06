#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxSVG.h"
#include "ofxGui.h"



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
		
    
    void setupLayers();
    
    ofxCv::ContourFinder finder;
    
    vector < vector < ofPolyline > > lines;
    ofImage img;
    ofImage img2;
    
    vector < vector < ofMesh > > meshes;
    
    
    ofShader gradientShader;
    
    ofImage spot;
    
    ofxSVG svg;
    vector < ofPolyline > svgLines;
    
    
    float minPerim;
    float maxPerim;
    
    
    ofParameterGroup group;
    ofxPanel panel;
    
    float myTime;
    
};
