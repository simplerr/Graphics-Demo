#include "CreationTool.h"
#include "Gwen/Controls/CollapsibleList.h"
#include "ModelLoaderXML.h"
#include "Input.h"
#include "Util.h"
#include "World.h"
#include "StaticObject.h"
#include "AnimatedObject.h"
#include "ModelImporter.h"

CreationTool::CreationTool(Gwen::Controls::Base* pParent, World* pWorld)
	: Gwen::Controls::CollapsibleCategory(pParent)
{
	mWorld = pWorld;
	mModelSelected = false;

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

void CreationTool::Update(float dt)
{
	if(gInput->KeyPressed(VK_LBUTTON) && mModelSelected && IsIn3DScreen())
	{
		string name = ToString(this->GetSelected()->GetText());

		ModelData data = mModelLoaderXML->GetData(name);
		XMFLOAT3 intersectPoint = mWorld->GetTerrainIntersectPoint(gInput->GetWorldPickingRay());
		if(intersectPoint.x != numeric_limits<float>::infinity())
		{
			// Static model.
			if(data.type == 0)
				CreateStaticModel(intersectPoint, data);
			else if(data.type == 1)
				CreateAnimatedModel(intersectPoint, data);

			// Deselect the button.
			this->GetSelected()->SetToggleState(false);
			mModelSelected = false;
		}
	}
	
}

void CreationTool::Draw(Graphics* pGraphics)
{
	
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

void CreationTool::CreateStaticModel(XMFLOAT3 position, ModelData data)
{
	StaticObject* object = new StaticObject(gModelImporter, data.filename);
	object->SetPosition(position);
	object->SetScale(XMFLOAT3(10, 10, 10));
	object->SetMaterial(Material(Colors::White));
	mWorld->AddObject(object);
}

void CreationTool::CreateAnimatedModel(XMFLOAT3 position, ModelData data)
{
	AnimatedObject* object = new AnimatedObject(gModelImporter, data.filename);
	object->SetPosition(position);
	mWorld->AddObject(object);
}

void CreationTool::OnSelectChange(Gwen::Controls::Base* pControl)
{
	mModelSelected = true;
}