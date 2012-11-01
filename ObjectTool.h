#pragma once

#include "d3dUtil.h"
#include <boost\function.hpp>
#include <boost\bind.hpp>

class Graphics;
class StaticObject;
class Object3D;
class ModelImporter;
class Light;
class BaseInspector;

enum MovingAxis
{
	X_AXIS,
	Y_AXIS, 
	Z_AXIS,
	NONE
};

//! The tool that move objects around.
class ObjectTool
{
public:
	ObjectTool(ModelImporter* pImporter);
	~ObjectTool();

	void Update(float dt);
	void Draw(Graphics* pGraphics);
	void SetObject(Object3D* pObject);
	void SetObject(Light* pLight);
	void SetInspector(BaseInspector* pInspector);

	// These are only supposed to be called by an BaseInspector type.
	void SetPosition(XMFLOAT3 position);

	// Callback hookups.
	template <class T>
	void AddOnPositionChange(void(T::*_callback)(XMFLOAT3), T* _object)	{
		onPositionChange = boost::bind(_callback, _object, _1);
	}

	template <class T>
	void AddOnScaleChange(void(T::*_callback)(XMFLOAT3), T* _object)	{
		onScaleChange = boost::bind(_callback, _object, _1);
	}
private:
	XMFLOAT3 MoveAxisX(XMFLOAT3 pos, XMFLOAT3 dir);
	XMFLOAT3 MoveAxisY(XMFLOAT3 pos, XMFLOAT3 dir);
	XMFLOAT3 MoveAxisZ(XMFLOAT3 pos, XMFLOAT3 dir);
	void UpdatePosition(XMFLOAT3 delta);

	// Callbacks.
	boost::function<void(XMFLOAT3)> onPositionChange;
	boost::function<void(XMFLOAT3)> onScaleChange;
private:
	StaticObject* mAxisX;
	StaticObject* mAxisY;
	StaticObject* mAxisZ;
	Object3D*	  mMovingObject;
	Light*		  mMovingLight;
	MovingAxis	  mMovingAxis;
	XMFLOAT3	  mLastPlanePos;
	BaseInspector*mInspector;
};