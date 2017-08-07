#pragma once

#include "stdafx.h"
#include "Resource.h"
#include "cocos2d.h"
#include "AppDelegate.h"
#include "ProjectConfig/ProjectConfig.h"
#include "ProjectConfig/SimulatorConfig.h"

class SimulatorWin
{
public:
    static SimulatorWin *getInstance();
    virtual ~SimulatorWin();
    int run();

    virtual void quit();
    virtual void relaunch();
    virtual void openNewPlayer();
    virtual void openNewPlayerWithProjectConfig(const ProjectConfig &config);
    virtual void openProjectWithProjectConfig(const ProjectConfig &config);

    virtual int getPositionX();
    virtual int getPositionY();

	void setZoom(float frameScale);

	ProjectConfig &getProjectConfig();
protected:
    SimulatorWin();

    static SimulatorWin *_instance;
	std::string _projectName;
    ProjectConfig _project;
    HWND _hwnd;
    HWND _hwndConsole;
    AppDelegate *_app;
    FILE *_writeDebugLogFile;

    // 
    void setupUI();

    // debug log
    void writeDebugLog(const char *log);
    void parseCocosProjectConfig(ProjectConfig &config);

    // helper
	void readConfig(const string &filepath = "config.json");
    std::string convertPathFormatToUnixStyle(const std::string& path);
    std::string getApplicationExePath();
    std::string getApplicationPath();
    static char* convertTCharToUtf8(const TCHAR* src);

    static LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

