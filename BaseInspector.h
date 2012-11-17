#pragma once
#include "Gwen/Gwen.h"
#include "Gwen/Controls/CollapsibleList.h"

// Forward declarations.
#pragma region Forward declarations.
namespace GLib {
	class Graphics;
	class Object3D;
}

class ObjectTool;
class Editor;
#pragma endregion

enum InspectorType
{
	OBJECT_INSPECTOR,
	LIGHT_INSPECTOR,
	TERRAIN_INSPECTOR
};

//! The base class that all different inspectors inherit from. 
class BaseInspector : public Gwen::Controls::CollapsibleList
{
public: 
	BaseInspector(Gwen::Controls::Base* pParent) : Gwen::Controls::CollapsibleList(pParent) {};
	virtual ~BaseInspector() {};

	virtual void Init() = 0;
	virtual void Cleanup() = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw(GLib::Graphics* pGraphics) = 0;
	virtual void SetObject(void* pObject) = 0;
	virtual bool IsResponsible(int type) = 0;
	virtual bool IsInspecting(int id) = 0;
	virtual GLib::Object3D* GetInspectedObject() = 0;

	void	SetInspectorType(InspectorType type) { mInspectorType = type; }
	void	SetEditor(Editor* pEditor)	{ mEditor = pEditor; }
	Editor* GetEditor()					{ return mEditor; }
	InspectorType GetInspectorType()	{ return mInspectorType; }
private:
	Editor* mEditor;
	InspectorType mInspectorType;
};