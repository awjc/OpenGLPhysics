#ifndef OBJECT_MODEL_H
#define OBJECT_MODEL_H

#define printaddress 0
#define doprint 0


#include <ObjFile.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Adjacency.h>

class ObjectModel {
public:
	ObjectModel(ObjFile* m, bool doAdjacency = false, std::string adjfilename = "") {
		this->adjVao = NULL;
		this->model = m;
		this->numSubmodels = m->GetNumSubmodels();
		this->vaos = new VertexArray*[numSubmodels];
		for(int i=0; i < numSubmodels; i++) {
			vaos[i] = new VertexArray();
			vaos[i]->AddAttribute("vPosition", m->GetVertices(), m->GetNumVertices());
			vaos[i]->AddAttribute("vNormal", m->GetNormals(), m->GetNumVertices());
			vaos[i]->AddIndices(m->GetSubmodelIndices(i), m->GetNumSubmodelIndices(i));
		}

		if(doAdjacency) {
			adjVao = new VertexArray();
			adjVao->AddAttribute("vPosition", m->GetVertices(), m->GetNumVertices());
			adjVao->AddAttribute("vNormal", m->GetNormals(), m->GetNumVertices());
			
			std::vector<int>* adjInfo = Adjacency::ReadAdjFile(adjfilename);
			if (!adjInfo) {
				adjInfo = Adjacency::ComputeAdjacency(m);
				Adjacency::WriteAdjFile(adjInfo, adjfilename);
			}

			adjVao->AddIndices((const unsigned int*) adjInfo->data(), adjInfo->size());

			delete adjInfo;
		}
	}

	~ObjectModel() {
		delete model;
		for(int i=0; i < numSubmodels; i++ )
		{
			delete vaos[i];
		}
		delete vaos;
		delete adjVao;
	}

	void Draw(Shader* shader, bool doMaterials, Shader* outlineShader = NULL, MaterialInfo* defaultMaterial = NULL) {
		shader->Bind();
		for(int i=0; i < numSubmodels; i++) {
			if(model->GetNumSubmodelIndices(i) > 0) {
				if(doMaterials) {
					if(model->HasSubmodels()) {
						MaterialInfo& mtl = model->GetMaterial(i);
						shader->SetUniform("materialProperties", mtl.GetADSProperties());
						shader->SetUniform("shininess", mtl.GetSpecularExponent());
					} else if(defaultMaterial) {
						shader->SetUniform("materialProperties", defaultMaterial->GetADSProperties());
						shader->SetUniform("shininess", defaultMaterial->GetSpecularExponent());
					}
				}

				vaos[i]->Bind(*shader);
				vaos[i]->Draw(GL_TRIANGLES);
			}
		}
		shader->Unbind();

		if(outlineShader && adjVao) {
			outlineShader->Bind();
			adjVao->Bind(*outlineShader);
			adjVao->Draw(GL_TRIANGLES_ADJACENCY);
			outlineShader->Unbind();
		}
	}

private:
	ObjFile* model;
	int numSubmodels;
	VertexArray** vaos;
	VertexArray* adjVao;
};

#endif