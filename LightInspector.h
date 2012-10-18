#pragma once
#include "BaseInspector.h"
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
	void SliderMoved(Base* pControl);
	void SetLightMaterial();
private:
	void CreateColorProperties(Gwen::Controls::Base* pParent);
	void CreateOrientationProperties(Gwen::Controls::Base* pParent);
private:
	Light* mLight;
	float mAmbientStrength;
	float mDiffuseStrength;
	float mSpecularStrength;
	Material mCurrentMaterial;
};