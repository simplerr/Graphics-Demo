#include "CreationTool.h"
#include "Gwen/Controls/CollapsibleList.h"
#include "ModelLoaderXML.h"
#include "Input.h"
#include "Util.h"
#include "World.h"
#include "StaticObject.h"
#include "AnimatedObject.h"
#include "ModelImporter.h"
#include "Editor.h"
#include "Graphics.h"
#include "RenderStates.h"
#include "LightObject.h"

using namespace GLib;

CreationTool::CreationTool(Gwen::Controls::Base* pParent, World* pWorld, GLib::ModelImporter* pModelImporter)
	: Gwen::Controls::CollapsibleCategory(pParent)
{
	mWorld = pWorld;
	mModelImporter = pModelImporter;
	mModelSelected = false;
	mEditor = nullptr;
	mPreviewObject = nullptr;

	SetText("Spawn list");
	SetSize(200, 400);

	// Add as a category.
	Gwen::Controls::CollapsibleList* parent = (Gwen::Controls::CollapsibleList*)pParent;
	parent->Add(this);

	mModelLoaderXML = new ModelLoaderXML("models.xml");
	BuildSpawnList();
}

CreationTool::~CreationTool()
{
	delete mModelLoaderXML;
}

void CreationTool::Update(GLib::Input* pInput, float dt)
{
	if(mPreviewObject != nullptr) {
		XMFLOAT3 offset = mPreviewObject->GetName() == "Light" ? XMFLOAT3(0, 20, 0) : XMFLOAT3(0, 0, 0);
		XMFLOAT3 intersectPoint = mWorld->GetTerrainIntersectPoint(pInput->GetWorldPickingRay());
		mPreviewObject->SetPosition(intersectPoint + offset);
	}

	if(pInput->KeyPressed(VK_LBUTTON) && mModelSelected && IsIn3DScreen(pInput))
	{
		string name = ToString(this->GetSelected()->GetText());

		ModelData data = mModelLoaderXML->GetData(name);
		XMFLOAT3 intersectPoint = mWorld->GetTerrainIntersectPoint(pInput->GetWorldPickingRay());
		if(intersectPoint.x != numeric_limits<float>::infinity())
		{
			Object3D* object;
			if(data.type == 0)		// Static object.
				object = CreateStaticModel(intersectPoint, data);
			else if(data.type == 1) // Animated object.
				object = CreateAnimatedModel(intersectPoint, data);
			else if(data.type == 2)	// Light object.
				object = CreateLightObject(intersectPoint + XMFLOAT3(0, 20, 0));

			// Add the created object to the world and make it the selected one.
			if(data.type != 2 || (data.type == 2 && mWorld->GetNumLights() < 10)) {
				mWorld->AddObject(object);
				mEditor->OnItemSelected(object, object->GetType());
				mEditor->UpdateWorldTree();
			}
			else
				delete object;

			// Deselect the button if SHIFT isn't held down.
			if(!pInput->KeyDown(VK_SHIFT)) {
				this->GetSelected()->SetToggleState(false);
				mModelSelected = false;
				delete mPreviewObject;
				mPreviewObject = nullptr;
			}
		}
	}

	if(pInput->KeyReleased(VK_SHIFT) && mModelSelected) {
		this->GetSelected()->SetToggleState(false);
		mModelSelected = false;
		delete mPreviewObject;
		mPreviewObject = nullptr;
	}
}

void CreationTool::Draw(GLib::Graphics* pGraphics)
{
	pGraphics->GetContext()->OMSetDepthStencilState(RenderStates::NoDoubleBlendDSS, 0);

	if(mPreviewObject != nullptr)
		mPreviewObject->Draw(pGraphics);

	pGraphics->GetContext()->OMSetDepthStencilState(0, 0);
}

void CreationTool::BuildSpawnList()
{
	// Loop through all names and add them to the list.
	vector<string> modelNames = mModelLoaderXML->GetModelNames();
	for(int i = 0; i < modelNames.size(); i++)
	{
		Gwen::Controls::Button* button = Add(modelNames[i]);
		button->onPress.Add(this, &CreationTool::OnSelectChange);
	}
}

StaticObject* CreationTool::CreateStaticModel(XMFLOAT3 position, ModelData data)
{
	StaticObject* object = new StaticObject(mModelImporter, data.filename);
	object->SetPosition(position);
	object->SetMaterials(Material(Colors::White));
	object->SetName(data.name);
	object->SetDefualtScale(data.defaultScale);
	return object;
}

AnimatedObject* CreationTool::CreateAnimatedModel(XMFLOAT3 position, ModelData data)
{
	AnimatedObject* object = new AnimatedObject(mModelImporter, data.filename);
	object->SetName(data.name);
	object->SetPosition(position);
	object->SetDefualtScale(data.defaultScale);
	return object;
}

LightObject* CreationTool::CreateLightObject(XMFLOAT3 position)
{
	LightObject* lightObject = new LightObject();
	lightObject->SetName("Light");
	lightObject->SetMaterials(Material(Colors::White, Colors::White, XMFLOAT4(229/255.0f, 106/255.0f, 5.0f/255.0f, 1.0f)));
	lightObject->SetRotation(XMFLOAT3(0.3f, -1.0f, 0.0f));
	lightObject->SetLightType(SPOT_LIGHT);
	lightObject->SetAtt(0, 0.1, 0);
	lightObject->SetRange(2000.0f);
	lightObject->SetSpot(16.0f);
	lightObject->SetPosition(position);
	lightObject->SetIntensity(1.0f, 1.0f, 0.2f);
	return lightObject;
}

void CreationTool::OnSelectChange(Gwen::Controls::Base* pControl)
{
	mModelSelected = true;

	string name = ToString(((Gwen::Controls::Button*)pControl)->GetText());
	ModelData data = mModelLoaderXML->GetData(name);
	XMFLOAT3 intersectPoint; //[NOTE][TODO]

	// Delete the old preview object if it cant be used again.
	if(mPreviewObject != nullptr && mPreviewObject->GetName() != data.name) {
		delete mPreviewObject;
		mPreviewObject = nullptr;
	}

	// Create the new preview object.
	if(mPreviewObject == nullptr || mPreviewObject->GetName() != data.name)
	{
		if(data.type == 0)		// Static object.
			mPreviewObject = CreateStaticModel(intersectPoint, data);
		else if(data.type == 1)	// Animated object.
			mPreviewObject = CreateAnimatedModel(intersectPoint, data);
		else if(data.type == 2) // Light object.
			mPreviewObject = CreateLightObject(intersectPoint);
	}

	// Orange with transparency.
	mPreviewObject->SetMaterials(Material(XMFLOAT4(1.0f, 127.0f/255.0f, 38/255.0f, 0.06f) * 6));
}

void CreationTool::SetEditor(Editor* pEditor)
{
	mEditor = pEditor;
}

bool CreationTool::IsCreatingModel()
{
	return mModelSelected;
}