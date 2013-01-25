-- This lua script file represents a lua implementation translation of sample00-mesh with a box instead of a duck.

function initialize()
    -- Display splash screen for at least 1 second.
    ScreenDisplayer.start("drawSplash", 1000)

    _touched = false
    _touchX = 0

	_colouredCubesVolume = GameplayColouredCubesVolume.create(0, 0, 0, 127, 31, 127, 32, 32)

	GameplayVolumeSerialisation.gameplayLoadData("res/level2.vol", _colouredCubesVolume)

    -- Load font
    _font = Font.create("res/arial40.gpb")

    -- Load mesh/scene from file
    local bundle = Bundle.create("res/box.gpb")
    _scene = bundle:loadScene()

    -- Get the box node
    _modelNode = _scene:findNode("box")

    -- Bind the material to the model
    _modelNode:getModel():setMaterial("res/box.material")

    -- Find the light node
    local lightNode = _scene:findNode("directionalLight1")

    -- Bind the light node's direction into the box material.
    _modelNode:getModel():getMaterial():getParameter("u_lightDirection"):bindValue(lightNode, "&Node::getForwardVectorView")

    -- Update the aspect ratio for our scene's camera to match the current device resolution
    local game = Game.getInstance()

	_cameraNode = _scene:addNode();
	_cameraNode:setCamera(_scene:getActiveCamera())
    _scene:getActiveCamera():setAspectRatio(game:getWidth() / game:getHeight())
	_cameraNode:setTranslation(0.0, 0.0, 100.0)

	local dummyValue = 42
	_scene:addNode(_colouredCubesVolume:getRootNodeForLua(dummyValue))

    ScreenDisplayer.finish()
end

function update(elapsedTime)
	local viewPos = Vector3.new(0, 0, 100);
	_colouredCubesVolume:performUpdate(viewPos, 50)
end

-- Avoid allocating new objects every frame.
textColor = Vector4.new(0, 0.5, 1, 1)

function render(elapsedTime)
    -- Clear the color and depth buffers.
    Game.getInstance():clear(Game.CLEAR_COLOR_DEPTH, Vector4.zero(), 1.0, 0)

    -- Visit all the nodes in the scene, drawing the models/mesh.
    _scene:visit("drawScene")

    -- Draw the fps.
    local buffer = string.format("%u", Game.getInstance():getFrameRate())
    _font:start()
    _font:drawText(buffer, 5, 1, textColor, _font:getSize())
    _font:finish()
end

function finalize()
    _font = nil
    _scene = nil
end

function drawScene(node)
    local model = node:getModel()
    if model then
        model:draw()
    end
    return true
end

function drawSplash()
    local game = Game.getInstance()
    game:clear(Game.CLEAR_COLOR_DEPTH, 0, 0, 0, 1, 1.0, 0)
    local batch = SpriteBatch.create("res/logo_powered_white.png")
    batch:start()
    batch:draw(game:getWidth() * 0.5, game:getHeight() * 0.5, 0.0, 512.0, 512.0, 0.0, 1.0, 1.0, 0.0, Vector4.one(), true)
    batch:finish()
end

function keyEvent(evt, key)
    if evt == Keyboard.KEY_PRESS then
        if key == Keyboard.KEY_ESCAPE then
            Game.getInstance():exit()
        end
    end
end

function touchEvent(evt, x, y, contactIndex)
    if evt == Touch.TOUCH_PRESS then
        _touchTime = Game.getAbsoluteTime()
        _touched = true
        _touchX = x
    elseif evt == Touch.TOUCH_RELEASE then
        _touched = false
        _touchX = 0
    elseif evt == Touch.TOUCH_MOVE then
        local deltaX = x - _touchX
        _touchX = x
        _modelNode:rotateY(math.rad(deltaX * 0.5))
    end    
end
