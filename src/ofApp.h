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

		//shaders
		ofShader outlineShader;

		//Font & Text
		ofTrueTypeFont ABfont;
		void SizeControl(const std::wstring& ctrlStr);

		ofRectangle rasterSize;
		float smallFontSize = 256.0f;
		float largeFontSize = 256.0f;
		size_t smallFontBreak = 60;
		size_t largeFontBreak = 12;
		float currFontSize = smallFontSize;
		size_t currFontBreak = smallFontBreak;
		float center_x;
		float center_y;
		float small_y_off = 22.0f;
		float large_y_off = 80.0f;
		float curr_y_off = small_y_off;
		float borderWidth = 2.0f;

		ofColor oldFillCol;
		bool currSmall = true;
		std::wstring oldMessage = L"";
		bool holdingLastMsg = false;
		int alpha = 255;
		int alphaTime = 6;
};
