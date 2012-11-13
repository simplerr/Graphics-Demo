#pragma once
#include <Windows.h>
#include <xnamath.h>
#include "d3dx11Effect.h"
#include "Light.h"
#include "xnacollision.h"

using namespace XNA;

// Forward declarations.
#pragma region Forward declarations.
namespace GLib {
	class Graphics;
	class BasicEffect;
	class Primitive;
	class StaticModel;
	class ModelImporter;
	struct Texture2D;
}

class World;
#pragma endregion

enum ObjectType
{
	BASIC_OBJECT,
	ANIMATED_OBJECT,
	STATIC_OBJECT,
	LIGHT_OBJECT,
	TERRAIN
};

//!
//	Abstract base class for all objects.
//	Has a position, rotation and scale.
//!
class Object3D
{
public:
	Object3D(ObjectType type, XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f));
	virtual ~Object3D();

	virtual void Init() = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw(GLib::Graphics* pGraphics) = 0;
	virtual AxisAlignedBox GetBoundingBox() = 0;
	virtual bool RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist) = 0;
	void Kill();

	// Setters.
	virtual void SetMaterials(GLib::Material material);
	virtual void SetPosition(XMFLOAT3 position);
	virtual void SetRotation(XMFLOAT3 rotation);
	void SetWorld(World* pWorld);
	void SetNormalMap(string filename);
	void SetNormalMap(GLib::Texture2D* normalMap);
	void SetScale(XMFLOAT3 scale);
	void SetDefaultOrientation();
	void SetType(ObjectType type);
	void SetBoundingBoxVisible(bool visible);
	void SetName(string name);
	void SetDefualtScale(float scale);
	void SetId(int id);

	// Getters.
	World*		GetWorld();
	XMMATRIX	GetWorldMatrix();
	XMFLOAT3	GetPosition();
	XMFLOAT3	GetRotation();
	XMFLOAT3	GetScale();
	GLib::Material	GetMaterial();
	ObjectType	GetType();
	string		GetName();
	bool		GetAlive();
	int			GetId();
	bool		IsBoundingBoxVisible();
private:
	World*		mWorld;
	XMFLOAT3	mPosition;
	XMFLOAT3	mRotation;
	XMFLOAT3	mScale;
	XMFLOAT3	mDefaultScale;
	ObjectType	mType;
	GLib::Material	mMaterial;
	bool		mDrawBoundingBox;
	bool		mAlive;
	string		mName;
	int			mId;
};