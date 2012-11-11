#include "Editor.h"
#include "DirectX11Renderer.h"
#include "d3dUtil.h"
#include "LightInspector.h"
#include "ObjectInspector.h"
#include "WorldTree.h"
#include "Object3D.h"
#include "StaticObject.h"
#include "AnimatedObject.h"
#include "Light.h"
#include "Input.h"
#include "ObjectTool.h"
#include "Graphics.h"
#include "Camera.h"
#include "World.h"
#include "TerrainTool.h"
#include "TerrainInspector.h"
#include "Effects.h"
#include "CreationTool.h"

//! Constructor.
Editor::Editor(int width, int height)
{
	// Init Gwen.
	GwenInit(width, height);

	// No inspector to start with.
	mActiveInspector = nullptr;
}

//! Cleanup.
Editor::~Editor()
{
	mGwenCanvas->Release();
	delete mGwenSkin;
	delete mGwenRenderer;
	delete mObjectTool;
	delete mTerrainTool;
}

//! Initializes everything.
void Editor::Init(GLib::ModelImporter* pImporter, World* pWorld)
{
	mWorld = pWorld;

	// Create the right list.
	mRightList = new Gwen::Controls::CollapsibleList(mGwenCanvas);
	mRightList->SetBounds(1000, 0, 200, 800);
	mRightList->SetShouldDrawBackground(false);

	// Create the world tree.
	mWorldTree = new WorldTree(mRightList);
	mWorldTree->SetEditor(this);
	mWorldTree->SetWorld(pWorld);
	mWorldTree->CreateTree();

	// Create the tools.
	mTerrainTool = new TerrainTool();
	mObjectTool = new ObjectTool(pImporter);
	mCreationTool = new CreationTool(mRightList, pWorld, pImporter);
	mCreationTool->SetEditor(this);
}

//! Inits the Gwen renderer, canvas, input and skin.
void Editor::GwenInit(int width, int height)
{
	// Create the renderer.
	mGwenRenderer = new Gwen::Renderer::DirectX11(GLib::GetD3DDevice(), GLib::GetD3DContext());
	mGwenRenderer->SetScreenSize(width, height);

	// Create a GWEN skin.
	mGwenSkin = new Gwen::Skin::TexturedBase();
	mGwenSkin->SetRender(mGwenRenderer);
	mGwenSkin->Init("DefaultSkin.png");

	// Create a Canvas (it's root, on which all other GWEN panels are created).
	mGwenCanvas = new Gwen::Controls::Canvas(mGwenSkin);
	mGwenCanvas->SetSize(width, height);
	mGwenCanvas->Initialize();
	mGwenCanvas->SetDrawBackground(false);
	
	// Init Gwen input.
	mGwenInput.Initialize( mGwenCanvas );
}
	
//! Update the tools.
void Editor::Update(GLib::Input* pInput, float dt)
{
	mCreationTool->Update(pInput, dt);

	// Was an object or light selected? [NOTE] Objects have priority.
	if(pInput->KeyPressed(VK_LBUTTON) && !pInput->KeyDown(VK_CONTROL) && IsIn3DScreen(pInput))	// [NOTE] Only true if CTRL is not held down.
	{
		Object3D* selectedObject = mWorld->GetSelectedObject(pInput->GetWorldPickingRay());
		if(selectedObject != nullptr)
			OnItemSelected(selectedObject, selectedObject->GetType());
	}
	// Remove any object that was pressed with CTRL.
	else if(pInput->KeyPressed(VK_LBUTTON) && pInput->KeyDown(VK_CONTROL))
	{
		Object3D* object = mWorld->GetSelectedObject(pInput->GetWorldPickingRay());
		if(object != nullptr)
		{
			// Is the object thats getting deleted being inspected?
			if(mActiveInspector != nullptr && mActiveInspector->IsInspecting(object->GetId()))
				RemoveInspector();

			mWorld->RemoveObject(object);
			mWorldTree->CreateTree();
		}
	}

	// DELETE pressed and an object selected?
	if(pInput->KeyPressed(VK_DELETE) && mActiveInspector != nullptr && mActiveInspector->GetInspectorType() != TERRAIN_INSPECTOR) {
		mWorld->RemoveObject(mActiveInspector->GetInspectedObject());
		mWorldTree->CreateTree();
		RemoveInspector();
	}

	// Update the active inspector.
	if(mActiveInspector != nullptr && !mCreationTool->IsCreatingModel())
		mActiveInspector->Update(dt);

	// [NOTE][HACK]
	if(mActiveInspector != nullptr && mActiveInspector->GetInspectorType() == TERRAIN_INSPECTOR)
		mTerrainTool->Update(pInput, dt);
	else if(mActiveInspector != nullptr && (mActiveInspector->GetInspectorType() == LIGHT_INSPECTOR 
		|| mActiveInspector->GetInspectorType() == OBJECT_INSPECTOR))
		mObjectTool->Update(pInput, dt);

}
	
//! Draws all elements in the editor.
void Editor::Draw(GLib::Graphics* pGraphics)
{
	// Update the camera.
	UpdateCamera(pGraphics->GetCamera());

	// Draw the active inspector.
	if(mActiveInspector != nullptr && !mCreationTool->IsCreatingModel())
		mActiveInspector->Draw(pGraphics);

	mCreationTool->Draw(pGraphics);

	// Draw the canvas and all controls in it.
	mGwenCanvas->RenderCanvas();
}
	
//! Callback that World calls when an object gets selected.
void Editor::OnItemSelected(void* pItem, int type)
{
	// Delete the current inspector if it's not responsible for the selected item.
	if(mActiveInspector != nullptr && !mActiveInspector->IsResponsible(type)) {
		delete mActiveInspector;
		mActiveInspector = nullptr;
	}

	// The active inspector haven't been created.
	if(mActiveInspector == nullptr) {
		if(type == STATIC_OBJECT || type == ANIMATED_OBJECT) 
			mActiveInspector = new ObjectInspector(mGwenCanvas, mObjectTool);
		else if(type == LIGHT_OBJECT) 
			mActiveInspector = new LightInspector(mGwenCanvas, mObjectTool);
		else if(type == TERRAIN) {
			mActiveInspector = new TerrainInspector(mGwenCanvas, mTerrainTool);
		}
		mActiveInspector->Init();
		mActiveInspector->SetEditor(this);
	}

	// Set the inspectors object, can be Object3D Light or Terrain.
	mActiveInspector->SetObject(pItem);
}

//! Called when the window gets resized.
void Editor::OnResize(int width, int height)
{
	mGwenRenderer->SetScreenSize(width, height);
	mRightList->SetBounds(width-200, 0, 200, height);
	mGwenCanvas->SetSize(width, height);
}

//! Lets Gwen::Input process messaages.
void Editor::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MSG message;
	message.hwnd =  hwnd;
	message.message = msg;
	message.wParam = wParam;
	message.lParam = lParam;

	mGwenInput.ProcessMessage(message);
}

//! Rotates and moves the camera.
void Editor::UpdateCamera(GLib::Camera* pCamera)
{
	// Rotate and move the camera.
	/*if(gInput->KeyDown(VK_MBUTTON))
		pCamera->Rotate();

	pCamera->Move();
	pCamera->UpdateViewMatrix();*/
}

void Editor::UpdateWorldTree()
{
	mWorldTree->CreateTree();
}

void Editor::RemoveInspector()
{
	delete mActiveInspector;
	mActiveInspector = nullptr;
}