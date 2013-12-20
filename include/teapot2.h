#include <Angel.h>
#include "teapot_data2.h"

class Teapot
{
  
  vec3 * vertices; 
  vec3 * normals;
  vec3 * truenormals;
  vec3 * tangents;
  vec2 * texCoords;
  int numVertices;
  bool calculateNormals;

  public:

  Teapot()
  {
    // this is 3 x number of triangles, not the actual number of vertices
    numVertices = 3072;  

    //vertices = teapot_triangles;
    vertices = new vec3[numVertices];
    for (int i = 0; i < numVertices; ++i)
    {
      vertices[i] = teapot_triangles[i] * 0.02;  // scale by 1/50
    }

    texCoords = teapot_texCoords;
    tangents = new vec3[numVertices];
    normals = new vec3[numVertices];
    truenormals = new vec3[numVertices];

    for (int i = 0; i < numVertices; ++i)
    {
      truenormals[i] = teapot_normals[i];
    }

    initNormals();

    initTangents();
  }

  ~Teapot()
  {
      delete[] normals;
      delete[] truenormals;
    delete[] tangents;
    delete[] vertices;
  }

  vec3 * getVertices()
  {
    return vertices;
  }

  vec3 * getNormals()
  {
    return truenormals;
  }

  vec3 * getFaceNormals()
  {
    return normals;
  }

  vec3 * getTangents()
  {
    //return normals;
    return tangents;
  }

  vec2 * getTexCoords()
  {
    return texCoords;
  }

  int getNumVertices()
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

  // assumes normals are already available
  void initTangents()
  {
    int numTriangles = numVertices / 3;
    for (int t = 0; t < numTriangles; ++t)
    {
      int i = 3 * t;
      vec3 a = teapot_triangles[i];
      vec3 b = teapot_triangles[i + 1];
      vec3 c = teapot_triangles[i + 2];

      vec3 Q1 = b - a;
      vec3 Q2 = c - a;

      vec2 p0 = texCoords[i];
      vec2 p1 = texCoords[i + 1];
      vec2 p2 = texCoords[i + 2];

      float s1 = (p1 - p0)[0];
      float t1 = (p1 - p0)[1];
      float s2 = (p2 - p0)[0];
      float t2 = (p2 - p0)[1];

      float m = 1.0 / (s1 * t2 - s2 * t1);

      vec3 T;
      T[0] = t2 * Q1[0] - t1 * Q2[0];
      T[1] = t2 * Q1[1] - t1 * Q2[1];
      T[2] = t2 * Q1[2] - t1 * Q2[2];
      T = T * m;

      vec3 B;
      B[0] = -s2 * Q1[0] + s1 * Q2[0];
      B[1] = -s2 * Q1[1] + s1 * Q2[1];
      B[2] = -s2 * Q1[2] + s1 * Q2[2];
      B = B * m;

      // can use same tangent per triangle, ideally would average with adjacent triangles
      // in any case we'll orthogonalize against the true normal at each vertex
      vec3 N = normals[i];
      //float dd = dot(T, N);
      //vec3 vv = T - dd * N;
      //vec3 nn = normalize(vv);
      //tangents[i] = nn;
      tangents[i] = normalize(T - dot(T, N) * N);
      N = normals[i + 1];
      tangents[i + 1] = normalize(T - dot(T, N) * N);
      N = normals[i + 2];
      tangents[i + 2] = normalize(T - dot(T, N) * N);
    }
  }
};
