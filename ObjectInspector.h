#include "BaseInspector.h"
#include "Gwen/Controls/NumericUpDown.h"
#include "Gwen/Controls/HorizontalSlider.h"
#include <string>
using namespace std;

class StaticObject;

class ObjectInspector : public BaseInspector
{
public:
	ObjectInspector(Gwen::Controls::Base* pParent);
	~ObjectInspector();

	void Init();
	void Cleanup();
	void SetObject(void* pObject);
	void OnRotationSliderMoved(Base* pControl);
	void OnRotationChange(Base* pControl);
	void OnScaleSliderMoved(Base* pControl);
	void OnScaleChange(Base* pControl);
	void OnOrientationChange(Gwen::Controls::Base* pControl);
	void OnAABBCheckBoxChange(Base* pControl);
	bool IsResponsible(int type);
private:
	void InitSlider(Gwen::Controls::HorizontalSlider* slider, string name, int y, float value, float start, float end, bool clamp = true);
private:
	StaticObject* mObject;
	Gwen::Controls::PropertyRow* mXProperty, *mYProperty, *mZProperty;
	Gwen::Controls::PropertyRow* mXRotationProperty, *mYRotationProperty, *mZRotationProperty;
	Gwen::Controls::PropertyRow* mXScaleProperty, *mYScaleProperty, *mZScaleProperty;
	Gwen::Controls::HorizontalSlider* mXRotationSlider, *mYRotationSlider, *mZRotationSlider;
	Gwen::Controls::HorizontalSlider* mXScaleSlider, *mYScaleSlider, *mZScaleSlider;
};