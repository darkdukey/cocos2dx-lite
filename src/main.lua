
package.path = '?.lua;src/?.lua;src/packages/?.lua'
local fu = cc.FileUtils:getInstance()
fu:setPopupNotify(false)
fu:addSearchPath("res/")
fu:addSearchPath("src/")


print = release_print

local STP = require "StackTracePlus"
local _traceback = debug.traceback
debug.traceback = STP.stacktrace

require "config"
require "cocos.init"

require 'pack'
require 'pbc.pbc'


__G__TRACKBACK__ = function ( msg )

    local message = msg
    local msg, origin_error = debug.traceback(msg, 3)
    print(msg)

    -- report lua exception
    if device.platform == 'ios' then
        buglyReportLuaException(tostring(message), _traceback())
    elseif device.platform == 'mac' then
        local msg = debug.getinfo(2)
        local info = cc.Label:createWithSystemFont(message, 'sans', 32)
        info:setWidth(display.width)
        info:setAnchorPoint(cc.p(0,1))
        info:setPosition(0, display.height)
        info:setTextColor(cc.c4b(255,0,0,255))
        cc.Director:getInstance():getRunningScene():addChild(info, 998)

        local function onTouchBegan(touch, event)
            return true
        end
        local function onTouchEnded(touch, event)
            os.execute(string.format('subl %s:%d', cc.FileUtils:getInstance():fullPathForFilename(msg.source), msg.currentline))
            info:removeSelf()
        end

        local listener = cc.EventListenerTouchOneByOne:create()
        listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN)
        listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED )
        local eventDispatcher = info:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(listener, info)
    end

    return msg
end

local function main()
    if CC_REMOTE_DEBUG then
        -- local ZBS = ''
        -- if device.platform == 'mac' then
            -- ZBS = '/Applications/ZeroBraneStudio.app/Contents/ZeroBraneStudio'
        -- end
        -- package.path = package.path..string.gsub(';%ZBS%/lualibs/?/?.lua;%ZBS%/lualibs/?.lua', '%%ZBS%%', ZBS)
        -- package.cpath = package.cpath..string.gsub('%ZBS%/bin/?.dll;%ZBS%/bin/clibs/?.dll', '%%ZBS%%', ZBS)
        require('mobdebug').start()
    end

    local app = require('app.App'):instance()
    app:run('LoginController')

    local audio = require 'fmod'
    audio.playBackgroundMusic('audio/background-music-aac.mp3', true)
    -- pbc test
    -- require 'test.test'
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    print(msg)
end
