#include "Main.h"

#include "GameplayColouredCubesVolume.h"
#include "GameplaySmoothTerrainVolume.h"
#include "GameplayRaycasting.h"

#include "GameplayVolumeSerialisation.h"

#include "MainThreadTaskProcessor.h"

#include <algorithm>
#include <sstream>

//using namespace PolyVox;
using namespace std;
using namespace Cubiquity;

//Rotates the goven node to point at the target.
//NOTE - Might only work if node position and target are in world space?
void pointNodeAtTarget(Node* node, const Vector3& target, const Vector3& up = Vector3(0.0f, 1.0f, 0.0f))
{
	//See http://www.gamedev.net/topic/429507-finding-the-quaternion-betwee-two-vectors/page__p__3856121#entry3856121
	Vector3 oldDir = node->getForwardVector();
	oldDir.normalize();

	Vector3 newDir = target - node->getTranslation();
	newDir.normalize();

	Vector3 crossProd;
	Vector3::cross(oldDir, newDir, &crossProd);
	crossProd.normalize();

	Quaternion rotation(crossProd, acos(oldDir.dot(newDir)));
	
	node->rotate(rotation);
}

void lookAt(Node* node, const Vector3& eye, const Vector3& target, const Vector3& up)
{
    // Create lookAt matrix
    Matrix matrix;
    Matrix::createLookAt(eye, target, up, &matrix);
    matrix.invert();  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< add this line

    // Pull SRT components out of matrix
    Vector3 scale;
    Quaternion rotation;
    Vector3 translation;
    matrix.decompose(&scale, &rotation, &translation);

    // Set SRT on node
    node->setScale(scale);
    node->setTranslation(translation);
    node->setRotation(rotation);
}

// Declare our game instance
MeshGame game;

MeshGame::MeshGame()
	:_font(NULL), mLastX(0), mLastY(0), mTimeBetweenUpdatesInSeconds(0.0f), mScreenPressed(false), mSphereVisible(false), mSelectedMaterial(0), mSphereNode(0)
	,mLeftMouseButtonPressed(0)
	,mRightMouseButtonPressed(0)
{
}

MeshGame::~MeshGame()
{
}

void MeshGame::initialize()
{   
	FileSystem::setResourcePath("C:/Code/cubiquity/CubiquityForGameplay/");

    // Load font
    _font = Font::create("res/Fonts/arial40.gpb");

	// Load the form for editing ParticleEmitters.
    mForm = Form::create("res/Forms/CppExample.form");
    mForm->setConsumeInputEvents(false);

	mRotateButton = (RadioButton*)mForm->getControl("RotateButton");
	mPaintButton = (RadioButton*)mForm->getControl("PaintButton");
    mSmoothButton = (RadioButton*)mForm->getControl("SmoothButton");
	mAddButton = (RadioButton*)mForm->getControl("AddButton");
	mSubtractButton = (RadioButton*)mForm->getControl("SubtractButton");

	mZoomInButton = (Button*)mForm->getControl("ZoomInButton");
	mZoomOutButton = (Button*)mForm->getControl("ZoomOutButton");

	mMat0Button = (Button*)mForm->getControl("Mat0Button");
	mMat1Button = (Button*)mForm->getControl("Mat1Button");
	mMat2Button = (Button*)mForm->getControl("Mat2Button");
	mMat3Button = (Button*)mForm->getControl("Mat3Button");

	mLod1StartSlider = (Slider*)mForm->getControl("Lod1StartSlider");
	mBrushSizeSlider = (Slider*)mForm->getControl("BrushSizeSlider");
	mBrushIntensitySlider = (Slider*)mForm->getControl("BrushIntensitySlider");
	mSmoothBiasSlider = (Slider*)mForm->getControl("SmoothBiasSlider");

	mWireframeCheckBox = (CheckBox*)mForm->getControl("WireframeCheckBox");

	mTasks = (Label*)mForm->getControl("Tasks");

	mZoomInButton->addListener(this, Listener::PRESS);
	mZoomOutButton->addListener(this, Listener::PRESS);

	mMat0Button->addListener(this, Listener::PRESS);
	mMat1Button->addListener(this, Listener::PRESS);
	mMat2Button->addListener(this, Listener::PRESS);
	mMat3Button->addListener(this, Listener::PRESS);

	_scene = Scene::create();

	// Load mesh/scene from file
    Bundle* bundle = Bundle::create("res/Models/Icosphere3.gpb");
	Mesh* sphere = bundle->loadMesh("Sphere_001");
    SAFE_RELEASE(bundle);

	Model* model = Model::create(sphere);
	model->setMaterial("res/Materials/VertexColouredMesh.material");
	mSphereNode = Node::create();
	mSphereNode->setModel(model);
	float scale = mBrushSizeSlider->getValue();
	mSphereNode->setScale(scale, scale, scale);
	_scene->addNode(mSphereNode);

	//Create some physics object for testing.
	Bundle* boxBundle = Bundle::create("res/Models/box.gpb");
	Mesh* objectMesh = boxBundle->loadMesh("boxShape");
	for(int z = 0; z < 128; z += 40)
	{
		for(int x = 0; x < 128; x += 40)
		{
			Model* objectModel = Model::create(objectMesh);
			objectModel->setMaterial("res/Materials/VertexColouredMesh.material");

			Node* objectNode = Node::create();
			objectNode->setModel(objectModel);
			_scene->addNode(objectNode);
			objectNode->translate(x, 100.0, z);

			PhysicsRigidBody::Parameters boxParams;
			boxParams.mass = 10.0f;
			objectNode->setCollisionObject(PhysicsCollisionObject::RIGID_BODY, PhysicsCollisionShape::box(), &boxParams);
		}
	}

    // Find the light node
	_light = Light::createDirectional(Vector3(0.75, 0.75, 0.75));
	_lightNode = Node::create();
	_lightNode->setLight(_light);
	_lightNode->setTranslation(70.0f, 100.0f, 50.0f);
	pointNodeAtTarget(_lightNode, Vector3(0.0f, 0.0f, 0.0f));
	_scene->addNode(_lightNode);

	_cameraPositionNode = _scene->addNode();
	_cameraYawNode = Node::create();
	_cameraPositionNode->addChild(_cameraYawNode);
	_cameraPitchNode = Node::create();
	_cameraYawNode->addChild(_cameraPitchNode);
	_cameraNode = Node::create();
	_cameraPitchNode->addChild(_cameraNode);

	Camera* camera = Camera::createPerspective(60.0f, 1.0f, 0.1f, 1000.0f);
	//_cameraNode = _scene->addNode();
	_cameraNode->setCamera(camera);
	_scene->setActiveCamera(camera);
	_scene->getActiveCamera()->setAspectRatio((float)getWidth() / (float)getHeight());

	
	//_cameraPositionNode->rotateX(3.142f * 0.5f);

	mCameraElevationAngle = MATH_DEG_TO_RAD(45.0f); //Value from voxeliens
	mCameraRotationAngle = 0.0f; //Value from voxeliens
	mCameraDistance = 145.0f; //Value from voxeliens

	// Create the volume and add it to the scene.
#ifdef TERRAIN_SMOOTH
	//mVolume = GameplaySmoothTerrainVolume::create(0, 0, 0, 127, 31, 127, 32, 16);
	//mVolume = GameplayVolumeSerialisation::gameplayImportSmoothSlices("../../SliceData/SmoothVoxeliensTerrain/");
	
	//mVolume = GameplaySmoothTerrainVolume::create("C:/Code/cubiquity/Examples/SliceData/SmoothVoxeliensTerrain/", "./", 32);
	mVolume = GameplaySmoothTerrainVolume::create(0, 0, 0, 127, 31, 127, "C:/temp/volume", 32, true, 8);
	mVolumeEditor = new GameplaySmoothTerrainVolumeEditor(mVolume);
#endif
#ifdef TERRAIN_CUBIC
	//mVolume = GameplayColouredCubesVolume::create(0, 0, 0, 127, 31, 127, 32, 16);
	//mVolume = GameplayVolumeSerialisation::gameplayImportColourSlices("../../SliceData/VoxeliensTerrain/");
	//mVolume = GameplayVolumeSerialisation::gameplayImportVxl("D:\\Google Drive\\VolumesOfFun\\VolumeData\\Industry.vxl");
	//mVolume = GameplayColouredCubesVolume::create("../../SliceData/VoxeliensTerrain/", "./", 32);
	//mVolume = GameplayColouredCubesVolume::create("../../SliceData/VoxeliensTerrain/", 32, "./");
	//mVolume = GameplayColouredCubesVolume::create("C:\\code\\cubiquity\\Tools\\FractalGenerator\\FractalGenerator\\output\\");
	//mVolume = GameplayColouredCubesVolume::create("D:\\Google Drive\\VolumesOfFun\\VolumeData\\Industry.vxl", "D:\\temp\\voldata\\", 16);

	mVolume = GameplayColouredCubesVolume::create("C:/Code/cubiquity/Examples/SliceData/VoxeliensTerrain/", "./", 32);

	mVolumeEditor = 0;
#endif

	//GameplayVolumeSerialisation::gameplayExportSmoothSlices(mVolume, "C:\\temp\\output");
	//GameplayVolumeSerialisation::gameplayExportColourSlices(mVolume, "D:/temp/mytest/");

	_cameraPositionNode->setTranslation(mVolume->getWidth() / 2.0f, mVolume->getHeight(), mVolume->getDepth());

	//_cameraPositionNode->setTranslation(10.0f, 10.0f, 10.0f);
	
	_polyVoxNode = mVolume->getRootNode();
	_scene->addNode(mVolume->getRootNode());
}

void MeshGame::finalize()
{
    SAFE_RELEASE(_font);
    SAFE_RELEASE(_scene);
	SAFE_RELEASE(mVolume);
}

void MeshGame::update(float elapsedTime)
{
	mTimeBetweenUpdatesInSeconds = elapsedTime;

	Vector3 forwardVector = _cameraNode->getForwardVectorWorld();
	Vector3 rightVector = _cameraNode->getRightVectorWorld();

	float speed = 0.1f;
	float distance = elapsedTime * speed;

	if(mPressedKeys.test(Keyboard::KEY_W))
	{
		forwardVector.scale(distance);
		_cameraPositionNode->translate(forwardVector);
	}
	else if(mPressedKeys.test(Keyboard::KEY_S))
	{
		forwardVector.scale(-distance);
		_cameraPositionNode->translate(forwardVector);
	}
	else if(mPressedKeys.test(Keyboard::KEY_A))
	{
		rightVector.scale(-distance);
		_cameraPositionNode->translate(rightVector);
	}
	else if(mPressedKeys.test(Keyboard::KEY_D))
	{
		rightVector.scale(distance);
		_cameraPositionNode->translate(rightVector);
	}

	if((mPaintButton->isSelected()) || (mSmoothButton->isSelected()))
	{
		mSphereVisible = true;
	}

	Ray ray;
	_cameraNode->getCamera()->pickRay(getViewport(), mLastX, mLastY, &ray);

	Vector3 dir = ray.getDirection();
	dir *= 200.0f;
	ray.setDirection(dir);

	Vector3 intersection;
	if(GameplayRaycasting::gameplayRaycast(mVolume, ray, 200.0f, intersection))
	{
		/*dir.normalize();
		dir *= 2.0f;*/
		mSphereNode->setTranslation(intersection/* + dir*/);
	}

#ifdef TERRAIN_SMOOTH
	if(mLeftMouseButtonPressed)
	{		 
		if(mPaintButton->isSelected())
		{
			mVolumeEditor->applyPaint(mSphereNode->getTranslation(), mBrushSizeSlider->getValue(), mSelectedMaterial, mTimeBetweenUpdatesInSeconds, mBrushIntensitySlider->getValue());
		}
		if(mSmoothButton->isSelected())
		{
			mVolumeEditor->smooth(mSphereNode->getTranslation(), mBrushSizeSlider->getValue(), mTimeBetweenUpdatesInSeconds, mSmoothBiasSlider->getValue(), mBrushIntensitySlider->getValue());
		}
		if(mAddButton->isSelected())
		{
			mVolumeEditor->addMaterial(mSphereNode->getTranslation(), mBrushSizeSlider->getValue(), mSelectedMaterial, mTimeBetweenUpdatesInSeconds, mBrushIntensitySlider->getValue());
		}
		if(mSubtractButton->isSelected())
		{
			mVolumeEditor->subtractMaterial(mSphereNode->getTranslation(), mBrushSizeSlider->getValue(), mTimeBetweenUpdatesInSeconds, mBrushIntensitySlider->getValue());
		}
	}
#endif

	/*_cameraNode->setTranslation(64.0f, 64.0f, 16.0f);
	_cameraNode->setRotation(Quaternion::identity());
	_cameraNode->rotateX(3.14f / 2.0f); //Why negative?
	_cameraNode->rotateY(mCameraRotationAngle);
	_cameraNode->rotateX(-mCameraElevationAngle); //Why negative?
	
	_cameraNode->translate(_cameraNode->getForwardVector() * -mCameraDistance);*/

	Vector3 viewPos = _cameraNode->getTranslationWorld();
	mVolume->performUpdate(viewPos, mLod1StartSlider->getValue());

	std::stringstream ss;
	ss << "Tasks = " << gBackgroundTaskProcessor.mPendingTasks.size();
	mTasks->setText(ss.str().c_str());

	// Update UI.
    mForm->update(elapsedTime);
}

void MeshGame::render(float elapsedTime)
{
    // Clear the color and depth buffers.
    clear(CLEAR_COLOR_DEPTH, Vector4::zero(), 1.0f, 0);
    
    // Visit all the nodes in the scene, drawing the models/mesh.
    _scene->visit(this, &MeshGame::drawScene);

	// Draw the UI.
    mForm->draw();

    // Draw the fps
    drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 5, 1, getFrameRate());
}

void MeshGame::controlEvent(Control* control, EventType evt)
{
	switch(evt)
    {
		case Listener::PRESS:
		{
			if(control == mZoomInButton)
			{
				//wheelDelta *= 10; //To match Voxeliens
				int wheelDelta = 10;
				// Pushing forward (positive wheelDelta) should reduce distance to world.
				mCameraDistance -= wheelDelta;
				//Values copied from Voxeliens
				//mCameraDistance = min(mCameraDistance, 200.0f);
				//mCameraDistance = max(mCameraDistance, 91.0f); //sqrt(64*64+64*64) to stop camera clipping with volume
			}
			else if(control == mZoomOutButton)
			{
				//wheelDelta *= 10; //To match Voxeliens
				int wheelDelta = 10;
				mCameraDistance += wheelDelta;
				//Values copied from Voxeliens
				mCameraDistance = min(mCameraDistance, 200.0f);
				mCameraDistance = max(mCameraDistance, 91.0f); //sqrt(64*64+64*64) to stop camera clipping with volume
			}
			else if(control == mMat0Button)
			{
				mSelectedMaterial = 0;
			}
			else if(control == mMat1Button)
			{
				mSelectedMaterial = 1;
			}
			else if(control == mMat2Button)
			{
				mSelectedMaterial = 2;
			}
			else if(control == mMat3Button)
			{
				mSelectedMaterial = 3;
			}

			break;
		}
		case Listener::VALUE_CHANGED:
		{
			if(control == mBrushSizeSlider)
			{
				float scale = mBrushSizeSlider->getValue();
				if(mSphereNode)
				{
					mSphereNode->setScale(scale, scale, scale);
				}
			}
		}
	}
}

void MeshGame::keyEvent(Keyboard::KeyEvent evt, int key)
{
	bool keyPressed = (evt == Keyboard::KEY_PRESS);
	bool keyReleased = (evt == Keyboard::KEY_RELEASE);

	if(keyPressed || keyReleased)
	{
		if((key >= 0) && (key < mNoOfKeys))
		{
			mPressedKeys.set(key, keyPressed);
		}
		else
		{
			GP_WARN("'key' is outside of valid range.");
		}
	}

    if(keyPressed)
    {
        switch (key)
        {
        case Keyboard::KEY_ESCAPE:
            exit();
            break;					
        }
    }	
}

bool MeshGame::mouseEvent (Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
	if(evt == Mouse::MOUSE_PRESS_LEFT_BUTTON)
	{
		mLeftMouseButtonPressed = true;
	}
	else if(evt == Mouse::MOUSE_RELEASE_LEFT_BUTTON)
	{
		mLeftMouseButtonPressed = false;
	}
	else if(evt == Mouse::MOUSE_PRESS_RIGHT_BUTTON)
	{
		mRightMouseButtonPressed = true;
	}
	else if(evt == Mouse::MOUSE_RELEASE_RIGHT_BUTTON)
	{
		mRightMouseButtonPressed = false;
	}
	else if(evt == Mouse::MOUSE_MOVE)
	{
        int deltaX = x - mLastX;
		int deltaY = y - mLastY;
		if(mRightMouseButtonPressed)
		{
			_cameraYawNode->rotateY(MATH_DEG_TO_RAD(deltaX * -0.5));
			_cameraPitchNode->rotateX(MATH_DEG_TO_RAD(deltaY * -0.5));
		}
	}

	mLastX = x;
	mLastY = y;

	return true;
}

bool MeshGame::drawScene(Node* node)
{
	if((node == mSphereNode) && (mSphereVisible == false))
	{
		return true;
	}

	const char* nodeTag = node->getTag("RenderThisNode");
	if(nodeTag != NULL)
	{
		if(strcmp(nodeTag, "f") == 0)
		{
			return true;
		}
	}

    Model* model = node->getModel();
    if (model)
	{
		model->getMaterial()->getParameter("u_lightColor")->setValue(_light->getColor());
		Vector3 lightVector = _lightNode->getForwardVectorWorld();
		lightVector.negate(); // NOTE: Negated to point *towards* light.
		model->getMaterial()->getParameter("u_worldSpaceLightVector")->setValue(lightVector);
		model->draw(mWireframeCheckBox->isChecked());
	}
    return true;
}

void MeshGame::drawFrameRate(Font* font, const Vector4& color, unsigned int x, unsigned int y, unsigned int fps)
{
    char buffer[10];
    sprintf(buffer, "%u", fps);
    font->start();
    font->drawText(buffer, x, y, color, font->getSize());
    font->finish();
}

void MeshGame::moveCamera(int x, int y)
{
	int deltaX = x -  mLastX;
	int deltaY = y -  mLastY;
	mLastX = x;
	mLastY = y;
	
	float cameraSensitivity = 0.01f;
	mCameraRotationAngle -= (deltaX * cameraSensitivity);
	mCameraElevationAngle += (deltaY * cameraSensitivity);
	
	//mCameraElevationAngle = min(mCameraElevationAngle, MATH_DEG_TO_RAD(70.0f)); //Value from voxeliens
	//mCameraElevationAngle = max(mCameraElevationAngle, MATH_DEG_TO_RAD(-5.0f)); //Value from voxeliens
}
