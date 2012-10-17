#pragma once

#include "Object3D.h"
class SkinnedModel;

/**
	Represents an animated object. 
*/
class AnimatedObject : public Object3D
{
public:
	AnimatedObject(ModelImporter* importer, string filename);
	virtual ~AnimatedObject();

	virtual void Update(float dt);
	virtual void Draw(Graphics* pGraphics);

	void SetAnimation(int index);

	AxisAlignedBox GetBoundingBox();
	SkinnedModel* GetModel();
private:
	SkinnedModel* mSkinnedModel;
	int		mCurrentAnimIndex;
	float	mElapsedTime;
};