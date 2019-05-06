#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool importAsset(const std::string& filePath);
bool loadModel(const std::string& filePath);
bool processNode(const aiNode* node, const aiScene* sceneObjPtr);
