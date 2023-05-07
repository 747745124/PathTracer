#pragma once
#include "../utils/matrix.hpp"
#include "../utils/transformations.hpp"
#include "./material.hpp"
#include <memory>
struct Vertex
{
    gl::vec3 position;
    gl::vec3 normal;
    gl::vec2 texCoords;
    std::shared_ptr<CustomMaterial> material;

    Vertex(gl::vec3 position = {0.0f, 0.0f, 0.0f}, gl::vec3 normal = {0.0f, 0.0f, 0.0f}, gl::vec2 texCoords = {0.0f, 0.0f}, std::shared_ptr<CustomMaterial> material = nullptr)
    {
        this->position = position;
        this->normal = normal;
        this->texCoords = texCoords;
        this->material = material;
    };

    bool operator==(const Vertex &other)
    {
        return position == other.position && normal == other.normal && texCoords == other.texCoords;
    }
};