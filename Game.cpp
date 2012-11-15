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
	mDrawHelp = false;
	mElapsedTime = 0.0f;
	SetFpsCap(100.0f);
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

	// Create the world.
	mWorld = new World();

	// Create and init the editor.
	mEditor = new Editor(GetClientWidth(), GetClientHeight());
	mEditor->Init(GetGraphics()->GetModelImporter(), mWorld);

	// Go fullscreen.
	//SwitchScreenMode();

	// Render the loading assets backround.
	mLoadingAssetsBkgd = GetGraphics()->LoadTexture("textures/black.png");
	mHelpScreen = GetGraphics()->LoadTexture("textures/help_screen.png");
	mPressH = GetGraphics()->LoadTexture("textures/press_h.png");
	RenderLoadingScreen(GetGraphics());

	// Init the world.
	mWorld->Init(GetGraphics());
	mWorld->AddItemSelectedListender(&Editor::OnItemSelected, mEditor);
	mEditor->UpdateWorldTree();

	// Connect the graphics light list to the one in World.
	GetGraphics()->SetLightList(mWorld->GetLights());

	// Set the fog color.
	GetGraphics()->SetFogColor(XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f));
}
	
void Game::GwenInit()
{
	
}

void Game::Update(GLib::Input* pInput, float dt)
{
	mElapsedTime += dt;

	// Toggle editors visibility?
	if(pInput->KeyPressed(VK_SPACE))
		mEditorVisible = !mEditorVisible;

	// Display the help screen?
	if(pInput->KeyPressed('H')) {
		mElapsedTime = 9999;	// > 6
		mDrawHelp = !mDrawHelp;
	}

	// Change the camera movement speed.
	if(pInput->KeyPressed('1'))
		GLib::GetCamera()->SetMoveSpeed(0.1f);
	else if(pInput->KeyPressed('2'))
		GLib::GetCamera()->SetMoveSpeed(0.6);
	else if(pInput->KeyPressed('3'))
		GLib::GetCamera()->SetMoveSpeed(1.6f);
	else if(pInput->KeyPressed('4'))
		GLib::GetCamera()->SetMoveSpeed(2.0f);

	mWorld->Update(dt);
	mEditor->Update(pInput, dt);

	// Close the program.
	if(pInput->KeyPressed(VK_ESCAPE)) 
		PostQuitMessage(0);
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

	if(mElapsedTime < 6.0f)
		pGraphics->DrawScreenQuad(mPressH, 600, 100, 164, 24);

	if(mDrawHelp)
		pGraphics->DrawScreenQuad(mHelpScreen, 600, 400, 681, 622);

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

void Game::RenderLoadingScreen(GLib::Graphics* pGraphics)
{
	pGraphics->ClearScene();
	pGraphics->DrawScreenQuad(mLoadingAssetsBkgd, 600, 400, 1200, 800);
	pGraphics->Present();
}