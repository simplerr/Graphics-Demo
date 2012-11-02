#include "ModelLoaderXML.h"
#include "tinyxml\tinystr.h";
#include "tinyxml\tinyxml.h"

ModelLoaderXML::ModelLoaderXML(string source)
{
	// Load all models from the XML file.
	TiXmlDocument doc("models.xml");
	doc.LoadFile();

	// Get the root element.
	TiXmlElement* root = doc.FirstChildElement();

	// Load items from the file.
	for(TiXmlElement* item = root->FirstChildElement("Model"); item != NULL; item = item->NextSiblingElement("Model"))
	{
		ModelData data;
		data.name = item->Attribute("name") == NULL ? "#NOVALUE" : item->Attribute("name");
		data.filename = item->Attribute("filename") == NULL ? "#NOVALUE" : item->Attribute("filename");
		data.type = atoi(item->Attribute("type") == NULL ? "#NOVALUE" : item->Attribute("type"));

		mDataMap[data.name] = data;
	}
}

ModelLoaderXML::~ModelLoaderXML()
{

}

ModelData ModelLoaderXML::GetData(string name)
{
	return mDataMap[name];
}

vector<string> ModelLoaderXML::GetModelNames()
{
	vector<string> names;
	for(auto iter = mDataMap.begin(); iter != mDataMap.end(); iter++)
		names.push_back((*iter).second.name);

	return names;
}