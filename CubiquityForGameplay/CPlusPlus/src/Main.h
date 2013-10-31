#ifndef MESHGAME_H_
#define MESHGAME_H_

#include "gameplay.h" //Would rather have this last, but it's being upset by Timer.h including Windows.h

#include "Colour.h"
#include "MaterialSet.h"

#include "GameplayColouredCubesVolume.h"
#include "GameplayTerrainVolume.h"
#include "GameplayTerrainVolumeEditor.h"

#include <bitset>

using namespace gameplay;
using namespace Cubiquity; // Bad!

#define TERRAIN_SMOOTH
//#define TERRAIN_CUBIC

/**
 * Sample game for rendering a scene with a model/mesh.
 */
class MeshGame: public Game, Control::Listener
{
public:
    MeshGame();
    virtual ~MeshGame();

    void keyEvent(Keyboard::KeyEvent evt, int key);
	bool mouseEvent (Mouse::MouseEvent evt, int x, int y, int wheelDelta);
	void controlEvent(Control* control, EventType evt);

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
	Node* _cameraPositionNode;
	Node* _cameraYawNode;
	Node* _cameraPitchNode;

	Node* mSphereNode;

	static const int mNoOfKeys = 0xFFFF;
	std::bitset<mNoOfKeys> mPressedKeys;
	bool mLeftMouseButtonPressed;
	bool mRightMouseButtonPressed;

	float mCameraElevationAngle;
	float mCameraRotationAngle;
	float mCameraDistance;

	bool mScreenPressed;
	bool mSphereVisible;

	float mTimeBetweenUpdatesInSeconds;

	Form* mForm;

	RadioButton* mRotateButton;
    RadioButton* mPaintButton;
    RadioButton* mSmoothButton;
	RadioButton* mAddButton;
	RadioButton* mSubtractButton;

	Button* mZoomInButton;
	Button* mZoomOutButton;

	Button* mMat0Button;
	Button* mMat1Button;
	Button* mMat2Button;
	Button* mMat3Button;

	Slider* mLod1StartSlider;
	Slider* mBrushInnerRadiusSlider;
	Slider* mBrushOuterRadiusSlider;
	Slider* mBrushOpacitySlider;

	CheckBox* mWireframeCheckBox;

	Label* mTasks;

	uint32_t mSelectedMaterial;

	GameplayTerrainVolumeEditor* mVolumeEditor;

#ifdef TERRAIN_SMOOTH
	GameplayTerrainVolume* mVolume;
#endif

#ifdef TERRAIN_CUBIC
	GameplayColouredCubesVolume* mVolume;
#endif
};

#endif
