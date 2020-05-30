#include <iostream>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h> 
#include <vector>

#define CRKHED extern "C" __declspec(dllexport)

std::string ftrunc(float x)
{	
	if ( !(ceil(x) == x) ) {
		char repr[_CVTBUFSIZE];
		int err;
		err = _gcvt_s(repr, _CVTBUFSIZE, roundf(x * 10000) / 10000, 4);
		if (err != 0) { std::cout << "Error code %d\n", err; exit(1); }
		std::string s(repr);

		return s;
	}
	else {
		std::string s = std::to_string((int)x);
		return s;
	};
	
}

CRKHED const char* writeans(float arr[], int stride, int numVerts, int chunkSize = 16) {
	
	std::string pos = "#VXP\n";
	std::string nos = "\n#VXN\n";

	int itemCount = 1;
	static std::string result;

	for (int v = 0; v < numVerts * stride; v += stride)
	{
		for (int ax = 0; ax < 3; ax++)
		{
			pos += ftrunc(arr[v + ax]) + "/";
			nos += ftrunc(arr[v + ax + 3]) + "/";
		}
		pos.pop_back();
		nos.pop_back();

		if (itemCount > chunkSize)
		{
			pos += "\n"; nos += "\n"; itemCount = 1;
		}
		else
		{
			pos += " "; nos += " "; itemCount++;
		}
	}

	pos.pop_back();
	nos.pop_back();

	result = (pos + nos);
	return result.c_str();

}

CRKHED const char* writecrk(float arr[], int stride, const char* stridemod,
	int numVerts, int chunkSize = 16)
{
	int itemCount = 1;
	static std::string result;

	std::string pos = "#VXP\n";
	std::string nos = "\n#VXN\n";
	std::string uvs = "\n#UVS\n";
	std::string col = "\n#VXC\n";
	
	bool hasColor = stridemod[0] == '1';

	for (int v = 0; v < numVerts*stride; v+=stride)
	{
		for (int ax = 0; ax < 3; ax++)
		{
			pos += ftrunc(arr[v + ax]) + "/";
			nos += ftrunc(arr[v + ax + 3]) + "/";
			if (ax < 2) { uvs += ftrunc(arr[v + ax + 6]) + "/"; }

			if (hasColor)
			{
				col += ftrunc(arr[v + ax + 8]) + "/";
				if(ax == 2){ col += ftrunc(arr[v + 11]) + "/"; }
			}
		}

		pos.pop_back();
		nos.pop_back();
		uvs.pop_back();
		col.pop_back();

		if (itemCount > chunkSize)
		{
			pos += "\n"; nos += "\n"; itemCount = 1;
			uvs += "\n"; col += "\n";
		}
		else
		{
			pos += " "; nos += " "; itemCount++;
			uvs += " "; col += " ";
		}
	}

	pos.pop_back();
	nos.pop_back();
	uvs.pop_back();
	col.pop_back();

	result = (pos + nos + uvs);
	if (hasColor) { result += col; }
	return result.c_str();

};
