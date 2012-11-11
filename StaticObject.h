#pragma once
#include "Object3D.h"

// Forward declarations.
#pragma region Forward declarations.
class World;
#pragma endregion

//!
//	Represents a static object. 
//	Can be loaded from a file or the primitive factory.
//!
class StaticObject : public Object3D
{
public:
	StaticObject(GLib::ModelImporter* importer, string filename);
	~StaticObject();

	virtual void Init();
	virtual void Update(float dt);
	virtual void Draw(GLib::Graphics* pGraphics);

	bool RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist);
	AxisAlignedBox GetBoundingBox();
	GLib::StaticModel* GetModel();
private:
	GLib::StaticModel* mModel;
};