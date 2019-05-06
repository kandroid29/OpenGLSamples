#pragma once

class MeshLoader {
public:
	MeshLoader();
	~MeshLoader();

	void loadObjFile(const char *fileName);
};