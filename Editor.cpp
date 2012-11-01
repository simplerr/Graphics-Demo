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
#include "ObjectMover.h"
#include "Graphics.h"
#include "Camera.h"
#include "World.h"
#include "TerrainTool.h"
#include "TerrainInspector.h"

Editor::Editor(int width, int height)
{
	GwenInit(width, height);

	mActiveInspector = nullptr;

	mWorldTree = new WorldTree(mGwenCanvas);
	mWorldTree->SetEditor(this);
}
	
Editor::~Editor()
{
	mGwenCanvas->Release();
	delete mGwenSkin;
	delete mGwenRenderer;
	delete mObjectMover;
	delete mTerrainTool;
}

void Editor::Init(ModelImporter* pImporter, World* pWorld)
{
	mObjectMover = new ObjectMover(pImporter);

	// Init the terrain tool.
	mTerrainTool = new TerrainTool();
	mTerrainTool->SetTerrain(pWorld->GetTerrain());
	mTerrainTool->SetEnabled(false);

	mWorld = pWorld;
	mWorldTree->CreateTree(mWorld);
	mWorld->AddObjectSelectedListender(&Editor::OnObjectSelected, this);
	mWorld->AddLightSelectedListender(&Editor::OnLightSelected, this);
	mWorld->AddTerrainSelectedListender(&Editor::OnTerrainSelected, this);
}

void Editor::GwenInit(int width, int height)
{
	mGwenRenderer = new Gwen::Renderer::DirectX11(GetD3DDevice(), GetD3DContext());
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
	
void Editor::Update(float dt)
{
	mObjectMover->Update(dt);
	mTerrainTool->Update(dt);
}
	
void Editor::Draw(Graphics* pGraphics)
{
	//
	// Rotate and move the camera.
	//
	Camera* camera = pGraphics->GetCamera();
	if(gInput->KeyDown(VK_MBUTTON))
		camera->Rotate();

	camera->Move();
	camera->UpdateViewMatrix();

	// Render the tools.
	mObjectMover->Draw(pGraphics);
	mTerrainTool->Draw(pGraphics);

	// Render the canvas and all controls in it.
	mGwenCanvas->RenderCanvas();
}
	
void Editor::ItemSelected(void* pItem, int type)
{
	// Delete the current inspector if not the same type as pItem.
	if(mActiveInspector != nullptr && !mActiveInspector->IsResponsible(type)) {
		delete mActiveInspector;
		mActiveInspector = nullptr;
	}

	if(mActiveInspector == nullptr) {
		mTerrainTool->SetEnabled(false);
		mObjectMover->SetVisible(true);
		if(type == STATIC_OBJECT || type == ANIMATED_OBJECT) 
			mActiveInspector = new ObjectInspector(mGwenCanvas);
		else if(type == LIGHT) 
			mActiveInspector = new LightInspector(mGwenCanvas);
		else if(type == TERRAIN) {
			mActiveInspector = new TerrainInspector(mGwenCanvas, mTerrainTool);
			mObjectMover->SetVisible(false);
			mTerrainTool->SetEnabled(true);
		}
		mActiveInspector->Init();
	}
	
	if(type == STATIC_OBJECT || type == ANIMATED_OBJECT) {
		mActiveInspector->SetObjectMover(mObjectMover);
		mObjectMover->SetObject((Object3D*)pItem);
		mObjectMover->AddOnPositionChange(&ObjectInspector::OnPositionChangeEvent, (ObjectInspector*)mActiveInspector);
		mObjectMover->AddOnScaleChange(&ObjectInspector::OnScaleChangeEvent, (ObjectInspector*)mActiveInspector);
	}
	else if(type == LIGHT) {
		mActiveInspector->SetObjectMover(mObjectMover);
		mObjectMover->SetObject((Light*)pItem);
		mObjectMover->AddOnPositionChange(&LightInspector::OnPositionChangeEvent, (LightInspector*)mActiveInspector);
	}


	mActiveInspector->SetObject(pItem);
}

void Editor::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MSG message;
	message.hwnd =  hwnd;
	message.message = msg;
	message.wParam = wParam;
	message.lParam = lParam;

	mGwenInput.ProcessMessage(message);
}

void Editor::OnObjectSelected(Object3D* pObject)
{
	ItemSelected(pObject, pObject->GetType());
}
	
void Editor::OnLightSelected(Light* pLight)
{
	ItemSelected(pLight, LIGHT);
}

void Editor::OnTerrainSelected(Terrain* pTerrain)
{
	ItemSelected(pTerrain, TERRAIN);
}