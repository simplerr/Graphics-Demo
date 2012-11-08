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
#include "LightObject.h"

// Set globals to nullptrs
Runnable*			gGame				= nullptr;
PrimitiveFactory*	gPrimitiveFactory	= nullptr;
Input*				gInput				= nullptr;
ModelImporter*		gModelImporter		= nullptr;

BillboardVertex* billboard;

Gwen::Renderer::DirectX11* pRenderer;

//! The program starts here.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
	/*#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif*/

	Game game(hInstance, "Graphics Demo", 1200, 800);
	gGame = &game;

	// Create the primitive factory.
	gPrimitiveFactory = new PrimitiveFactory();	

	// Init the app.
	game.Init();
	game.GwenInit();

	// Create the input class.
	gInput = new Input();

	return gGame->Run();
}

Game::Game(HINSTANCE hInstance, string caption, int width, int height)
	: Runnable(hInstance, caption, width, height)
{
	mEditor = nullptr;
}
	
Game::~Game()
{
	delete gPrimitiveFactory;	
	delete gInput;
	delete gModelImporter;
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
	mWorld->Init();
	mWorld->AddItemSelectedListender(&Editor::OnItemSelected, mEditor);

	// Create the model importer.
	gModelImporter = new ModelImporter(gPrimitiveFactory);

	// Connect the graphics light list to the one in World.
	GetGraphics()->SetLightList(mWorld->GetLights());

	// Add some lights.
	LightObject* lightObject = new LightObject();
	lightObject->SetMaterials(Material(Colors::White, Colors::White, XMFLOAT4(229/255.0f, 106/255.0f, 5.0f/255.0f, 1.0f)));
	lightObject->SetRotation(XMFLOAT3(0.0f, -1.0f, 0.0f));
	lightObject->SetLightType(DIRECTIONAL_LIGHT);
	lightObject->SetAtt(0, 0.1, 0);
	lightObject->SetRange(2000.0f);
	lightObject->SetSpot(16.0f);
	lightObject->SetPosition(XMFLOAT3(0, 50, 0));
	lightObject->SetIntensity(0.2f, 1.0f, 0.2f);
	mWorld->AddObject(lightObject);

	GetGraphics()->SetFogColor(XMFLOAT4(1.0f, 0.2f, 0.8, 1.0f));
	
	// Add test billboards.
	//billboard = GetGraphics()->AddBillboard(XMFLOAT3(0, 10, 0), XMFLOAT2(5, 5), "textures\\crate.dds");

	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	GetGraphics()->GetContext()->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);

	mAnimatedObject = new AnimatedObject(gModelImporter, "models/smith/smith.x");
	mAnimatedObject->SetScale(XMFLOAT3(0.2f, 0.2f, 0.2f));
	mAnimatedObject->SetPosition(XMFLOAT3(0, 30, 0));
	mAnimatedObject->SetRotation(XMFLOAT3(0.7, 0.6, 0.6));
	mWorld->AddObject(mAnimatedObject);

	/*mObject = new StaticObject(gModelImporter, "models/bandit_male/bandit_male.obj");
	mObject->SetPosition(XMFLOAT3(0, 30, 0));
	mObject->SetMaterial(Material(Colors::White));
	mObject->SetScale(XMFLOAT3(10, 10, 10));
	mWorld->AddObject(mObject);*/

	mEditor->Init(gModelImporter, mWorld);
}
	
void Game::GwenInit()
{
	
}

void Game::Update(float dt)
{
	gInput->Update(dt);
	GetGraphics()->Update(dt);
	mWorld->Update(dt);
	mEditor->Update(dt);

	return;

	static float speed = 0.05;
	if(gInput->KeyDown('1'))
		mAnimatedObject->SetPosition(mAnimatedObject->GetPosition() + XMFLOAT3(speed, 0, 0));
	else if(gInput->KeyDown('2'))
		mAnimatedObject->SetPosition(mAnimatedObject->GetPosition() + XMFLOAT3(-speed, 0, 0));
	if(gInput->KeyDown('3'))
		mAnimatedObject->SetPosition(mAnimatedObject->GetPosition() + XMFLOAT3(0, 0, speed));
	else if(gInput->KeyDown('4'))
		mAnimatedObject->SetPosition(mAnimatedObject->GetPosition() + XMFLOAT3(0, 0, -speed));
	if(gInput->KeyDown('5'))
		mAnimatedObject->SetPosition(mAnimatedObject->GetPosition() + XMFLOAT3(0, speed, 0));
	else if(gInput->KeyDown('6'))
		mAnimatedObject->SetPosition(mAnimatedObject->GetPosition() + XMFLOAT3(0, -speed, 0));

	if(gInput->KeyDown('Z'))
		mLight->SetDirection(mLight->GetDirection() + XMFLOAT3(0, 0.0000, 0.0005));
	else if(gInput->KeyDown('X'))
		mLight->SetDirection(mLight->GetDirection() + XMFLOAT3(0, -0.0000, -0.0005));

	if(gInput->KeyDown(VK_LBUTTON)) {
		mAnimatedObject->SetRotation(mAnimatedObject->GetRotation() + XMFLOAT3(0.003, 0.003, 0.003));
	}
	else if(gInput->KeyDown(VK_RBUTTON)) {
		mAnimatedObject->SetRotation(mAnimatedObject->GetRotation() - XMFLOAT3(0.003, 0.003, 0.003));
		mAnimatedObject->SetScale(mAnimatedObject->GetScale() + XMFLOAT3(0.003, 0.003, 0.003));
	}

	if(gInput->KeyDown('V'))
		mAnimatedObject->SetAnimation(0);
	if(gInput->KeyDown('B'))
		mAnimatedObject->SetAnimation(1);
}
	
void Game::Draw(Graphics* pGraphics)
{
	// Clear the render target and depth/stencil.
	pGraphics->ClearScene();

	// Draw all objects.
	mWorld->Draw(pGraphics);	
	pGraphics->DrawBillboards();

	mEditor->Draw(pGraphics);

	// Present the backbuffer.
	pGraphics->Present();

	// Unbind the SRVs from the pipeline so they can be used as DSVs instead.
	ID3D11ShaderResourceView *const nullSRV[4] = {NULL, NULL, NULL, NULL};
	pGraphics->GetContext()->PSSetShaderResources(0, 4, nullSRV);
	Effects::BasicFX->Apply();
	Effects::BuildShadowMapFX->Apply();
}

//! Called when the window gets resized.
void Game::OnResize(int width, int height)
{
	mEditor->OnResize(width, height);
}

LRESULT Game::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Toggle screen mode?
	if(msg == WM_CHAR)
	{
		if(wParam == 'f')	// Toggle by pressing F.
			SwitchScreenMode();
	}

	gInput->MsgProc(msg, wParam, lParam);
	if(mEditor != nullptr)
		mEditor->MsgProc(hwnd, msg, wParam, lParam);
	return Runnable::MsgProc(hwnd, msg, wParam, lParam);
}