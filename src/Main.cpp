#include "Main.h"
#include "Volume.h"

#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"

using namespace PolyVox;

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

// Declare our game instance
MeshGame game;

MeshGame::MeshGame()
    : _font(NULL), _touchX(0), _touchY(0)
{
}

MeshGame::~MeshGame()
{
}

void MeshGame::initialize()
{    
    // Load font
    _font = Font::create("res/arial40.gpb");

	_scene = Scene::createScene();

    // Find the light node
	_light = Light::createDirectional(Vector3(1.0, 1.0, 1.0));
	_lightNode = Node::create();
	_lightNode->setLight(_light);
	_lightNode->setTranslation(0.0f, 100.0f, 0.0f);
	pointNodeAtTarget(_lightNode, Vector3(0.0f, 0.0f, 0.0f));
	_scene->addNode(_lightNode);

	Camera* camera = Camera::createPerspective(60.0f, 1.0f, 0.1f, 1000.0f);
	_cameraNode = _scene->addNode();
	_cameraNode->setCamera(camera);
	_scene->setActiveCamera(camera);
	_scene->getActiveCamera()->setAspectRatio((float)getWidth() / (float)getHeight());

	mCameraElevationAngle = MATH_DEG_TO_RAD(30.0f); //Value from voxeliens
	mCameraRotationAngle = 0.0f; //Value from voxeliens
	mCameraDistance = 145.0f; //Value from voxeliens

	// Create the volume and add it to the scene.
	Volume<Material16>* volume = Volume<Material16>::create(VolumeTypes::ColouredCubes, 0, 0, 0, 127, 31, 127, 32, 32, 32);
	//Rather dirty hack until I figure out how to package volume data with gameplay
#ifdef WIN32
	volume->loadData("res/level2.vol");
#else
	volume->loadData("/sdcard/external_sd/level2.vol");
#endif
	volume->updateMeshes();
	_polyVoxNode = volume->getRootNode();
	_scene->addNode(volume->getRootNode());
}

void MeshGame::finalize()
{
    SAFE_RELEASE(_font);
    SAFE_RELEASE(_scene);
}

void MeshGame::update(float elapsedTime)
{
	_cameraNode->setTranslation(64.0f, 16.0f, 64.0f);
	_cameraNode->setRotation(Quaternion::identity());
	_cameraNode->rotateY(mCameraRotationAngle);
	_cameraNode->rotateX(-mCameraElevationAngle); //Why negative?
	
	_cameraNode->translate(_cameraNode->getForwardVector() * -mCameraDistance);
}

void MeshGame::render(float elapsedTime)
{
    // Clear the color and depth buffers.
    clear(CLEAR_COLOR_DEPTH, Vector4::zero(), 1.0f, 0);
    
    // Visit all the nodes in the scene, drawing the models/mesh.
    _scene->visit(this, &MeshGame::drawScene);

    // Draw the fps
    drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 5, 1, getFrameRate());
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

void MeshGame::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    switch (evt)
    {
    case Touch::TOUCH_PRESS:
        {
            _touchX = x;
			_touchY = y;
        }
        break;
    case Touch::TOUCH_RELEASE:
        {
            _touchX = 0;
			_touchY = 0;
        }
        break;
    case Touch::TOUCH_MOVE:
        {
            int deltaX = x - _touchX;
			int deltaY = y - _touchY;
            _touchX = x;
            _touchY = y;
			
			float cameraSensitivity = 0.01f;
			mCameraRotationAngle -= (deltaX * cameraSensitivity);
			mCameraElevationAngle += (deltaY * cameraSensitivity);

			mCameraElevationAngle = min(mCameraElevationAngle, MATH_DEG_TO_RAD(70.0f)); //Value from voxeliens
			mCameraElevationAngle = max(mCameraElevationAngle, MATH_DEG_TO_RAD(-5.0f)); //Value from voxeliens

			/*float cameraSpeed = 0.5f;
			Vector3 cameraMovement(0.0f, 0.0f, 0.0f);

			cameraMovement += _cameraNode->getRightVector() * cameraSpeed * deltaX;
			//cameraMovement += _cameraNode->getUpVector() * cameraSpeed * deltaY;

			Vector3 cameraPos = _cameraNode->getTranslation();
			cameraPos += cameraMovement;
			_cameraNode->setTranslation(cameraPos);	
			pointNodeAtTarget(_cameraNode, Vector3(64.0f, 4.0f, 64.0f));*/
        }
        break;
    default:
        break;
    };
}

bool MeshGame::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
	wheelDelta *= 10; //To match Voxeliens

	// Pushing forward (positive wheelDelta) should reduce distance to world.
	mCameraDistance -= wheelDelta;

	//Values copied from Voxeliens
	mCameraDistance = min(mCameraDistance, 200.0f);
	mCameraDistance = max(mCameraDistance, 91.0f); //sqrt(64*64+64*64) to stop camera clipping with volume

	return false;
}

bool MeshGame::drawScene(Node* node)
{
    Model* model = node->getModel();
    if (model)
	{
		model->getMaterial()->getParameter("u_lightColor")->setValue(_light->getColor());
		model->getMaterial()->getParameter("u_lightDirection")->setValue(_lightNode->getForwardVectorWorld());
        model->draw();
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