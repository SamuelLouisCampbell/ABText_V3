#include "ofApp.h"
#include "StringHandling.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//OF Things
	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	ofSetBackgroundColor(0.0f);


	//Font Stuff
	rasterSize.setSize(3840, 1620);
	center_x = rasterSize.getWidth() / 2.0f;
	center_y = rasterSize.getHeight() / 2.0f;
#
	//Spout init
	sendClean.init("A&B Text Clean", rasterSize.getWidth(), rasterSize.getHeight());
	sendOutline.init("A&B Text Outline", rasterSize.getWidth(), rasterSize.getHeight());

	////server things
	mongoose.init("127.0.0.1", 8000);

	//GUI
	gui.setup();

	//Font
	ABfontLarge = std::make_unique<ofTrueTypeFont>();
	ABfontSmall = std::make_unique<ofTrueTypeFont>();
	ABfontLarge->load("AGS.otf", largeFontSize, true, true, true);
	ABfontSmall->load("AGS.otf", smallFontSize, true, true, true);

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
	mongoose.update(16);
	//server->Update(-1, false);
	//RunHealthCheck();

	//update Terminal
	std::string termTemp = mongoose.getStream(); "";//server->GetInfoStream();
	if (termTemp != oldTermMessage)
	{
		terminalEntries.push_back(termTemp);
		oldTermMessage = termTemp;
	}
	//clear terminal entries to prevent memory leak
	if (terminalEntries.size() > 1024)
		terminalEntries.clear();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	//Gui things
	gui.begin();
	ImGui::Begin("A&B Renderer Controls");
	if (ImGui::BeginChild("A&B Renderer Controls"))
	{
		if (ImGui::InputInt("!!! Large Font Size !!!", &largeFontSize))
		{
			ABfontLarge.release();
			ABfontLarge = std::make_unique<ofTrueTypeFont>();
			ABfontLarge->load("ABF.ttf", largeFontSize, true, true, true);
		}
		if (ImGui::InputInt("!!! Small Font Size !!!", &smallFontSize))
		{
			ABfontSmall.release();
			ABfontSmall = std::make_unique<ofTrueTypeFont>();
			ABfontSmall->load("ABF.ttf", smallFontSize, true, true, true);
		}
		ImGui::InputInt("Large Line Break Number", &largeFontBreak);
		ImGui::InputInt("Small Line Break Number", &smallFontBreak);
		ImGui::InputFloat("Large Outline Stroke", &borderWidthLarge, 0.01f, 0.1f);
		ImGui::InputFloat("Small Outline Stroke", &borderWidthSmall, 0.01f, 0.1f);
		ImGui::InputFloat("Large Y Offset", &large_y_off, 0.1f, 1.0f);
		ImGui::InputFloat("small Y Offset", &small_y_off, 0.1f, 1.0f);
		ImGui::Checkbox("Draw Center Cross", &drawCross);
		ImGui::Checkbox("Draw Demo Text", &demoText);
		if (ImGui::Button("Toggle Large Text"))
		{
			updateText = true;
			currLarge = !currLarge;
		}
		ImGui::ListBoxHeader("", { 390, 195 });
		for (auto& item : terminalEntries)
		{
			ImGui::TextColored({ 0.0f, 1.0f, 0.6f, 1.0f }, item.c_str());
			ImGui::SetScrollHere();
		}
		ImGui::ListBoxFooter();

		ImGui::EndChild();
	}
	ImGui::End();
	gui.end();


	UpdateTime();
	std::string temp;
	//Get string from server
	if (!demoText)
	{
		//temp = wideToString(server->GetMessageStream());
		temp = mongoose.getStream();
		mongoose.getStream().clear();
	}
	else
	{
		temp = "NULL....Above & Beyond";
	}
	//parse out Control string and erase from message
	std::string controlString = temp.substr(0, 8);
	temp.erase(0, 8);

	if (controlString != "NULL....")
	{
		 updateText = SizeControl(controlString);
	}
	
	//Draw Text etc.
	float lineHeight = !currLarge ? ABfontSmall->getLineHeight() : ABfontLarge->getLineHeight();
	if (temp != oldMessage || updateText)
	{
		if (temp.size() > 0)
		{
			oldMessage = temp;
			alpha = 255;
			//FBO clean
			cleanFBO.begin();
			ofClear(0);

			ofSetColor(255, alpha);
			StringHandling sh = { temp, size_t(!currLarge ? smallFontBreak : largeFontBreak) };
			float totalHeight = sh.GetStringies().size() * lineHeight;
			float Y_Start = center_y - (totalHeight / 2.0f) + lineHeight - (!currLarge ? small_y_off : large_y_off);

			//clear out previous font locations
			fontLocs.clear();

			for (auto& string : sh.GetStringies())
			{
				float X_start = !currLarge ?
					ABfontSmall->getStringBoundingBox(string, 0.0f, 0.0f).getWidth()
					:
					ABfontLarge->getStringBoundingBox(string, 0.0f, 0.0f).getWidth();

				fontLocs.push_back({ center_x - (X_start / 2.0f), Y_Start });
				ofSetColor(0, 0, 0, alpha);
				!currLarge ?
					ABfontSmall->drawString(string, center_x - (X_start / 2.0f), Y_Start)
					:
					ABfontLarge->drawString(string, center_x - (X_start / 2.0f), Y_Start);

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
			outlineShader.setUniform1f("thickness", !currLarge ? borderWidthSmall : borderWidthLarge);

			for (int i = 0; i < sh.GetStringies().size(); i++)
			{
				fontPaths = !currLarge ?
					ABfontSmall->getStringAsPoints(sh.GetStringies()[i])
					:
					ABfontLarge->getStringAsPoints(sh.GetStringies()[i]);
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
			cleanFBO.draw(0, 0, rasterSize.getWidth(), rasterSize.getHeight());
			cleanFBO_Fade.end();

			//Outline FBO;
			outlineFBO_Fade.begin();
			ofClear(0);
			ofSetColor(255, alpha);
			outlineFBO.draw(0, 0, rasterSize.getWidth(), rasterSize.getHeight());
			outlineFBO_Fade.end();

			holdingLastMsg = true;
			alpha -= alphaTime;
			if (alpha < 0)
			{
				alpha = 255;
				oldMessage = temp;
				clearFBO_All();
			}
			//Send SPOUT 
			sendClean.send(cleanFBO_Fade.getTexture());
			sendOutline.send(outlineFBO_Fade.getTexture());
		}
	}

	//draw things to our screen
	ofSetColor(255);
	holdingLastMsg ? outlineFBO_Fade.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight()) 
		:
		outlineFBO.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	
	if(drawCross)
		DrawCenterCross();

	updateText = false;

}

void ofApp::exit()
{
	mongoose.close();
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


void ofApp::clearFBO()
{
	cleanFBO.begin();
	ofClear(0);
	cleanFBO.end();
	//cleanFBO_Fade.begin();
	//ofClear(0);
	//cleanFBO_Fade.end();

	outlineFBO.begin();
	ofClear(0);
	outlineFBO.end();
	/*outlineFBO_Fade.begin();
	ofClear(0);
	outlineFBO_Fade.end();*/

}

void ofApp::clearFBO_All()
{
	cleanFBO.begin();
	ofClear(0);
	cleanFBO.end();
	cleanFBO_Fade.begin();
	ofClear(0);
	cleanFBO_Fade.end();

	outlineFBO.begin();
	ofClear(0);
	outlineFBO.end();
	outlineFBO_Fade.begin();
	ofClear(0);
	outlineFBO_Fade.end();
}

//void ofApp::RunHealthCheck()
//{
//	//send healthcheck to clients
//	if (netLooper >= 60)
//	{
//		netLooper = 0;
//		server->CheckClientsHealth();
//		//Purge any messages in the server
//		server->EraseMessageQ();
//	}
//	netLooper++;
//}

bool ofApp::SizeControl(const std::string& ctrlStr)
{
	if (ctrlStr == "LARGE...")
	{
		currLarge = true;
		return true;
	}

	else if (ctrlStr == "SMALL...")
	{
		currLarge = false;
		return true;
	}
	return false;
}