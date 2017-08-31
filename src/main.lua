
package.path = '?.lua;src/?.lua;src/packages/?.lua'
local fu = cc.FileUtils:getInstance()
fu:setPopupNotify(false)
fu:addSearchPath("res/")
fu:addSearchPath("src/")


print = release_print
require "config"
require "cocos.init"

require 'pack'
require 'pbc.pbc'


__G__TRACKBACK__ = function ( msg )

    local message = msg

    local msg = debug.traceback(msg, 3)
    print(msg)

    -- report lua exception
    if device.platform == 'ios' then
        buglyReportLuaException(tostring(message), debug.traceback())
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
