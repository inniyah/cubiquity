#ifndef MESHGAME_H_
#define MESHGAME_H_

#include "gameplay.h"

#include <bitset>

using namespace gameplay;

class ColouredCubesVolume;
class SmoothTerrainVolume;

/**
 * Sample game for rendering a scene with a model/mesh.
 */
class MeshGame: public Game
{
public:
    MeshGame();
    virtual ~MeshGame();
    void keyEvent(Keyboard::KeyEvent evt, int key);
    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);
	bool mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta);

protected:
    void initialize();
    void finalize();
    void update(float elapsedTime);
    void render(float elapsedTime);

private:

    bool drawScene(Node* node);

    void drawFrameRate(Font* font, const Vector4& color, unsigned int x, unsigned int y, unsigned int fps);

	void moveCamera(int x, int y);

    Font* _font;
    Scene* _scene;
    Node* _modelNode;
	Node* _polyVoxNode;
    int mLastX;
	int mLastY;
	Light* _light;
	Node* _lightNode;

	Node* _cameraNode;

	Node* mSphereNode;

	static const int mNoOfKeys = 0xFFFF;
	std::bitset<mNoOfKeys> mPressedKeys;

	float mCameraElevationAngle;
	float mCameraRotationAngle;
	float mCameraDistance;

	bool mRightMouseDown;

	SmoothTerrainVolume* mVolume;
	//ColouredCubesVolume* mVolume;
};

#endif
