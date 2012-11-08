#pragma once

#include <Windows.h>
#include <xnamath.h>
#include "d3dx11Effect.h"
#include "Light.h"
#include "xnacollision.h"

using namespace XNA;

class Graphics;
class BasicEffect;
class Primitive;
class StaticModel;
class ModelImporter;
struct Texture2D;

enum ObjectType
{
	OBJECT,
	ANIMATED_OBJECT,
	STATIC_OBJECT,
	LIGHT,
	TERRAIN
};

/**
	Abstract base class for all objects.
	Has a position, rotation and scale.
*/
class Object3D
{
public:
	Object3D(XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f));
	virtual ~Object3D();

	virtual void Update(float dt) = 0;
	virtual void Draw(Graphics* pGraphics) = 0;

	// Getters.
	XMMATRIX	GetWorldMatrix();
	XMFLOAT3	GetPosition();
	XMFLOAT3	GetRotation();
	XMFLOAT3	GetScale();
	Material	GetMaterial();
	ObjectType	GetType();
	string		GetName();
	bool		GetAlive();
	int			GetId();
	bool		IsBoundingBoxVisible();
	virtual AxisAlignedBox GetBoundingBox() = 0;
	
	// Setters.
	void SetMaterial(Material material);
	void SetNormalMap(string filename);
	void SetNormalMap(Texture2D* normalMap);
	void SetPosition(XMFLOAT3 position);
	void SetRotation(XMFLOAT3 rotation);
	void SetScale(XMFLOAT3 scale);
	void SetDefaultOrientation();
	void SetType(ObjectType type);
	void SetBoundingBoxVisible(bool visible);
	void Kill();
	void SetName(string name);
	void SetDefualtScale(float scale);
	void SetId(int id);
private:
	XMFLOAT3	mPosition;
	XMFLOAT3	mRotation;
	XMFLOAT3	mScale;
	XMFLOAT3	mDefaultScale;
	ObjectType	mType;
	Material	mMaterial;
	bool		mDrawBoundingBox;
	bool		mAlive;
	string		mName;
	int			mId;
};