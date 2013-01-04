#ifndef MESHGAME_H_
#define MESHGAME_H_

#include "MultiMaterial.h"

#include "gameplay.h"

#include <bitset>

using namespace gameplay;

class ColouredCubesVolume;
class GameplaySmoothTerrainVolume;

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
    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);
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

	Node* mSphereNode;

	static const int mNoOfKeys = 0xFFFF;
	std::bitset<mNoOfKeys> mPressedKeys;

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
	Slider* mBrushSizeSlider;
	Slider* mBrushIntensitySlider;
	Slider* mSmoothBiasSlider;

	CheckBox* mWireframeCheckBox;

	uint32_t mSelectedMaterial;

#ifdef TERRAIN_SMOOTH
	GameplaySmoothTerrainVolume* mVolume;
#endif

#ifdef TERRAIN_CUBIC
	ColouredCubesVolume* mVolume;
#endif
};

#endif
