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

// Set globals to nullptrs
Runnable*			gGame				= nullptr;
PrimitiveFactory*	gPrimitiveFactory	= nullptr;
Input*				gInput				= nullptr;

BillboardVertex* billboard;

Gwen::Renderer::DirectX11* pRenderer;

void drawText(ID3D11Device *pDevice, ID3D11DeviceContext *pContext) {
	IFW1Factory *pFW1Factory;
	HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	
	IFW1FontWrapper *pFontWrapper;
	hResult = pFW1Factory->CreateFontWrapper(pDevice, L"Arial", &pFontWrapper);
	
	pFontWrapper->DrawString(
		pContext,
		L"Text",// String
		12.0f,// Font size
		100.0f,// X position
		50.0f,// Y position
		0xff0099ff,// Text color, 0xAaBbGgRr
		0// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
	);
	
	pFontWrapper->Release();
	pFW1Factory->Release();
}

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
	delete mWorld;
	delete mModelImporter;
	delete mEditor;
}

void Game::Init()
{
	// Important to run Systems Init() function.
	Runnable::Init();

	mEditor = new Editor(GetScreenWidth(), GetScreenHeight());
	
	// Create the world.
	mWorld = new World();
	mWorld->Init();

	// Create the model importer.
	mModelImporter = new ModelImporter(gPrimitiveFactory);
	mEditor->Init(mModelImporter);

	// Connect the graphics light list to the one in World.
	GetGraphics()->SetLightList(mWorld->GetLights());

	// Add some lights.
	mLight = new Light();
	mLight->SetMaterials(Colors::White, Colors::Red, Colors::White);
	mLight->SetDirection(0.0f, -1.0f, 0.0f);
	mLight->SetType(DIRECTIONAL_LIGHT);
	mLight->SetAtt(0, 0.1, 0);
	mLight->SetRange(2000.0f);
	mLight->SetSpot(64.0f);
	mLight->SetPosition(0, 50, 5);
	mLight->SetIntensity(1.0f, 0.8f, 0.2f);
	mWorld->AddLight(mLight);

	GetGraphics()->SetFogColor(XMFLOAT4(1.0f, 0.2f, 0.8, 1.0f));
	
	// Add test billboards.
	billboard = GetGraphics()->AddBillboard(XMFLOAT3(0, 10, 0), XMFLOAT2(5, 5), "textures\\crate.dds");

	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	GetGraphics()->GetContext()->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);

	mAnimatedObject = new AnimatedObject(mModelImporter, "models/smith/smith.x");
	mAnimatedObject->SetScale(XMFLOAT3(0.2f, 0.2f, 0.2f));
	mAnimatedObject->SetPosition(XMFLOAT3(0, 30, 0));
	mAnimatedObject->SetRotation(XMFLOAT3(0.7, 0.6, 0.6));
	mWorld->AddObject(mAnimatedObject);

	mObject = new StaticObject(mModelImporter, "models/sword/uld-sword.obj");
	mObject->SetPosition(XMFLOAT3(0, 30, 0));
	mObject->SetMaterial(Material(Colors::Red));
	mObject->SetScale(XMFLOAT3(10, 10, 10));
	mWorld->AddObject(mObject);

	mEditor->SetLight(mLight);
	mEditor->SetWorld(mWorld);
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

	// Only
	if(gInput->KeyDown(VK_CONTROL))
		GetGraphics()->UpdateCamera(dt);

	if(gInput->KeyPressed(VK_SPACE)) {
		XMFLOAT3 pos = GetGraphics()->GetCamera()->GetPosition();
		char buffer[256];
		sprintf(buffer, "x: %f, y: %f, z: %f\n", pos.x, pos.y, pos.z);
		OutputDebugString(buffer);
	}

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

	pGraphics->DrawScreenQuad(0, 600, 400, 4, 4);

	// Present the backbuffer.
	pGraphics->Present();

	// Unbind the SRVs from the pipeline so they can be used as DSVs instead.
	ID3D11ShaderResourceView *const nullSRV[4] = {NULL, NULL, NULL, NULL};
	pGraphics->GetContext()->PSSetShaderResources(0, 4, nullSRV);
	Effects::BasicFX->Apply();
	Effects::BuildShadowMapFX->Apply();
}

LRESULT Game::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	gInput->MsgProc(msg, wParam, lParam);
	if(mEditor != nullptr)
		mEditor->MsgProc(hwnd, msg, wParam, lParam);
	return Runnable::MsgProc(hwnd, msg, wParam, lParam);
}