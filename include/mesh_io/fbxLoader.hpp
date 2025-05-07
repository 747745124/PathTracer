#pragma once
#include "./meshData.hpp"
#include "base/primitive.hpp"
#include "primitives/triangleMesh.hpp"
#include <memory>
#include <string>

// Forward declarations for FBX SDK
namespace fbxsdk {
class FbxManager;
class FbxScene;
class FbxNode;
} // namespace fbxsdk

std::shared_ptr<TriangleMesh> loadFBXMesh(const std::string &path,
                                          std::shared_ptr<Material> mat);

// Helper functions
void processFBXNode(fbxsdk::FbxNode *node, MeshData &data);
void processFBXMesh(fbxsdk::FbxNode *node, MeshData &data);