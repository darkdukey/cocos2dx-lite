
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "stdafx.h"
#include <io.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <fcntl.h>
#include <Commdlg.h>
#include <Shlobj.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <shellapi.h>

#include <unordered_map>

#include "SimulatorWin.h"

#include "json/document.h"

#include "glfw3.h"
#include "glfw3native.h"

#include "scripting/lua-bindings/manual/CCLuaEngine.h"

// define 1 to open console ui and setup windows system menu, 0 to disable
//#if (CC_CODE_IDE_DEBUG_SUPPORT > 0)
#define SIMULATOR_WITH_CONSOLE_AND_MENU 1
//#else
//#define SIMULATOR_WITH_CONSOLE_AND_MENU 0
//#endif

#define ID_PORTRAIT 2001
#define ID_LANDSCAPE 2002


USING_NS_CC;

static std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

HMENU hViewMenu = NULL;

static void checkMenuItem(HMENU menu, int id, bool checked = true)
{
	MENUITEMINFO menuitem;
	menuitem.cbSize = sizeof(menuitem);
	menuitem.fMask = MIIM_STATE;
	menuitem.fState = (checked) ? MFS_CHECKED : MFS_UNCHECKED;
	if (SetMenuItemInfo(menu, id, MF_BYCOMMAND, &menuitem))
	{

	}
}

static WNDPROC g_oldWindowProc = NULL;
INT_PTR CALLBACK AboutDialogCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
void onHelpAbout()
{
    DialogBox(GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_DIALOG_ABOUT),
        Director::getInstance()->getOpenGLView()->getWin32Window(),
        AboutDialogCallback);
}

void shutDownApp()
{
    auto glview = dynamic_cast<GLViewImpl*> (Director::getInstance()->getOpenGLView());
    HWND hWnd = glview->getWin32Window();
    ::SendMessage(hWnd, WM_CLOSE, NULL, NULL);
}

std::string getCurAppPath(void)
{
    TCHAR szAppDir[MAX_PATH] = { 0 };
    if (!GetModuleFileName(NULL, szAppDir, MAX_PATH))
        return "";
    int nEnd = 0;
    for (int i = 0; szAppDir[i]; i++)
    {
        if (szAppDir[i] == '\\')
            nEnd = i;
    }
    szAppDir[nEnd] = 0;
    int iLen = 2 * wcslen(szAppDir);
    char* chRtn = new char[iLen + 1];
    wcstombs(chRtn, szAppDir, iLen + 1);
    std::string strPath = chRtn;
    delete[] chRtn;
    chRtn = NULL;
    char fuldir[MAX_PATH] = { 0 };
    _fullpath(fuldir, strPath.c_str(), MAX_PATH);
    return fuldir;
}

static void initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

SimulatorWin *SimulatorWin::_instance = nullptr;

SimulatorWin *SimulatorWin::getInstance()
{
    if (!_instance)
    {
        _instance = new SimulatorWin();
    }
    return _instance;
}

SimulatorWin::SimulatorWin()
    : _app(nullptr)
    , _hwnd(NULL)
    , _hwndConsole(NULL)
    , _writeDebugLogFile(nullptr)
	, _projectName("player")
{
}

SimulatorWin::~SimulatorWin()
{
    CC_SAFE_DELETE(_app);
    if (_writeDebugLogFile)
    {
        fclose(_writeDebugLogFile);
    }
}

void SimulatorWin::quit()
{
    Director::getInstance()->end();
}

void SimulatorWin::relaunch()
{
    _project.setWindowOffset(Vec2(getPositionX(), getPositionY()));
    openNewPlayerWithProjectConfig(_project);

    quit();
}

void SimulatorWin::openNewPlayer()
{
    openNewPlayerWithProjectConfig(_project);
}

void SimulatorWin::openNewPlayerWithProjectConfig(const ProjectConfig &config)
{
    static long taskid = 100;
    stringstream buf;
    buf << taskid++;

    string commandLine;
    commandLine.append(getApplicationExePath());
    commandLine.append(" ");
    commandLine.append(config.makeCommandLine());

    CCLOG("SimulatorWin::openNewPlayerWithProjectConfig(): %s", commandLine.c_str());

    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms682499(v=vs.85).aspx
    SECURITY_ATTRIBUTES sa = {0};
    sa.nLength = sizeof(sa);

    PROCESS_INFORMATION pi = {0};
    STARTUPINFO si = {0};
    si.cb = sizeof(STARTUPINFO);

#define MAX_COMMAND 1024 // lenth of commandLine is always beyond MAX_PATH

    WCHAR command[MAX_COMMAND];
    memset(command, 0, sizeof(command));
    MultiByteToWideChar(CP_UTF8, 0, commandLine.c_str(), -1, command, MAX_COMMAND);

    BOOL success = CreateProcess(NULL,
                                 command,   // command line
                                 NULL,      // process security attributes
                                 NULL,      // primary thread security attributes
                                 FALSE,     // handles are inherited
                                 0,         // creation flags
                                 NULL,      // use parent's environment
                                 NULL,      // use parent's current directory
                                 &si,       // STARTUPINFO pointer
                                 &pi);      // receives PROCESS_INFORMATION

    if (!success)
    {
        CCLOG("PlayerTaskWin::run() - create process failed, for execute %s", commandLine.c_str());
    }
}

void SimulatorWin::openProjectWithProjectConfig(const ProjectConfig &config)
{
    openNewPlayerWithProjectConfig(config);
    quit();
}

int SimulatorWin::getPositionX()
{
    RECT rect;
    GetWindowRect(_hwnd, &rect);
    return rect.left;
}

int SimulatorWin::getPositionY()
{
    RECT rect;
    GetWindowRect(_hwnd, &rect);
    return rect.top;
}

int SimulatorWin::run()
{
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    parseCocosProjectConfig(_project);

    // load project config from command line args
    vector<string> args;
    for (int i = 0; i < __argc; ++i)
    {
        wstring ws(__wargv[i]);
        string s;
        s.assign(ws.begin(), ws.end());
        args.push_back(s);
    }
    _project.parseCommandLine(args);

    if (_project.getProjectDir().empty())
    {
        if (args.size() == 2)
        {
            // for Code IDE before RC2
            _project.setProjectDir(args.at(1));
            //_project.setDebuggerType(kCCRuntimeDebuggerCodeIDE);
        }
		else
		{
			_project.setProjectDir(getCurAppPath() + "/../../");
		}
    }

    // create the application instance
    _app = new AppDelegate();
    //RuntimeEngine::getInstance()->setProjectConfig(_project);

#if (SIMULATOR_WITH_CONSOLE_AND_MENU > 0)
    // create console window
    if (_project.isShowConsole())
    {
        AllocConsole();
        _hwndConsole = GetConsoleWindow();
        if (_hwndConsole != NULL)
        {
            ShowWindow(_hwndConsole, SW_SHOW);
            BringWindowToTop(_hwndConsole);
            freopen("CONOUT$", "wt", stdout);
            freopen("CONOUT$", "wt", stderr);

            HMENU hmenu = GetSystemMenu(_hwndConsole, FALSE);
            if (hmenu != NULL)
            {
                DeleteMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
            }
        }
    }
#endif

    // log file
    if (_project.isWriteDebugLogToFile())
    {
        const string debugLogFilePath = _project.getDebugLogFilePath();
        _writeDebugLogFile = fopen(debugLogFilePath.c_str(), "w");
        if (!_writeDebugLogFile)
        {
            CCLOG("Cannot create debug log file %s", debugLogFilePath.c_str());
        }
    }

    // set environments
    SetCurrentDirectoryA(_project.getProjectDir().c_str());
    FileUtils::getInstance()->setDefaultResourceRootPath(_project.getProjectDir());
    FileUtils::getInstance()->setWritablePath(_project.getWritableRealPath().c_str());

    // check screen DPI
    HDC screen = GetDC(0);
    int dpi = GetDeviceCaps(screen, LOGPIXELSX);
    ReleaseDC(0, screen);

    // set scale with DPI
    //  96 DPI = 100 % scaling
    // 120 DPI = 125 % scaling
    // 144 DPI = 150 % scaling
    // 192 DPI = 200 % scaling
    // http://msdn.microsoft.com/en-us/library/windows/desktop/dn469266%28v=vs.85%29.aspx#dpi_and_the_desktop_scaling_factor
    //
    // enable DPI-Aware with DeclareDPIAware.manifest
    // http://msdn.microsoft.com/en-us/library/windows/desktop/dn469266%28v=vs.85%29.aspx#declaring_dpi_awareness
    float screenScale = 1.0f;
    if (dpi >= 120 && dpi < 144)
    {
        screenScale = 1.25f;
    }
    else if (dpi >= 144 && dpi < 192)
    {
        screenScale = 1.5f;
    }
    else if (dpi >= 192)
    {
        screenScale = 2.0f;
    }
    CCLOG("SCREEN DPI = %d, SCREEN SCALE = %0.2f", dpi, screenScale);

    // create opengl view
    Size frameSize = _project.getFrameSize();
    float frameScale = 1.0f;
    if (_project.isRetinaDisplay())
    {
        frameSize.width *= screenScale;
        frameSize.height *= screenScale;
    }
    else
    {
        frameScale = screenScale;
    }

    const Rect frameRect = Rect(0, 0, frameSize.width, frameSize.height);
    //ConfigParser::getInstance()->setInitViewSize(frameSize);
    const bool isResize = _project.isResizeWindow();
    std::stringstream title;
    title << "Cocos2d-x lite - " << _projectName << "(" << cocos2dVersion() << ") " << frameRect.size.width << "x" << frameRect.size.height;
    initGLContextAttrs();
    auto glview = GLViewImpl::createWithRect(title.str(), frameRect, frameScale);
    _hwnd = glview->getWin32Window();
    //player::PlayerWin::createWithHwnd(_hwnd);
    DragAcceptFiles(_hwnd, TRUE);
    //SendMessage(_hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
    //SendMessage(_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
    //FreeResource(icon);

    auto director = Director::getInstance();
    director->setOpenGLView(glview);

    director->setAnimationInterval(1.0 / 60.0);

    // set window position
    if (_project.getProjectDir().length())
    {
        setZoom(_project.getFrameScale());
    }
    Vec2 pos = _project.getWindowOffset();
    if (pos.x != 0 && pos.y != 0)
    {
        RECT rect;
        GetWindowRect(_hwnd, &rect);
        MoveWindow(_hwnd, pos.x, pos.y, rect.right - rect.left, rect.bottom - rect.top, FALSE);
    }

    // path for looking Lang file, Studio Default images
    FileUtils::getInstance()->addSearchPath(getApplicationPath().c_str());

#if SIMULATOR_WITH_CONSOLE_AND_MENU > 0
    // init player services
    setupUI();
    DrawMenuBar(_hwnd);
#endif

    // prepare
    FileUtils::getInstance()->setPopupNotify(false);
    _project.dump();
    auto app = Application::getInstance();

    g_oldWindowProc = (WNDPROC)SetWindowLong(_hwnd, GWL_WNDPROC, (LONG)SimulatorWin::windowProc);

    // startup message loop
    return app->run();
}

// services

void SimulatorWin::setupUI()
{
	hViewMenu = CreateMenu();

	SimulatorConfig *config = SimulatorConfig::getInstance();
	int current = config->checkScreenSize(_project.getFrameSize());
	// screen size
	for (int i = 0; i < config->getScreenSizeCount(); i++)
	{
		SimulatorScreenSize size = config->getScreenSize(i);
		auto index = 3000 + i;
		AppendMenu(hViewMenu, MF_STRING, index, s2ws(size.title).c_str());
		if (current == i) 
		{
			checkMenuItem(hViewMenu, index);
		}
	}

	// 
	AppendMenuW(hViewMenu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hViewMenu, MF_STRING, ID_PORTRAIT, L"Portrait");
	AppendMenuW(hViewMenu, MF_STRING, ID_LANDSCAPE, L"Landscape");

	if (_project.isLandscapeFrame()) {
		checkMenuItem(hViewMenu, ID_LANDSCAPE);
	}
	else {
		checkMenuItem(hViewMenu, ID_PORTRAIT);
	}

	// scale
	AppendMenuW(hViewMenu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hViewMenu, MF_STRING, 1100, s2ws("Zoom Out (100%)").c_str());
	AppendMenuW(hViewMenu, MF_STRING, 1075, s2ws("Zoom Out (75%)").c_str());
	AppendMenuW(hViewMenu, MF_STRING, 1050, s2ws("Zoom Out (50%)").c_str());
	AppendMenuW(hViewMenu, MF_STRING, 1025, s2ws("Zoom Out (25%)").c_str());
	int scale = int(_project.getFrameScale() * 100);
	checkMenuItem(hViewMenu, 1000 + scale);

	HMENU menu = GetSystemMenu(_hwnd, FALSE);
	AppendMenuW(menu, MF_POPUP, (UINT_PTR)hViewMenu, L"&View");

	AppendMenuW(menu, MF_STRING, ID_HELP_ABOUT, s2ws("About").c_str());
	AppendMenuW(menu, MF_STRING, 30, s2ws("Documents").c_str());
}

void SimulatorWin::setZoom(float frameScale)
{
    _project.setFrameScale(frameScale);
    cocos2d::Director::getInstance()->getOpenGLView()->setFrameZoomFactor(frameScale);
}

ProjectConfig &SimulatorWin::getProjectConfig()
{
	return _project;
}

// debug log
void SimulatorWin::writeDebugLog(const char *log)
{
    if (!_writeDebugLogFile) return;

    fputs(log, _writeDebugLogFile);
    fputc('\n', _writeDebugLogFile);
    fflush(_writeDebugLogFile);
}

void SimulatorWin::parseCocosProjectConfig(ProjectConfig &config)
{
    // get project directory
    ProjectConfig tmpConfig;
    // load project config from command line args
    vector<string> args;
    for (int i = 0; i < __argc; ++i)
    {
        wstring ws(__wargv[i]);
        string s;
        s.assign(ws.begin(), ws.end());
        args.push_back(s);
    }

    if (args.size() >= 2)
    {
        if (args.size() && args.at(1).at(0) == '/')
        {
            // FIXME:
            // for Code IDE before RC2
            tmpConfig.setProjectDir(args.at(1));
        }

        tmpConfig.parseCommandLine(args);
    }
	else 
	{
#if (COCOS2D_DEBUG > 0)
		if (tmpConfig.getProjectDir().empty()) {
			tmpConfig.setProjectDir(getApplicationPath() + "/../../");
			tmpConfig.setScriptFile("src/main.lua");
		}
#endif
	}

    // set project directory as search root path
    FileUtils::getInstance()->setDefaultResourceRootPath(tmpConfig.getProjectDir().c_str());
	readConfig();
}

//
// D:\aaa\bbb\ccc\ddd\abc.txt --> D:/aaa/bbb/ccc/ddd/abc.txt
//
std::string SimulatorWin::convertPathFormatToUnixStyle(const std::string& path)
{
    std::string ret = path;
    int len = ret.length();
    for (int i = 0; i < len; ++i)
    {
        if (ret[i] == '\\')
        {
            ret[i] = '/';
        }
    }
    return ret;
}

//
// convert Unicode/LocalCode TCHAR to Utf8 char
//
char* SimulatorWin::convertTCharToUtf8(const TCHAR* src)
{
#ifdef UNICODE
    WCHAR* tmp = (WCHAR*)src;
    size_t size = wcslen(src) * 3 + 1;
    char* dest = new char[size];
    memset(dest, 0, size);
    WideCharToMultiByte(CP_UTF8, 0, tmp, -1, dest, size, NULL, NULL);
    return dest;
#else
    char* tmp = (char*)src;
    uint32 size = strlen(tmp) + 1;
    WCHAR* dest = new WCHAR[size];
    memset(dest, 0, sizeof(WCHAR)*size);
    MultiByteToWideChar(CP_ACP, 0, src, -1, dest, (int)size); // convert local code to unicode.

    size = wcslen(dest) * 3 + 1;
    char* dest2 = new char[size];
    memset(dest2, 0, size);
    WideCharToMultiByte(CP_UTF8, 0, dest, -1, dest2, size, NULL, NULL); // convert unicode to utf8.
    delete[] dest;
    return dest2;
#endif
}

//
std::string SimulatorWin::getApplicationExePath()
{
    TCHAR szFileName[MAX_PATH];
    GetModuleFileName(NULL, szFileName, MAX_PATH);
    std::u16string u16ApplicationName;
    char *applicationExePath = convertTCharToUtf8(szFileName);
    std::string path(applicationExePath);
    CC_SAFE_FREE(applicationExePath);

    return path;
}

std::string SimulatorWin::getApplicationPath()
{
    std::string path = getApplicationExePath();
    size_t pos;
    while ((pos = path.find_first_of("\\")) != std::string::npos)
    {
        path.replace(pos, 1, "/");
    }
    size_t p = path.find_last_of("/");
    string workdir;
    if (p != path.npos)
    {
        workdir = path.substr(0, p);
    }

    return workdir;
}

void SimulatorWin::readConfig(const string &filepath)
{
	string fullPathFile = filepath;

	// read config file
	string fileContent = FileUtils::getInstance()->getStringFromFile(fullPathFile);

	if (fileContent.empty())
		return;

	rapidjson::Document _docRootjson;

	if (_docRootjson.Parse<0>(fileContent.c_str()).HasParseError()) {
		cocos2d::log("read json file %s failed because of %d", fullPathFile.c_str(), _docRootjson.GetParseError());
		return;
	}

	if (_docRootjson.HasMember("init_cfg"))
	{
		if (_docRootjson["init_cfg"].IsObject())
		{
			const rapidjson::Value& objectInitView = _docRootjson["init_cfg"];
			if (objectInitView.HasMember("width") && objectInitView.HasMember("height"))
			{
				_project.setFrameSize(cocos2d::Size(objectInitView["width"].GetUint(), objectInitView["height"].GetUint()));
			}
			if (objectInitView.HasMember("name") && objectInitView["name"].IsString())
			{
				_projectName = objectInitView["name"].GetString();
			}
			if (objectInitView.HasMember("isLandscape") && objectInitView["isLandscape"].IsBool())
			{
				_project.changeFrameOrientationToLandscape();
			}
			if (objectInitView.HasMember("entry") && objectInitView["entry"].IsString())
			{
				_project.setScriptFile(objectInitView["entry"].GetString());
			}
		}
	}
	if (_docRootjson.HasMember("simulator_screen_size"))
	{
		const rapidjson::Value& ArrayScreenSize = _docRootjson["simulator_screen_size"];
		if (ArrayScreenSize.IsArray())
		{
			ScreenSizeArray screenSizeArray;
			
			for (int i = 0; i < ArrayScreenSize.Size(); i++)
			{
				const rapidjson::Value& objectScreenSize = ArrayScreenSize[i];
				if (objectScreenSize.HasMember("title") && objectScreenSize.HasMember("width") && objectScreenSize.HasMember("height"))
				{
					screenSizeArray.push_back(SimulatorScreenSize(objectScreenSize["title"].GetString(), objectScreenSize["width"].GetUint(), objectScreenSize["height"].GetUint()));
				}
			}

			if (!screenSizeArray.empty())
			{
				SimulatorConfig::getInstance()->setScreenArray(screenSizeArray);
			}
		}
	}
}


LRESULT CALLBACK SimulatorWin::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!_instance) return 0;

    switch (uMsg)
    {
    case WM_SYSCOMMAND:
    case WM_COMMAND:
    {
        if (HIWORD(wParam) == 0)
        {
            // menu
            WORD menuId = LOWORD(wParam);
            if (menuId == ID_HELP_ABOUT)
            {
                onHelpAbout();
            }
			else if (menuId == 30) 
			{
				cocos2d::Application::getInstance()->openURL("https://github.com/c0i/cocos2dx-lite");
			}
			else if (menuId >= 3000 && menuId < (3000+SimulatorConfig::getInstance()->getScreenSizeCount()))
			{ // screen size
				int index = menuId - 3000;
				auto size = SimulatorConfig::getInstance()->getScreenSize(index);
				if (SimulatorWin::getInstance()->getProjectConfig().isLandscapeFrame()) 
				{
					std::swap(size.width, size.height);
				}
				_instance->getProjectConfig().setFrameSize(cocos2d::Size(size.width, size.height));
				_instance->relaunch();
			}
			else if (menuId >= 2000) { //
				if (menuId == ID_PORTRAIT) 
				{
					_instance->getProjectConfig().changeFrameOrientationToPortait();
					_instance->relaunch();
				}
				else if (menuId == ID_LANDSCAPE) 
				{
					_instance->getProjectConfig().changeFrameOrientationToLandscape();
					_instance->relaunch();
				}
			}
			else if (menuId >= 1000) 
			{
				checkMenuItem(hViewMenu, 1100, false);
				checkMenuItem(hViewMenu, 1075, false);
				checkMenuItem(hViewMenu, 1050, false);
				checkMenuItem(hViewMenu, 1025, false);
				checkMenuItem(hViewMenu, menuId);

				float scale = (menuId - 1000) / 100.0f;
				_instance->setZoom(scale);
			}

        }
        break;
    }
    case WM_KEYDOWN:
    {
#if (SIMULATOR_WITH_CONSOLE_AND_MENU > 0)
        if (wParam == VK_F5)
        {
            _instance->relaunch();
        }
#endif
        break;
    }

    case WM_COPYDATA:
        {
            PCOPYDATASTRUCT pMyCDS = (PCOPYDATASTRUCT) lParam;
            if (pMyCDS->dwData == 1)
            {
                const char *szBuf = (const char*)(pMyCDS->lpData);
                SimulatorWin::getInstance()->writeDebugLog(szBuf);
                break;
            }
        }

    case WM_DESTROY:
    {
        DragAcceptFiles(hWnd, FALSE);
        break;
    }

    case WM_DROPFILES:
    {
        //HDROP hDrop = (HDROP)wParam;

        //const int count = DragQueryFileW(hDrop, 0xffffffff, NULL, 0);

        //if (count > 0)
        //{
        //    int fileIndex = 0;

        //    const UINT length = DragQueryFileW(hDrop, fileIndex, NULL, 0);
        //    WCHAR* buffer = (WCHAR*)calloc(length + 1, sizeof(WCHAR));

        //    DragQueryFileW(hDrop, fileIndex, buffer, length + 1);
        //    char *utf8 = SimulatorWin::convertTCharToUtf8(buffer);
        //    std::string firstFile(utf8);
        //    CC_SAFE_FREE(utf8);
        //    DragFinish(hDrop);

        //    // broadcast drop event
        //    AppEvent forwardEvent("APP.EVENT.DROP", APP_EVENT_DROP);
        //    forwardEvent.setDataString(firstFile);

        //    Director::getInstance()->getEventDispatcher()->dispatchEvent(&forwardEvent);
        //}
    }   // WM_DROPFILES

    }
    return g_oldWindowProc(hWnd, uMsg, wParam, lParam);
}
