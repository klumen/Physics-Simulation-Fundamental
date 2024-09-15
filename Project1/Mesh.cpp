#include "Mesh.h"

Mesh::Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures, const AABB& box) : name(name), vertices(vertices), indices(indices), textures(textures), box(box)
{
	setup();
}

Mesh::Mesh(const std::string& name, const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures, const AABB& box) : name(name), indices(indices), textures(textures), box(box)
{
	this->vertices.resize(vertices.size());
	for (unsigned int i = 0; i < vertices.size(); i++)
		this->vertices[i].position = vertices[i];

	setup();
}

void Mesh::setup()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Mesh::recalculate_normals()
{
	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		const glm::vec3& v0 = vertices[indices[i]].position;
		const glm::vec3& v1 = vertices[indices[i + 1]].position;
		const glm::vec3& v2 = vertices[indices[i + 2]].position;

		glm::vec3 edge1 = v1 - v0;
		glm::vec3 edge2 = v2 - v0;

		glm::vec3 normal = glm::cross(edge1, edge2);
		normal = glm::normalize(normal);

		vertices[indices[i]].normal += normal;
		vertices[indices[i + 1]].normal += normal;
		vertices[indices[i + 2]].normal += normal;
	}

	for (unsigned int i = 0; i < vertices.size(); i++)
		vertices[i].normal = glm::normalize(vertices[i].normal);
}

void Mesh::draw(Shader& shader) const
{
	unsigned int diffuseN = 0;
	unsigned int specularN = 0;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string number;
		std::string name = textures[i].type;
		if (name == "diffuse")
			number = std::to_string(diffuseN++);
		else if (name == "specular")
			number = std::to_string(specularN++);

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		shader.set_int("material." + name + number, i);
	}
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}