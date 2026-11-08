#pragma once

#include <cstdint>
#include <math.h>

#include "vector.hpp"
#include "vertex.hpp"

static constexpr auto CONSTANT_PI = 3.14159265358979323846;


/*! \brief Mesh is a base class for all sorts of 3D meshes.
 *
 *  A mesh consits of positions, normals and texture coordinates.
 *  No indices are used in this data structure.
 */
struct Mesh
{
    virtual ~Mesh() = default;

    virtual Vector3D* getVertices() = 0;
    virtual Vector3D* getNormals() = 0;
    virtual Vector3D* getTexcoord() = 0;
    virtual int32_t* getTriIndices() = 0;
    virtual int32_t getNumVertices() = 0;
    virtual int32_t getNumTris() = 0;
};


/*! \brief Torus is a templated Torus implementation.
 *
 *  Torus takes two template parameters, SegX and SegY which define the amount of
 *  segmentations around the X and Y axis. The amount of segmentation influences the
 *  number of vertices used for the Mesh.
 */
template <size_t SegX, size_t SegY>
struct Torus : public Mesh
{
    Torus(float size, float size2)
    {
        float uvscale = 1.0f;
        uint32_t i, j;

        // create vertices
        {
            Vector3D ref;
            Vector3D* vtx = vertices;
            Vector3D* nrm = normals;
            Vector3D* tex = texcoord;
            const auto deltav = static_cast<float>(2.0f * CONSTANT_PI / SegY);
            const auto deltau = static_cast<float>(2.0f * CONSTANT_PI / SegX);
            float v = 0.0f;

            for (i = 0; i <= SegY; i++)
            {
                float u = 0.0f;
                const auto p = static_cast<float>(cos(v));
                const auto r = static_cast<float>(sin(v) * size2 + size);
                float z = p * size2;
                v += deltav;

                for (j = 0; j <= SegX; j++)
                {
                    const auto x = static_cast<float>(sin(u));
                    const auto y = static_cast<float>(cos(u));
                    u += deltau;

                    // centroid on the xy-plane
                    ref.x = x * size;
                    ref.y = y * size;
                    ref.z = 0;

                    // circle around centroid
                    vtx->x = x * r;
                    vtx->y = y * r;
                    vtx->z = z;

                    tex->x = u * uvscale * 2;
                    tex->y = v * uvscale;

                    nrm->x = vtx->x - ref.x;
                    nrm->y = vtx->y - ref.y;
                    nrm->z = vtx->z - ref.z;
                    nrm->normalize();

                    vtx++;
                    nrm++;
                    tex++;
                }
            }
        }

        // create indices
        {
            int32_t* tris = triIndices;

            for (i = 0; i < SegY; i++)
            {
                int32_t index1 = i * (SegX + 1);
                int32_t index2 = (i + 1) * (SegX + 1);

                int32_t v1 = index1 + SegX - 1;
                int32_t v2 = index1;
                int32_t v3 = index2 + SegX - 1;
                int32_t v4 = index2;

                for (j = 0; j < SegX; j++)
                {
                    // two triangles per quad
                    *tris++ = v1;
                    *tris++ = v2;
                    *tris++ = v3;

                    *tris++ = v2;
                    *tris++ = v4;
                    *tris++ = v3;

                    v1 = v2;
                    v3 = v4;
                    v2++;
                    v4++;
                }

                index1 = index2;
            }
        }
    }

    Vector3D* getVertices() override
    {
        return vertices;
    }

    Vector3D* getNormals() override
    {
        return normals;
    }

    Vector3D* getTexcoord() override 
    {
        return texcoord;
    }

    int32_t* getTriIndices() override 
    {
        return triIndices;
    }

    static constexpr size_t NUM_VERTICES = (SegX + 1) * (SegY + 1);
    static constexpr size_t NUM_TRIS = SegX * SegY * 2;

    int32_t getNumVertices() override
    {
        return NUM_VERTICES;
    };

    int32_t getNumTris() override
    {
        return NUM_TRIS;
    }

    Vector3D vertices[NUM_VERTICES];
    Vector3D normals[NUM_VERTICES];
    Vector3D texcoord[NUM_VERTICES];
    int32_t triIndices[NUM_TRIS * 3];
};
