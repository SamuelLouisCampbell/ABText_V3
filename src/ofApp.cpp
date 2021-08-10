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
	rasterSize.setSize(7680.0f, 4320.0f);
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
	ABfont.load("ABF.ttf", 256, true, true, true);

	//Shader
	outlineShader.setGeometryInputType(GL_LINES);
	outlineShader.setGeometryOutputType(GL_TRIANGLE_STRIP);
	outlineShader.setGeometryOutputCount(6);
	outlineShader.load("shaders/vert.glsl", "shaders/frag.glsl", "shaders/geom.glsl");

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
	cleanFBO.allocate(rasterSize.getWidth(), rasterSize.getHeight(), GL_RGBA);
	outlineFBO.allocate(rasterSize.getWidth(), rasterSize.getHeight(), GL_RGBA);

	//Draw Text etc.
	float lineHeight = ABfont.getLineHeight();
	if (temp.size() > 0)
	{
		//FBO clean
		cleanFBO.begin();
		ofClear(0, 0, 0, 0);

		alpha = 255;
		ofSetColor(255, alpha);
		oldMessage = temp;
		StringHandling sh = { temp, currFontBreak };
		float totalHeight = sh.GetStringies().size() * lineHeight;
		float Y_Start = center_y - (totalHeight / 2.0f) + lineHeight - curr_y_off;
		
		//clear out previous font locations
		fontLocs.clear();

		for (auto& string : sh.GetStringies())
		{
			float X_start = ABfont.getStringBoundingBox(wideToString(string), 0.0f, 0.0f).getWidth();
			fontLocs.push_back({ center_x - (X_start / 2.0f), Y_Start });
			ofSetColor(0, 128, 0);
			ABfont.drawString(wideToString(string), center_x - (X_start / 2.0f), Y_Start);
			Y_Start += lineHeight;
		}

		//Finish clean things into Buffer.
		cleanFBO.end();
		//FBO outline
		
		outlineFBO.begin();
		ofClear(0, 0, 0, 0);

		//Shader
		outlineShader.begin();
		outlineShader.setUniform4f("colorIn", 1.0f, 1.0f, 1.0f, 1.0f);
		// set thickness of ribbons
		outlineShader.setUniform1f("thickness", 4.0f);
		//ofTranslate(fontLocs[0].first, fontLocs[0].second);
		for (int i = 0; i < sh.GetStringies().size(); i++)
		{
			fontPaths = ABfont.getStringAsPoints(wideToString(sh.GetStringies()[i]));
			for (int j = 0; j < fontPaths.size(); j++)
			{
				fontPaths[j].setStrokeWidth(1.0f);
				fontPaths[j].draw(fontLocs[i].first, fontLocs[i].second);
			}

		}
		outlineShader.end();
		outlineFBO.end();
		holdingLastMsg = false;
	}
	else if (temp.size() == 0)
	{
		ofSetColor(255, alpha);
		holdingLastMsg = true;
		StringHandling sh = { oldMessage, currFontBreak };
		float totalHeight = sh.GetStringies().size() * lineHeight;
		float Y_Start = center_y - (totalHeight / 2.0f) - (lineHeight / 2.0f) + curr_y_off;
		
		for (auto& string : sh.GetStringies())
		{
			//pre calc size of text
			float X_start = ABfont.getStringBoundingBox(wideToString(string), 0.0f, 0.0f).getWidth();
			Y_Start += lineHeight;
			ABfont.drawString(wideToString(string), center_x - (X_start / 2.0f), Y_Start);
		}
		outlineShader.end();
		alpha -= alphaTime;
	}

	//Send SPOUT 
	sendClean.send(cleanFBO.getTexture());
	sendOutline.send(outlineFBO.getTexture());

	//draw things to our screen
	ofSetColor(255);
	//shader.begin();
	//ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	outlineFBO.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());

	//shader.end();
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
		//currFontSize = largeFontSize;
		//currFontBreak = largeFontBreak;
		curr_y_off = large_y_off;
	}

	else if (ctrlStr == L"SMALL...")
	{
		//currFontSize = smallFontSize;
		//currFontBreak = smallFontBreak;
		curr_y_off = small_y_off;
	}
}