#include "LightInspector.h"
#include "Light.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/PropertyTree.h"
#include "Util.h"
#include "Object3D.h"
#include "ObjectTool.h"
#include "ModelImporter.h"
#include "Effects.h"
#include "LightObject.h"
#include "World.h"

using namespace GLib;

LightInspector::LightInspector(Gwen::Controls::Base* pParent, ObjectTool* pTool)
	: BaseInspector(pParent)
{
	SetInspectorType(LIGHT_INSPECTOR);
	mLight = nullptr;
	mRangeSlider = mSpotSlider = nullptr;
	mObjectTool = pTool;

	Effects::TerrainFX->SetToolCenter(XMFLOAT2(-999999, -999999));
	Effects::TerrainFX->Apply(GetD3DContext());
}
	
LightInspector::~LightInspector()
{
	Cleanup();
}

void LightInspector::Init()
{
	// Create all the controls.
	SetBounds(0, 0, 220, 800);

	Gwen::Controls::CollapsibleCategory* dataCategory = Add("Data");
	Gwen::Controls::CollapsibleCategory* colorCategory = Add("Colors");
	Gwen::Controls::CollapsibleCategory* orientationCategory = Add("Orientation");
	colorCategory->SetSize(200, 200);

	// Add the color properties.
	CreateColorProperties(colorCategory);

	// Add the orientation properties.
	CreateOrientationProperties(orientationCategory);

	// Add the data properties.
	CreateDataProperties(dataCategory);
	colorCategory->SetSize(200, 200);

	SetShouldDrawBackground(true);
}
	
void LightInspector::Cleanup()
{
	// Cleanup all the controls.
}

void LightInspector::Update(float dt)
{

}

void LightInspector::Draw(GLib::Graphics* pGraphics)
{
	mObjectTool->Draw(pGraphics);
}

void LightInspector::SetObject(void* pObject)
{
	// Send to the object tool.
	mObjectTool->SetObject((LightObject*)pObject);
	mObjectTool->AddOnPositionChange(&LightInspector::OnPositionChangeEvent, this);

	mLight = (LightObject*)pObject;
	mCurrentMaterial = mLight->GetMaterial();
	Material oldMat = mLight->GetMaterial();

	//
	// Set the position.
	//
	XMFLOAT3 pos = mLight->GetPosition();
	char buffer[256];
	sprintf(buffer, "%.2f", pos.x);
	mXProperty->GetProperty()->SetPropertyValue(buffer);
	sprintf(buffer, "%.2f", pos.y);
	mYProperty->GetProperty()->SetPropertyValue(buffer);
	sprintf(buffer, "%.2f", pos.z);
	mZProperty->GetProperty()->SetPropertyValue(buffer);

	//
	// Set the range and spot controls.
	//
	mRangeSlider->SetValue(mLight->GetRange());
	mRangeNumeric->SetValue(mLight->GetRange());
	mSpotSlider->SetValue(mLight->GetSpot());
	mSpotNumeric->SetValue(mLight->GetSpot());
	
	// Set the color intensity slider.
	XMFLOAT3 intensity = mLight->GetIntensity();
	mAmbientIntensitySlider->SetValue(intensity.x);
	mDiffuseIntensitySlider->SetValue(intensity.y);
	mSpecularIntensitySlider->SetValue(intensity.z);

	//
	// Set the color selectors.
	//
	// CRASHAR HÄR!!!
	mLight->SetMaterials(Material(oldMat.ambient, oldMat.diffuse, oldMat.specular));
	Material material = mLight->GetMaterial();
	int r, g, b;

	// Ambient.
	r = material.ambient.x * 255.0f;
	g = material.ambient.y * 255.0f;
	b = material.ambient.z * 255.0f;

	sprintf(buffer, "%i %i %i", b, g, r);
	mAmbientSelector->SetPropertyValue(buffer, true);

	// Diffuse.
	r = material.diffuse.x * 255.0f;
	g = material.diffuse.y * 255.0f;
	b = material.diffuse.z * 255.0f;

	sprintf(buffer, "%i %i %i", b, g, r);
	mDiffuseSelector->SetPropertyValue(buffer, true);

	// Specular.
	r = material.specular.x * 255.0f;
	g = material.specular.y * 255.0f;
	b = material.specular.z * 255.0f;

	sprintf(buffer, "%i %i %i", b, g, r);
	mSpecularSelector->SetPropertyValue(buffer, true);

	// Restores the light color.
	// [NOTE] All the SetXXX on controls changes the light color to the value they had before, which is wrong. Gets restored here.
	mCurrentMaterial = oldMat;
	SetLightMaterial();

	// Light type.
	if(mLight->GetLightType() == DIRECTIONAL_LIGHT)
		mLigtTypeCombo->OnItemSelected(mDirectionalLight);
	else if(mLight->GetLightType() == SPOT_LIGHT)
		mLigtTypeCombo->OnItemSelected(mSpotLight);
	else if(mLight->GetLightType() == POINT_LIGHT)
		mLigtTypeCombo->OnItemSelected(mPointLight);
}

void LightInspector::SetLightMaterial()
{
	// Get the current material.
	Material material = mCurrentMaterial;
	XMFLOAT4 ambient = XMFLOAT4(material.ambient.x, material.ambient.y, material.ambient.z, material.ambient.w);
	XMFLOAT4 diffuse = XMFLOAT4(material.diffuse.x, material.diffuse.y, material.diffuse.z, material.diffuse.w);
	XMFLOAT4 specular = XMFLOAT4(material.specular.x, material.specular.y, material.specular.z, material.specular.w);

	// Set the light material and intensity.
	mLight->SetMaterials(Material(ambient, diffuse, specular));
	mLight->SetIntensity(mAmbientIntensitySlider->GetValue(), mDiffuseIntensitySlider->GetValue(), mSpecularIntensitySlider->GetValue());
}

void LightInspector::OnOrientationChange(Gwen::Controls::Base* pControl)
{
	Gwen::Controls::PropertyRow* row = (Gwen::Controls::PropertyRow*)pControl;
	string label = ToString(row->GetLabel()->GetText().c_str());

	XMFLOAT3 pos = mLight->GetPosition();
	float xyz = atof(ToString(row->GetProperty()->GetPropertyValue().c_str()).c_str());
	if(label == "X")
		pos.x = xyz;
	else if(label == "Y")
		pos.y = xyz;
	else if(label == "Z")
		pos.z = xyz;

	// Set the lights position.
	mLight->SetPosition(pos);

	// Move the ObjectMover.
	mObjectTool->SetPosition(pos);
}

void LightInspector::OnDirectionSliderMoved(Base* pControl)
{
	Gwen::Controls::Slider* slider = (Gwen::Controls::Slider*)pControl;

	XMFLOAT3 direction = mLight->GetRotation();
	float value = slider->GetValue();
	if(slider->GetName() == "DirectionSliderX") 
		direction.x = value;
	else if(slider->GetName() == "DirectionSliderY")		
		direction.y = value;
	else if(slider->GetName() == "DirectionSliderZ")		
		direction.z = value;
	
	mLight->SetRotation(direction);
}

void LightInspector::OnColorChange(Gwen::Controls::Base* pControl)
{
	Gwen::Controls::Property::ColorSelector* colorSelector = (Gwen::Controls::Property::ColorSelector*)pControl;
	Gwen::Color color = colorSelector->m_Button->m_Color;

	if(colorSelector->GetName() == "AmbientColor")
		mCurrentMaterial.ambient = XMFLOAT4(color.b/255.0f, color.g/255.0f, color.r/255.0f, color.a/255.0f);
	else if(colorSelector->GetName() == "DiffuseColor")
		mCurrentMaterial.diffuse = XMFLOAT4(color.b/255.0f, color.g/255.0f, color.r/255.0f, color.a/255.0f);
	else if(colorSelector->GetName() == "SpecularColor")
		mCurrentMaterial.specular = XMFLOAT4(color.b/255.0f, color.g/255.0f, color.r/255.0f, color.a/255.0f);

	if(mLight != nullptr)
		SetLightMaterial();
}

void LightInspector::OnColorIntensitySliderMoved(Base* pControl)
{
	SetLightMaterial();
}

void LightInspector::OnLightTypeChange(Base* pControl)
{
	if(!mLight)
		return;

	Gwen::Controls::ComboBox* combo = (Gwen::Controls::ComboBox*)pControl;
	string name = combo->GetSelectedItem()->GetName();

	if(name == "Directional")
		mLight->SetLightType(DIRECTIONAL_LIGHT);
	else if(name == "Spot")
		mLight->SetLightType(SPOT_LIGHT);
	else if(name == "Point")
		mLight->SetLightType(POINT_LIGHT);
}

void LightInspector::OnRangeChange(Base* pControl)
{
	Gwen::Controls::NumericUpDown* numeric = (Gwen::Controls::NumericUpDown*)pControl;
	string s = ToString(numeric->GetText());
	float range = atof(s.c_str());
	
	if(mLight)
		mLight->SetRange(range);

	if(mRangeSlider)
		mRangeSlider->SetValue(range);
}
	
void LightInspector::OnSpotChange(Base* pControl)
{
	Gwen::Controls::NumericUpDown* numeric = (Gwen::Controls::NumericUpDown*)pControl;

	string s = ToString(numeric->GetText());
	float spot = atof(s.c_str());
	
	if(mLight) 
		mLight->SetSpot(spot);

	if(mSpotSlider)
		mSpotSlider->SetValue(spot);
}

void LightInspector::OnRangeSlider(Base* pControl)
{
	Gwen::Controls::Slider* slider = (Gwen::Controls::Slider*)pControl;

	if(mLight)
		mLight->SetRange(slider->GetValue());

	mRangeNumeric->SetValue(slider->GetValue());
}
	
void LightInspector::OnSpotSlider(Base* pControl)
{
	Gwen::Controls::Slider* slider = (Gwen::Controls::Slider*)pControl;

	if(mLight)
		mLight->SetSpot(slider->GetValue());

	mSpotNumeric->SetValue(slider->GetValue());
}

void LightInspector::CreateDataProperties(Gwen::Controls::Base* pParent)
{
	Gwen::Controls::Label* label = new Gwen::Controls::Label(pParent);
	label->SetText("Light type:");
	label->SetPos(5, 25);

	mLigtTypeCombo = new Gwen::Controls::ComboBox(pParent);
	mLigtTypeCombo->onSelection.Add(this, &LightInspector::OnLightTypeChange);
	mLigtTypeCombo->SetPos(5, 45);
	mLigtTypeCombo->SetWidth(170);
	mDirectionalLight = mLigtTypeCombo->AddItem(L"Directional light", "Directional");
	mSpotLight = mLigtTypeCombo->AddItem(L"Spot light", "Spot");
	mPointLight = mLigtTypeCombo->AddItem(L"Point light", "Point");

	// Range.
	mRangeNumeric = new Gwen::Controls::NumericUpDown(pParent);
	mRangeNumeric->onChanged.Add(this, &LightInspector::OnRangeChange);
	mRangeNumeric->SetBounds(60, 85, 50, 20);
	mRangeNumeric->SetValue(50);
	mRangeNumeric->SetMax(1000);
	mRangeNumeric->SetMin(0);

	label = new Gwen::Controls::Label(pParent);
	label->SetText("Range:");
	label->SetPos(10, 90);

	mRangeSlider = new Gwen::Controls::HorizontalSlider(pParent);
	InitSlider(mRangeSlider, "RangeSlider", 105, 0.0f, 0, 1000, false);
	mRangeSlider->SetPos(10, 105);
	mRangeSlider->onValueChanged.Add(this, &LightInspector::OnRangeSlider);

	// Spot factor.
	mSpotNumeric = new Gwen::Controls::NumericUpDown(pParent);
	mSpotNumeric->onChanged.Add(this, &LightInspector::OnSpotChange);
	mSpotNumeric->SetBounds(60, 145, 50, 20);
	mSpotNumeric->SetValue(64);
	mSpotNumeric->SetMax(256);
	mSpotNumeric->SetMin(0);

	label = new Gwen::Controls::Label(pParent);
	label->SetText("Spot :");
	label->SetPos(10, 145);

	mSpotSlider = new Gwen::Controls::HorizontalSlider(pParent);
	InitSlider(mSpotSlider, "SpotSlider", 110, 0.0f, 0, 256, false);
	mSpotSlider->SetPos(10, 165);
	mSpotSlider->onValueChanged.Add(this, &LightInspector::OnSpotSlider);
}

void LightInspector::CreateOrientationProperties(Gwen::Controls::Base* pParent)
{
	Gwen::Controls::PropertyTree* ptree = new Gwen::Controls::PropertyTree(pParent);
	ptree->SetBounds(0, 20, 200, 300);

	//
	//Position properties.
	//
	Gwen::Controls::Properties* positionProps = ptree->Add(L"Position");
	mXProperty = positionProps->Add("X");
	mXProperty->onChange.Add(this, &LightInspector::OnOrientationChange);
	mYProperty = positionProps->Add("Y");
	mYProperty->onChange.Add(this, &LightInspector::OnOrientationChange);
	mZProperty = positionProps->Add("Z");
	mZProperty->onChange.Add(this, &LightInspector::OnOrientationChange);

	//
	// Direction properties.
	//
	Gwen::Controls::Properties* directionProps = ptree->Add(L"Direction");

	// X.
	Gwen::Controls::Label* directionLabel = new Gwen::Controls::Label(directionProps);
	directionLabel->SetText("X rotation");
	directionLabel->SetPos(3, 10);

	Gwen::Controls::HorizontalSlider* directionSlider = new Gwen::Controls::HorizontalSlider(directionProps);
	InitSlider(directionSlider, "DirectionSliderX", 23, 0.0f, -1.0f, 1.0f, false);
	directionSlider->onValueChanged.Add(this, &LightInspector::OnDirectionSliderMoved);

	// Y.
	directionLabel = new Gwen::Controls::Label(directionProps);
	directionLabel->SetText("Y rotation");
	directionLabel->SetPos(3, 55);

	directionSlider = new Gwen::Controls::HorizontalSlider(directionProps);
	InitSlider(directionSlider, "DirectionSliderY", 68, 0.0f, -1.0f, 1.0f, false);
	directionSlider->onValueChanged.Add(this, &LightInspector::OnDirectionSliderMoved);

	// Z.
	directionLabel = new Gwen::Controls::Label(directionProps);
	directionLabel->SetText("Z rotation");
	directionLabel->SetPos(3, 100);

	directionSlider = new Gwen::Controls::HorizontalSlider(directionProps);
	InitSlider(directionSlider, "DirectionSliderZ", 113, 0.0f, -1.0f, 1.0f, false);
	directionSlider->onValueChanged.Add(this, &LightInspector::OnDirectionSliderMoved);

	ptree->ExpandAll();
}

void LightInspector::CreateColorProperties(Gwen::Controls::Base* pParent)
{
	//
	// Color selection.
	//
	Gwen::Controls::PropertyTree* ptree = new Gwen::Controls::PropertyTree(pParent);
	ptree->SetBounds(0, 20, 200, 300 );

	// Ambient.
	Gwen::Controls::Properties* ambientProps = ptree->Add(L"Ambient");
	mAmbientSelector = new Gwen::Controls::Property::ColorSelector(ambientProps);
	mAmbientSelector->onChange.Add(this, &LightInspector::OnColorChange);
	mAmbientSelector->SetName("AmbientColor");
	ambientProps->Add(L"Color", mAmbientSelector, L"255 255 255");

	mAmbientIntensitySlider = new Gwen::Controls::HorizontalSlider(ambientProps);
	mAmbientIntensitySlider->SetName("AmbientSlider");
	mAmbientIntensitySlider->SetWidth(170);
	mAmbientIntensitySlider->SetPos(0, 33);
	mAmbientIntensitySlider->SetHeight(30);
	mAmbientIntensitySlider->SetValue(1.0f);
	mAmbientIntensitySlider->SetNotchCount(20);
	mAmbientIntensitySlider->SetRange(0.0f, 1.0f);
	mAmbientIntensitySlider->SetClampToNotches(true);
	mAmbientIntensitySlider->onValueChanged.Add( this, &LightInspector::OnColorIntensitySliderMoved );
	
	Gwen::Controls::Label* ambientLabel = new Gwen::Controls::Label(ambientProps);
	ambientLabel->SetText("Intensity");
	ambientLabel->SetPos(3, 20);

	// Diffuse.
	Gwen::Controls::Properties* diffuseProps = ptree->Add(L"Diffuse");
	mDiffuseSelector = new Gwen::Controls::Property::ColorSelector(diffuseProps);
	mDiffuseSelector->onChange.Add(this, &LightInspector::OnColorChange);
	mDiffuseSelector->SetName("DiffuseColor");
	diffuseProps->Add(L"Color", mDiffuseSelector, L"255 255 255");

	mDiffuseIntensitySlider = new Gwen::Controls::HorizontalSlider(diffuseProps);
	mDiffuseIntensitySlider->SetName("DiffuseSlider");
	mDiffuseIntensitySlider->SetWidth(170);
	mDiffuseIntensitySlider->SetPos(0, 33);
	mDiffuseIntensitySlider->SetHeight(20);
	mDiffuseIntensitySlider->SetValue(0.0f);
	mDiffuseIntensitySlider->SetNotchCount(20);
	mDiffuseIntensitySlider->SetRange(0.0f, 1.0f);
	mDiffuseIntensitySlider->SetClampToNotches(true);
	mDiffuseIntensitySlider->onValueChanged.Add( this, &LightInspector::OnColorIntensitySliderMoved );
	
	Gwen::Controls::Label* diffuseLabel = new Gwen::Controls::Label(diffuseProps);
	diffuseLabel->SetText("Intensity");
	diffuseLabel->SetPos(3, 20);

	// Specular.
	Gwen::Controls::Properties* specularProps = ptree->Add(L"Specular");
	mSpecularSelector = new Gwen::Controls::Property::ColorSelector(specularProps);
	mSpecularSelector->onChange.Add(this, &LightInspector::OnColorChange);
	mSpecularSelector->SetName("SpecularColor");
	specularProps->Add(L"Color", mSpecularSelector, L"255 255 255");

	mSpecularIntensitySlider = new Gwen::Controls::HorizontalSlider(specularProps);
	mSpecularIntensitySlider->SetName("SpecularSlider");
	mSpecularIntensitySlider->SetWidth(170);
	mSpecularIntensitySlider->SetPos(0, 33);
	mSpecularIntensitySlider->SetHeight(20);
	mSpecularIntensitySlider->SetValue(0.0f);
	mSpecularIntensitySlider->SetNotchCount(20);
	mSpecularIntensitySlider->SetRange(0.0f, 1.0f);
	mSpecularIntensitySlider->SetClampToNotches(true);
	mSpecularIntensitySlider->onValueChanged.Add( this, &LightInspector::OnColorIntensitySliderMoved );
	
	Gwen::Controls::Label* specularLabel = new Gwen::Controls::Label(specularProps);
	specularLabel->SetText("Intensity");
	specularLabel->SetPos(3, 20);

	ptree->ExpandAll();
}

void LightInspector::InitSlider(Gwen::Controls::HorizontalSlider* slider, string name, int y, float value, float start, float end, bool clamp)
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

bool LightInspector::IsResponsible(int type)
{
	if(type == LIGHT_OBJECT)
		return true;
	else 
		return false;
}

void LightInspector::OnPositionChangeEvent(XMFLOAT3 position)
{
	char buffer[246];
	sprintf(buffer, "%.3f", position.x);
	mXProperty->GetProperty()->SetPropertyValue(buffer);

	sprintf(buffer, "%.3f", position.y);
	mYProperty->GetProperty()->SetPropertyValue(buffer);

	sprintf(buffer, "%.3f", position.z);
	mZProperty->GetProperty()->SetPropertyValue(buffer);
}

bool LightInspector::IsInspecting(int id)
{
	if(mLight->GetId() == id)
		return true;
	else
		return false;
}

GLib::Object3D* LightInspector::GetInspectedObject()
{
	return mLight;
}