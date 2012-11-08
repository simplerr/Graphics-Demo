#pragma once

#include "Runnable.h"

class StaticObject;
class World;
class Light;
class RenderTarget;
class Primitive;
class Texture2D;
class ShadowMap;
class ModelImporter;
class Model;
class SkinnedModel;
class AnimatedObject;
class Editor;

class Game : public Runnable
{
public:
	Game(HINSTANCE hInstance, string caption, int width, int height);
	~Game();

	void Init();
	void Update(float dt);
	void Draw(Graphics* pGraphics);
	void OnResize(int width, int height);

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void GwenInit();
private:
	World* mWorld;
	Light* mLight;
	Light* mLight2;
	StaticObject*	mObject;
	StaticObject*	mObject2;
	StaticObject*	mTerrain;
	AnimatedObject* mAnimatedObject;
	Editor*			mEditor;
};