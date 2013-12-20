#include <Adjacency.h>
#include <string>
#include <iostream>
#include <sstream>

void reverse(int*, int, int);

namespace Adjacency 
{
	/**
	 * \brief Read an existing adjacency file and extract the adjacency info
	 *
	 * \return A vector containing the adjacent indices in the format expected by OpenGL,
	 *         or NULL if there is no file by the given name
	 */
	std::vector<int>* ReadAdjFile(std::string adjfilename)
	{
		std::ifstream adjfile(adjfilename);
		if(!adjfile.good()) 
		{
			// No adj file found
			return NULL;
		}

		unsigned int size; // Number of adjacency indices
		std::string line;

		//Get the size (first number in the file)
		while(adjfile.good()) {
			std::getline(adjfile, line);

			if(!line.empty() && line.compare(0, 1, "#") != 0) { 
				std::stringstream tparser(line);
				tparser >> size;
				break;
			}	
		}

		std::vector<int>* ret = new std::vector<int>();

		int i=0;
		while(adjfile.good() && i < size) 
		{
			std::getline(adjfile, line);

			//Ignore all empty lines and comment lines starting with #
			if(!line.empty() && line.compare(0, 1, "#") != 0) {
				std::stringstream tparser(line);
				while(tparser.good()) 
				{
					int adjIdx = -1;
					std::string str = "";
				
					tparser >> str;
					if(!str.empty())
					{
						adjIdx = std::stoi(str);
						if(adjIdx >= 0) 
						{
							ret->push_back(adjIdx);
						}
					}
				}
			}
		}

		//The size functions like a sort of sanity check or parity bit
		if(ret->size() != size)
		{
			std::cerr << "Error: adjacency file has invalid number of indices. Recomputing adjacency info manually.." << std::endl;
			return NULL;
		}

		return ret;
	}

	/**
	 * \brief Write adjacency information to a file
	 */
	void WriteAdjFile(std::vector<int>* adjInfo, std::string adjfilename) 
	{
		std::ofstream adjfile;
		adjfile.open(adjfilename);
		if(!adjfile.good()) 
		{
			std::cerr << "Invalid adjacency file \"" << adjfilename << "\". Aborting write." << std::endl;
			return;
		}

		adjfile << "##################" << std::endl;
		adjfile << "# Adjacency File #" << std::endl;
		adjfile << "##################" << std::endl << std::endl;

		adjfile << "#Size:" << std::endl;
		adjfile << adjInfo->size() << std::endl << std::endl;

		adjfile << "#Adjacent indices:" << std::endl;
		for(int i=0; i < adjInfo->size(); i++) 
		{
			adjfile << adjInfo->at(i) << " ";
		}
		adjfile << std::endl;
		adjfile.close();

		std::cout << "Successfully wrote adjacency file \"" << adjfilename << "\"." << std::endl;
	}

	/**
	 * \brief Compute adjacency information for a given .obj file
	 *
	 * Goes through every triangle in the model and finds triangles that share an edge,
	 * then creates an adjacency index list (to be used with GL_TRIANGLE_ADJACENCY) and returns it.
	 */
	std::vector<int>* ComputeAdjacency(ObjFile* model) 
	{
		const unsigned int* modelIdxs = model->GetIndices();
		int ml = model->GetNumIndices();

		const vec3* v = model->GetVertices();
		std::vector<int>* adjInfo = new std::vector<int>();

		for(int i=0; i < ml; i+=3) {
			int vertexIdxs[9] = {};
			int modelIdxsIdxs[9] = {};
			int n = 0;
			int numAdjTri = 0;
			int o = 0;
			int numAdj = 0;
			float tol = 0.000001f;
			int res[6] = {};

			vec3 a[] = {v[modelIdxs[i]], v[modelIdxs[i+1]], v[modelIdxs[i+2]]};
			for(int j=0; j < ml; j+=3) {
				if(j != i) {
					vec3 b[] = {v[modelIdxs[j]], v[modelIdxs[j+1]], v[modelIdxs[j+2]]};
					int count = 0;
					int sharedIdxs[4] = {};
					int m=0;
					for(int k=0; k < sizeof(a)/sizeof(a[0]); k++) {
						for(int l=0; l < sizeof(b)/sizeof(b[0]); l++) {
							if(abs(a[k].x - b[l].x) < tol && abs(a[k].y - b[l].y) < tol && abs(a[k].z - b[l].z) < tol && count < 2){
								count++;
								sharedIdxs[m++] = i + k;
								sharedIdxs[m++] = j + l;
							}
						}
					}

					if(count == 2) {
						numAdjTri++;
						for(int k=j; k < j + 3; k++) {
							if(modelIdxs[k] != modelIdxs[sharedIdxs[1]] && modelIdxs[k] != modelIdxs[sharedIdxs[3]]) {
								int nonadj = modelIdxs[k];
								if(n >= 7) {
									break;
								}
								vertexIdxs[n++] = modelIdxs[sharedIdxs[0]];
								vertexIdxs[n++] = nonadj;

								vertexIdxs[n++] = modelIdxs[sharedIdxs[2]];
								modelIdxsIdxs[o++] = sharedIdxs[0];
								modelIdxsIdxs[o++] = k;
								modelIdxsIdxs[o++] = sharedIdxs[2];
							}
						}
					}
				}
			}

			if(numAdjTri == 3) {
				if(modelIdxsIdxs[0] == modelIdxsIdxs[3]) {
					reverse(modelIdxsIdxs, 0, 2);
					reverse(vertexIdxs, 0, 2);
				} else if(modelIdxsIdxs[0] == modelIdxsIdxs[5]) {
					reverse(modelIdxsIdxs, 0, 2);
					reverse(vertexIdxs, 0, 2);
					reverse(modelIdxsIdxs, 3, 5);
					reverse(vertexIdxs, 3, 5);
				} else if(modelIdxsIdxs[2] == modelIdxsIdxs[3]) {

				} else if(modelIdxsIdxs[2] == modelIdxsIdxs[5]) {
					reverse(modelIdxsIdxs, 3, 5);
					reverse(vertexIdxs, 3, 5);
				} else {
					std::cerr << "\t\t\t\tERROR!" << std::endl;
					exit(1);
				}

				if(modelIdxsIdxs[3] == modelIdxsIdxs[6]) {
					reverse(modelIdxsIdxs, 3, 5);
					reverse(vertexIdxs, 3, 5);
				} else if(modelIdxsIdxs[3] == modelIdxsIdxs[8]) {
					reverse(modelIdxsIdxs, 3, 5);
					reverse(vertexIdxs, 3, 5);
					reverse(modelIdxsIdxs, 6, 8);
					reverse(vertexIdxs, 6, 8);
				} else if(modelIdxsIdxs[5] == modelIdxsIdxs[6]) {

				} else if(modelIdxsIdxs[5] == modelIdxsIdxs[8]) {
					reverse(modelIdxsIdxs, 6, 8);
					reverse(vertexIdxs, 6, 8);
				} else {
					std::cerr << "\t\t\t\tERROR!" << std::endl;
					exit(1);
				}

				res[0] = vertexIdxs[0];
				res[1] = vertexIdxs[1];
				res[2] = vertexIdxs[3];
				res[3] = vertexIdxs[4];
				res[4] = vertexIdxs[6];
				res[5] = vertexIdxs[7];

				for(int p=0; p < 6; p++) {
					adjInfo->push_back(res[p]);
				}
			}
		}

		return adjInfo;
	}
}

void swap(int arr[], int a, int b) 
{
	int temp = arr[a];
	arr[a] = arr[b];
	arr[b] = temp;
}

void reverse(int arr[], int start, int end) 
{
	if(start > end) {
		int temp = start;
		start = end;
		end = temp;
	}

	while(start < end) {
		swap(arr, start, end);
		start++;
		end--;
	}
}