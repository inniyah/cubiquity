#ifndef MESHGAME_H_
#define MESHGAME_H_

#include "gameplay.h"

#include <bitset>

using namespace gameplay;

/**
 * Sample game for rendering a scene with a model/mesh.
 */
class MeshGame: public Game
{
public:

    /**
     * Constructor.
     */
    MeshGame();

    /**
     * Destructor.
     */
    virtual ~MeshGame();

    /**
     * @see Game::keyEvent
     */
    void keyEvent(Keyboard::KeyEvent evt, int key);

    /**
     * @see Game::touchEvent
     */
    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

protected:

    /**
     * @see Game::initialize
     */
    void initialize();

    /**
     * @see Game::finalize
     */
    void finalize();

    /**
     * @see Game::update
     */
    void update(float elapsedTime);

    /**
     * @see Game::render
     */
    void render(float elapsedTime);

private:

    bool drawScene(Node* node);

    void drawFrameRate(Font* font, const Vector4& color, unsigned int x, unsigned int y, unsigned int fps);

    //Font* _font;
    Scene* _scene;
    Node* _modelNode;
	Node* _polyVoxNode;
    int _touchX;
	int _touchY;

	Node* _cameraNode;

	static const int mNoOfKeys = 0xFFFF;
	std::bitset<mNoOfKeys> mPressedKeys;
};

#endif
