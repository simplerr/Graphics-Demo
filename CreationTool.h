#pragma once
#include "d3dUtil.h"
#include "Gwen/Controls/CollapsibleCategory.h"
#include "ModelLoaderXML.h"

class Graphics;
class World;

class CreationTool : public Gwen::Controls::CollapsibleCategory
{
public:
	CreationTool(Gwen::Controls::Base* pParent, World* pWorld);
	~CreationTool();

	void BuildSpawnList();
	void Update(float dt);
	void Draw(Graphics* pGraphics);
	void CreateStaticModel(XMFLOAT3 position, ModelData data);
	void CreateAnimatedModel(XMFLOAT3 position, ModelData data);
	void OnSelectChange(Gwen::Controls::Base* pControl);
private:
	ModelLoaderXML* mModelLoaderXML;
	World*	mWorld;
	bool	mModelSelected;
};
