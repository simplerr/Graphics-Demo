#include "LightInspector.h"
#include "Light.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/HorizontalSlider.h"
#include "Gwen/Controls/Property/ColorSelector.h"
#include "Gwen/Controls/PropertyTree.h"
#include "Gwen/Controls/NumericUpDown.h"
#include "Util.h"

LightInspector::LightInspector(Gwen::Controls::Base* pParent)
	: BaseInspector(pParent)
{
	mLight = nullptr;
	mAmbientStrength = 1.0f;
	mDiffuseStrength = mSpecularStrength =  0.0f;
}
	
LightInspector::~LightInspector()
{
	Cleanup();
}

void LightInspector::Init()
{
	// Create all the controls.
	SetBounds(0, 0, 200, 700);

	Gwen::Controls::CollapsibleCategory* colorCategory = Add("Colors");
	onSelection.Add(this, &LightInspector::OnSelection);

	Gwen::Controls::CollapsibleCategory* orientationCategory = Add("Orientation");
	onSelection.Add(this, &LightInspector::OnSelection);

	// Add the color properties.
	CreateColorProperties(colorCategory);

	// Add the orientation properties.
	CreateOrientationProperties(orientationCategory);
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

void LightInspector::SliderMoved(Base* pControl)
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

void LightInspector::CreateOrientationProperties(Gwen::Controls::Base* pParent)
{
	Gwen::Controls::PropertyTree* ptree = new Gwen::Controls::PropertyTree(pParent);
	ptree->SetBounds(0, 20, 200, 300);

	Gwen::Controls::Properties* positionProps = ptree->Add(L"Position");

	positionProps->Add("X")->onChange.Add(this, &LightInspector::OnOrientationChange);
	positionProps->Add("Y")->onChange.Add(this, &LightInspector::OnOrientationChange);
	positionProps->Add("Z")->onChange.Add(this, &LightInspector::OnOrientationChange);

	Gwen::Controls::Properties* rotationProps = ptree->Add(L"Rotation");
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
	ambientSlider->onValueChanged.Add( this, &LightInspector::SliderMoved );
	
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
	diffuseSlider->onValueChanged.Add( this, &LightInspector::SliderMoved );
	
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
	specularSlider->onValueChanged.Add( this, &LightInspector::SliderMoved );
	
	Gwen::Controls::Label* specularLabel = new Gwen::Controls::Label(specularProps);
	specularLabel->SetText("Strength");
	specularLabel->SetPos(3, 20);
}