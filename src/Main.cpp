#include "Main.h"

#include "ColouredCubesVolume.h"
#include "SmoothTerrainVolume.h"

#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/RawVolume.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"
#include "PolyVoxCore/VolumeResampler.h"
#include "PolyVoxCore/LowPassFilter.h"

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

	// Load the form for editing ParticleEmitters.
    _form = Form::create("res/editor.form");
    _form->setConsumeInputEvents(false);

	_scene = Scene::create();

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
		mVolume->loadData("res/level2MultiMaterial8Bit.vol");
	}
	else
	{
		mVolume->loadData("res/level2.vol");
	}
#else
	if(mVolume->getType() == VolumeTypes::SmoothTerrain)
	{
		mVolume->loadData("/sdcard/external_sd/level2MultiMaterial8Bit.vol");
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
		MultiMaterial4 material;
		material.setMaterial(0, 255);
		//createSphereAt(mSphereNode->getTranslation(), 5, material);
		smoothAt(mSphereNode->getTranslation(), 10);
	}
#endif

	_cameraNode->setTranslation(64.0f, 16.0f, 64.0f);
	_cameraNode->setRotation(Quaternion::identity());
	_cameraNode->rotateY(mCameraRotationAngle);
	_cameraNode->rotateX(-mCameraElevationAngle); //Why negative?
	
	_cameraNode->translate(_cameraNode->getForwardVector() * -mCameraDistance);

	mVolume->updateMeshes();

	// Update UI.
    _form->update(elapsedTime);
}

void MeshGame::render(float elapsedTime)
{
    // Clear the color and depth buffers.
    clear(CLEAR_COLOR_DEPTH, Vector4::zero(), 1.0f, 0);

    // Draw the UI.
    _form->draw();
    
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

void MeshGame::createSphereAt(const gameplay::Vector3& centre, float radius, MultiMaterial4 value)
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
				//amountToAdd *= 10.0f;

				uint8_t uToAdd = static_cast<uint8_t>(amountToAdd + 0.5f);

				if((centre - Vector3(x,y,z)).lengthSquared() <= radiusSquared)
				{
					MultiMaterial4 originalMat = mVolume->getVoxelAt(x, y, z);	
					addToMaterial(0, uToAdd, originalMat);
					mVolume->setVoxelAt(x,y,z, originalMat);
				}
			}
		}
	}
#endif
}

void MeshGame::smoothAt(const gameplay::Vector3& centre, float radius)
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

	Region region(firstX, firstY, firstZ, lastX, lastY, lastZ);
	RawVolume<MultiMaterial4> tempVolume(region);
	
	//We might not need to do this at sloat precision, it should be tested again.
	LowPassFilter< SimpleVolume<MultiMaterial4>, RawVolume<MultiMaterial4>, Vector<4, float> > lowPassFilter(mVolume->mVolData, region, &tempVolume, region, 3);
	lowPassFilter.execute();

	//LowPassFilter< RawVolume<MultiMaterial4>, SimpleVolume<MultiMaterial4>, MultiMaterial16> lowPassFilter2(&tempVolume, region, mVolume->mVolData, region, 3);
	//lowPassFilter2.execute();

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

				MultiMaterial4 originalMat = mVolume->getVoxelAt(x, y, z);
				MultiMaterial4 smoothedMat = tempVolume.getVoxelAt(x, y, z);

				//FIXME - expose linear interpolation as well as trilinear interpolation from PolyVox?
				float orig0 = static_cast<float>(originalMat.getMaterial(0));
				float orig1 = static_cast<float>(originalMat.getMaterial(1));
				float orig2 = static_cast<float>(originalMat.getMaterial(2));
				float orig3 = static_cast<float>(originalMat.getMaterial(3));

				float smooth0 = static_cast<float>(smoothedMat.getMaterial(0));
				float smooth1 = static_cast<float>(smoothedMat.getMaterial(1));
				float smooth2 = static_cast<float>(smoothedMat.getMaterial(2));
				float smooth3 = static_cast<float>(smoothedMat.getMaterial(3));

				float interp0 = (smooth0 - orig0) * amountToAdd + orig0;
				float interp1 = (smooth1 - orig1) * amountToAdd + orig1;
				float interp2 = (smooth2 - orig2) * amountToAdd + orig2;
				float interp3 = (smooth3 - orig3) * amountToAdd + orig3;

				MultiMaterial4 interpMat;
				// In theory we should add 0.5f before casting to round
				// properly, but this seems to cause material to grow too much.
				interpMat.setMaterial(0, static_cast<uint32_t>(interp0 + 0.25f));
				interpMat.setMaterial(1, static_cast<uint32_t>(interp1 + 0.25f));
				interpMat.setMaterial(2, static_cast<uint32_t>(interp2 + 0.25f));
				interpMat.setMaterial(3, static_cast<uint32_t>(interp3 + 0.25f));

				mVolume->setVoxelAt(x,y,z, interpMat);
			}
		}
	}
	
#endif
}

void MeshGame::addToMaterial(uint32_t index, uint8_t amountToAdd, MultiMaterial4& material)
{
	uint32_t indexToRemoveFrom = 0; //FIXME - start somewhere random
	uint32_t iterationWithNoRemovals = 0;
	while(amountToAdd > 0)
	{
		if(indexToRemoveFrom != index)
		{
			if(material.getMaterial(indexToRemoveFrom) > 0)
			{
				material.setMaterial(index, material.getMaterial(index) + 1);
				material.setMaterial(indexToRemoveFrom, material.getMaterial(indexToRemoveFrom) - 1);
				amountToAdd--;
				iterationWithNoRemovals = 0;
			}
			else
			{
				iterationWithNoRemovals++;
			}
		}
		else
		{
			iterationWithNoRemovals++;
		}

		if(iterationWithNoRemovals == MultiMaterial4::getNoOfMaterials())
		{
			break;
		}

		indexToRemoveFrom++;
		indexToRemoveFrom %= MultiMaterial4::getNoOfMaterials();
	}
}