#include "ofMain.h"
#include "ofApp.h"

//Strange code to make stuff 
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
//========================================================================
int main( ){
	ofSetupOpenGL(1280,360,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
