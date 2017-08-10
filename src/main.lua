
package.path = 'src/?.lua;src/packages/?.lua'
local fu = cc.FileUtils:getInstance()
fu:setPopupNotify(false)
fu:addSearchPath("res/")
fu:addSearchPath("src/")


print = release_print
require "config"
require "cocos.init"

require 'pack'
require 'pbc.pbc'


__G__TRACKBACK__ = function ( ... )
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
    require("app.MyApp"):create():run()

    -- pbc test
    require 'test.test'
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    print(msg)
end
