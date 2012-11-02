#pragma once

/** GWEN **/
#include <objbase.h>
#include <GdiPlus.h>

#include "Gwen/Gwen.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Skins/TexturedBase.h"
#include "Gwen/Input/Windows.h"
#include "Gwen/Renderers/GDIPlus.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/CollapsibleList.h"
#include "DirectX11Renderer.h"

class Gwen::Renderer::DirectX11;
class Graphics;
class BaseInspector;
class Light;
class World;
class WorldTree;
class ObjectTool;
class ModelImporter;
class Object3D;
class TerrainTool;
class Terrain;
class Camera;
class CreationTool;

//! Contains the UI and the different tools.
class Editor
{
public:
	Editor(int width, int height);
	~Editor();
	
	void Init(ModelImporter* pImporter, World* pWorld);
	void GwenInit(int width, int height);
	void Update(float dt);
	void UpdateCamera(Camera* pCamera);
	void Draw(Graphics* pGraphics);
	void MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void OnItemSelected(void* pItem, int type);
private:
	Gwen::Controls::Canvas*		mGwenCanvas;
	Gwen::Skin::TexturedBase*	mGwenSkin;
	Gwen::Input::Windows		mGwenInput;
	Gwen::Renderer::DirectX11*	mGwenRenderer;
	Gwen::Controls::CollapsibleList* mRightList;
	BaseInspector*				mActiveInspector;
	WorldTree*					mWorldTree;
	TerrainTool*				mTerrainTool;
	ObjectTool*					mObjectTool;
	CreationTool*			    mCreationTool;
};