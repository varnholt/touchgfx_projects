#ifndef VERTEX_H
#define VERTEX_H

typedef struct
{
   float x,y;  // 2d screen coordinates
   float z;    // z for sorting
   float u,v;  // texture coordinates
} Vertex;

#endif

