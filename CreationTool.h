#pragma once
#include "d3dUtil.h"
#include "Gwen/Controls/CollapsibleCategory.h"
#include "ModelLoaderXML.h"

class Graphics;
class World;
class Editor;
class Object3D;
class StaticObject;
class AnimatedObject;
class LightObject;

class CreationTool : public Gwen::Controls::CollapsibleCategory
{
public:
	CreationTool(Gwen::Controls::Base* pParent, World* pWorld);
	~CreationTool();

	void BuildSpawnList();
	void Update(float dt);
	void Draw(Graphics* pGraphics);
	StaticObject*	CreateStaticModel(XMFLOAT3 position, ModelData data);
	AnimatedObject* CreateAnimatedModel(XMFLOAT3 position, ModelData data);
	LightObject*	CreateLightObject(XMFLOAT3 position);
	void OnSelectChange(Gwen::Controls::Base* pControl);

	void SetEditor(Editor* pEditor);
	bool IsCreatingModel();
private:
	ModelLoaderXML* mModelLoaderXML;
	World*		mWorld;
	Editor*		mEditor;
	Object3D*	mPreviewObject;
	bool		mModelSelected;
};
