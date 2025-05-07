#pragma once
#include "./meshData.hpp"
#include "base/primitive.hpp"
#include "primitives/triangleMesh.hpp"
#include <memory>
#include <string>

std::shared_ptr<TriangleMesh> loadOBJMesh(const std::string &path,
                                          std::shared_ptr<Material> mat);