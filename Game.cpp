#include <crtdbg.h> 
#include <assert.h>
#include <time.h>
#include "Game.h"
#include "D3DCore.h"
#include "d3dUtil.h"
#include "Graphics.h"
#include "PrimitiveFactory.h"
#include "Input.h"
#include "Light.h"
#include "BillboardManager.h"
#include "Vertex.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "Effects.h"
#include "RenderStates.h"
#include "ShadowMap.h"
#include "ModelImporter.h"
#include "StaticModel.h"
#include "SkinnedModel.h"
#include "Object3D.h"
#include "World.h"
#include "StaticObject.h"
#include "AnimatedObject.h"
#include "vld.h"
#include "Primitive.h"
#include "Editor.h"
#include "LightObject.h"

using namespace GLib;

// Set global to NULL.
GLib::Runnable* GLib::GlobalApp = nullptr;

Gwen::Renderer::DirectX11* pRenderer;

LightObject* lightObject;

//! The program starts here.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
	/*#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif*/

	Game game(hInstance, "Graphics Demo", 1200, 800);
	GLib::GlobalApp = &game;

	// Init the app.
	game.Init();
	game.GwenInit();

	return GLib::GlobalApp->Run();
}

Game::Game(HINSTANCE hInstance, string caption, int width, int height)
	: Runnable(hInstance, caption, width, height)
{
	mEditor = nullptr;
	mEditorVisible = true;
}
	
Game::~Game()
{
	delete mWorld;
	delete mEditor;
}

void Game::Init()
{
	// Important to run Systems Init() function.
	Runnable::Init();

	mEditor = new Editor(GetClientWidth(), GetClientHeight());
	
	// Create the world.
	mWorld = new World();
	mWorld->Init(GetGraphics());
	mWorld->AddItemSelectedListender(&Editor::OnItemSelected, mEditor);

	// Connect the graphics light list to the one in World.
	GetGraphics()->SetLightList(mWorld->GetLights());

	// Add some lights.
	lightObject = new LightObject();
	lightObject->SetMaterials(Material(Colors::White, Colors::White, XMFLOAT4(229/255.0f, 106/255.0f, 5.0f/255.0f, 1.0f)));
	lightObject->SetRotation(XMFLOAT3(0.3f, -1.0f, 0.0f));
	lightObject->SetLightType(DIRECTIONAL_LIGHT);
	lightObject->SetAtt(0, 0.1, 0);
	lightObject->SetRange(2000.0f);
	lightObject->SetSpot(16.0f);
	lightObject->SetPosition(XMFLOAT3(0, 50, 0));
	lightObject->SetIntensity(0.2f, 1.0f, 0.2f);
	mWorld->AddObject(lightObject);

	GetGraphics()->SetFogColor(XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f));

	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	GetGraphics()->GetContext()->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);

	mAnimatedObject = new AnimatedObject(GetGraphics()->GetModelImporter(), "models/smith/smith.x");
	mAnimatedObject->SetScale(XMFLOAT3(0.2f, 0.2f, 0.2f));
	mAnimatedObject->SetPosition(XMFLOAT3(0, 30, 0));
	mAnimatedObject->SetRotation(XMFLOAT3(0.7, 0.6, 0.6));
	//mWorld->AddObject(mAnimatedObject);

	mEditor->Init(GetGraphics()->GetModelImporter(), mWorld);
}
	
void Game::GwenInit()
{
	
}

void Game::Update(GLib::Input* pInput, float dt)
{
	// Toggle editors visibility?
	if(pInput->KeyPressed(VK_SPACE))
		mEditorVisible = !mEditorVisible;

	mWorld->Update(dt);
	mEditor->Update(pInput, dt);
}
	
void Game::Draw(GLib::Graphics* pGraphics)
{
	// Clear the render target and depth/stencil.
	pGraphics->ClearScene();

	// Draw all objects.
	mWorld->Draw(pGraphics);	
	pGraphics->DrawBillboards();

	if(mEditorVisible)
		mEditor->Draw(pGraphics);

	// Present the backbuffer.
	pGraphics->Present();

	// Unbind the SRVs from the pipeline so they can be used as DSVs instead.
	ID3D11ShaderResourceView *const nullSRV[4] = {NULL, NULL, NULL, NULL};
	pGraphics->GetContext()->PSSetShaderResources(0, 4, nullSRV);
	Effects::BasicFX->Apply(GetD3DContext());
	//Effects::BuildShadowMapFX->Apply(GetD3DContext());
}

//! Called when the window gets resized.
void Game::OnResize(int width, int height)
{
	mEditor->OnResize(width, height);
}

LRESULT Game::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Toggle screen mode?
	if(msg == WM_CHAR) {
		if(wParam == 'f')	
			SwitchScreenMode();
	}

	if(mEditor != nullptr)
		mEditor->MsgProc(hwnd, msg, wParam, lParam);

	return Runnable::MsgProc(hwnd, msg, wParam, lParam);
}