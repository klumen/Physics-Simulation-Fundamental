#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string name;
};

struct AABB
{
	glm::vec3 max;
	glm::vec3 min;
};

class Mesh
{
public:
	Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures, const AABB& box);
	Mesh(const std::string& name, const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures, const AABB& box);
	~Mesh() = default;

	void draw(Shader& shader) const;
	void recalculate_normals();

	std::string name;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	AABB box;

private:
	void setup();

	unsigned int VAO, VBO, EBO;

};

#endif // !MESH_H
