#import "AppController.h"

#import "ConsoleWindowController.h"

#include "AppDelegate.h"
#include "glfw3.h"
#include "glfw3native.h"

#include "cocos2d.h"
#include "audio/include/SimpleAudioEngine.h"
//#include "CCLuaEngine.h"

//#include "PlayerMac.h"

USING_NS_CC;

static std::string getCurAppPath(void)
{
    return [[[NSBundle mainBundle] bundlePath] UTF8String];
}

@implementation AppController

@synthesize menu;

- (void) dealloc
{
    if (_buildTask)
    {
        [_buildTask interrupt];
        _buildTask = nil;
    }
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
//    auto player = player::PlayerMac::create();
//    player->setController(self);

    _isAlwaysOnTop = NO;
    _hasPopupDialog = NO;
    _debugLogFile = 0;

    _buildTask = nil;
    _isBuildingFinished = YES;

    [self updateProjectFromCommandLineArgs:&_project];
    [self createWindowAndGLView];
    [self startup];
}

- (BOOL) windowShouldClose:(id)sender
{
    return YES;
}

- (void) windowWillClose:(NSNotification *)notification
{
    [[NSRunningApplication currentApplication] terminate];
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)theApplication
{
    return YES;
}

- (BOOL) applicationShouldHandleReopen:(NSApplication *)sender hasVisibleWindows:(BOOL)flag
{
    return NO;
}

- (BOOL) validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)anItem
{
    return _hasPopupDialog == NO;
}

- (void) updateUI
{
    NSMenu *menuPlayer = [[[_window menu] itemWithTitle:@"Player"] submenu];
    NSMenuItem *itemWriteDebugLogToFile = [menuPlayer itemWithTitle:@"Write Debug Log to File"];
    [itemWriteDebugLogToFile setState:_project.isWriteDebugLogToFile() ? NSOnState : NSOffState];

    NSMenu *menuScreen = [[[_window menu] itemWithTitle:@"Screen"] submenu];
    NSMenuItem *itemPortait = [menuScreen itemWithTitle:@"Portait"];
    NSMenuItem *itemLandscape = [menuScreen itemWithTitle:@"Landscape"];
    if (_project.isLandscapeFrame())
    {
        [itemPortait setState:NSOffState];
        [itemLandscape setState:NSOnState];
    }
    else
    {
        [itemPortait setState:NSOnState];
        [itemLandscape setState:NSOffState];
    }

    int scale = _project.getFrameScale() * 100;

    NSMenuItem *itemZoom100 = [menuScreen itemWithTitle:@"Actual (100%)"];
    NSMenuItem *itemZoom75 = [menuScreen itemWithTitle:@"Zoom Out (75%)"];
    NSMenuItem *itemZoom50 = [menuScreen itemWithTitle:@"Zoom Out (50%)"];
    NSMenuItem *itemZoom25 = [menuScreen itemWithTitle:@"Zoom Out (25%)"];
    [itemZoom100 setState:NSOffState];
    [itemZoom75 setState:NSOffState];
    [itemZoom50 setState:NSOffState];
    [itemZoom25 setState:NSOffState];
    if (scale == 100)
    {
        [itemZoom100 setState:NSOnState];
    }
    else if (scale == 75)
    {
        [itemZoom75 setState:NSOnState];
    }
    else if (scale == 50)
    {
        [itemZoom50 setState:NSOnState];
    }
    else if (scale == 25)
    {
        [itemZoom25 setState:NSOnState];
    }

    NSArray *recents = [[NSUserDefaults standardUserDefaults] arrayForKey:@"recents"];
    NSMenu *menuRecents = [[[[[_window menu] itemWithTitle:@"File"] submenu] itemWithTitle:@"Open Recent"] submenu];
    while (true)
    {
        NSMenuItem *item = [menuRecents itemAtIndex:0];
        if ([item isSeparatorItem]) break;
        [menuRecents removeItemAtIndex:0];
    }

    for (NSInteger i = [recents count] - 1; i >= 0; --i)
    {
        NSDictionary *recentItem = [recents objectAtIndex:i];
        NSMenuItem *item = [[[NSMenuItem alloc] initWithTitle:[recentItem objectForKey:@"title"]
                                                       action:@selector(onFileOpenRecent:)
                                                keyEquivalent:@""] autorelease];
        [menuRecents insertItem:item atIndex:0];
    }

    [_window setTitle:[NSString stringWithFormat:@"quick player (%0.0f%%)", _project.getFrameScale() * 100]];
}

- (void) updateOpenRect
{
    NSMutableArray *recents = [NSMutableArray arrayWithArray:[[NSUserDefaults standardUserDefaults] arrayForKey:@"recents"]];

    for (NSInteger i = [recents count] - 1; i >= 0; --i)
    {
        id recentItem = [recents objectAtIndex:i];
        if (![[recentItem class] isSubclassOfClass:[NSDictionary class]])
        {
            [recents removeObjectAtIndex:i];
            continue;
        }

        NSString *title = [recentItem objectForKey:@"title"];
        if (!title || [title length] == 0 || !FileUtils::getInstance()->isDirectoryExist([title cStringUsingEncoding:NSUTF8StringEncoding]))
        {
            [recents removeObjectAtIndex:i];
        }
    }

    NSString *title = [NSString stringWithCString:_project.getProjectDir().c_str() encoding:NSUTF8StringEncoding];
    if ([title length] > 0)
    {
        for (NSInteger i = [recents count] - 1; i >= 0; --i)
        {
            id recentItem = [recents objectAtIndex:i];
            if ([title compare:[recentItem objectForKey:@"title"]] == NSOrderedSame)
            {
                [recents removeObjectAtIndex:i];
            }
        }

        NSMutableArray *args = [self makeCommandLineArgsFromProjectConfig:kProjectConfigOpenRecent];
        NSDictionary *item = [NSDictionary dictionaryWithObjectsAndKeys:title, @"title", args, @"args", nil];
        [recents insertObject:item atIndex:0];
    }
    [[NSUserDefaults standardUserDefaults] setObject:recents forKey:@"recents"];
}

- (void) showModelSheet
{
    _hasPopupDialog = YES;
    if (_app)
    {
        Director::getInstance()->pause();
        CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
        CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();
    }
}

- (void) stopModelSheet
{
    _hasPopupDialog = NO;
    if (_app)
    {
        Director::getInstance()->resume();
        CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
        CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();
    }
}

- (NSMutableArray*) makeCommandLineArgsFromProjectConfig
{
    return [self makeCommandLineArgsFromProjectConfig:kProjectConfigAll];
}

- (NSMutableArray*) makeCommandLineArgsFromProjectConfig:(unsigned int)mask
{
    _project.setWindowOffset(Vec2(_window.frame.origin.x, _window.frame.origin.y));
    NSString *commandLine = [NSString stringWithCString:_project.makeCommandLine(mask).c_str()
                                               encoding:NSUTF8StringEncoding];
    return [NSMutableArray arrayWithArray:[commandLine componentsSeparatedByString:@" "]];
}

- (void) updateProjectFromCommandLineArgs:(ProjectConfig*)config
{
    NSArray *nsargs = [[NSProcessInfo processInfo] arguments];
    long n = [nsargs count];
    if (n >= 2)
    {
        vector<string> args;
        for (int i = 0; i < [nsargs count]; ++i)
        {
            string arg = [[nsargs objectAtIndex:i] cStringUsingEncoding:NSUTF8StringEncoding];
            if (arg.length()) args.push_back(arg);
        }

        if (args.size() && args.at(1).at(0) == '/')
        {
            // for Code IDE before RC2
            config->setProjectDir(args.at(1));
            config->setDebuggerType(kCCLuaDebuggerCodeIDE);
        }
        config->parseCommandLine(args);
    }

    if (config->getProjectDir().empty())
    {
#if defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)
        config->setProjectDir(getCurAppPath() + "/../../../");
#endif
    }
}

- (void) launch:(NSArray*)args
{
    NSURL *url = [NSURL fileURLWithPath:[[NSBundle mainBundle] bundlePath]];
    NSMutableDictionary *configuration = [NSMutableDictionary dictionaryWithObject:args forKey:NSWorkspaceLaunchConfigurationArguments];
    NSError *error = [[[NSError alloc] init] autorelease];
    [[NSWorkspace sharedWorkspace] launchApplicationAtURL:url
                                                  options:NSWorkspaceLaunchNewInstance
                                            configuration:configuration error:&error];
}

- (void) relaunch:(NSArray*)args
{
    [self launch:args];
    [[NSApplication sharedApplication] terminate:self];
}

- (void) relaunch
{
    [self relaunch:[self makeCommandLineArgsFromProjectConfig]];
}

- (void) showAlertWithoutSheet:(NSString*)message withTitle:(NSString*)title
{
    NSAlert *alert = [[[NSAlert alloc] init] autorelease];
    [alert addButtonWithTitle:@"OK"];
    [alert setMessageText:message];
    [alert setInformativeText:title];
    [alert setAlertStyle:NSWarningAlertStyle];
    [alert runModal];
}

- (void) adjustEditMenuIndex
{
    NSApplication *thisApp = [NSApplication sharedApplication];
    NSMenu *mainMenu = [thisApp mainMenu];

    NSMenuItem *editMenuItem = [mainMenu itemWithTitle:@"Edit"];
    if (editMenuItem)
    {
        NSUInteger index = 2;
        if (index > [mainMenu itemArray].count)
            index = [mainMenu itemArray].count;
        [[editMenuItem menu] removeItem:editMenuItem];
        [mainMenu insertItem:editMenuItem atIndex:index];
    }
}

#pragma mark -
#pragma mark functions

- (void) createWindowAndGLView
{
    if (_project.isShowConsole())
    {
        [self openConsoleWindow];
        CCLOG("%s\n",Configuration::getInstance()->getInfo().c_str());
    }
    
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    GLView::setGLContextAttrs(glContextAttrs);

    float frameScale = _project.getFrameScale();

    // create opengl view
    cocos2d::Size frameSize = _project.getFrameSize();

    const cocos2d::Rect frameRect = cocos2d::Rect(0, 0, frameSize.width, frameSize.height);
    NSString *title = [NSString stringWithFormat:@"quick (%s)", cocos2dVersion()];
    GLViewImpl *eglView = GLViewImpl::createWithRect([title UTF8String], frameRect, frameScale, _project.isResizeWindow());

    auto director = Director::getInstance();
    director->setOpenGLView(eglView);

    _window = eglView->getCocoaWindow();
    [NSApp setDelegate:self];
    [_window center];

    if (_project.getProjectDir().length())
    {
        [self setZoom:_project.getFrameScale()];
        Vec2 pos = _project.getWindowOffset();
        if (pos.x != 0 && pos.y != 0)
        {
            [_window setFrameOrigin:NSMakePoint(pos.x, pos.y)];
        }
    }
}

- (void) startup
{
    const string projectDir = _project.getProjectDir();
    if (!projectDir.empty())
    {
        FileUtils::getInstance()->setDefaultResourceRootPath(projectDir);
        if (_project.isWriteDebugLogToFile())
        {
            [self writeDebugLogToFile:_project.getDebugLogFilePath()];
        }
    }

    // add .app/Contents/Resources to search path
    //FileUtils::getInstance()->addSearchPath([[NSBundle mainBundle] resourcePath].UTF8String);

    [self adjustEditMenuIndex];

    // app
    _app = new AppDelegate();
    _app->setProjectConfig(_project);
    Application::getInstance()->run();
    // After run, application needs to be terminated immediately.
    [NSApp terminate: self];
}

- (void) openConsoleWindow
{
    if (!_consoleController)
    {
        _consoleController = [[ConsoleWindowController alloc] initWithWindowNibName:@"ConsoleWindow"];
    }
    [_consoleController.window orderFrontRegardless];

    //set console pipe
    _pipe = [NSPipe pipe] ;
    _pipeReadHandle = [_pipe fileHandleForReading] ;

    int outfd = [[_pipe fileHandleForWriting] fileDescriptor];
    if (dup2(outfd, fileno(stderr)) != fileno(stderr) || dup2(outfd, fileno(stdout)) != fileno(stdout))
    {
        perror("Unable to redirect output");
        //        [self showAlert:@"Unable to redirect output to console!" withTitle:@"player error"];
    }
    else
    {
        [[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(handleNotification:) name: NSFileHandleReadCompletionNotification object: _pipeReadHandle] ;
        [_pipeReadHandle readInBackgroundAndNotify] ;
    }
}

- (bool) writeDebugLogToFile:(const string)path
{
    if (_debugLogFile) return true;
    //log to file
    if(_fileHandle) return true;
    NSString *fPath = [NSString stringWithCString:path.c_str() encoding:[NSString defaultCStringEncoding]];
    [[NSFileManager defaultManager] createFileAtPath:fPath contents:nil attributes:nil] ;
    _fileHandle = [NSFileHandle fileHandleForWritingAtPath:fPath];
    [_fileHandle retain];
    return true;
}

- (void)handleNotification:(NSNotification *)note
{
    //NSLog(@"Received notification: %@", note);
    [_pipeReadHandle readInBackgroundAndNotify] ;
    NSData *data = [[note userInfo] objectForKey:NSFileHandleNotificationDataItem];
    NSString *str = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];

    //show log to console
    [_consoleController trace:str];
    if(_fileHandle!=nil){
        [_fileHandle writeData:[str dataUsingEncoding:NSUTF8StringEncoding]];
    }

}

- (void) closeDebugLogFile
{
    if(_fileHandle){
        [_fileHandle closeFile];
        [_fileHandle release];
        _fileHandle = nil;
    }
    if (_debugLogFile)
    {
        close(_debugLogFile);
        _debugLogFile = 0;
        NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
        [nc removeObserver:self];
    }
}

- (void) setZoom:(float)scale
{
    Director::getInstance()->getOpenGLView()->setFrameZoomFactor(scale);
    _project.setFrameScale(scale);
}

-(void) setAlwaysOnTop:(BOOL)alwaysOnTop
{
    NSMenuItem *windowMenu = [[_window menu] itemWithTitle:@"Window"];
    NSMenuItem *menuItem = [[windowMenu submenu] itemWithTitle:@"Always On Top"];
    if (alwaysOnTop)
    {
        [_window setLevel:NSFloatingWindowLevel];
        [menuItem setState:NSOnState];
    }
    else
    {
        [_window setLevel:NSNormalWindowLevel];
        [menuItem setState:NSOffState];
    }
    _isAlwaysOnTop = alwaysOnTop;
}

#pragma mark -
#pragma mark IB Actions

- (IBAction) onFileOpenRecent:(id)sender
{
    NSArray *recents = [[NSUserDefaults standardUserDefaults] objectForKey:@"recents"];
    NSDictionary *recentItem = nil;
    NSString *title = [sender title];
    for (NSInteger i = [recents count] - 1; i >= 0; --i)
    {
        recentItem = [recents objectAtIndex:i];
        if ([title compare:[recentItem objectForKey:@"title"]] == NSOrderedSame)
        {
            [self relaunch:[recentItem objectForKey:@"args"]];
            break;
        }
    }
}

- (IBAction) onFileOpenRecentClearMenu:(id)sender
{
    [[NSUserDefaults standardUserDefaults] setObject:[NSArray array] forKey:@"recents"];
    [self updateUI];
}

- (IBAction) onFileClose:(id)sender
{
    [[NSApplication sharedApplication] terminate:self];
}

- (IBAction) onPlayerWriteDebugLogToFile:(id)sender
{
    bool isWrite = _project.isWriteDebugLogToFile();
    if (!isWrite)
    {
        if ([self writeDebugLogToFile:_project.getDebugLogFilePath()])
        {
            _project.setWriteDebugLogToFile(true);
            [(NSMenuItem*)sender setState:NSOnState];
        }
    }
    else
    {
        _project.setWriteDebugLogToFile(false);
        [self closeDebugLogFile];
        [(NSMenuItem*)sender setState:NSOffState];
    }
}

- (IBAction) onPlayerOpenDebugLog:(id)sender
{
    const string path = _project.getDebugLogFilePath();
    [[NSWorkspace sharedWorkspace] openFile:[NSString stringWithCString:path.c_str() encoding:NSUTF8StringEncoding]];
}

- (IBAction) onPlayerRelaunch:(id)sender
{
    [self relaunch];
}

- (IBAction) onPlayerShowProjectSandbox:(id)sender
{
    [[NSWorkspace sharedWorkspace] openFile:[NSString stringWithCString:FileUtils::getInstance()->getWritablePath().c_str() encoding:NSUTF8StringEncoding]];
}

- (IBAction) onPlayerShowProjectFiles:(id)sender
{
    [[NSWorkspace sharedWorkspace] openFile:[NSString stringWithCString:_project.getProjectDir().c_str() encoding:NSUTF8StringEncoding]];
}

- (IBAction) onScreenChangeFrameSize:(id)sender
{
    NSInteger i = [sender tag];
    if (i >= 0 && i < SimulatorConfig::getInstance()->getScreenSizeCount())
    {
        SimulatorScreenSize size = SimulatorConfig::getInstance()->getScreenSize((int)i);
        _project.setFrameSize(_project.isLandscapeFrame() ?
                              cocos2d::Size(size.height, size.width) :
                              cocos2d::Size(size.width, size.height));
        _project.setFrameScale(1.0f);
        [self relaunch];
    }
}

- (IBAction) onScreenPortait:(id)sender
{
    if ([sender state] == NSOnState) return;
    [sender setState:NSOnState];
    [[[[[_window menu] itemWithTitle:@"Screen"] submenu] itemWithTitle:@"Landscape"] setState:NSOffState];
    _project.changeFrameOrientationToPortait();
    [self relaunch];
}

- (IBAction) onScreenLandscape:(id)sender
{
    if ([sender state] == NSOnState) return;
    [sender setState:NSOnState];
    [[[[[_window menu] itemWithTitle:@"Screen"] submenu] itemWithTitle:@"Portait"] setState:NSOffState];
    _project.changeFrameOrientationToLandscape();
    [self relaunch];
}

- (IBAction) onScreenZoomOut:(id)sender
{
    if ([sender state] == NSOnState) return;
    float scale = (float)[sender tag] / 100.0f;
    [self setZoom:scale];
    [self updateUI];
    [self updateOpenRect];
}

-(IBAction) onWindowAlwaysOnTop:(id)sender
{
    [self setAlwaysOnTop:!_isAlwaysOnTop];
}

-(IBAction)onRelaunch:(id)sender
{
    [self relaunch];
}

@end
