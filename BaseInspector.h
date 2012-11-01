#pragma once

#include "Gwen/Gwen.h"
#include "Gwen/Controls/CollapsibleList.h"

class ObjectTool;
class Graphics;

//! The base class that all different inspectors inherit from. 
class BaseInspector : public Gwen::Controls::CollapsibleList
{
public: 
	BaseInspector(Gwen::Controls::Base* pParent) : Gwen::Controls::CollapsibleList(pParent) {};
	virtual ~BaseInspector() {};

	virtual void Init() = 0;
	virtual void Cleanup() = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw(Graphics* pGraphics) = 0;
	virtual void SetObject(void* pObject) = 0;
	virtual bool IsResponsible(int type) = 0;
};