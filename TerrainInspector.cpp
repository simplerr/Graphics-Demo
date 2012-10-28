#include "TerrainInspector.h"
#include "Gwen/Controls/PropertyTree.h"
#include "Terrain.h"
#include "Object3D.h"
#include "TerrainTool.h"
#include "Util.h"

TerrainInspector::TerrainInspector(Gwen::Controls::Base* pParent, TerrainTool* pTerrainTool)
	: BaseInspector(pParent)
{
	mTerrain = nullptr;
	mTerrainTool = pTerrainTool;
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
	InitSlider(mRadiusSlider, "RadiusSlider", 73, 0.0f, 1.0f, 60.0f, false);
	mRadiusSlider->onValueChanged.Add(this, &TerrainInspector::OnRadiusChange);

	// Strength.
	Gwen::Controls::Label* directionLabel = new Gwen::Controls::Label(ptree);
	directionLabel->SetText("Strength");
	directionLabel->SetPos(3, 100);

	mStrengthSlider = new Gwen::Controls::HorizontalSlider(ptree);
	InitSlider(mStrengthSlider, "StrengthSlider", 113, 0.0f, 0.6f, 10.0f, false);
	mStrengthSlider->onValueChanged.Add(this, &TerrainInspector::OnStrengthChange);

	//
	// Set the values.
	//

	mRadiusSlider->SetValue(mTerrainTool->GetRadius());
	mStrengthSlider->SetValue(mTerrainTool->GetStrength());

	ptree->ExpandAll();
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
	mTerrain = (Terrain*)pObject;
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