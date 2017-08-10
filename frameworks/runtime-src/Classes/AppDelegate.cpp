#include "AppDelegate.h"
#include "audio/include/AudioEngine.h"
#include "cocos2d.h"

#include "lua-modules/lua_modules.h"

// lua
#include "scripting/lua-bindings/manual/CCLuaEngine.h"
#include "scripting/lua-bindings/manual/lua_module_register.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "bugly/CrashReport.h"
#include "bugly/lua/BuglyLuaAgent.h"
#endif

USING_NS_CC;
using namespace std;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
    //SimpleAudioEngine::end();
    experimental::AudioEngine::end();
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,
// don't modify or remove this function
static int register_all_packages()
{
    //register custom function
    auto engine = LuaEngine::getInstance();
    LuaStack* stack = engine->getLuaStack();
    preload_lua_modules(stack->getLuaState());

    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // set default FPS
    Director::getInstance()->setAnimationInterval(1.0 / 60.0f);

    // register lua module
    auto engine = LuaEngine::getInstance();
    ScriptEngineManager::getInstance()->setScriptEngine(engine);
    lua_State* L = engine->getLuaStack()->getLuaState();
    lua_module_register(L);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CrashReport::initCrashReport("ed866d20d2", false);
    BuglyLuaAgent::registerLuaExceptionHandler(engine);
#endif
    
    register_all_packages();

    LuaStack* stack = engine->getLuaStack();
    stack->setXXTEAKeyAndSign("2dxLua", strlen("2dxLua"), "XXTEA", strlen("XXTEA"));
    
#if CC_64BITS
    FileUtils::getInstance()->addSearchPath("src/64bit");
#endif
    FileUtils::getInstance()->addSearchPath("src");
    FileUtils::getInstance()->addSearchPath("res");

    FileUtils::getInstance()->setPopupNotify(false);
    if (engine->executeScriptFile("main.lua"))
    {
        return false;
    }

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

    //SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    experimental::AudioEngine::pauseAll();
    NotificationCenter::getInstance()->postNotification("APP_ENTER_BACKGROUND_EVENT");
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

    //SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    experimental::AudioEngine::resumeAll();
    NotificationCenter::getInstance()->postNotification("APP_ENTER_FOREGROUND_EVENT");
}

void AppDelegate::setProjectConfig(const ProjectConfig& config)
{
    _projectConfig = config;
}

