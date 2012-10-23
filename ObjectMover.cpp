#include "ObjectMover.h"
#include "Object3D.h"
#include "ModelImporter.h"
#include "StaticObject.h"
#include "Input.h"
#include "Camera.h"
#include "Runnable.h"
#include "Graphics.h"
#include "d3dUtil.h"
#include <limits>
#include "Vertex.h"
#include "Primitive.h"
#include "Effects.h"

ObjectMover::ObjectMover(ModelImporter* pImporter)
{
	mMovingObject = nullptr;
	mMovingLight = nullptr;
	mAxisX = new StaticObject(pImporter, "models/arrow/arrow.obj");
	mAxisX->SetPosition(XMFLOAT3(0, 30, 30));
	mAxisX->SetMaterial(Material(Colors::Green));
	mAxisX->SetRotation(XMFLOAT3(3.14f/2.0f, 3.14f/2.0f, 0));

	mAxisY = new StaticObject(pImporter, "models/arrow/arrow.obj");
	mAxisY->SetPosition(XMFLOAT3(0, 30, 30));
	mAxisY->SetMaterial(Material(Colors::Red));
	mAxisY->SetRotation(XMFLOAT3(0, 1, 0));

	mAxisZ = new StaticObject(pImporter, "models/arrow/arrow.obj");
	mAxisZ->SetPosition(XMFLOAT3(0, 30, 30));
	mAxisZ->SetMaterial(Material(Colors::Blue));
	mAxisZ->SetRotation(XMFLOAT3(0, 3.14f/2.0f, 3.14f/2.0f));

	mMovingAxis = NONE;
}
	
ObjectMover::~ObjectMover()
{

}

void ObjectMover::Update(float dt)
{
	if(gInput->KeyPressed(VK_LBUTTON)) {
		float dist = numeric_limits<float>::infinity();
		XMVECTOR pos  = XMLoadFloat3(&gGame->GetGraphics()->GetCamera()->GetPosition());
		XMFLOAT3 d = gInput->GetWorldPickingRay().direction;
		XMVECTOR dir = XMLoadFloat3(&d);
		if(mAxisX->RayIntersect(pos, dir, dist)) 
			mMovingAxis = X_AXIS;	
		else if(mAxisY->RayIntersect(pos, dir, dist)) 
			mMovingAxis = Y_AXIS;	
		else if(mAxisZ->RayIntersect(pos, dir, dist)) 
			mMovingAxis = Z_AXIS;	

		// Store as the last plane pos.
		XMFLOAT3 p;
		XMStoreFloat3(&p, pos);
		mLastPlanePos = p + d * dist;
		mLastPlanePos = XMFLOAT3(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity());
	}

	if(gInput->KeyReleased(VK_LBUTTON))
		mMovingAxis = NONE;

	if(mMovingAxis != NONE)
	{
		XMFLOAT3 pos  = gGame->GetGraphics()->GetCamera()->GetPosition();
		XMFLOAT3 dir = gInput->GetWorldPickingRay().direction;

		if(mMovingAxis == X_AXIS)
			UpdatePosition(MoveAxisX(pos, dir));
		else if(mMovingAxis == Y_AXIS)
			UpdatePosition(MoveAxisY(pos, dir));
		else if(mMovingAxis == Z_AXIS)
			UpdatePosition(MoveAxisZ(pos, dir));
	}
}

void ObjectMover::UpdatePosition(XMFLOAT3 delta)
{
	if(mMovingObject != nullptr) {
		mMovingObject->SetPosition(mMovingObject->GetPosition() + delta);
		onPositionChange(mMovingObject->GetPosition());
	}
	else if(mMovingLight != nullptr) {
		mMovingLight->SetPosition(mMovingLight->GetPosition() + delta);
		onPositionChange(mMovingLight->GetPosition());
	}

	mAxisX->SetPosition(mAxisX->GetPosition() + delta);
	mAxisY->SetPosition(mAxisY->GetPosition() + delta);
	mAxisZ->SetPosition(mAxisZ->GetPosition() + delta);
}

XMFLOAT3 ObjectMover::MoveAxisX(XMFLOAT3 pos, XMFLOAT3 dir)
{
	// Top right triangle.
	XMVECTOR v0 = XMLoadFloat3(&XMFLOAT3(-10000, mAxisX->GetPosition().y, -10000));
	XMVECTOR v1 = XMLoadFloat3(&XMFLOAT3(-10000, mAxisX->GetPosition().y, 10000));
	XMVECTOR v2 = XMLoadFloat3(&XMFLOAT3(10000, mAxisX->GetPosition().y, 10000));
	float dist = numeric_limits<float>::infinity();
	if(!XNA::IntersectRayTriangle(XMLoadFloat3(&pos), XMLoadFloat3(&dir), v0, v1, v2, &dist))
	{
		// Bottom left triangle.
		v0 = XMLoadFloat3(&XMFLOAT3(10000, mAxisX->GetPosition().y, 10000));
		v1 = XMLoadFloat3(&XMFLOAT3(10000, mAxisX->GetPosition().y, -10000));
		v2 = XMLoadFloat3(&XMFLOAT3(-10000, mAxisX->GetPosition().y, -10000));
		dist = numeric_limits<float>::infinity();
		XNA::IntersectRayTriangle(XMLoadFloat3(&pos), XMLoadFloat3(&dir), v0, v1, v2, &dist);
	}

	float dx;
	if(mLastPlanePos.x != numeric_limits<float>::infinity()) {
		XMFLOAT3 planePos = pos + dir * dist;
		dx = planePos.x - mLastPlanePos.x;
		mLastPlanePos = planePos;
	}
	else {
		mLastPlanePos = pos + dir * dist;
		dx = 0.0f;
	}

	return XMFLOAT3(dx, 0, 0);
}
	
XMFLOAT3 ObjectMover::MoveAxisY(XMFLOAT3 pos, XMFLOAT3 dir)
{
	// Top right triangle.
	XMFLOAT3 right = gGame->GetGraphics()->GetCamera()->GetRight();
	XMFLOAT3 up = XMFLOAT3(0, 1, 0);
	XMFLOAT3 objectPos = mAxisY->GetPosition();
	float halfWidth = 60.0f;

	//mLastPlanePos - 1000*XMLoadFloat3(&right);
	XMVECTOR v0 = XMLoadFloat3(&(objectPos + right*halfWidth + up*halfWidth));
	XMVECTOR v1 = XMLoadFloat3(&(objectPos - right*halfWidth + up*halfWidth));
	XMVECTOR v2 = XMLoadFloat3(&(objectPos - right*halfWidth - up*halfWidth));
	float dist = numeric_limits<float>::infinity();
	if(!XNA::IntersectRayTriangle(XMLoadFloat3(&pos), XMLoadFloat3(&dir), v0, v1, v2, &dist))
	{
		// Bottom left triangle.
		v0 = XMLoadFloat3(&(objectPos + right*halfWidth + up*halfWidth));
		v1 = XMLoadFloat3(&(objectPos - right*halfWidth - up*halfWidth));
		v2 = XMLoadFloat3(&(objectPos + right*halfWidth - up*halfWidth));
		dist = numeric_limits<float>::infinity();
		XNA::IntersectRayTriangle(XMLoadFloat3(&pos), XMLoadFloat3(&dir), v0, v1, v2, &dist);
	}

	float dy;
	if(mLastPlanePos.x != numeric_limits<float>::infinity()) {
		XMFLOAT3 planePos = pos + dir * dist;
		dy = planePos.y - mLastPlanePos.y;
		mLastPlanePos = planePos;
	}
	else {
		mLastPlanePos = pos + dir * dist;
		dy = 0.0f;
	}

	return XMFLOAT3(0, dy, 0);
}
	
XMFLOAT3 ObjectMover::MoveAxisZ(XMFLOAT3 pos, XMFLOAT3 dir)
{
	// Top right triangle.
	XMVECTOR v0 = XMLoadFloat3(&XMFLOAT3(-10000, mAxisX->GetPosition().y, -10000));
	XMVECTOR v1 = XMLoadFloat3(&XMFLOAT3(-10000, mAxisX->GetPosition().y, 10000));
	XMVECTOR v2 = XMLoadFloat3(&XMFLOAT3(10000, mAxisX->GetPosition().y, 10000));
	float dist = numeric_limits<float>::infinity();
	if(!XNA::IntersectRayTriangle(XMLoadFloat3(&pos), XMLoadFloat3(&dir), v0, v1, v2, &dist))
	{
		// Bottom left triangle.
		v0 = XMLoadFloat3(&XMFLOAT3(10000, mAxisX->GetPosition().y, 10000));
		v1 = XMLoadFloat3(&XMFLOAT3(10000, mAxisX->GetPosition().y, -10000));
		v2 = XMLoadFloat3(&XMFLOAT3(-10000, mAxisX->GetPosition().y, -10000));
		dist = numeric_limits<float>::infinity();
		XNA::IntersectRayTriangle(XMLoadFloat3(&pos), XMLoadFloat3(&dir), v0, v1, v2, &dist);
	}

	float dz;
	if(mLastPlanePos.z != numeric_limits<float>::infinity()) {
		XMFLOAT3 planePos = pos + dir * dist;
		dz = planePos.z - mLastPlanePos.z;
		mLastPlanePos = planePos;
	}
	else {
		mLastPlanePos = pos + dir * dist;
		dz = 0.0f;
	}

	return XMFLOAT3(0, 0, dz);
}


void ObjectMover::Draw(Graphics* pGraphics)
{
	mAxisX->Draw(pGraphics);
	mAxisY->Draw(pGraphics);
	mAxisZ->Draw(pGraphics);

	XMMATRIX view = XMLoadFloat4x4(&pGraphics->GetCamera()->GetViewMatrix());
	XMMATRIX proj = XMLoadFloat4x4(&pGraphics->GetCamera()->GetProjectionMatrix());
}

void ObjectMover::SetObject(Object3D* pObject)
{
	mMovingLight = nullptr;
	mMovingObject = pObject;
	mAxisX->SetPosition(pObject->GetPosition());

	AxisAlignedBox lightBox = mAxisX->GetBoundingBox();
	float scale = mMovingObject->GetBoundingBox().Extents.x / lightBox.Extents.x;
	mAxisX->SetScale(XMFLOAT3(1, max(scale, 1.0f), 1));
	mAxisX->SetPosition(pObject->GetPosition() + XMFLOAT3(mAxisX->GetBoundingBox().Extents.x*0.6, 0, 0));

	lightBox = mAxisY->GetBoundingBox();
	scale = mMovingObject->GetBoundingBox().Extents.y / lightBox.Extents.y;
	mAxisY->SetScale(XMFLOAT3(1, max(scale, 1.0f), 1));
	mAxisY->SetPosition(pObject->GetPosition() + XMFLOAT3(0, mAxisY->GetBoundingBox().Extents.y*0.6, 0));

	lightBox = mAxisZ->GetBoundingBox();
	scale = mMovingObject->GetBoundingBox().Extents.z / lightBox.Extents.z;
	mAxisZ->SetScale(XMFLOAT3(1, max(scale, 1.0f), 1));
	mAxisZ->SetPosition(pObject->GetPosition() + XMFLOAT3(0, 0, mAxisZ->GetBoundingBox().Extents.z*0.6));
}

void ObjectMover::SetObject(Light* pLight)
{
	mMovingObject = nullptr;
	mMovingLight = pLight;
	mAxisX->SetPosition(pLight->GetPosition() + XMFLOAT3(mAxisX->GetBoundingBox().Extents.x*0.6, 0, 0));
	mAxisY->SetPosition(pLight->GetPosition() + XMFLOAT3(0, mAxisY->GetBoundingBox().Extents.y*0.6, 0));
	mAxisZ->SetPosition(pLight->GetPosition() + XMFLOAT3(0, 0, mAxisZ->GetBoundingBox().Extents.z*0.6));
	mAxisX->SetScale(XMFLOAT3(1, 1, 1));
	mAxisY->SetScale(XMFLOAT3(1, 1, 1));
	mAxisZ->SetScale(XMFLOAT3(1, 1, 1));
}

void ObjectMover::SetPosition(XMFLOAT3 position)
{
	// Acctually the position don't need to be set, the Inspector does that.
	/*if(mMovingObject != nullptr) 
		mMovingObject->SetPosition(position);
	else if(mMovingLight != nullptr) 
		mMovingLight->SetPosition(position);*/

	mAxisX->SetPosition(position + XMFLOAT3(mAxisX->GetBoundingBox().Extents.x*0.6, 0, 0));
	mAxisY->SetPosition(position + XMFLOAT3(0, mAxisY->GetBoundingBox().Extents.y*0.6, 0));
	mAxisZ->SetPosition(position + XMFLOAT3(0, 0, mAxisZ->GetBoundingBox().Extents.z*0.6));
}

void ObjectMover::SetScale(XMFLOAT3 scale)
{

}