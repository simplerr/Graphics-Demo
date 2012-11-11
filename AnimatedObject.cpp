#include "AnimatedObject.h"
#include "ModelImporter.h"
#include "SkinnedModel.h"
#include "Primitive.h"
#include "Graphics.h"
#include "SkinnedMesh.h"
#include "Effects.h"

using namespace GLib;

AnimatedObject::AnimatedObject(GLib::ModelImporter* importer, string filename)
	: Object3D(ANIMATED_OBJECT)
{
	// Load the model.
	mSkinnedModel = importer->LoadSkinnedModel(filename);
	mElapsedTime = 0.0f;
	mCurrentAnimIndex = 0;
}

//! Cleanup.
AnimatedObject::~AnimatedObject()
{
	
}

void AnimatedObject::Init()
{

}

//! Updates the object.
void AnimatedObject::Update(float dt)
{
	// Increments the elapsed time counter used for animations. 
	mElapsedTime += dt;
	mSkinnedModel->Update(dt);
}

//! Draws the objects model.
void AnimatedObject::Draw(GLib::Graphics* pGraphics)
{
	mSkinnedModel->SetAnimation(mCurrentAnimIndex);
	mSkinnedModel->SetElapsedTime(mElapsedTime);
	Effects::BasicFX->SetMaterial(GetMaterial());
	mSkinnedModel->Draw(pGraphics, GetWorldMatrix());

	Effects::BasicFX->SetUseAnimation(false);
	Effects::BasicFX->Apply(GetD3DContext());
	if(IsBoundingBoxVisible())
		pGraphics->DrawBoundingBox(&GetBoundingBox(), GetWorldMatrix(), Material(Colors::Blue));
}

//! Sets which animation to use by index.
void AnimatedObject::SetAnimation(int index)
{
	mCurrentAnimIndex = index;
}

//! Returns the bounding box in world space. [NOTE] Does not work [TODO].
AxisAlignedBox AnimatedObject::GetBoundingBox()
{
	SkinnedMeshList* meshList = mSkinnedModel->GetMeshList();
	AxisAlignedBox aabb = (*meshList)[0]->GetPrimitive()->GetBoundingBox();
	XMFLOAT3 min = aabb.Center - aabb.Extents;
	XMFLOAT3 max = aabb.Center + aabb.Extents;
	for(int i = 1; i < meshList->size(); i++)
	{
		AxisAlignedBox meshAABB = (*meshList)[i]->GetPrimitive()->GetBoundingBox();
		XMFLOAT3 meshMin = meshAABB.Center - meshAABB.Extents;
		XMFLOAT3 meshMax = meshAABB.Center + meshAABB.Extents;

		XMStoreFloat3(&min, XMVectorMin(XMLoadFloat3(&meshMin), XMLoadFloat3(&min)));
		XMStoreFloat3(&max, XMVectorMax(XMLoadFloat3(&meshMax), XMLoadFloat3(&max)));
	}

	aabb.Center =  (min + max) * 0.5f;
	aabb.Extents = (max - min) * 0.5f;

	// Break up the world matrix into it's components.
	XMVECTOR scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, GetWorldMatrix());

	// Transform the AABB with the components.
	TransformAxisAlignedBoxCustom(&aabb, &aabb, scale, rotation, translation);

	aabb.Center = GetPosition();	// [NOTE] Not like this in StaticObject.

	return aabb;
}

//! Returns the model.
GLib::SkinnedModel* AnimatedObject::GetModel()
{
	return mSkinnedModel;
}