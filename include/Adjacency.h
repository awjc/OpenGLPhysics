#ifndef ADJACENCY_H
#define ADJACENCY_H

#include <string>
#include <ObjFile.h>

/**
 * Namespace for all things related to triangle adjacency
 * 
 * \author Adam Campbell
 */
namespace Adjacency 
{
	/**
	 * \brief Read an existing adjacency file and extract the adjacency info
	 */
	std::vector<int>* ReadAdjFile(std::string);

	/**
	 * \brief Write adjacency information to a file
	 */
	void WriteAdjFile(std::vector<int>*, std::string);

	/**
	 * \brief Compute adjacency information for a given .obj file
	 *
	 * Goes through every triangle in the model and finds triangles that share an edge,
	 * then creates an adjacency index list (to be used with GL_TRIANGLE_ADJACENCY) and returns it.
	 */
	std::vector<int>* ComputeAdjacency(ObjFile*);
}

#endif