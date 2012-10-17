#include "Editor.h"
#include "DirectX11Renderer.h"
#include "d3dUtil.h"
#include "LightInspector.h"

Editor::Editor(int width, int height)
{
	GwenInit(width, height);

	mActiveInspector = new LightInspector(mGwenCanvas);
	mActiveInspector->Init();
}
	
Editor::~Editor()
{
	mGwenCanvas->Release();
	delete mGwenSkin;
	delete mGwenRenderer;
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
	//mGwenCanvas->SetBackgroundColor(Gwen::Color(0, 255, 170, 255));

	Gwen::Controls::Button* button = new Gwen::Controls::Button(mGwenCanvas);
	button->SetText("hehe");
	
	// Init Gwen input.
	mGwenInput.Initialize( mGwenCanvas );
}
	
void Editor::Update(float dt)
{

}
	
void Editor::Draw(Graphics* pGraphics)
{
	// Render the canvas and all controls in it.
	mGwenCanvas->RenderCanvas();
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
	mActiveInspector->SetObject(light);
}