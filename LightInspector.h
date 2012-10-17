#pragma once
#include "BaseInspector.h"

class Light;

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
private:
	Light* mLight;
};