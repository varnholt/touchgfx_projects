#pragma once

#include "vector.hpp"
#include "vertex.hpp"
#include <cmath>

struct CameraVertex
{
    Vector3D position;
    float u;
    float v;
};

// A triangle clipped against one plane has at most four vertices.
inline int clipNearPlane(const CameraVertex* triangle, CameraVertex* output, float nearZ)
{
    if (!std::isfinite(nearZ) || nearZ <= 0) return 0;
    for (int i = 0; i < 3; ++i)
        if (!std::isfinite(triangle[i].position.x) || !std::isfinite(triangle[i].position.y) ||
            !std::isfinite(triangle[i].position.z) || !std::isfinite(triangle[i].u) ||
            !std::isfinite(triangle[i].v)) return 0;
    int count = 0;
    for (int i = 0; i < 3; ++i)
    {
        const CameraVertex& a = triangle[i];
        const CameraVertex& b = triangle[(i + 1) % 3];
        const bool aInside = a.position.z >= nearZ;
        const bool bInside = b.position.z >= nearZ;
        if (aInside) output[count++] = a;
        if (aInside != bInside)
        {
            const float t = (nearZ - a.position.z) / (b.position.z - a.position.z);
            output[count++] = {Vector3D(a.position.x + t * (b.position.x - a.position.x),
                                       a.position.y + t * (b.position.y - a.position.y), nearZ),
                               a.u + t * (b.u - a.u), a.v + t * (b.v - a.v)};
        }
    }
    return count;
}

inline Vertex projectCameraVertex(const CameraVertex& vertex, int width, int height)
{
    const float cx = width * 0.5f;
    const float cy = height * 0.5f;
    return {cx + vertex.position.x * cx / vertex.position.z,
            cy + vertex.position.y * cx / vertex.position.z,
            vertex.position.z, vertex.u, vertex.v};
}
