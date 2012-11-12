#include "WorldTree.h"
#include "d3dUtil.h"
#include "World.h"
#include "Object3D.h"
#include "Util.h"
#include "Editor.h"

WorldTree::WorldTree(Gwen::Controls::Base* pParent)
	: Gwen::Controls::CollapsibleCategory(pParent)
{
	SetText("World objects");
	SetSize(200, 400);

	mTreeControl = nullptr;

	// Add as a category.
	Gwen::Controls::CollapsibleList* parent = (Gwen::Controls::CollapsibleList*)pParent;
	parent->Add(this);
}
	
WorldTree::~WorldTree()
{
	delete mTreeControl;
}

void WorldTree::OnSelectChange(Gwen::Controls::Base* pControl)
{
	Gwen::Controls::TreeNode* node = (Gwen::Controls::TreeNode*) pControl;
	string name = ToString(node->GetButton()->GetText());

	WorldTreeNode worldNode = mNodeMap[name];
	mEditor->OnItemSelected(worldNode.pData, worldNode.type);
}

void WorldTree::CreateTree()
{
	// Remove the old content.
	mNodeMap.clear();
	delete mTreeControl;
	mTreeControl = new Gwen::Controls::TreeControl(this);
	mTreeControl->SetBounds(0, 20, 200, 380);
	mTreeControl->onSelect.Add(this, &WorldTree::OnSelectChange);

	Gwen::Controls::TreeNode* objects = mTreeControl->AddNode("Objects");
	Gwen::Controls::TreeNode* staticObjects = objects->AddNode("Static");
	Gwen::Controls::TreeNode* animatedObjects = objects->AddNode("Animated");
	Gwen::Controls::TreeNode* lights = mTreeControl->AddNode("Lights (max 10)");

	// Add the terrain node.
	Gwen::Controls::TreeNode* terrain = mTreeControl->AddNode("Terrain");
	terrain->onSelect.Add(this, &WorldTree::OnSelectChange);
	mNodeMap["Terrain"].pData = mWorld->GetTerrain();
	mNodeMap["Terrain"].type = TERRAIN;

	// Add all static objects.
	ObjectList* objectList = mWorld->GetObjects();
	map<string, int> counter;
	for(int i = 0; i < objectList->size(); i++)
	{
		Object3D* object = objectList->operator[](i);
		WorldTreeNode node;
		node.pData = object;
		node.type = object->GetType();

		char buffer[256];
		string name = object->GetName();

		if(counter.find(name) != counter.end())
			counter[name]++;
		else
			counter[name] = 1;

		if(node.type == STATIC_OBJECT) {
			sprintf(buffer, "%s %i", name.c_str(), counter[name]);
			staticObjects->AddNode(buffer)->onSelect.Add(this, &WorldTree::OnSelectChange);
		}
		else if(node.type == ANIMATED_OBJECT) {
			sprintf(buffer, "%s %i", name.c_str(), counter[name]);
			animatedObjects->AddNode(buffer)->onSelect.Add(this, &WorldTree::OnSelectChange);
		}
		else if(node.type == LIGHT_OBJECT) {
			sprintf(buffer,  "Light %i", i+1);
			lights->AddNode(buffer)->onSelect.Add(this, &WorldTree::OnSelectChange);
		}

		mNodeMap[buffer] = node;
	}

	objects->ExpandAll();
	lights->ExpandAll();
}

void WorldTree::SetEditor(Editor* pEditor)
{
	mEditor = pEditor;
}

void WorldTree::SetWorld(World* pWorld)
{
	mWorld = pWorld;
}