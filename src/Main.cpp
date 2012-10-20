#include "Main.h"

#include "ColouredCubesVolume.h"
#include "SmoothTerrainVolume.h"

#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"

#include <algorithm>

using namespace PolyVox;
using namespace std;

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
	: _font(NULL), mLastX(0), mLastY(0), mRightMouseDown(false), mLeftMouseDown(false), mTimeBetweenUpdates(0.0f)
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

	// Load mesh/scene from file
    Bundle* bundle = Bundle::create("res/Icosphere3.gpb");
	Mesh* sphere = bundle->loadMesh("Sphere_001");
    SAFE_RELEASE(bundle);

	Model* model = Model::create(sphere);
	model->setMaterial("res/Icosphere3.material");
	mSphereNode = Node::create();
	mSphereNode->setModel(model);
	mSphereNode->setScale(5, 5, 5);
	_scene->addNode(mSphereNode);

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
#ifdef TERRAIN_SMOOTH
	mVolume = SmoothTerrainVolume::create(VolumeTypes::SmoothTerrain, 0, 0, 0, 127, 31, 127, 32, 32, 32);
#endif
#ifdef TERRAIN_CUBIC
	mVolume = ColouredCubesVolume::create(VolumeTypes::ColouredCubes, 0, 0, 0, 127, 31, 127, 32, 32, 32);
#endif

	//Rather dirty hack until I figure out how to package volume data with gameplay
#ifdef WIN32
	if(mVolume->getType() == VolumeTypes::SmoothTerrain)
	{
		mVolume->loadData("res/level2MultiMaterial.vol");
	}
	else
	{
		mVolume->loadData("res/level2.vol");
	}
#else
	if(mVolume->getType() == VolumeTypes::SmoothTerrain)
	{
		mVolume->loadData("/sdcard/external_sd/level2MultiMaterial.vol");
	}
	else
	{
		mVolume->loadData("/sdcard/external_sd/level2.vol");
	}
#endif

	//mVolume->createSphereAt(Vector3(10.0f, 10.0f, 10.0f), 20, 0);
	
	_polyVoxNode = mVolume->getRootNode();
	_scene->addNode(mVolume->getRootNode());
}

void MeshGame::finalize()
{
    SAFE_RELEASE(_font);
    SAFE_RELEASE(_scene);
}

void MeshGame::update(float elapsedTime)
{
	mTimeBetweenUpdates = elapsedTime;

#ifdef TERRAIN_SMOOTH
	if(mLeftMouseDown)
	{
		MultiMaterial material;
		material.setMaterial(0, 255);
		createSphereAt(mSphereNode->getTranslation(), 5, material);
	}
#endif

	_cameraNode->setTranslation(64.0f, 16.0f, 64.0f);
	_cameraNode->setRotation(Quaternion::identity());
	_cameraNode->rotateY(mCameraRotationAngle);
	_cameraNode->rotateX(-mCameraElevationAngle); //Why negative?
	
	_cameraNode->translate(_cameraNode->getForwardVector() * -mCameraDistance);

	mVolume->updateMeshes();
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
	if(hasMouse())
	{
		// Ignore touch event on platform which support a mouse,
		// as these touch events seem to be duplicates of mouse events.
		return;
	}

    switch (evt)
    {
    case Touch::TOUCH_PRESS:
		mLastX = x;
		mLastY = y;
		break;
    case Touch::TOUCH_RELEASE:
		mLastX = 0;
		mLastY = 0;
        break;
    case Touch::TOUCH_MOVE:
		moveCamera(x,y);
		break;
    default:
        break;
    };
}

bool MeshGame::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
	if(evt == Mouse::MOUSE_PRESS_RIGHT_BUTTON)
	{
		mLastX = x;
		mLastY = y;
		mRightMouseDown = true;
	}
	if(evt == Mouse::MOUSE_RELEASE_RIGHT_BUTTON)
	{
		mLastX = 0;
		mLastY = 0;
		mRightMouseDown = false;
	}
	if(evt == Mouse::MOUSE_PRESS_LEFT_BUTTON)
	{
		mLeftMouseDown = true;
	}
	if(evt == Mouse::MOUSE_RELEASE_LEFT_BUTTON)
	{
		mLeftMouseDown = false;
	}	

	if(mRightMouseDown)
	{
		moveCamera(x,y);
	}

	Ray ray;
	_cameraNode->getCamera()->pickRay(getViewport(), x, y, &ray);

	Vector3 dir = ray.getDirection();
	dir *= 200.0f;
	ray.setDirection(dir);

	Vector3 intersection;
	if(mVolume->raycast(ray, 200.0f, intersection))
	{
		mSphereNode->setTranslation(intersection);
	}

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

void MeshGame::moveCamera(int x, int y)
{
	int deltaX = x -  mLastX;
	int deltaY = y -  mLastY;
	mLastX = x;
	mLastY = y;
	
	float cameraSensitivity = 0.01f;
	mCameraRotationAngle -= (deltaX * cameraSensitivity);
	mCameraElevationAngle += (deltaY * cameraSensitivity);
	
	mCameraElevationAngle = min(mCameraElevationAngle, MATH_DEG_TO_RAD(70.0f)); //Value from voxeliens
	mCameraElevationAngle = max(mCameraElevationAngle, MATH_DEG_TO_RAD(-5.0f)); //Value from voxeliens
}

void MeshGame::createSphereAt(const gameplay::Vector3& centre, float radius, MultiMaterial value)
{
#ifdef TERRAIN_SMOOTH
	int firstX = static_cast<int>(std::floor(centre.x - radius));
	int firstY = static_cast<int>(std::floor(centre.y - radius));
	int firstZ = static_cast<int>(std::floor(centre.z - radius));

	int lastX = static_cast<int>(std::ceil(centre.x + radius));
	int lastY = static_cast<int>(std::ceil(centre.y + radius));
	int lastZ = static_cast<int>(std::ceil(centre.z + radius));

	float radiusSquared = radius * radius;

	//Check bounds.
	firstX = std::max(firstX,mVolume->mVolData->getEnclosingRegion().getLowerCorner().getX());
	firstY = std::max(firstY,mVolume->mVolData->getEnclosingRegion().getLowerCorner().getY());
	firstZ = std::max(firstZ,mVolume->mVolData->getEnclosingRegion().getLowerCorner().getZ());

	lastX = std::min(lastX,mVolume->mVolData->getEnclosingRegion().getUpperCorner().getX());
	lastY = std::min(lastY,mVolume->mVolData->getEnclosingRegion().getUpperCorner().getY());
	lastZ = std::min(lastZ,mVolume->mVolData->getEnclosingRegion().getUpperCorner().getZ());

	for(int z = firstZ; z <= lastZ; ++z)
	{
		for(int y = firstY; y <= lastY; ++y)
		{
			for(int x = firstX; x <= lastX; ++x)
			{
				float amountToAdd = (centre - Vector3(x,y,z)).length() / radius;
				amountToAdd = max(amountToAdd, 0.0f);
				amountToAdd = min(amountToAdd, 1.0f);
				amountToAdd = 1.0f - amountToAdd;
				amountToAdd *= 255.0f;

				amountToAdd *= (mTimeBetweenUpdates / 1000.0f);
				amountToAdd *= 10.0f;

				uint8_t uToAdd = static_cast<uint8_t>(amountToAdd);

				if((centre - Vector3(x,y,z)).lengthSquared() <= radiusSquared)
				{
					MultiMaterial originalMat = mVolume->getVoxelAt(x, y, z);
					MultiMaterial afterAdding(originalMat);
					//Vector4DUint8 vec = material.getMaterial();
					float sum = originalMat.getSumOfMaterials();
					afterAdding.setMaterial(0, min(originalMat.getMaterial(0) + uToAdd, 255));
					float newSum = afterAdding.getSumOfMaterials();
					float factor = sum / newSum;

					MultiMaterial normalisedMat(afterAdding);
					normalisedMat.setMaterial(0, static_cast<uint8_t>((static_cast<float>(afterAdding.getMaterial(0)) + 0.5f) * factor));
					normalisedMat.setMaterial(1, static_cast<uint8_t>((static_cast<float>(afterAdding.getMaterial(1)) + 0.5f) * factor));
					normalisedMat.setMaterial(2, static_cast<uint8_t>((static_cast<float>(afterAdding.getMaterial(2)) + 0.5f) * factor));
					normalisedMat.setMaterial(3, static_cast<uint8_t>((static_cast<float>(afterAdding.getMaterial(3)) + 0.5f) * factor));
					

					//
					/*if(material.getSumOfMaterials() != newMaterial.getSumOfMaterials())
					{
						cout << "SUM MISMATCH!" << endl;
					}
					else
					{
						cout << "Match" << endl;
					}*/

					//addToMaterial(0, uToAdd, material);

					mVolume->setVoxelAt(x,y,z,normalisedMat);

					uint32_t oldSumMat = afterAdding.getSumOfMaterials();
					uint32_t newSumMat = normalisedMat.getSumOfMaterials();
					assert(newSumMat == oldSumMat);
				}
			}
		}
	}
#endif
}

void MeshGame::addToMaterial(uint32_t index, uint8_t amountToAdd, MultiMaterial& material)
{
	uint8_t current = material.getMaterial(index);
	if(current < material.getMaxMaterialValue())
	{
		material.setMaterial(index, current + amountToAdd);

		uint8_t excess = amountToAdd;
		bool someExcessRemoved = false;
		uint32_t nextToShed = 0;
		do
		{
			nextToShed++;
			nextToShed %= material.getNoOfMaterials();

			if(nextToShed == index)
			{
				continue;
			}

			uint8_t val = material.getMaterial(nextToShed);
			if(val > 0)
			{
				material.setMaterial(nextToShed, val - 1);
				someExcessRemoved = true;
			}
			

		}while((excess > 0) && (someExcessRemoved));
	}

}