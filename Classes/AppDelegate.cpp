#include "AppDelegate.h"
#include "StartScene.h"
#include "LogoSence.h"
#include <string>

USING_NS_CC;

#define DESIGN_RESOLUTION_480X320    0
#define DESIGN_RESOLUTION_960X640   1
#define DESIGN_RESOLUTION_2048X1536  2

/* If you want to switch design resolution, change next line */
#define TARGET_DESIGN_RESOLUTION_SIZE  DESIGN_RESOLUTION_960X640

typedef struct tagResource
{
	cocos2d::CCSize size;
	char directory[100];
}Resource;

static Resource smallResource  =  { cocos2d::CCSizeMake(480, 320),  "iphone" };
static Resource mediumResource =  { cocos2d::CCSizeMake(960, 640),  "iphonehd" };
static Resource largeResource  =  { cocos2d::CCSizeMake(2048, 1536), "ipadhd" };

#if (TARGET_DESIGN_RESOLUTION_SIZE == DESIGN_RESOLUTION_480X320)
static cocos2d::CCSize designResolutionSize = cocos2d::CCSizeMake(480, 320);
#elif (TARGET_DESIGN_RESOLUTION_SIZE == DESIGN_RESOLUTION_960X640)
static cocos2d::CCSize designResolutionSize = cocos2d::CCSizeMake(960, 640);
#elif (TARGET_DESIGN_RESOLUTION_SIZE == DESIGN_RESOLUTION_2048X1536)
static cocos2d::CCSize designResolutionSize = cocos2d::CCSizeMake(2048, 1536);
#else
#error unknown target design resolution!
#endif

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
	GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		glview = cocos2d::GLViewImpl::createWithRect("My Window", cocos2d::Rect(0, 0, 640, 960));
#else
        glview = GLViewImpl::create("My Game");
#endif
        director->setOpenGLView(glview);
    }

	Size frameSize = glview->getFrameSize();
	if (frameSize.width < frameSize.height)
	{
		float tmp = frameSize.width;
		frameSize.width = frameSize.height;
		frameSize.height = tmp;
	}

	glview->setDesignResolutionSize(
		MIN(designResolutionSize.width, designResolutionSize.height),
		MAX(designResolutionSize.width, designResolutionSize.height),
		ResolutionPolicy::SHOW_ALL);

	// if the frame's height is larger than the height of medium resource size, select large resource.
// 	if (frameSize.height > mediumResource.size.height)
// 	{
// 		FileUtils::getInstance()->addSearchPath(largeResource.directory, true);
// 		director->setContentScaleFactor(MIN(frameSize.height/largeResource.size.height, 
// 			frameSize.width/largeResource.size.width));
// 	}
// 	// if the frame's height is larger than the height of small resource size, select medium resource.
// 	else if (frameSize.height > smallResource.size.height)
	{
		FileUtils::getInstance()->addSearchPath(mediumResource.directory, true);
		director->setContentScaleFactor(MIN(mediumResource.size.height/designResolutionSize.height,
			mediumResource.size.width/designResolutionSize.width));
	}
	// if the frame's height is smaller than the height of medium resource size, select small resource.
// 	else
// 	{
// 		FileUtils::getInstance()->addSearchPath(smallResource.directory, true);
// 		director->setContentScaleFactor(MIN(smallResource.size.height/designResolutionSize.height, smallResource.size.width/designResolutionSize.width));
// 	}
	
    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    FileUtils::getInstance()->addSearchPath("res");

    // create a scene. it's an autorelease object
    auto scene = LogoLayer::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
