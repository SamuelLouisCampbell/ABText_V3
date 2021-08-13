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

	//TODO Load this shit from file;
	//Font Stuff
	rasterSize.setSize(3840, 1620);
	center_x = rasterSize.getWidth() / 2.0f;
	center_y = rasterSize.getHeight() / 2.0f;
#
	//Spout init
	sendClean.init("A&B Text Clean", rasterSize.getWidth(), rasterSize.getHeight());
	sendOutline.init("A&B Text Outline", rasterSize.getWidth(), rasterSize.getHeight());

	//server things
	server = std::make_unique<CustomServer>(6000);
	server->Start();
	std::cout << "Server Started on Default Port: 6000" << std::endl;

	//Font
	ABfontLarge.load("ABF.ttf", largeFontSize, true, true, true);
	ABfontSmall.load("ABF.ttf", smallFontSize, true, true, true);

	//Shader
	outlineShader.setGeometryInputType(GL_LINES);
	outlineShader.setGeometryOutputType(GL_TRIANGLE_STRIP);
	outlineShader.setGeometryOutputCount(6);
	outlineShader.load("shaders/vert.glsl", "shaders/frag.glsl", "shaders/geom.glsl");

	//Allocate Storage for FBO
	cleanFBO.allocate(rasterSize.getWidth(), rasterSize.getHeight(), GL_RGBA);
	outlineFBO.allocate(rasterSize.getWidth(), rasterSize.getHeight(), GL_RGBA);
	cleanFBO_Fade.allocate(rasterSize.getWidth(), rasterSize.getHeight(), GL_RGBA);
	outlineFBO_Fade.allocate(rasterSize.getWidth(), rasterSize.getHeight(), GL_RGBA);
}

//--------------------------------------------------------------
void ofApp::update()
{
	server->Update(-1, false);
	RunHealthCheck();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	UpdateTime();

	//Get string from server
	std::wstring temp = server->GetMessageStream();
	//std::wstring temp = L"NULL....Hello You";
	//parse out Control string and erase from message
	std::wstring controlString = temp.substr(0, 8);
	temp.erase(0, 8);

	if (controlString != L"NULL....")
	{
		SizeControl(controlString);
	}
	//cleanFBO.allocate(rasterSize.getWidth(), rasterSize.getHeight(), GL_RGBA);
	//outlineFBO.allocate(rasterSize.getWidth(), rasterSize.getHeight(), GL_RGBA);

	//Draw Text etc.
	float lineHeight = !currLarge ? ABfontSmall.getLineHeight() : ABfontLarge.getLineHeight();
	if (temp.size() > 0)
	{
		oldMessage = temp;
		alpha = 255;
		//FBO clean
		cleanFBO.begin();
		ofClear(0);

		ofSetColor(255, alpha);
		StringHandling sh = { temp, currFontBreak };
		float totalHeight = sh.GetStringies().size() * lineHeight;
		float Y_Start = center_y - (totalHeight / 2.0f) + lineHeight - curr_y_off;
		
		//clear out previous font locations
		fontLocs.clear();

		for (auto& string : sh.GetStringies())
		{
			float X_start = !currLarge ?
				ABfontSmall.getStringBoundingBox(wideToString(string), 0.0f, 0.0f).getWidth()
				:
				ABfontLarge.getStringBoundingBox(wideToString(string), 0.0f, 0.0f).getWidth();

			fontLocs.push_back({ center_x - (X_start / 2.0f), Y_Start });
			ofSetColor(0, 0, 0, alpha);
			!currLarge ?
				ABfontSmall.drawString(wideToString(string), center_x - (X_start / 2.0f), Y_Start)
				:
				ABfontLarge.drawString(wideToString(string), center_x - (X_start / 2.0f), Y_Start);
			Y_Start += lineHeight;
		}

		//Finish clean things into Buffer.
		cleanFBO.end();
		//FBO outline
		outlineFBO.begin();
		ofClear(0);
		
		//Shader
		outlineShader.begin();
		outlineShader.setUniform4f("colorIn", 1.0f, 1.0f, 1.0f, 1.0f);
		// set thickness of ribbons
		outlineShader.setUniform1f("thickness", borderWidth);
		//ofTranslate(fontLocs[0].first, fontLocs[0].second);
		for (int i = 0; i < sh.GetStringies().size(); i++)
		{
			fontPaths = !currLarge ?
				ABfontSmall.getStringAsPoints(wideToString(sh.GetStringies()[i]))
				:
				ABfontLarge.getStringAsPoints(wideToString(sh.GetStringies()[i]));
			for (int j = 0; j < fontPaths.size(); j++)
			{
				fontPaths[j].setStrokeWidth(1.0f);
				fontPaths[j].draw(fontLocs[i].first, fontLocs[i].second);
			}

		}
		outlineShader.end();
		outlineFBO.end();
		holdingLastMsg = false;
		//Send SPOUT 
		sendClean.send(cleanFBO.getTexture());
		sendOutline.send(outlineFBO.getTexture());
	}
	else if (temp.size() == 0)
	{
		//Clean FBO;
		cleanFBO_Fade.begin();
		ofClear(0);
		ofSetColor(255, alpha);
		cleanFBO.draw(0,0, rasterSize.getWidth(), rasterSize.getHeight());
		cleanFBO_Fade.end();

		//Outline FBO;
		outlineFBO_Fade.begin();
		ofClear(0);
		ofSetColor(255, alpha);
		outlineFBO.draw(0, 0, rasterSize.getWidth(), rasterSize.getHeight());
		outlineFBO_Fade.end();

		//Send SPOUT 
		sendClean.send(cleanFBO_Fade.getTexture());
		sendOutline.send(outlineFBO_Fade.getTexture());

		holdingLastMsg = true;
		alpha -= alphaTime;
	}

	//draw things to our screen
	ofSetColor(255);
	holdingLastMsg ? outlineFBO_Fade.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight()) 
		:
		outlineFBO.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
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

void ofApp::SizeControl(const std::wstring& ctrlStr)
{
	if (ctrlStr == L"LARGE...")
	{
		currLarge = true;
		curr_y_off = large_y_off;
	}

	else if (ctrlStr == L"SMALL...")
	{
		currLarge = false;
		curr_y_off = small_y_off;
	}
}