-- This lua script file represents a lua implementation translation of sample00-mesh with a box instead of a duck.

function initialize()
    -- Display splash screen for at least 1 second.
    -- ScreenDisplayer.start("drawSplash", 1000)

	wPressed = false
	sPressed = false
	aPressed = false
	dPressed = false

    leftMousePressed = false
	rightMousePressed = false
    _touchX = 0
	_touchY = 0

	cameraPitch = 0.0
	cameraYaw = 0.0

	_colouredCubesVolume = GameplayColouredCubesVolume.create(0, 0, 0, 127, 31, 127, 32, 16)

	GameplayVolumeSerialisation.gameplayLoadData("res/level2.vol", _colouredCubesVolume)

    -- Load font
    _font = Font.create("res/arial40.gpb")

    -- Load mesh/scene from file
    local bundle = Bundle.create("res/box.gpb")
    _scene = bundle:loadScene()

    -- Get the box node
    _modelNode = _scene:findNode("box")
	_modelNode:scale(10, 10, 10)

    -- Bind the material to the model
    _modelNode:getModel():setMaterial("res/box.material")

    -- Create the light node
	local lightDirection = Vector3.new(0.75, 0.75, 0.75)
	_light = Light.createDirectional(lightDirection)
	lightNode = Node.create()
	lightNode:setLight(_light)
	lightNode:setTranslation(0.0, 100.0, 0.0)
	lightNode:rotateX(-1.57) -- Point light down
	_scene:addNode(lightNode)

    -- Bind the light node's direction into the box material.
    _modelNode:getModel():getMaterial():getParameter("u_lightDirection"):bindValue(lightNode, "&Node::getForwardVectorView")

    -- Update the aspect ratio for our scene's camera to match the current device resolution
    local game = Game.getInstance()

	--Camera model based on http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Creating+a+simple+first-person+camera+system
	_cameraPositionNode = _scene:addNode()
	_cameraYawNode = Node.create()
	_cameraPositionNode:addChild(_cameraYawNode)
	_cameraPitchNode = Node.create()
	_cameraYawNode:addChild(_cameraPitchNode)
	_cameraNode = Node.create()
	_cameraPitchNode:addChild(_cameraNode)
	_cameraNode:setCamera(_scene:getActiveCamera())
    _scene:getActiveCamera():setAspectRatio(game:getWidth() / game:getHeight())
	_cameraPositionNode:setTranslation(0.0, 0.0, 100.0)

	local dummyValue = 42
	_scene:addNode(_colouredCubesVolume:getRootNodeForLua(dummyValue))

    -- ScreenDisplayer.finish()
end

function update(elapsedTime)
	local forwardVector = _cameraNode:getForwardVectorWorld()
	local rightVector = _cameraNode:getRightVectorWorld()

	local speed = 0.1
	local distance = elapsedTime * speed

	if(wPressed) then
		forwardVector:scale(distance)
		_cameraPositionNode:translate(forwardVector)
	elseif(sPressed) then
		forwardVector:scale(-distance)
		_cameraPositionNode:translate(forwardVector)
	elseif(aPressed) then
		rightVector:scale(-distance)
		_cameraPositionNode:translate(rightVector)
	elseif(dPressed) then
		rightVector:scale(distance)
		_cameraPositionNode:translate(rightVector)
	end

	local viewPos = _cameraPositionNode:getTranslationWorld()
	_colouredCubesVolume:performUpdate(viewPos, 0.5)

	if(_colouredCubesVolume) then
		ray = Ray.new();
		_cameraNode:getCamera():pickRay(Game.getInstance():getViewport(), _touchX, _touchY, ray);

		dir = ray:getDirection()
		dir:scale(200.0)
		ray:setDirection(dir)

		intersection = Vector3.new()
		if(GameplayRaycasting.gameplayRaycast(_colouredCubesVolume, ray, 200.0, intersection)) then
			_modelNode:setTranslation(intersection)
		end
	end
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
	-- Skip note if the flag is set to false.
	local nodeTag = tostring(node:getTag("RenderThisNode"))
	if(nodeTag == "f") then
	 	return true
	end

	local model = node:getModel()
    if model then
		model:getMaterial():getParameter("u_lightColor"):setValue(_light:getColor());
		model:getMaterial():getParameter("u_lightDirection"):setValue(lightNode:getForwardVectorWorld());
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
		elseif key == Keyboard.KEY_W then
			wPressed = true;
		elseif key == Keyboard.KEY_A then
			aPressed = true;
		elseif key == Keyboard.KEY_S then
			sPressed = true;
		elseif key == Keyboard.KEY_D then
			dPressed = true;
        end
	elseif evt == Keyboard.KEY_RELEASE then
		if key == Keyboard.KEY_W then
			wPressed = false;
		elseif key == Keyboard.KEY_A then
			aPressed = false;
		elseif key == Keyboard.KEY_S then
			sPressed = false;
		elseif key == Keyboard.KEY_D then
			dPressed = false;
        end

    end
end

function mouseEvent(evt, x, y, wheelDelta)
    if evt == Mouse.MOUSE_PRESS_LEFT_BUTTON then
        leftMousePressed = true
    elseif evt == Mouse.MOUSE_RELEASE_LEFT_BUTTON then
        leftMousePressed = false
	elseif evt == Mouse.MOUSE_PRESS_RIGHT_BUTTON then
        rightMousePressed = true
    elseif evt == Mouse.MOUSE_RELEASE_RIGHT_BUTTON then
        rightMousePressed = false
    elseif evt == Mouse.MOUSE_MOVE  then
        local deltaX = x - _touchX
		local deltaY = y - _touchY
        _touchX = x
		_touchY = y
		if(rightMousePressed) then
			_cameraYawNode:rotateY(math.rad(deltaX * -0.5))
			_cameraPitchNode:rotateX(math.rad(deltaY * -0.5))
		end
    end  
	return true;  
end
