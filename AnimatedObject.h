#pragma once

#include "Object3D.h"

// Forward declarations.
#pragma region Forward declarations.
namespace GLib {
	class SkinnedModel;
}

class World;
#pragma endregion

//!
//	Represents an animated object. 
//!
class AnimatedObject : public Object3D
{
public:
	AnimatedObject(GLib::ModelImporter* importer, string filename);
	virtual ~AnimatedObject();

	virtual void Init();
	virtual void Update(float dt);
	virtual void Draw(GLib::Graphics* pGraphics);
	
	bool RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist);
	void SetAnimation(int index);
	AxisAlignedBox GetBoundingBox();
	GLib::SkinnedModel* GetModel();
private:
	GLib::SkinnedModel* mSkinnedModel;
	int		mCurrentAnimIndex;
	float	mElapsedTime;
};