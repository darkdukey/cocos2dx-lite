local LoginView = {}

function LoginView:ctor()
end

function LoginView:layout()
    local root = self.ui:getChildByName('MainPanel')
    root:setContentSize(cc.size(display.width,display.height))
    root:setPosition(display.cx,display.cy)

    root:getnode('version'):pos(0, display.height)

    local webp = display.newSprite('test.webp'):move(display.cx, display.cy):addTo(self.ui)
    local webpLabel = cc.Label:createWithSystemFont('webp=>', 'sans', 28)
    webpLabel:anchor(1,0):addTo(webp)
end

return LoginView
