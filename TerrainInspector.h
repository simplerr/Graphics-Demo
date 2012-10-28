#pragma once
#include "BaseInspector.h"
#include "Gwen/Controls/CollapsibleList.h"
#include "Gwen/Controls/HorizontalSlider.h"
#include "Gwen\Controls\Properties.h"
#include "d3dUtil.h"

class Terrain;
class TerrainTool;

class TerrainInspector : public BaseInspector
{
public:
	TerrainInspector(Gwen::Controls::Base* pParent, TerrainTool* pTerrain);
	~TerrainInspector();

	void Init();
	void Cleanup();
	void SetObject(void* pObject);
	void SetObjectMover(ObjectMover* pObjectMover) {}; // Doesn't have any.
	bool IsResponsible(int type);
	void OnRadiusChange(Gwen::Controls::Base* pControl);
	void OnStrengthChange(Gwen::Controls::Base* pControl);
	void OnPropertyChange(Gwen::Controls::Base* pControl);
private:
	void InitSlider(Gwen::Controls::HorizontalSlider* slider, string name, int y, float value, float start, float end, bool clamp = true);
private:
	Terrain*	 mTerrain;
	TerrainTool* mTerrainTool;
	Gwen::Controls::HorizontalSlider* mRadiusSlider;
	Gwen::Controls::HorizontalSlider* mStrengthSlider;
	Gwen::Controls::PropertyRow*	  mRadiusProperty;
	Gwen::Controls::PropertyRow*	  mStrengthProperty;
};
