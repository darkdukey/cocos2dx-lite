
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


local function main()
    require("app.MyApp"):create():run()

    -- pbc test
    require 'test.test'
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    print(msg)
end
