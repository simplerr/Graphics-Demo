#pragma once

#include "d3dUtil.h"

class Graphics;
class StaticObject;
class Object3D;
class ModelImporter;
class Light;

enum MovingAxis
{
	X_AXIS,
	Y_AXIS, 
	Z_AXIS,
	NONE
};

class ObjectMover
{
public:
	ObjectMover(ModelImporter* pImporter);
	~ObjectMover();

	void Update(float dt);
	void Draw(Graphics* pGraphics);
	void SetObject(Object3D* pObject);
	void SetObject(Light* pLight);
private:
	XMFLOAT3 MoveAxisX(XMFLOAT3 pos, XMFLOAT3 dir);
	XMFLOAT3 MoveAxisY(XMFLOAT3 pos, XMFLOAT3 dir);
	XMFLOAT3 MoveAxisZ(XMFLOAT3 pos, XMFLOAT3 dir);
	void UpdatePosition(XMFLOAT3 delta);
private:
	StaticObject* mAxisX;
	StaticObject* mAxisY;
	StaticObject* mAxisZ;
	Object3D*	  mMovingObject;
	Light*		  mMovingLight;
	MovingAxis	  mMovingAxis;
	XMFLOAT3	  mLastPlanePos;
};