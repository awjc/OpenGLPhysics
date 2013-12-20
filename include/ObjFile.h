#ifndef OBJFILE_H
#define OBJFILE_H

#include <fstream>
#include <vector>
#include <Angel.h>


/**
 * \brief Class to hold information about a material
 * 
 *  MaterialInfo currently consists of 
 *      -Material Name
 *      -ADS properties
 *      -Specular exponent (shininess)
 *
 * \author Adam Campbell
 */
class MaterialInfo {
public:
	/**
	 * \brief Construct a MaterialInfo with the given parameters
	 */
	MaterialInfo(std::string mtlname = "empty", mat3 ads = mat3(), float specExp = 1.0) {
		this->mtlname = mtlname;
		this->ADSProps = ads;
		this->specularExponent = specExp;
	}

	/**
	 * \brief Get the ADS properties as a 3x3 matrix
	 *
	 * Organized as a matrix: [ A D S ]', where A, D, and S are the rows of the
	 * matrix and are the ambient, diffuse, and specular properties, respectively
	 *
	 * \return The ADS properties
	 */
	inline mat3 GetADSProperties() {
		return ADSProps;
	}

	/**
	 * \brief Get the specular exponent
	 *
	 * \return The specular exponent
	 */
	inline float GetSpecularExponent() {
		return specularExponent;
	}

	/**
	 * \brief Get the material name, as given in the .mtl file
	 * 
	 * \return The material name
	 */
	inline std::string GetMaterialName() {
		return mtlname;
	}

	/**
	 * \brief Set the specular exponent
	 */
	void SetSpecularExponent(float specularExponent) 
	{
		this->specularExponent = specularExponent;
	}

	/**
	 * \brief Set the entire ADS properties as a matrix
	 */
	void SetADSProperties(mat3 ADSProperties) 
	{
		this->ADSProps = ADSProperties;
	}

	/**
	 * \brief Set the ambient property of the ADS
	 */
	void SetAmbient(vec3 amb)
	{
		this->ADSProps[0] = amb;
	}

	/**
	 * \brief Set the diffuse property of the ADS
	 */
	void SetDiffuse(vec3 diffuse) 
	{
		this->ADSProps[1] = diffuse;
	}

	/**
	 * \brief Set the specular property of the ADS
	 */
	void SetSpecular(vec3 spec)
	{
		this->ADSProps[2] = spec;
	}

	/**
	 * \brief Set the name of the material
	 */
	void SetMaterialName(std::string mtlname)
	{
		this->mtlname = mtlname;
	}

private:
	mat3 ADSProps;            //!< ADS properties - Ambient, Diffuse, & Specular colors
	float specularExponent;   //!< Specular exponent - shininess
	std::string mtlname;      //!< Material name, as given in the .mtl file
};

/**
 * \brief Class to read a model from an obj file
 *
 * \author Jonathan Henze
 */
class ObjFile
{
public:

    /**
     * \brief Imports a model from an obj file
     *
     * \param[in] filename - File name and path to read from
     */
    ObjFile(const char* filename);

    /**
     * \brief Creates a deep copy of an ObjFile
     *
     * \param[in] other - ObjFile to copy
     */
    ObjFile(const ObjFile& other);

    /**
     * \brief Assigns this ObjFile to be a deep copy of an ObjFile
     *
     * \param[in] other - ObjFile to copy
     */
    ObjFile& operator=(const ObjFile& other);

    /**
     * \brief ObjFile destructor
     */
    ~ObjFile();

    /**
     * \brief Gets the array of vertices in the model
     *
     * \return The array of vertices in the model
     * \return Null if the obj file was not read successfully
     */
    inline const vec3* GetVertices() const 
    { 
        return vertices; 
    }

    /**
     * \brief Gets the array of normals in the model
     *
     * \return The array of normals in the model
     * \return Null if the obj file was not read successfully
     */
    inline const vec3* GetNormals() const 
    { 
        return normals; 
    }

    /**
     * \brief Gets the array of texture coordinates in the model
     *
     * \return The array of texture coordinates in the model
     * \return Null if the obj file did not have texture coordinates
     */
    inline const vec2* GetTexCoords() const 
    { 
        return texCoords; 
    }

    /**
     * \brief Gets the array of tangent vectors in the model
     *
     * Gets the array of tangent vectors for the model, which is generated
     * based on the positions and texture coordinates.
     *
     * \return The array of tangent vectors in the model
     * \return Null if the obj file did not have texture coordinates
     */
    inline const vec3* GetTangents() const
    {
        return tangents;
    }

    /**
     * \brief Gets the array of indices in the model for indexed rendering
     *
     * \return The array of indices in the model
     * \return Null if the obj file was not read successfully
     */
    inline const unsigned int* GetIndices() const 
    { 
        return indices; 
    }

    /**
     * \brief Gets the number of vertices in the model
     *
     * Gets the number of vertices in the model.  The vertices, normals and
     * texture coordinate arrays all contain this many elements.
     *
     * \return The number of vertices in the model
     */
    inline int GetNumVertices() const 
    { 
        return numVertices; 
    }

    /**
     * \brief Gets the number of indices in the indices array
     *
     * \return The number of indices in the indices array
     */
    inline int GetNumIndices() const 
    { 
        return numIndices; 
    }

	/**
	 * \brief Gets the number of submodels in the model. Returns 1 if there
	 * are no submodels explicitly specified
	 *
	 * \return The number of submodels in the model. 1 if there are no 
	 * submodels explicitly specified
	 */
	inline int GetNumSubmodels() const 
	{
		return numSubmodels == 0 ? 1 : numSubmodels;
	}

	/**
	 * \brief Get the indices for the given submodel
	 *
	 * \return The indices for the given submodel
	 */
	inline unsigned int* GetSubmodelIndices(int submodel) const 
	{
		if(HasSubmodels()) {
			return subindices[submodel];
		} else {
			return indices;
		}
	}

	/**
	 * \brief Gets the number of indices in the given submodel
	 *
	 * \return The number of indices in the given submodel
	 */
	int GetNumSubmodelIndices(int submodel) const 
	{
		if(HasSubmodels()) {
			if(submodelSizes && submodel >= 0 && submodel < numSubmodels) {
				return submodelSizes[submodel];
			} else {
				return -1;
			}
		} else {
			return numIndices;
		}
	}

    /**
     * \brief Gets the lower left corner of a bounding box containing the model
     *
     * \return The minimum x, y and z values of any vertex in the model
     */
    inline const vec3& getMinXYZ() const 
    { 
        return minXYZ; 
    }

    /**
     * \brief Gets the upper right corner of a bounding box containing the model
     *
     * \return The maximum x, y and z values of any vertex in the model
     */
    inline const vec3& getMaxXYZ() const 
    { 
        return maxXYZ; 
    }

	/**
	 * \brief Gets the material associated with the given submodel
	 *
	 * \return The material associated with the given submodel
	 */
	inline MaterialInfo& GetMaterial(int submodel) const
	{
		return materialList->at(submodel);
	}

	/**
	 * \brief Get whether this model has submodels or not
	 *
	 * \return Whether this model has submodels
	 */
	inline bool HasSubmodels() const 
	{
		return numSubmodels > 0;
	}

    /**
     * \brief Gets the point in the center of the model's bounding box
     *
     * \return A rough approximation of the center of the model
     */
    vec3 GetCenter() const 
    { 
        return (minXYZ + maxXYZ) / 2; 
    }

    /**
     * \brief Gets a scaling factor that give the model a radius of ~1
     *
     * \return A scale factor to size a model to have a radius of ~1
     */
    float GetScaleFactor() const;


private:

    /**
     * \brief Helper method to copy another ObjFile to this one
     *
     * \param[in] other - ObjFile to copy
     */
    void Copy(const ObjFile& other);

    /**
     * \brief Helper method to free up memory used by this ObjFile
     */
    void FreeMemory();

    /**
     * \brief Reads an obj file into the data arrays
     *
     * \param[in] file - Open file stream to read from
     */
    void ReadObjFile(std::string filename);

	/**
	 * \brief Read a material file and add the found materials to the material list
	 *
     * \return The number of new materials found in the file
	 */
	int ReadMtlFile(std::string matfilename);

	/**
	 * \brief Find a material in the material list by name
	 *
	 * \return The index of the material in the list, -1 if not found
	 */
	int FindMaterialIndexByName(std::string mtlname);

    /**
     * \brief Calculates the vertex normals by averaging face normals
     */
    void CalculateNormals();

    /**
     * \brief Calculate the tangent vectors based on the texture coordinates
     */
    void CalculateTangents();

	/**
	 * \brief Gets the square of the length of a vector
	 *
	 * \param[in] v - Vector to get length of
	 *
	 * \return Length of v squared
	 */
	inline float LengthSquared(const vec3& v)
	{
		return dot(v, v);
	}

    int numVertices;    //!< Number of vertices the model has
    int numIndices;     //!< Number of indices the model has (3 for each triangle)
	int numSubmodels;   //!< Number of materials (submodels)
	int* submodelSizes; //!< Sizes for each submodel

    vec3*                      vertices;     //!< Array of vertices or NULL
    vec3*                      normals;      //!< Array of normals or NULL
    vec2*                      texCoords;    //!< Array of texture coordinates or NULL
    vec3*                      tangents;     //!< Array of tangents or NULL
    unsigned int*              indices;      //!< Array of indices or NULL
	unsigned int**             subindices;   //!< Array of arrays of subindices for each submodel
	std::vector<MaterialInfo>* materialList; //!< List of materials used in the models

    vec3  minXYZ; //!< Minimum x, y and z values for a bounding box
    vec3  maxXYZ; //!< Maximum x, y and z values for a bounding box
};


#endif
