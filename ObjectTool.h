#pragma once
#include "d3dUtil.h"
#include <boost\function.hpp>
#include <boost\bind.hpp>

// Forward declarations.
#pragma region Forward declarations.
namespace GLib {
	class Graphics;
	class Light;
	class Input;
	class ModelImporter;
	class StaticObject;
	class Object3D;
}

class BaseInspector;
#pragma endregion

enum MovingAxis
{
	X_AXIS,
	Y_AXIS, 
	Z_AXIS,
	XZ_AXIS,
	NONE
};

//! The tool that move objects around.
class ObjectTool
{
public:
	ObjectTool(GLib::ModelImporter* pImporter);
	~ObjectTool();

	void Update(GLib::Input* pInput, float dt);
	void Draw(GLib::Graphics* pGraphics);
	void SetObject(GLib::Object3D* pObject);
	void SetInspector(BaseInspector* pInspector);
	bool IsMovingObject();

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
	void InitStartingPosition(GLib::Input* pInput, XMFLOAT3& dir, XMFLOAT3& cameraPos, float& dist);
	void ScaleAxisArrows();

	// Callbacks.
	boost::function<void(XMFLOAT3)> onPositionChange;
	boost::function<void(XMFLOAT3)> onScaleChange;
private:
	GLib::StaticObject* mAxisX;
	GLib::StaticObject* mAxisY;
	GLib::StaticObject* mAxisZ;
	GLib::Object3D*		mMovingObject;
	MovingAxis	  mMovingAxis;
	XMFLOAT3	  mLastPlanePos;
	BaseInspector*mInspector;
};