#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;

struct ModelData
{
	string name;
	string filename;
	int type;
};

class ModelLoaderXML
{
public:
	ModelLoaderXML(string source);
	~ModelLoaderXML();

	ModelData GetData(string name);
	vector<string> GetModelNames();
private:
	map<string, ModelData> mDataMap;
};
