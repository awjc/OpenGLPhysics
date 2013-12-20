#include <Angel.h>
#include "teapot_data.h"

// Wrapper for teapot data generated by 3dsMax
class Teapot
{
  
  vec3 * vertices; 
  vec3 * normals;
  int numVertices;
  bool calculateNormals;

  public:

  Teapot(bool useTrueNormals = false)
  {
    // this is 3 x number of triangles, not the actual number of vertices
    numVertices = 3072;  

    vertices = teapot_triangles;
    if (useTrueNormals)
    {
      normals = teapot_normals;
      calculateNormals = false;
    }
    else
    {
      normals = new vec3[numVertices];
      calculateNormals = true;
      initNormals();
    }    
  }

  ~Teapot()
  {
    if (calculateNormals)
    {
      delete[] normals;
    }
  }

  vec3 * GetVertices()
  {
    return vertices;
  }

  vec3 * GetNormals()
  {
    return normals;
  }

  int GetNumVertices()
  {
    return numVertices;
  }

private:
  void initNormals()
  {
    int numTriangles = numVertices / 3;
    for (int t = 0; t < numTriangles; ++t)
    {
      int i = 3 * t;
      vec3 a = teapot_triangles[i];
      vec3 b = teapot_triangles[i + 1];
      vec3 c = teapot_triangles[i + 2];
      vec3  normal = normalize( cross(b - a, c - b) );
      normals[i] = normal;
      normals[i + 1] = normal;
      normals[i + 2] = normal;
    }
  }
};
