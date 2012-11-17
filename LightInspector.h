#pragma once
#include "BaseInspector.h"
#include "Gwen\Controls\Properties.h"
#include "Gwen/Controls/NumericUpDown.h"
#include "Gwen/Controls/HorizontalSlider.h"
#include "Gwen/Controls/Property/ColorSelector.h"
#include "Gwen/Controls/ComboBox.h"
#include "Light.h"
#include <xnamath.h>

// Forward declarations.
#pragma region Forward declarations.
namespace GLib {
	class LightObject;
	Object3D;
}
class ObjectTool;
#pragma endregion

//! The inspector for Light objects.
class LightInspector : public BaseInspector
{
public:
	LightInspector(Gwen::Controls::Base* pParent, ObjectTool* pTool);
	~LightInspector();

	void Init();
	void Cleanup();
	void Update(float dt);
	void Draw(GLib::Graphics* pGraphics);
	void SetObject(void* pObject);
	void OnColorChange(Gwen::Controls::Base* pControl);
	void OnOrientationChange(Gwen::Controls::Base* pControl);
	void OnColorIntensitySliderMoved(Base* pControl);
	void OnDirectionSliderMoved(Base* pControl);
	void OnLightTypeChange(Base* pControl);
	void OnRangeChange(Base* pControl);
	void OnSpotChange(Base* pControl);
	void OnRangeSlider(Base* pControl);
	void OnSpotSlider(Base* pControl);
	void SetLightMaterial();
	bool IsResponsible(int type);
	bool IsInspecting(int id);
	GLib::Object3D* GetInspectedObject();

	// Callback.
	void OnPositionChangeEvent(XMFLOAT3 position);
private:
	void CreateColorProperties(Gwen::Controls::Base* pParent);
	void CreateOrientationProperties(Gwen::Controls::Base* pParent);
	void CreateDataProperties(Gwen::Controls::Base* pParent);
	void InitSlider(Gwen::Controls::HorizontalSlider* slider, string name, int y, float value, float start, float end, bool clamp = true);
private:
	GLib::LightObject* mLight;
	GLib::Material mCurrentMaterial;

	Gwen::Controls::PropertyRow* mXProperty, *mYProperty, *mZProperty;
	Gwen::Controls::HorizontalSlider* mAmbientIntensitySlider, *mDiffuseIntensitySlider, *mSpecularIntensitySlider;
	Gwen::Controls::NumericUpDown* mRangeNumeric;
	Gwen::Controls::NumericUpDown* mSpotNumeric;
	Gwen::Controls::HorizontalSlider* mRangeSlider;
	Gwen::Controls::HorizontalSlider* mSpotSlider;
	Gwen::Controls::Property::ColorSelector* mAmbientSelector, *mDiffuseSelector, *mSpecularSelector;
	Gwen::Controls::ComboBox* mLigtTypeCombo;
	Gwen::Controls::MenuItem* mDirectionalLight, *mSpotLight, *mPointLight;
	ObjectTool* mObjectTool;
};