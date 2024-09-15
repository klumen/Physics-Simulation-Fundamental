#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"
#include "Shader.h"
#include "Mesh.h"

class Model
{
public:
	Model(const std::string& path);
	~Model() = default;

	void draw(Shader& shader) const;
	Mesh* find_mesh(const std::string& name);

private:
	void load(const std::string& path);
	void process_node(aiNode* node, const aiScene* scene);
	Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> load_textures(aiMaterial* mtl, aiTextureType type, const std::string& typeName);

	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;

};

unsigned int load_texture(const std::string& directory, const std::string& name);

#endif // !MODEL_H
