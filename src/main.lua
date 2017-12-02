
package.path = '?.lua;src/?.lua;src/packages/?.lua'
local fu = cc.FileUtils:getInstance()
fu:setPopupNotify(false)
fu:addSearchPath("res/")
fu:addSearchPath("src/")

print = release_print
function currfileline(  )
    local t = debug.getinfo(2)
    local text = string.format('%s:%d', t.source, t.currentline)
    return text
end
function printfileline(  )
    print(currfileline())
end

local STP = require "StackTracePlus"
local _traceback = debug.traceback
debug.traceback = STP.stacktrace

local winsize = cc.Director:getInstance():getWinSize()
local target = cc.Application:getInstance():getTargetPlatform()
__G__TRACKBACK__ = function ( msg )

    local message = msg
    local msg, origin_error = debug.traceback(msg, 3)
    print(msg)

    -- report lua exception
    if target == 4 or target == 5 then -- ios
        buglyReportLuaException(tostring(message), _traceback())
    elseif target == 2 then -- mac
        local msg = debug.getinfo(2)
        local info = cc.Label:createWithSystemFont(message, 'sans', 32)
        info:setWidth(winsize.width)
        info:setAnchorPoint({x=0,y=1})
        info:setPosition(0, winsize.height)
        info:setTextColor({r=255,g=0,b=0,a=255})

        local scene = cc.Director:getInstance():getRunningScene()
        if not scene then
            scene = cc.Scene:create()
            cc.Director:getInstance():runWithScene(scene)
        end
        scene:addChild(info, 998)

        local function onTouchBegan(touch, event)
            return true
        end
        local function onTouchEnded(touch, event)
            local cmd = string.format('subl %s:%d', cc.FileUtils:getInstance():fullPathForFilename(msg.source), msg.currentline)
            os.execute(cmd)
            info:removeSelf()
        end

        local listener = cc.EventListenerTouchOneByOne:create()
        listener:registerScriptHandler(onTouchBegan,40)--cc.Handler.EVENT_TOUCH_BEGAN
        listener:registerScriptHandler(onTouchEnded,42)--cc.Handler.EVENT_TOUCH_ENDED
        local eventDispatcher = info:getEventDispatcher()
        eventDispatcher:addEventListenerWithSceneGraphPriority(listener, info)
    end

    return msg
end

local function main()
    require "config"
    require "cocos.init"

    require 'pack'
    require 'pbc.pbc'
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

    local xxtea = require 'xxtea'
    local s = xxtea.encrypt('abc', 'xx')
    print(xxtea.decrypt(s, 'xx') == 'abc')

    local md5 = require 'md5'
    print(md5.sum('helloworld'))

    -- pbc test
    -- require 'test.test'

    leafTest()
end

function leafTest(  )
    local cjson = require 'cjson'
    local ws = cc.WebSocket:create('ws://127.0.0.1:3653')
    ws:registerScriptHandler(function (  )
        print('open')
        ws:sendString(cjson.encode({Hello={Name='from cocos2dx-lite'}}))

    end, cc.WEBSOCKET_OPEN)
    ws:registerScriptHandler(function ( msg )
        print('message', msg)

        local buf = {}
        for i,v in ipairs(msg) do
            buf[#buf+1] = string.char(v)
        end
        local json = table.concat(buf)
        local gameMsg = cjson.decode(json)
        dump(gameMsg, 'game msg')

    end, cc.WEBSOCKET_MESSAGE)
    ws:registerScriptHandler(function (  )
        print('close')
    end, cc.WEBSOCKET_CLOSE)
    ws:registerScriptHandler(function (  )
        print('error')
    end, cc.WEBSOCKET_ERROR)
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    print(msg)
end
