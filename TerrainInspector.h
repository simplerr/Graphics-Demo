#pragma once
#include "BaseInspector.h"
#include "Gwen/Controls/CollapsibleList.h"
#include "Gwen/Controls/HorizontalSlider.h"
#include "Gwen\Controls\Properties.h"
#include "Gwen/Controls/ImagePanel.h"
#include "d3dUtil.h"
#include "Util.h"

class Terrain;
class TerrainTool;
class World;

//! The inspector for the terrain.
class TerrainInspector : public BaseInspector
{
public:
	TerrainInspector(Gwen::Controls::Base* pParent, TerrainTool* pTool);
	~TerrainInspector();

	void Init();
	void Cleanup();
	void Update(float dt);
	void Draw(Graphics* pGraphics);
	void SetObject(void* pObject);
	bool IsResponsible(int type);
	bool IsInspecting(int id);
	void SetActiveTool(ToolType tool);
	void OnRadiusChange(Gwen::Controls::Base* pControl);
	void OnStrengthChange(Gwen::Controls::Base* pControl);
	void OnPropertyChange(Gwen::Controls::Base* pControl);
	void OnComboSelect(Gwen::Controls::Base* pControl);
	void OnTextureSelected(Gwen::Controls::Base* pControl);
	Object3D* GetInspectedObject();
private:
	void InitSlider(Gwen::Controls::HorizontalSlider* slider, string name, int y, float value, float start, float end, bool clamp = true);
	Gwen::Controls::Button* CreateTerrainTexture(string texture, string name, float x, float y, Base* pParent);
private:
	TerrainTool* mTerrainTool;
	Gwen::Controls::HorizontalSlider* mRadiusSlider;
	Gwen::Controls::HorizontalSlider* mStrengthSlider;
	Gwen::Controls::PropertyRow*	  mRadiusProperty;
	Gwen::Controls::PropertyRow*	  mStrengthProperty;
	Gwen::Controls::Button*			  mTexture0, *mTexture1, *mTexture2, *mTexture3, *mTexture4;
};
