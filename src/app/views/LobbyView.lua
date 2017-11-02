
local LobbyView = {}
local RubCardFunc = require('src.packages.RubCardLayer')


function LobbyView:ctor()
  self.ui:setPosition(display.cx, display.cy)
end

function LobbyView:layout()

    local layer = RubCardFunc("res/pokes.plist", "card_back.png", "card_fort.png", 1.0)
    self:addChild(layer)
end

return LobbyView

