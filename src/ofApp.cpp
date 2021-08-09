#include "ofApp.h"
#include "StringHandling.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//OF Things
	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	//ofDisableAntiAliasing();
	ofSetBackgroundColor(0.0f);
	ofSetLogLevel(OF_LOG_VERBOSE);

	//Spout init
	sendClean.init("A&B Text Clean");
	sendOutline.init("A&B Text Outline");

	//Font Stuff


	//server things
	server = std::make_unique<CustomServer>(6000);
	server->Start();
	std::cout << "Server Started on Default Port: 6000" << std::endl;

	//Font
	ABfont.load("ABF.ttf", 192, true, true, true);

}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofSetColor(255);
	ABfont.drawString("HELLO!",100.0f, 100.0f);
	DrawCenterCross();
}

void ofApp::exit()
{
	server->Stop();
	server.release();
}

void ofApp::DrawCenterCross()
{
	ofSetColor(255, 0, 0, 255);
	float width = ofGetWindowWidth();
	float hw = width / 2.0f;
	float height = ofGetWindowHeight();
	float hh = height / 2.0f;
	ofDrawLine(hw, 0, hw, height);
	ofDrawLine(0, hh, width, hh);
}

std::string ofApp::wideToString(const std::wstring wStr)
{
	//Use WCSTOMBS to transfer into std::string
	char buffer[bufferSize];
	int ret;
	ret = wcstombs(buffer, wStr.c_str(), sizeof(buffer));
	if (ret == bufferSize) buffer[bufferSize - 1] = '\0';
	return buffer;
}

void ofApp::UpdateTime()
{
	accuFrame++;
	fps.update();
	fps.newFrame();
	accuTime += fps.getFps();
	std::stringstream ss;
	if (accuFrame >= 30)
	{
		accuTime /= accuFrame;
		ss << std::setprecision(3) << accuTime;
		ofSetWindowTitle("A&B Text Render. FPS : " + ss.str());
		accuTime = 0.0f;
		accuFrame = 0;
	}

}

void ofApp::RunHealthCheck()
{
	//send healthcheck to clients
	if (netLooper >= 60)
	{
		netLooper = 0;
		server->CheckClientsHealth();
		//Purge any messages in the server
		server->EraseMessageQ();
	}
	netLooper++;
}

