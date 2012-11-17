#pragma once
#include "d3dUtil.h"
#include "Gwen/Controls/CollapsibleCategory.h"
#include "ModelLoaderXML.h"

// Forward declarations.
#pragma region Forward declarations.
namespace GLib {
	class Graphics;
	class Input;
	class ModelImporter;
	class Object3D;
	class World;
	class StaticObject;
	class AnimatedObject;
	class LightObject;
}

class Editor;
#pragma endregion

class CreationTool : public Gwen::Controls::CollapsibleCategory
{
public:
	CreationTool(Gwen::Controls::Base* pParent, GLib::World* pWorld, GLib::ModelImporter* pModelImporter);
	~CreationTool();

	void BuildSpawnList();
	void Update(GLib::Input* pInput, float dt);
	void Draw(GLib::Graphics* pGraphics);
	GLib::StaticObject*		CreateStaticModel(XMFLOAT3 position, ModelData data);
	GLib::AnimatedObject*	CreateAnimatedModel(XMFLOAT3 position, ModelData data);
	GLib::LightObject*		CreateLightObject(XMFLOAT3 position);
	void OnSelectChange(Gwen::Controls::Base* pControl);

	void SetEditor(Editor* pEditor);
	bool IsCreatingModel();
private:
	ModelLoaderXML*		mModelLoaderXML;
	GLib::World*		mWorld;
	GLib::Object3D*		mPreviewObject;
	GLib::ModelImporter*mModelImporter;
	Editor*				mEditor;
	bool				mModelSelected;
};
