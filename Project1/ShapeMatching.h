#ifndef SHAPE_MATCHING_H
#define SHAPE_MATCHING_H

#include <thread>
#include <mutex>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Math.h"
#include "Shader.h"
#include "Mesh.h"

class ShapeMatching
{
public:
	ShapeMatching(Mesh* mesh);
	~ShapeMatching();

	void update(float deltaTime, Shader& shader);

	Mesh* mesh;
	unsigned int particlesNr;
	std::vector<float> m;
	float mass;

	float vDamping;
	glm::vec3 gravity;

	std::vector<glm::vec3> v;
	std::vector<glm::vec3> x;

	glm::vec3 x0_cm;
	std::vector<glm::vec3> q;
	glm::mat3 qq_inv;

	float alpha;

private:
	void shape_matching(float deltaTime, Shader& shader);

};

#endif // !SHAPE_MATCHING_H
