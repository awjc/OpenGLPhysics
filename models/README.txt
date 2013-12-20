***** An overview of the material update: *****

The .obj files will reference zero or more "material library" files, usually with .mtl extensions. 
Each .mtl file specifies one or more materials to be used by the model. Each material usually has ADS and specular highlight intensity at the bare bones but some materials specify a texture to use as the diffuse color, some specify normal maps, and some do some still crazier stuff. The only functionality currently supported is the ADS properties (just a mat3) and specular highlight intensity (or "shininess"). These are the bulk of what we did with the models in this course anyway. All of this is saved in a class called MaterialInfo, so if more functionality is desired then that class can be upgraded.

Throughout the .obj file, there will be "usemtl XXXXXX" lines, where the currently active material is specified. Any faces in the .obj file declared after this line shall be associated with the given material. Behavior is unspecified for materials that don't exist in any of the referenced material libraries.

The number of distinct materials in all of the combined referenced material library files is precisely the number of submodels in the model. These submodels are not necessarily nonempty. If, for example, an .obj file references a .mtl file that has 3 materials, but only uses 1 of them in the actual model, then the other 2 submodels will not be used. Perhaps submodels is not the best word, since it's grouped only with regard to the material, but at any rate that's how it is now.

In terms of how this is actually implemented, in addition to just saving a big array of all the vertex indices, it now also saves a array of arrays of indices for each of the submodels. Because there can be any number of submodels, the size of each submodel (that is, the number of indices in the corresponding array) is saved in a separate array of submodel sizes. 

In addition to saving the indices for each submodel, the ObjFile also maintains the indices for the entire model as a whole. This means that in order to draw the entire model at once, one can just use the indices for the whole model. Also, any .obj file that doesn't specify any material information doesn't have to be treated specially, since it just won't have any submodel information (rather, the number of submodels will be 1 and getting the indices for that submodel will return the indices for the entire model).

So as an example suppose we have an .obj file that references 2 material files, mymat1.mtl & mymat2.mtl, each with 3 materials defined. The total number of submodels in this case would be 6. The .obj file specifies which faces use which material, and that's all read in and processed. To draw the model, it would be as simple as iterating through each submodel, retrieving the indices and storing them in a VAO, then drawing them with a shader whose material properties have been set according to the submodel's material info.



***** An overview of the adjacency update: *****
Meshes are made of triangles, some of these are adjacent in that they share at least one edge. This adjacency info needs to be computed!

The Adjacency namespace has been created to hold a few things related to adjacency information. Currently this functionality simply consists of computing the adjacency information for a given model, saving that information to .adj files, and reading said files to extract the adjacency info instead of recomputing it.

The Adjacency::ComputeAdjacency(ObjFile*) function takes in a model and spits out a vector<int>* containing the adjacency indices. This is a list in the specified OpenGL adjacency format where every 6 indices in the list correspond to 4 triangles: 1 triangle and its 3 adjacency neighbors. 

The Adjacency::ReadAdjFile(std::string) and Adjacency::WriteAdjFile(std::string) functions should be pretty self-explanatory. The .adj file format is really simple, just the size at the beginning and then the list of space-separated indices.