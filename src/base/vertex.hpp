#pragma once
#include "../utils/matrix.hpp"
#include "../utils/transformations.hpp"
struct Vertex
{
    gl::vec3 position;
    gl::vec3 normal;
    gl::vec2 texCoords;
    uint material_index;

    Vertex(gl::vec3 position = {0.0f, 0.0f, 0.0f}, gl::vec3 normal = {0.0f, 0.0f, 0.0f}, gl::vec2 texCoords = {0.0f, 0.0f}, uint material_index = 0)
    {
        this->position = position;
        this->normal = normal;
        this->texCoords = texCoords;
        this->material_index = material_index;
    };

    bool operator==(const Vertex &other)
    {
        return position == other.position && normal == other.normal && texCoords == other.texCoords;
    }
};