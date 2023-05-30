#include "./primitive.hpp"

// Primitives _get_primitives_from_io(const ObjIO *io) {
//   if (io == nullptr)
//     return {};

//   std::vector<Sphere> spheres;
//   std::vector<PolySet> polysets;

//   while (io != nullptr) {
//     Materials materials;
//     std::cout << "This object has " << io->numMaterials << " materials"
//               << std::endl;

//     for (int i = 0; i < io->numMaterials; i++)
//       materials.push_back(std::make_shared<CustomMaterial>(io->material[i]));

//     switch (io->type) {
//       // For sphere, there's only one material
//     case ObjType::SPHERE_OBJ: {
//       auto sphere_io = static_cast<SphereIO *>(io->data);
//       float radius = sphere_io->radius;
//       gl::vec3 center = sphere_io->origin;
//       auto sphere = Sphere(center, radius);

//       if (io->numMaterials != 1)
//         throw std::runtime_error(
//             "No material for sphere or Too many materials for sphere");

//       sphere.material = materials[0];
//       spheres.push_back(sphere);
//     } break;
//     // A simplified version, assuming triangle mesh
//     case ObjType::POLYSET_OBJ: {
//       auto polyset_io = static_cast<PolySetIO *>(io->data);
//       std::vector<Triangle> triangles;

//       for (int i = 0; i < polyset_io->numPolys; i++) {
//         if (polyset_io->poly->numVertices != 3)
//           throw std::runtime_error("Only support triangle mesh");

//         auto v0 = polyset_io->poly[i].vert[0];
//         auto v1 = polyset_io->poly[i].vert[1];
//         auto v2 = polyset_io->poly[i].vert[2];

//         gl::vec3 p0(v0.pos);
//         gl::vec3 p1(v1.pos);
//         gl::vec3 p2(v2.pos);

//         gl::vec3 normal0, normal1, normal2;
//         std::shared_ptr<CustomMaterial> material0, material1, material2;

//         if (polyset_io->normType == NormType::PER_FACE_NORMAL) {
//           auto edge1 = p1 - p0;
//           auto edge2 = p2 - p0;
//           normal0 = gl::cross(edge1, edge2).normalize();
//           normal1 = normal0;
//           normal2 = normal0;
//         } else if (polyset_io->normType == NormType::PER_VERTEX_NORMAL) {
//           auto n0 = v0.norm;
//           auto n1 = v1.norm;
//           auto n2 = v2.norm;
//           normal0 = gl::vec3(n0);
//           normal1 = gl::vec3(n1);
//           normal2 = gl::vec3(n2);
//         } else {
//           throw std::runtime_error("Unsupported normal type");
//         }

//         if (polyset_io->materialBinding ==
//             MaterialBinding::PER_OBJECT_MATERIAL) {
//           assert(io->numMaterials == 1 && "Too many materials");
//           material0 = materials[0];
//           material1 = materials[0];
//           material2 = materials[0];
//         } else if (polyset_io->materialBinding ==
//                    MaterialBinding::PER_VERTEX_MATERIAL) {
//           auto m0 = v0.materialIndex;
//           auto m1 = v1.materialIndex;
//           auto m2 = v2.materialIndex;
//           material0 = materials[m0];
//           material1 = materials[m1];
//           material2 = materials[m2];
//         } else {
//           throw std::runtime_error("Unsupported material binding");
//         }

//         Vertex _v0(p0, normal0, gl::vec2(0.f), material0);
//         Vertex _v1(p1, normal1, gl::vec2(0.f), material1);
//         Vertex _v2(p2, normal2, gl::vec2(0.f), material2);
//         auto triangle = Triangle(_v0, _v1, _v2);
//         triangles.push_back(triangle);
//       }

//       polysets.push_back(PolySet(triangles));
//     } break;

//     default:
//       break;
//     }

//     io = io->next;
//   };

//   return {spheres, polysets};
// };
