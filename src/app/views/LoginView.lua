local LoginView = {}

function LoginView:ctor()
end

function LoginView:layout()
    local root = self.ui:getChildByName('MainPanel')
    root:setContentSize(cc.size(display.width,display.height))
    root:setPosition(display.cx,display.cy)

    root:getnode('version'):pos(0, display.height)
end

return LoginView
