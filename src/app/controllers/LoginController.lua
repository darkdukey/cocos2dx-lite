
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
end

-- event binding end ~~

return LoginController
