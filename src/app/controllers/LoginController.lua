
local Controller = require('mvc.Controller')
local LoginController = class("LoginController", Controller)

function LoginController:ctor()
    LoginController.super.ctor(self)
    cc.load('event'):bind(self)
end

function LoginController:viewDidLoad()
    self.view:layout()
end

function LoginController:finalize()
    print('LoginController:finalize')
end

function LoginController:clickLogin( ... )
    print('clickLogin')
    local app = require('app.App'):instance()
    app:switch('LobbyController')
end

function LoginController:clickTouristLogin( ... )
    print('clickTouristLogin')

    local audio = require 'fmod'
    audio.playEffect('audio/background-music-aac.mp3')
end

-- event binding end ~~

return LoginController
