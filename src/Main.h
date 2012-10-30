#ifndef MESHGAME_H_
#define MESHGAME_H_

#include "MultiMaterial.h"

#include "gameplay.h"

#include <bitset>

using namespace gameplay;

class ColouredCubesVolume;
class SmoothTerrainVolume;

#define TERRAIN_SMOOTH
//#define TERRAIN_CUBIC

namespace EditActions
{
	enum EditAction
	{
		Add,
		Subtract,
		Paint, 
		Smooth
	};
}
typedef EditActions::EditAction EditAction;

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

	void applyPaint(const gameplay::Vector3& centre, float radius, uint32_t materialToPaintWith);
	void addMaterial(const gameplay::Vector3& centre, float radius, uint32_t materialToAdd);
	void subtractMaterial(const gameplay::Vector3& centre, float radius);
	void smooth(const gameplay::Vector3& centre, float radius);

	void edit(const gameplay::Vector3& centre, float radius, uint32_t materialToUse, EditAction editAction, float timeElapsedInSeconds, float amount, float smoothBias);

	void addToMaterial(uint32_t index, uint8_t amountToAdd, MultiMaterial4& material);
	void subtractFromMaterial(uint8_t amountToAdd, MultiMaterial4& material);

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

	Slider* mBrushSizeSlider;
	Slider* mAddSubtractRateSlider;
	Slider* mPaintIntensitySlider;
	Slider* mSmoothRateSlider;
	Slider* mSmoothBiasSlider;

	uint32_t mSelectedMaterial;

#ifdef TERRAIN_SMOOTH
	SmoothTerrainVolume* mVolume;
#endif

#ifdef TERRAIN_CUBIC
	ColouredCubesVolume* mVolume;
#endif
};

#endif
