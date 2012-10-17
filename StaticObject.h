#pragma once

#include "Object3D.h"

/**
	Represents a static object. 
	Can be loaded from a file or the primitive factory.
*/
class StaticObject : public Object3D
{
public:
	StaticObject(ModelImporter* importer, string filename);
	~StaticObject();

	virtual void Update(float dt);
	virtual void Draw(Graphics* pGraphics);

	AxisAlignedBox GetBoundingBox();
	StaticModel* GetModel();
private:
	StaticModel* mModel;
};