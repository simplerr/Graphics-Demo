#include "ObjectTool.h"
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
#include "RenderStates.h"
#include "Util.h"

using namespace GLib;

ObjectTool::ObjectTool(GLib::ModelImporter* pImporter)
{
	// nullptr as default.
	mMovingObject = nullptr;

	// Create the X axis.
	mAxisX = new StaticObject(pImporter, "models/arrow/arrow.obj");
	mAxisX->SetPosition(XMFLOAT3(0, 30, 30));
	mAxisX->SetMaterials(Material(Colors::Green));
	mAxisX->SetRotation(XMFLOAT3(3.14f/2.0f, 3.14f/2.0f, 0));
	mAxisX->SetScale(XMFLOAT3(2.0f, 2.0f, 2.0f));

	// Create the Y axis.
	mAxisY = new StaticObject(pImporter, "models/arrow/arrow.obj");
	mAxisY->SetPosition(XMFLOAT3(0, 30, 30));
	mAxisY->SetMaterials(Material(Colors::Red));
	mAxisY->SetRotation(XMFLOAT3(0, 1, 0));
	mAxisY->SetScale(XMFLOAT3(2.0f, 2.0f, 2.0f));

	// Create the Z axis.
	mAxisZ = new StaticObject(pImporter, "models/arrow/arrow.obj");
	mAxisZ->SetPosition(XMFLOAT3(0, 30, 30));
	mAxisZ->SetMaterials(Material(Colors::Blue));
	mAxisZ->SetRotation(XMFLOAT3(0, 3.14f/2.0f, 3.14f/2.0f));
	mAxisZ->SetScale(XMFLOAT3(2.0f, 2.0f, 2.0f));

	mMovingAxis = NONE;
}
	
//! Cleanup.
ObjectTool::~ObjectTool()
{
	delete mAxisX;
	delete mAxisY;
	delete mAxisZ;
}

//! Poll for input and perform actions.
void ObjectTool::Update(GLib::Input* pInput, float dt)
{
	// LBUTTON pressed and inside 3D screen.
	if(pInput->KeyPressed(VK_LBUTTON) && IsIn3DScreen(pInput)) {
		float dist = numeric_limits<float>::infinity();
		XMVECTOR pos  = XMLoadFloat3(&GetCamera()->GetPosition());
		XMFLOAT3 d = pInput->GetWorldPickingRay().direction;
		XMVECTOR dir = XMLoadFloat3(&d);

		// Find out which axis arrow was pressed.
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

	// Not moving any axis any more.
	if(pInput->KeyReleased(VK_LBUTTON))
		mMovingAxis = NONE;

	// Update the position.
	if(mMovingAxis != NONE)
	{
		XMFLOAT3 pos  = GetCamera()->GetPosition();
		XMFLOAT3 dir = pInput->GetWorldPickingRay().direction;

		if(mMovingAxis == X_AXIS)
			UpdatePosition(MoveAxisX(pos, dir));
		else if(mMovingAxis == Y_AXIS)
			UpdatePosition(MoveAxisY(pos, dir));
		else if(mMovingAxis == Z_AXIS)
			UpdatePosition(MoveAxisZ(pos, dir));
	}
}

//! Draws the arrow axis.
void ObjectTool::Draw(GLib::Graphics* pGraphics)
{
	// Disable the depth test.
	ID3D11DepthStencilState* oldState = nullptr;
	pGraphics->GetContext()->OMGetDepthStencilState(&oldState, 0);
	pGraphics->GetContext()->OMSetDepthStencilState(RenderStates::EnableAllDSS, 0);

	Effects::BasicFX->SetUseLighting(false);

	// The axes will be rendered through the object.
	mAxisX->Draw(pGraphics);
	mAxisY->Draw(pGraphics);
	mAxisZ->Draw(pGraphics);

	Effects::BasicFX->SetUseLighting(true);

	// Restore to standard depth stencil state (enable depth testing).
	pGraphics->GetContext()->OMSetDepthStencilState(oldState, 0);

	XMMATRIX view = XMLoadFloat4x4(&pGraphics->GetCamera()->GetViewMatrix());
	XMMATRIX proj = XMLoadFloat4x4(&pGraphics->GetCamera()->GetProjectionMatrix());
}

//! Updates the moving objects position.
void ObjectTool::UpdatePosition(XMFLOAT3 delta)
{
	if(mMovingObject != nullptr) {
		mMovingObject->SetPosition(mMovingObject->GetPosition() + delta);
		onPositionChange(mMovingObject->GetPosition());
	}

	mAxisX->SetPosition(mAxisX->GetPosition() + delta);
	mAxisY->SetPosition(mAxisY->GetPosition() + delta);
	mAxisZ->SetPosition(mAxisZ->GetPosition() + delta);
}

//! Move the object on the X axis.
XMFLOAT3 ObjectTool::MoveAxisX(XMFLOAT3 pos, XMFLOAT3 dir)
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
	
//! Move the object on the Y axis.
XMFLOAT3 ObjectTool::MoveAxisY(XMFLOAT3 pos, XMFLOAT3 dir)
{
	// Top right triangle.
	XMFLOAT3 right = GetCamera()->GetRight();
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
	
//! Move the object on the Z axis.
XMFLOAT3 ObjectTool::MoveAxisZ(XMFLOAT3 pos, XMFLOAT3 dir)
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

//! Set the moving object.
void ObjectTool::SetObject(Object3D* pObject)
{
	mMovingObject = pObject;
	mAxisX->SetPosition(pObject->GetPosition());

	SetPosition(pObject->GetPosition());
}

//! Sets the axis positions.
void ObjectTool::SetPosition(XMFLOAT3 position)
{
	mAxisX->SetPosition(position + XMFLOAT3(mAxisX->GetBoundingBox().Extents.x*0.6, 0, 0));
	mAxisY->SetPosition(position + XMFLOAT3(0, mAxisY->GetBoundingBox().Extents.y*0.6, 0));
	mAxisZ->SetPosition(position + XMFLOAT3(0, 0, mAxisZ->GetBoundingBox().Extents.z*0.6));
}