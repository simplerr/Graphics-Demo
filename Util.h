#pragma once

#include <string>
//#include <wstring>
using namespace std;

string ToString(wstring str)
{
	return string(str.begin(), str.end());
}