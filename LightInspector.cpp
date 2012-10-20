#include "LightInspector.h"
#include "Light.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/Property/ColorSelector.h"
#include "Gwen/Controls/PropertyTree.h"
#include "Gwen/Controls/ComboBox.h"
#include "Util.h"
#include "Object3D.h"

LightInspector::LightInspector(Gwen::Controls::Base* pParent)
	: BaseInspector(pParent)
{
	mLight = nullptr;
	mAmbientStrength = 1.0f;
	mDiffuseStrength = mSpecularStrength =  0.0f;
	mRangeSlider = mSpotSlider = nullptr;
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
	onSelection.Add(this, &LightInspector::OnSelection);

	Gwen::Controls::CollapsibleCategory* colorCategory = Add("Colors");
	colorCategory->SetSize(200, 200);
	onSelection.Add(this, &LightInspector::OnSelection);

	Gwen::Controls::CollapsibleCategory* orientationCategory = Add("Orientation");
	onSelection.Add(this, &LightInspector::OnSelection);

	// Add the color properties.
	CreateColorProperties(colorCategory);

	// Add the orientation properties.
	CreateOrientationProperties(orientationCategory);

	// Add the data properties.
	CreateDataProperties(dataCategory);
	colorCategory->SetSize(200, 200);
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

void LightInspector::OnColorStrengthSliderMoved(Base* pControl)
{
	Gwen::Controls::Slider* slider = (Gwen::Controls::Slider*)pControl;

	if(slider->GetName() == "AmbientSlider") 
		mAmbientStrength = slider->GetValue();
	else if(slider->GetName() == "DiffuseSlider")		
		mDiffuseStrength = slider->GetValue();
	else if(slider->GetName() == "SpecularSlider")		
		mSpecularStrength = slider->GetValue();

	SetLightMaterial();
}

void LightInspector::OnDirectionSliderMoved(Base* pControl)
{
	Gwen::Controls::Slider* slider = (Gwen::Controls::Slider*)pControl;

	XMFLOAT3 direction = mLight->GetDirection();
	float value = slider->GetValue();
	if(slider->GetName() == "DirectionSliderX") 
		direction.x = value;
	else if(slider->GetName() == "DirectionSliderY")		
		direction.y = value;
	else if(slider->GetName() == "DirectionSliderZ")		
		direction.z = value;
	
	mLight->SetDirection(direction);
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

void LightInspector::SetLightMaterial()
{
	Material material = mCurrentMaterial;
	XMFLOAT4 ambient = XMFLOAT4(material.ambient.x * mAmbientStrength, material.ambient.y * mAmbientStrength, material.ambient.z * mAmbientStrength, material.ambient.w);
	XMFLOAT4 diffuse = XMFLOAT4(material.diffuse.x * mDiffuseStrength, material.diffuse.y * mDiffuseStrength, material.diffuse.z * mDiffuseStrength, material.diffuse.w);
	XMFLOAT4 specular = XMFLOAT4(material.specular.x * mSpecularStrength, material.specular.y * mSpecularStrength, material.specular.z * mSpecularStrength, material.specular.w);

	mLight->SetMaterials(ambient, diffuse, specular);
}

void LightInspector::SetObject(void* pObject)
{
	mLight = (Light*)pObject;
	mCurrentMaterial = mLight->GetMaterial();
	SetLightMaterial();

	// Set the position.
	XMFLOAT3 pos = mLight->GetPosition();
	char buffer[256];
	sprintf(buffer, "%.2f", pos.x);
	mXProperty->GetProperty()->SetPropertyValue(buffer);
	sprintf(buffer, "%.2f", pos.y);
	mYProperty->GetProperty()->SetPropertyValue(buffer);
	sprintf(buffer, "%.2f", pos.z);
	mZProperty->GetProperty()->SetPropertyValue(buffer);

	// Set the range and spot controls.
	mRangeSlider->SetValue(mLight->GetRange());
	mRangeNumeric->SetValue(mLight->GetRange());
	mSpotSlider->SetValue(mLight->GetSpot());
	mSpotNumeric->SetValue(mLight->GetSpot());
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

	mLight->SetPosition(pos);
}

void LightInspector::OnLightTypeChange(Base* pControl)
{
	if(!mLight)
		return;

	Gwen::Controls::ComboBox* combo = (Gwen::Controls::ComboBox*)pControl;
	string name = combo->GetSelectedItem()->GetName();

	if(name == "Directional")
		mLight->SetType(DIRECTIONAL_LIGHT);
	else if(name == "Spot")
		mLight->SetType(SPOT_LIGHT);
	else if(name == "Point")
		mLight->SetType(POINT_LIGHT);
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

	Gwen::Controls::ComboBox* combo = new Gwen::Controls::ComboBox(pParent);
	combo->onSelection.Add(this, &LightInspector::OnLightTypeChange);
	combo->SetPos(5, 45);
	combo->SetWidth(170);
	combo->AddItem(L"Directional light", "Directional");
	combo->AddItem(L"Spot light", "Spot");
	combo->AddItem(L"Point light", "Point");

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
	Gwen::Controls::Property::ColorSelector* colorSelector = new Gwen::Controls::Property::ColorSelector(ambientProps);
	colorSelector->onChange.Add(this, &LightInspector::OnColorChange);
	colorSelector->SetName("AmbientColor");
	ambientProps->Add(L"Color", colorSelector, L"255 255 255");

	Gwen::Controls::HorizontalSlider* ambientSlider = new Gwen::Controls::HorizontalSlider(ambientProps);
	ambientSlider->SetName("AmbientSlider");
	ambientSlider->SetWidth(170);
	ambientSlider->SetPos(0, 33);
	ambientSlider->SetHeight(30);
	ambientSlider->SetValue(1.0f);
	ambientSlider->SetNotchCount(20);
	ambientSlider->SetRange(0.0f, 1.0f);
	ambientSlider->SetClampToNotches(true);
	ambientSlider->onValueChanged.Add( this, &LightInspector::OnColorStrengthSliderMoved );
	
	Gwen::Controls::Label* ambientLabel = new Gwen::Controls::Label(ambientProps);
	ambientLabel->SetText("Strength");
	ambientLabel->SetPos(3, 20);

	// Diffuse.
	Gwen::Controls::Properties* diffuseProps = ptree->Add(L"Diffuse");
	colorSelector = new Gwen::Controls::Property::ColorSelector(diffuseProps);
	colorSelector->onChange.Add(this, &LightInspector::OnColorChange);
	colorSelector->SetName("DiffuseColor");
	diffuseProps->Add(L"Color", colorSelector, L"255 255 255");

	Gwen::Controls::HorizontalSlider* diffuseSlider = new Gwen::Controls::HorizontalSlider(diffuseProps);
	diffuseSlider->SetName("DiffuseSlider");
	diffuseSlider->SetWidth(170);
	diffuseSlider->SetPos(0, 33);
	diffuseSlider->SetHeight(20);
	diffuseSlider->SetValue(0.0f);
	diffuseSlider->SetNotchCount(20);
	diffuseSlider->SetRange(0.0f, 1.0f);
	diffuseSlider->SetClampToNotches(true);
	diffuseSlider->onValueChanged.Add( this, &LightInspector::OnColorStrengthSliderMoved );
	
	Gwen::Controls::Label* diffuseLabel = new Gwen::Controls::Label(diffuseProps);
	diffuseLabel->SetText("Strength");
	diffuseLabel->SetPos(3, 20);

	// Specular.
	Gwen::Controls::Properties* specularProps = ptree->Add(L"Specular");
	colorSelector = new Gwen::Controls::Property::ColorSelector(specularProps);
	colorSelector->onChange.Add(this, &LightInspector::OnColorChange);
	colorSelector->SetName("SpecularColor");
	specularProps->Add(L"Color", colorSelector, L"255 255 255");

	Gwen::Controls::HorizontalSlider* specularSlider = new Gwen::Controls::HorizontalSlider(specularProps);
	specularSlider->SetName("SpecularSlider");
	specularSlider->SetWidth(170);
	specularSlider->SetPos(0, 33);
	specularSlider->SetHeight(20);
	specularSlider->SetValue(0.0f);
	specularSlider->SetNotchCount(20);
	specularSlider->SetRange(0.0f, 1.0f);
	specularSlider->SetClampToNotches(true);
	specularSlider->onValueChanged.Add( this, &LightInspector::OnColorStrengthSliderMoved );
	
	Gwen::Controls::Label* specularLabel = new Gwen::Controls::Label(specularProps);
	specularLabel->SetText("Strength");
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
	if(type == LIGHT)
		return true;
	else 
		return false;
}