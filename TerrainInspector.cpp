#include "TerrainInspector.h"
#include "Gwen/Controls/PropertyTree.h"
#include "Gwen/Controls/ComboBox.h"
#include "Terrain.h"
#include "Object3D.h"
#include "TerrainTool.h"
#include "Util.h"
#include "World.h"

using namespace GLib;

TerrainInspector::TerrainInspector(Gwen::Controls::Base* pParent, TerrainTool* pTool)
	: BaseInspector(pParent)
{
	SetInspectorType(TERRAIN_INSPECTOR);

	// Init the terrain tool.
	mTerrainTool = pTool;
}
	
TerrainInspector::~TerrainInspector()
{
	Cleanup();
}

void TerrainInspector::Cleanup()
{

}

void TerrainInspector::Init()
{
	// Create all the controls.
	SetBounds(0, 0, 220, 800);

	Gwen::Controls::CollapsibleCategory* dataCategory = Add("Terrain data");

	//
	// Properties.
	//
	Gwen::Controls::PropertyTree* ptree = new Gwen::Controls::PropertyTree(dataCategory);
	ptree->SetBounds(0, 20, 200, 600);

	Gwen::Controls::Properties* scaleProps = ptree->Add(L"Properties");

	mRadiusProperty = scaleProps->Add("Radius");
	mRadiusProperty->onChange.Add(this, &TerrainInspector::OnPropertyChange);
	mStrengthProperty = scaleProps->Add("Strength");
	mStrengthProperty->onChange.Add(this, &TerrainInspector::OnPropertyChange);

	// Radius.
	Gwen::Controls::Label* label = new Gwen::Controls::Label(ptree);
	label->SetText("Radius");
	label->SetPos(3, 60);

	mRadiusSlider = new Gwen::Controls::HorizontalSlider(ptree);
	InitSlider(mRadiusSlider, "RadiusSlider", 73, 0.0f, 3.0f, 20.0f, false);
	mRadiusSlider->onValueChanged.Add(this, &TerrainInspector::OnRadiusChange);

	// Strength.
	Gwen::Controls::Label* directionLabel = new Gwen::Controls::Label(ptree);
	directionLabel->SetText("Strength");
	directionLabel->SetPos(3, 100);

	mStrengthSlider = new Gwen::Controls::HorizontalSlider(ptree);
	InitSlider(mStrengthSlider, "StrengthSlider", 113, 0.0f, 0.6f, 10.0f, false);
	mStrengthSlider->onValueChanged.Add(this, &TerrainInspector::OnStrengthChange);

	// Tool type.
	Gwen::Controls::ComboBox* combo = new Gwen::Controls::ComboBox(dataCategory);
	combo->SetPos(0, 200);
	combo->SetWidth(200);
	combo->AddItem(L"Height tool", "height");
	combo->AddItem(L"Smooth tool", "smooth");
	combo->AddItem(L"Texture tool", "texture");
	combo->onSelection.Add(this, &TerrainInspector::OnComboSelect);

	// Terrain textures.
	mTexture0 = CreateTerrainTexture("textures/grass.dds", "4", 10, 230, this);
	mTexture1 = CreateTerrainTexture("textures/darkdirt.dds", "0", 60, 230, this);
	mTexture2 = CreateTerrainTexture("textures/stone.dds", "1", 110, 230, this);
	mTexture3 = CreateTerrainTexture("textures/lightdirt.dds", "2", 10, 280, this);
	mTexture4 = CreateTerrainTexture("textures/snow.dds", "3", 60, 280, this);

	// Set the values.
	mRadiusSlider->SetValue(mTerrainTool->GetRadius());
	mStrengthSlider->SetValue(mTerrainTool->GetStrength());

	ptree->ExpandAll();

	SetActiveTool(mTerrainTool->GetActiveTool());
}

void TerrainInspector::Update(float dt)
{

}

void TerrainInspector::Draw(GLib::Graphics* pGraphics)
{
	mTerrainTool->Draw(pGraphics);
}

void TerrainInspector::OnTextureSelected(Gwen::Controls::Base* pControl)
{
	string name = pControl->GetName();

	int layer = atoi(name.c_str());
	mTerrainTool->SetSelectedLayer(layer);
}

void TerrainInspector::OnComboSelect(Gwen::Controls::Base* pControl)
{
	Gwen::Controls::ComboBox* combo = (Gwen::Controls::ComboBox*)pControl;
	string name = combo->GetSelectedItem()->GetName();

	if(name == "height")
		SetActiveTool(TOOL_HEIGHT);
	else if(name == "smooth")
		SetActiveTool(TOOL_SMOTH);
	else if(name == "texture")
		SetActiveTool(TOOL_TEXTURE);
}

void TerrainInspector::OnRadiusChange(Gwen::Controls::Base* pControl)
{
	Gwen::Controls::HorizontalSlider* slider = (Gwen::Controls::HorizontalSlider*)pControl;
	mTerrainTool->SetRadius(slider->GetValue());

	char buffer[246];
	sprintf(buffer, "%.3f", slider->GetValue());
	mRadiusProperty->GetProperty()->SetPropertyValue(buffer);
}
	
void TerrainInspector::OnStrengthChange(Gwen::Controls::Base* pControl)
{
	Gwen::Controls::HorizontalSlider* slider = (Gwen::Controls::HorizontalSlider*)pControl;
	mTerrainTool->SetStrength(slider->GetValue());

	char buffer[246];
	sprintf(buffer, "%.3f", slider->GetValue());
	mStrengthProperty->GetProperty()->SetPropertyValue(buffer);
}

void TerrainInspector::OnPropertyChange(Gwen::Controls::Base* pControl)
{
	Gwen::Controls::PropertyRow* row = (Gwen::Controls::PropertyRow*)pControl;
	string label = ToString(row->GetLabel()->GetText().c_str());

	float value = atof(ToString(row->GetProperty()->GetPropertyValue().c_str()).c_str());
	if(label == "Radius") {
		mRadiusSlider->SetValue(value);
		mTerrainTool->SetRadius(value);
	}
	else if(label == "Strength") {
		mStrengthSlider->SetValue(value);
		mTerrainTool->SetStrength(value);
	}
}

void TerrainInspector::SetObject(void* pObject)
{
	mTerrainTool->SetTerrain((Terrain*)pObject);
}
	
bool TerrainInspector::IsResponsible(int type)
{
	return type == TERRAIN ? true : false;
}

void TerrainInspector::InitSlider(Gwen::Controls::HorizontalSlider* slider, string name, int y, float value, float start, float end, bool clamp)
{
	slider->SetName(name);
	slider->SetWidth(170);
	slider->SetPos(0, y);
	slider->SetHeight(30);
	slider->SetRange(start, end);
	slider->SetValue(value);
	slider->SetNotchCount(20);
	slider->SetClampToNotches(clamp);
}

Gwen::Controls::Button* TerrainInspector::CreateTerrainTexture(string texture, string name, float x,  float y, Base* pParent)
{
	Gwen::Controls::Button* img = new Gwen::Controls::Button(pParent);
	img->onPress.Add(this, &TerrainInspector::OnTextureSelected);
	img->SetImage(texture, true);
	img->SetName(name);
	img->SetBounds(x, y, 40, 40);
	return img;
}

void TerrainInspector::SetActiveTool(ToolType tool)
{
	if(tool == TOOL_TEXTURE)
	{
		mTexture0->Show();
		mTexture1->Show();
		mTexture2->Show();
		mTexture3->Show();
		mTexture4->Show();
	}
	else
	{
		mTexture0->Hide();
		mTexture1->Hide();
		mTexture2->Hide();
		mTexture3->Hide();
		mTexture4->Hide();
	}

	mTerrainTool->SetTool(tool);
}

bool TerrainInspector::IsInspecting(int id)
{
	return false;
}

GLib::Object3D* TerrainInspector::GetInspectedObject()
{
	return nullptr;
}