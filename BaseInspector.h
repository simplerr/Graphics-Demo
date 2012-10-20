#pragma once

#include "Gwen/Gwen.h"
#include "Gwen/Controls/CollapsibleList.h"

//! The base class that all different inspectors ineherit from. 
class BaseInspector : public Gwen::Controls::CollapsibleList
{
public: 
	BaseInspector(Gwen::Controls::Base* pParent) : Gwen::Controls::CollapsibleList(pParent) {};
	virtual ~BaseInspector() {};

	virtual void Init() = 0;
	virtual void Cleanup() = 0;
	virtual void SetObject(void* pObject) = 0;
	virtual bool IsResponsible(int type) = 0;
};