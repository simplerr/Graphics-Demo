#include "Object3D.h"
#include "Graphics.h"
#include "Effects.h"
#include "Runnable.h"
#include "Primitive.h"
#include "PrimitiveFactory.h"
#include "D3DCore.h"
#include "StaticModel.h"
#include "StaticMesh.h"
#include "ModelImporter.h"

Object3D::Object3D(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
{
	mPosition = position;
	mRotation = rotation;
	mScale = scale;
	SetBoundingBoxVisible(false);
	SetName("Object");
}

//! Cleanup.
Object3D::~Object3D()
{
	
}

//! Sets the position.
void Object3D::SetPosition(XMFLOAT3 position)
{
	mPosition = position;
}
	
//! Sets the rotation.
void Object3D::SetRotation(XMFLOAT3 rotation)
{
	mRotation = rotation;
}
	
//! Sets the scale.
void Object3D::SetScale(XMFLOAT3 scale)
{
	mScale = scale;
}

//! Sets the default orientation.
void Object3D::SetDefaultOrientation()
{
	SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
}

//! Returns the world matrix.
XMMATRIX Object3D::GetWorldMatrix()
{
	XMMATRIX T, R, S, W;
	T = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
	R = XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z);
	S = XMMatrixScaling(mScale.x, mScale.y, mScale.z);

	return S*R*T;
}

//! Returns the position.
XMFLOAT3 Object3D::GetPosition()
{
	return mPosition;
}

//! Returns the rotation.
XMFLOAT3 Object3D::GetRotation()
{
	return mRotation;
}
	
//! Returns the scale.
XMFLOAT3 Object3D::GetScale()
{
	return mScale;

}

//! Returns the material.
Material Object3D::GetMaterial()
{
	return mMaterial;
}

//! Sets the object type.
void Object3D::SetType(ObjectType type)
{
	mType = type;
}

//! Sets the material.
void Object3D::SetMaterial(Material material)
{
	mMaterial = material;
}

//! Returns the object type.
ObjectType Object3D::GetType()
{
	return mType;
}

string Object3D::GetName()
{
	return mName;
}

//! True if the bounding box should be drawn.
void Object3D::SetBoundingBoxVisible(bool visible)
{
	mDrawBoundingBox = visible;
}

//! Returns true if the bounding box is visible.
bool Object3D::IsBoundingBoxVisible()
{
	return mDrawBoundingBox;
}

void Object3D::SetName(string name)
{
	mName = name;
}