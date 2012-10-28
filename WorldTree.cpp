#include "WorldTree.h"
#include "d3dUtil.h"
#include "World.h"
#include "Object3D.h"
#include "Util.h"
#include "Editor.h"

WorldTree::WorldTree(Gwen::Controls::Base* pParent)
	: Gwen::Controls::TreeControl(pParent)
{
	SetBounds(1000, 0, 200, 800);
	onSelect.Add(this, &WorldTree::OnSelectChange);
}
	
WorldTree::~WorldTree()
{

}

void WorldTree::OnSelectChange(Gwen::Controls::Base* pControl)
{
	Gwen::Controls::TreeNode* node = (Gwen::Controls::TreeNode*) pControl;
	string name = ToString(node->GetButton()->GetText());

	WorldTreeNode worldNode = mNodeMap[name];
	mEditor->ItemSelected(worldNode.pData, worldNode.type);
}

void WorldTree::CreateTree(World* pWorld)
{
	Gwen::Controls::TreeNode* objects = AddNode("Objects");
	objects->onSelect.Add(this, &WorldTree::OnSelectChange);
	Gwen::Controls::TreeNode* staticObjects = objects->AddNode("Static");
	Gwen::Controls::TreeNode* animatedObjects = objects->AddNode("Animated");
	Gwen::Controls::TreeNode* lights = AddNode("Lights");

	// Add the terrain node.
	Gwen::Controls::TreeNode* terrain = AddNode("Terrain");
	terrain->onSelect.Add(this, &WorldTree::OnSelectChange);
	mNodeMap["Terrain"].pData = pWorld->GetTerrain();
	mNodeMap["Terrain"].type = TERRAIN;

	// Add all static objects.
	ObjectList* objectList = pWorld->GetObjects();
	int statics, animated;
	statics = animated = 1;
	for(int i = 0; i < objectList->size(); i++)
	{
		Object3D* object = objectList->operator[](i);
		WorldTreeNode node;
		node.pData = object;
		node.type = object->GetType();

		char buffer[256];
		if(node.type == STATIC_OBJECT) {
			sprintf(buffer, "Static %i", statics);
			staticObjects->AddNode(buffer)->onSelect.Add(this, &WorldTree::OnSelectChange);
			statics++;
		}
		else if(node.type == ANIMATED_OBJECT) {
			sprintf(buffer, "Animated %i", animated);
			animatedObjects->AddNode(buffer)->onSelect.Add(this, &WorldTree::OnSelectChange);
			animated++;
		}

		mNodeMap[buffer] = node;
	}

	LightList* lightList = pWorld->GetLights();
	for(int i = 0; i < lightList->size(); i++)
	{
		Light* light = lightList->operator[](i);
		WorldTreeNode node;
		node.pData = light;
		node.type = LIGHT;

		char buffer[256];
		sprintf(buffer,  "Light %i", i+1);
		lights->AddNode(buffer)->onSelect.Add(this, &WorldTree::OnSelectChange);
		mNodeMap[buffer] = node;
	}

	objects->ExpandAll();
	lights->ExpandAll();
}

void WorldTree::SetEditor(Editor* pEditor)
{
	mEditor = pEditor;
}