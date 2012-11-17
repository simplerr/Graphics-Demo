#pragma once

/** GWEN INCLUDES **/
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

// Forward declarations.
#pragma region Forward declarations.
namespace GLib {
	class Graphics;
	class Input;
	class Terrain;
	class Camera;
	class ModelImporter;
	class Light;
	class World;
	class Object3D;
}

class Gwen::Renderer::DirectX11;
class BaseInspector;
class WorldTree;
class ObjectTool;
class TerrainTool;
class CreationTool;
#pragma endregion

//! Contains the UI and the different tools.
class Editor
{
public:
	Editor(int width, int height);
	~Editor();
	
	void Init(GLib::ModelImporter* pImporter, GLib::World* pWorld);
	void GwenInit(int width, int height);
	void Update(GLib::Input* pInput, float dt);
	void OnResize(int width, int height);
	void UpdateCamera(GLib::Camera* pCamera);
	void Draw(GLib::Graphics* pGraphics);
	void MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void OnItemSelected(void* pItem, int type);
	void UpdateWorldTree();
	void RemoveInspector();
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
	GLib::World*				mWorld;
};