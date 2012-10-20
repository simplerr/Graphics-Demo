#pragma once
#include "BaseInspector.h"
#include "Gwen\Controls\Properties.h"
#include "Gwen/Controls/NumericUpDown.h"
#include "Gwen/Controls/HorizontalSlider.h"
#include "Light.h"
#include <xnamath.h>

class Light;

/*
	TODO
	- Set the correct color in the color selectors.
*/

class LightInspector : public BaseInspector
{
public:
	LightInspector(Gwen::Controls::Base* pParent);
	~LightInspector();

	void Init();
	void Cleanup();
	void SetObject(void* pObject);
	void OnSelection();
	void OnColorChange(Gwen::Controls::Base* pControl);
	void OnOrientationChange(Gwen::Controls::Base* pControl);
	void OnColorStrengthSliderMoved(Base* pControl);
	void OnDirectionSliderMoved(Base* pControl);
	void OnLightTypeChange(Base* pControl);
	void OnRangeChange(Base* pControl);
	void OnSpotChange(Base* pControl);
	void OnRangeSlider(Base* pControl);
	void OnSpotSlider(Base* pControl);
	void SetLightMaterial();
	bool IsResponsible(int type);
private:
	void CreateColorProperties(Gwen::Controls::Base* pParent);
	void CreateOrientationProperties(Gwen::Controls::Base* pParent);
	void CreateDataProperties(Gwen::Controls::Base* pParent);
	void InitSlider(Gwen::Controls::HorizontalSlider* slider, string name, int y, float value, float start, float end, bool clamp = true);
private:
	Light* mLight;
	float mAmbientStrength;
	float mDiffuseStrength;
	float mSpecularStrength;
	Material mCurrentMaterial;

	Gwen::Controls::PropertyRow* mXProperty, *mYProperty, *mZProperty;
	Gwen::Controls::NumericUpDown* mRangeNumeric;
	Gwen::Controls::NumericUpDown* mSpotNumeric;
	Gwen::Controls::HorizontalSlider* mRangeSlider;
	Gwen::Controls::HorizontalSlider* mSpotSlider;
};