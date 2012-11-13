#pragma once

#include "Object3D.h"

// Forward declarations.
#pragma region Forward declarations.
namespace GLib {
	class Light;
	class BillboardVertex;
}

class World;
#pragma endregion

//!
//	Represents an object containing a light source.
//!
class LightObject : public Object3D
{
public:
	LightObject();
	~LightObject();

	virtual void Init();
	virtual void Update(float dt);
	virtual void Draw(GLib::Graphics* pGraphics);

	void SetMaterials(GLib::Material material);
	void SetPosition(XMFLOAT3 position);
	void SetRotation(XMFLOAT3 rotation);

	// Light functions.
	void SetRange(float range);
	void SetSpot(float spot);
	void SetAtt(float a0, float a1, float a2);
	void SetLightType(GLib::LightType type);
	void SetIntensity(float ambient, float diffuse, float specular);

	XMFLOAT3 GetAtt();
	XMFLOAT3 GetIntensity();
	float	 GetRange();
	float	 GetSpot();
	int GetLightType();

	bool RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist);
	AxisAlignedBox GetBoundingBox();
private:
	GLib::BillboardVertex* mBillboard;
	GLib::Light* mLight;
};