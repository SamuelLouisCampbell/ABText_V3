#pragma once

#include "ofMain.h"
#include "TCPNet.h"
#include "ofxSpout.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();	
		void exit();

		//Utility
		void DrawCenterCross();
		std::string wideToString(const std::wstring wStr);
		void UpdateTime();
		ofFpsCounter fps;
		float accuTime = 0.0f;
		int accuFrame = 0;

		//Server
		void RunHealthCheck();
		std::unique_ptr<CustomServer> server;
		int netLooper = 60;
		static constexpr int bufferSize = 1024;

		//SPOUT
		ofxSpout::Sender sendClean;
		ofxSpout::Sender sendOutline;

		//FBO stuff
		ofFbo bufferFBO;
		float screenRatio;
		ofPixels pixels;

		//shaders
		ofShader outlineShader;

		//Font
		ofTrueTypeFont ABfont;
};
