
local MainScene = class("MainScene", cc.load("mvc").ViewBase)
local audio = require 'fmod'

function MainScene:onCreate()
    -- add background image
    display.newSprite("HelloWorld.png")
        :move(display.center)
        :addTo(self)

    -- add HelloWorld label
    cc.Label:createWithSystemFont("Hello World", "Arial", 40)
        :move(display.cx, display.cy + 200)
        :addTo(self)

    dump(audio)
    audio.playBackgroundMusic('audio/background-music-aac.mp3', true)
end

return MainScene
