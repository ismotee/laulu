#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"
#include "staticSettings.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;

    // settings for main window
    settings.width = windowWidth;
    settings.height = windowHeight;
    settings.setPosition(ofVec2f(0,0));
    settings.resizable = true;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);


    // settings for the secondary window aka. monitor
    settings.width = monitorWidth;
    settings.height = monitorHeight;
    settings.setPosition(ofVec2f(windowWidth,0));
    settings.resizable = true;
    
    shared_ptr<ofAppBaseWindow> monitorWindow = ofCreateWindow(settings);
    monitorWindow->setVerticalSync(false);
    
    // creating a main application
    shared_ptr<ofApp> mainApp(new ofApp);
    
    // monitor setup needs to be run manually
    mainApp->setupMonitor();
    
    // listeners for monitor
    ofAddListener(monitorWindow->events().draw,mainApp.get(),&ofApp::drawMonitor);
    ofAddListener(monitorWindow->events().windowResized,mainApp.get(),&ofApp::monitorResized);    
    
    // starting a application
    ofRunApp(mainWindow,mainApp);
    ofRunMainLoop();
    
}
