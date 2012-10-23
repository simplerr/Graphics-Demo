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
}

void Editor::Init(ModelImporter* pImporter)
{
	mObjectMover = new ObjectMover(pImporter);
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
}
	
void Editor::Draw(Graphics* pGraphics)
{
	// Render the canvas and all controls in it.
	mObjectMover->Draw(pGraphics);
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
		if(type == STATIC_OBJECT || type == ANIMATED_OBJECT) 
			mActiveInspector = new ObjectInspector(mGwenCanvas);
		else if(type == LIGHT) 
			mActiveInspector = new LightInspector(mGwenCanvas);

		mActiveInspector->Init();
	}
	
	if(type == STATIC_OBJECT || type == ANIMATED_OBJECT) {
		mObjectMover->SetObject((Object3D*)pItem);
		mObjectMover->AddOnPositionChange(&ObjectInspector::OnPositionChangeEvent, (ObjectInspector*)mActiveInspector);
		mObjectMover->AddOnScaleChange(&ObjectInspector::OnScaleChangeEvent, (ObjectInspector*)mActiveInspector);
	}
	else if(type == LIGHT) {
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

void Editor::SetLight(Light* light)
{
	//mActiveInspector->SetObject(light);
}

void Editor::SetWorld(World* world)
{
	mWorld = world;
	mWorldTree->CreateTree(mWorld);
}