#include "LightInspector.h"
#include "Light.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/Property/ColorSelector.h"
#include "Util.h"

LightInspector::LightInspector(Gwen::Controls::Base* pParent)
	: BaseInspector(pParent)
{
	mLight = nullptr;
}
	
LightInspector::~LightInspector()
{
	Cleanup();
}

void LightInspector::Init()
{
	// Create all the controls.
	SetBounds(0, 0, 200, 700);
	/*Gwen::Controls::Button* button = new Gwen::Controls::Button(this);
	button->SetPos(0, 200);
	button->SetText("LOLLER");*/

	Gwen::Controls::CollapsibleCategory* cat = Add("Objects");
	onSelection.Add(this, &LightInspector::OnSelection);
	cat->Add("Bot");
	cat->Add("Player");

	Gwen::Controls::CollapsibleCategory* cat1 = Add("Cats");
	onSelection.Add(this, &LightInspector::OnSelection);
	cat1->Add("Doris");
	cat1->Add("Nicke");

	Gwen::Controls::Properties* props = new Gwen::Controls::Properties( this );
	props->SetBounds(0, 300, 200, 300 );
	Gwen::Controls::Property::ColorSelector* colorSelector = new Gwen::Controls::Property::ColorSelector(props);
	colorSelector->onChange.Add(this, &LightInspector::OnColorChange);
	props->Add( L"Colour", colorSelector, L"255 0 0" );
}
	
void LightInspector::Cleanup()
{
	// Cleanup all the controls.
}

void LightInspector::OnSelection()
{
	string selectedText = ToString(GetSelected()->GetText().c_str());
	OutputDebugString(selectedText.c_str());
}

void LightInspector::OnColorChange(Gwen::Controls::Base* pControl)
{
	Gwen::Controls::Property::ColorSelector* colorSelector = (Gwen::Controls::Property::ColorSelector*)pControl;
	Gwen::Color color = colorSelector->m_Button->m_Color;
	XMFLOAT4 xmColor(color.b/255.0f, color.g/255.0f, color.r/255.0f, color.a/255.0f);

	if(mLight != nullptr)
		mLight->SetMaterials(xmColor, xmColor, xmColor);
}

void LightInspector::SetObject(void* pObject)
{
	mLight = (Light*)pObject;
}