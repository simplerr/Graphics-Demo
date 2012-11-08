#include "LightObject.h"
#include "BillboardManager.h"
#include "Runnable.h"
#include "Graphics.h"
#include "Light.h"
#include "World.h"

LightObject::LightObject()
	: Object3D(LIGHT_OBJECT)
{
	// Add test billboards.
	mBillboard = GetGraphics()->AddBillboard(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(5, 5), "textures\\light_icon.png"); //[NOTE] Ugly.

	// Create the light.
	mLight = new Light();
}

LightObject::~LightObject()
{
	// Remove the billboard.
	mBillboard->Remove();

	// Remove the light from the world.
	if(GetWorld() != nullptr)
		GetWorld()->RemoveLight(mLight);
}

void LightObject::Init()
{
	// Create and add the light.
	GetWorld()->AddLight(mLight);
}

void LightObject::Update(float dt)
{

}

void LightObject::Draw(Graphics* pGraphics)
{
	
}

XNA::AxisAlignedBox LightObject::GetBoundingBox()
{
	AxisAlignedBox box;
	box.Center = GetPosition();
	box.Extents = XMFLOAT3(3, 3, 3);
	return box;
}

void LightObject::SetMaterials(Material material)
{
	Object3D::SetMaterials(material);
	mLight->SetMaterials(material.ambient, material.diffuse, material.specular);
}

void LightObject::SetPosition(XMFLOAT3 position)
{
	Object3D::SetPosition(position);
	mLight->SetPosition(position);
	mBillboard->SetPos(position);
}

void LightObject::SetRotation(XMFLOAT3 rotation)
{
	Object3D::SetRotation(rotation);
	mLight->SetDirection(rotation);
}

void LightObject::SetRange(float range)
{
	mLight->SetRange(range);
}

void LightObject::SetSpot(float spot)
{
	mLight->SetSpot(spot);
}

void LightObject::SetAtt(float a0, float a1, float a2)
{
	mLight->SetAtt(a0, a1, a2);
}

void LightObject::SetLightType(LightType type)
{
	mLight->SetType(type);
}

void LightObject::SetIntensity(float ambient, float diffuse, float specular)
{
	mLight->SetIntensity(ambient, diffuse, specular);
}

XMFLOAT3 LightObject::GetAtt()
{
	return mLight->GetAtt();
}

XMFLOAT3 LightObject::GetIntensity()
{
	return mLight->GetIntensity();
}

float LightObject::GetRange()
{
	return mLight->GetRange();
}

float LightObject::GetSpot()
{
	return mLight->GetSpot();
}