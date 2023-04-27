#include "./primitive.hpp"
Primitives _get_primitives_from_io(const ObjIO *io)
{
    if (io == nullptr)
        return {};

    std::vector<Sphere> spheres;
    std::vector<PolySet> polysets;

    while (io != nullptr)
    {
        assert(io->numMaterials == 1 && "Only support one material per object");
        auto material = CustomMaterial(io->material);
        
        switch (io->type)
        {

        case ObjType::SPHERE_OBJ:
        {
            auto sphere_io = static_cast<SphereIO *>(io->data);
            float radius = sphere_io->radius;
            gl::vec3 center = sphere_io->origin;
            auto sphere = Sphere(center, radius);
            sphere.material = std::make_shared<CustomMaterial>(material);
            spheres.push_back(sphere);
        }
        break;
        // A simplified version
        case ObjType::POLYSET_OBJ:
        {
            auto polyset_io = static_cast<PolySetIO *>(io->data);
            std::vector<Triangle> triangles;
            for (int i = 0; i < polyset_io->numPolys; i++)
            {
                assert(polyset_io->type == PolySetType::POLYSET_TRI_MESH && "Only support triangle mesh");

                auto poly = polyset_io->poly[i];
                gl::vec3 v0 = poly.vert->pos;
                gl::vec3 v1 = poly.vert->pos;
                gl::vec3 v2 = poly.vert->pos;
                triangles.push_back(Triangle(v0, v1, v2));
            }
            auto polyset = PolySet(triangles);
            polyset.material = std::make_shared<CustomMaterial>(material);
            polysets.push_back(polyset);
        }
        break;

        default:
            break;
        }

        io = io->next;
    };

    return {spheres, polysets};
};