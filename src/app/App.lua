
local Application = cc.load("mvc").Application
local App = class("App", Application)

local app = nil
function App:ctor(  )
	app = self
end

function App.instance()
	print('App.instance')
	app = app or App.new()
	return app
end

function App:onCreate()
    math.randomseed(os.time())
end

return App
