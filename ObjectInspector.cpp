#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/Property/ColorSelector.h"
#include "Gwen/Controls/PropertyTree.h"
#include "Gwen/Controls/ComboBox.h"
#include "Gwen/Controls/CheckBox.h"
#include "ObjectInspector.h"
#include "Object3D.h"
#include "Util.h"
#include "StaticObject.h"

ObjectInspector::ObjectInspector(Gwen::Controls::Base* pParent)
	: BaseInspector(pParent)
{

}
	
ObjectInspector::~ObjectInspector()
{

}

void ObjectInspector::Init()
{
	// Create all the controls.
	SetBounds(0, 0, 220, 800);

	Gwen::Controls::CollapsibleCategory* dataCategory = Add("Orientation");

	Gwen::Controls::PropertyTree* ptree = new Gwen::Controls::PropertyTree(dataCategory);
	ptree->SetBounds(0, 20, 200, 600);

	//
	//Position properties.
	//
	Gwen::Controls::Properties* positionProps = ptree->Add(L"Position");
	mXProperty = positionProps->Add("X");
	mXProperty->onChange.Add(this, &ObjectInspector::OnOrientationChange);
	mYProperty = positionProps->Add("Y");
	mYProperty->onChange.Add(this, &ObjectInspector::OnOrientationChange);
	mZProperty = positionProps->Add("Z");
	mZProperty->onChange.Add(this, &ObjectInspector::OnOrientationChange);

	//
	// Rotation properties.
	//
	Gwen::Controls::Properties* rotationProps = ptree->Add(L"Rotation");

	mXRotationProperty = rotationProps->Add("X rotation");
	mXRotationProperty->onChange.Add(this, &ObjectInspector::OnRotationChange);
	mYRotationProperty = rotationProps->Add("Y rotation");
	mYRotationProperty->onChange.Add(this, &ObjectInspector::OnRotationChange);
	mZRotationProperty = rotationProps->Add("Z rotation");
	mZRotationProperty->onChange.Add(this, &ObjectInspector::OnRotationChange);

	// X rotation. 
	Gwen::Controls::Label* rotationLabel = new Gwen::Controls::Label(rotationProps);
	rotationLabel->SetText("X rotation");
	rotationLabel->SetPos(3, 60);

	mXRotationSlider = new Gwen::Controls::HorizontalSlider(rotationProps);
	InitSlider(mXRotationSlider, "RotationSliderX", 23+50, 0.0f, -3.14f, 3.14f, false);
	mXRotationSlider->onValueChanged.Add(this, &ObjectInspector::OnRotationSliderMoved);

	// Y rotation.
	rotationLabel = new Gwen::Controls::Label(rotationProps);
	rotationLabel->SetText("Y rotation");
	rotationLabel->SetPos(3, 55+50);

	mYRotationSlider = new Gwen::Controls::HorizontalSlider(rotationProps);
	InitSlider(mYRotationSlider, "RotationSliderY", 68+50, 0.0f, -3.14f, 3.14f, false);
	mYRotationSlider->onValueChanged.Add(this, &ObjectInspector::OnRotationSliderMoved);

	// Z rotation.
	rotationLabel = new Gwen::Controls::Label(rotationProps);
	rotationLabel->SetText("Z rotation");
	rotationLabel->SetPos(3, 100+50);
	
	mZRotationSlider = new Gwen::Controls::HorizontalSlider(rotationProps);
	InitSlider(mZRotationSlider, "RotationSliderZ", 113+50, 0.0f, -3.14f, 3.14f, false);
	mZRotationSlider->onValueChanged.Add(this, &ObjectInspector::OnRotationSliderMoved);

	//
	// Scale properties
	//
	Gwen::Controls::Properties* scaleProps = ptree->Add(L"Scale");

	mXScaleProperty = scaleProps->Add("X scale");
	mXScaleProperty->onChange.Add(this, &ObjectInspector::OnScaleChange);
	mYScaleProperty = scaleProps->Add("Y scale");
	mYScaleProperty->onChange.Add(this, &ObjectInspector::OnScaleChange);
	mZScaleProperty = scaleProps->Add("Z scale");
	mZScaleProperty->onChange.Add(this, &ObjectInspector::OnScaleChange);

	// X scale.
	Gwen::Controls::Label* scaleLabel = new Gwen::Controls::Label(scaleProps);
	scaleLabel->SetText("X scale");
	scaleLabel->SetPos(3, 10+50);

	mXScaleSlider = new Gwen::Controls::HorizontalSlider(scaleProps);
	InitSlider(mXScaleSlider, "ScaleSliderX", 23+50, 0.0f, 0.1, 15.0f, false);
	mXScaleSlider->onValueChanged.Add(this, &ObjectInspector::OnScaleSliderMoved);

	// Y scale.
	scaleLabel = new Gwen::Controls::Label(scaleProps);
	scaleLabel->SetText("Y scale");
	scaleLabel->SetPos(3, 55+50);

	mYScaleSlider = new Gwen::Controls::HorizontalSlider(scaleProps);
	InitSlider(mYScaleSlider, "ScaleSliderY", 68+50, 0.0f, 0.1, 15.0f, false);
	mYScaleSlider->onValueChanged.Add(this, &ObjectInspector::OnScaleSliderMoved);

	// Z scale.
	scaleLabel = new Gwen::Controls::Label(scaleProps);
	scaleLabel->SetText("Z scale");
	scaleLabel->SetPos(3, 100+50);

	mZScaleSlider = new Gwen::Controls::HorizontalSlider(scaleProps);
	InitSlider(mZScaleSlider, "ScaleSliderZ", 113+50, 0.0f, 0.1, 15.0f, false);
	mZScaleSlider->onValueChanged.Add(this, &ObjectInspector::OnScaleSliderMoved);

	ptree->ExpandAll();

	//
	// Properties.
	//
	Gwen::Controls::CollapsibleCategory* propertiesCategory = Add("Properties");
	Gwen::Controls::CheckBoxWithLabel* checkBox = new Gwen::Controls::CheckBoxWithLabel(propertiesCategory);
	checkBox->SetPos(10, 25);
	checkBox->Label()->SetText("Draw bounding box");
	checkBox->Checkbox()->onCheckChanged.Add(this, &ObjectInspector::OnAABBCheckBoxChange);
}
	
void ObjectInspector::Cleanup()
{

}

void ObjectInspector::OnAABBCheckBoxChange(Base* pControl)
{
	Gwen::Controls::CheckBox* check = (Gwen::Controls::CheckBox*)pControl;
	mObject->SetBoundingBoxVisible(check->IsChecked());
}

void ObjectInspector::OnRotationSliderMoved(Base* pControl)
{
	Gwen::Controls::Slider* slider = (Gwen::Controls::Slider*)pControl;

	XMFLOAT3 rotation = mObject->GetRotation();
	float value = slider->GetValue();
	char buffer[256];
	sprintf(buffer, "%.3f", value);
	if(slider->GetName() == "RotationSliderX") {
		rotation.x = value;
		mXRotationProperty->GetProperty()->SetPropertyValue(buffer);
	}
	else if(slider->GetName() == "RotationSliderY") {
		rotation.y = value;
		mYRotationProperty->GetProperty()->SetPropertyValue(buffer);
	}
	else if(slider->GetName() == "RotationSliderZ") {
		rotation.z = value;
		mZRotationProperty->GetProperty()->SetPropertyValue(buffer);
	}
	
	mObject->SetRotation(rotation);
}
	
void ObjectInspector::OnRotationChange(Base* pControl)
{
	Gwen::Controls::PropertyRow* row = (Gwen::Controls::PropertyRow*)pControl;
	string label = ToString(row->GetLabel()->GetText().c_str());

	XMFLOAT3 rotation = mObject->GetRotation();
	float xyz = atof(ToString(row->GetProperty()->GetPropertyValue().c_str()).c_str());
	if(label == "X rotation") {
		rotation.x = xyz;
		mXRotationSlider->SetValue(rotation.x);
	}
	else if(label == "Y rotation") {
		rotation.y = xyz;
		mYRotationSlider->SetValue(rotation.y);
	}
	else if(label == "Z rotation") {
		rotation.z = xyz;
		mZRotationSlider->SetValue(rotation.z);
	}

	mObject->SetRotation(rotation);
}

void ObjectInspector::OnOrientationChange(Gwen::Controls::Base* pControl)
{
	Gwen::Controls::PropertyRow* row = (Gwen::Controls::PropertyRow*)pControl;
	string label = ToString(row->GetLabel()->GetText().c_str());

	XMFLOAT3 pos = mObject->GetPosition();
	float xyz = atof(ToString(row->GetProperty()->GetPropertyValue().c_str()).c_str());
	if(label == "X")
		pos.x = xyz;
	else if(label == "Y")
		pos.y = xyz;
	else if(label == "Z")
		pos.z = xyz;

	mObject->SetPosition(pos);
}

void ObjectInspector::OnScaleSliderMoved(Base* pControl)
{
	Gwen::Controls::Slider* slider = (Gwen::Controls::Slider*)pControl;

	XMFLOAT3 scale = mObject->GetScale();
	float value = slider->GetValue();
	char buffer[256];
	sprintf(buffer, "%.3f", value);
	if(slider->GetName() == "ScaleSliderX") {
		scale.x = value;
		mXScaleProperty->GetProperty()->SetPropertyValue(buffer);
	}
	else if(slider->GetName() == "ScaleSliderY") {
		scale.y = value;
		mYScaleProperty->GetProperty()->SetPropertyValue(buffer);
	}
	else if(slider->GetName() == "ScaleSliderZ") {
		scale.z = value;
		mZScaleProperty->GetProperty()->SetPropertyValue(buffer);
	}
	
	mObject->SetScale(scale);
}

void ObjectInspector::OnScaleChange(Base* pControl)
{
	Gwen::Controls::PropertyRow* row = (Gwen::Controls::PropertyRow*)pControl;
	string label = ToString(row->GetLabel()->GetText().c_str());

	XMFLOAT3 scale = mObject->GetScale();
	float xyz = atof(ToString(row->GetProperty()->GetPropertyValue().c_str()).c_str());
	if(label == "X scale") {
		scale.x = xyz;
		mXScaleSlider->SetValue(scale.x);
	}
	else if(label == "Y scale") {
		scale.y = xyz;
		mYScaleSlider->SetValue(scale.y);
	}
	else if(label == "Z scale") {
		scale.z = xyz;
		mZScaleSlider->SetValue(scale.z);
	}

	mObject->SetScale(scale);
}

void ObjectInspector::SetObject(void* pObject)
{
	mObject = (StaticObject*)pObject;

	// Set the position properties.
	XMFLOAT3 position = mObject->GetPosition();
	char buffer[256];
	sprintf(buffer, "%.3f", position.x);
	mXProperty->GetProperty()->SetPropertyValue(buffer);
	sprintf(buffer, "%.3f", position.y);
	mYProperty->GetProperty()->SetPropertyValue(buffer);
	sprintf(buffer, "%.3f", position.z);
	mZProperty->GetProperty()->SetPropertyValue(buffer);

	// Set the rotation properties.
	XMFLOAT3 rotation = mObject->GetRotation();
	mXRotationSlider->SetValue(rotation.x);
	mYRotationSlider->SetValue(rotation.y);
	mZRotationSlider->SetValue(rotation.z);

	sprintf(buffer, "%.3f", rotation.x);
	mXRotationProperty->GetProperty()->SetPropertyValue(buffer);
	sprintf(buffer, "%.3f", rotation.y);
	mYRotationProperty->GetProperty()->SetPropertyValue(buffer);
	sprintf(buffer, "%.3f", rotation.z);
	mZRotationProperty->GetProperty()->SetPropertyValue(buffer);

	// Set the scale properties.
	XMFLOAT3 scale = mObject->GetScale();
	mXScaleSlider->SetValue(scale.x);
	mYScaleSlider->SetValue(scale.y);
	mZScaleSlider->SetValue(scale.z);

	sprintf(buffer, "%.3f", scale.x);
	mXScaleProperty->GetProperty()->SetPropertyValue(buffer);
	sprintf(buffer, "%.3f", scale.y);
	mYScaleProperty->GetProperty()->SetPropertyValue(buffer);
	sprintf(buffer, "%.3f", scale.z);
	mZScaleProperty->GetProperty()->SetPropertyValue(buffer);
}

void ObjectInspector::InitSlider(Gwen::Controls::HorizontalSlider* slider, string name, int y, float value, float start, float end, bool clamp)
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

bool ObjectInspector::IsResponsible(int type)
{
	if(type == STATIC_OBJECT || type == ANIMATED_OBJECT)
		return true;
	else 
		return false;
}