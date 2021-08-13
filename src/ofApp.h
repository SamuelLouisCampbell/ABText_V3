#pragma once

#include "ofMain.h"
#include "TCPNet.h"
#include "ofxSpout.h"
#include "ofxImGui.h"

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
		int port = 6000;

		//SPOUT
		ofxSpout::Sender sendClean;
		ofxSpout::Sender sendOutline;

		//FBO stuff
		ofFbo cleanFBO;
		ofFbo outlineFBO;
		ofFbo cleanFBO_Fade;
		ofFbo outlineFBO_Fade;

		//shaders
		ofShader outlineShader;
		std::vector<ofPath> fontPaths;
		std::vector<std::pair<float, float>> fontLocs;

		//Font & Text
		std::unique_ptr<ofTrueTypeFont> ABfontLarge;
		std::unique_ptr<ofTrueTypeFont> ABfontSmall;
		bool SizeControl(const std::wstring& ctrlStr);

		//GUI
		ofxImGui::Gui gui;
		bool drawCross = false;
		bool demoText = false;
		bool updateText = false;

		
		ofRectangle rasterSize;
		bool currLarge = false;
		int largeFontSize = 360;
		int smallFontSize = 128;
		size_t smallFontBreak = 60;
		size_t largeFontBreak = 12;
		float currFontSize = smallFontSize;
		size_t currFontBreak = smallFontBreak;
		float center_x;
		float center_y;
		float small_y_off = 40.0F;
		float large_y_off = 115.0f;
		float borderWidthSmall = 2.0f;
		float borderWidthLarge = 4.0f;

		ofColor oldFillCol;
		bool currSmall = true;
		std::wstring oldMessage = L"";
		bool holdingLastMsg = false;
		int alpha = 255;
		int alphaTime = 6;
};
