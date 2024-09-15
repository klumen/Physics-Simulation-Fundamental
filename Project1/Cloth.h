#ifndef CLOTH_H
#define CLOTH_H

#include <algorithm>
#include <set>
#include <thread>
#include <mutex>

#include "Mesh.h"

class Cloth
{
public:
	Cloth(Mesh* mesh);
	~Cloth();

	void update(float deltaTime, Shader& shader);

	float spring_k = 80000;
	glm::vec3 gravity = glm::vec3(0.f, -9.8f, 0.f);
	float damping = 0.2f;

private:
	void particle_system(float deltaTime, Shader& shader);
	void get_gradient(float dt_inv, std::vector<glm::vec3>& g, const std::vector<glm::vec3>& x_hat);

	Mesh* mesh;
	unsigned int particlesNr = 0;
	std::vector<glm::u32vec2> springs;
	std::vector<float> length;
	
	std::vector<float> mass;
	std::vector<glm::vec3> x;
	std::vector<glm::vec3> v;

	unsigned int s0 = 0;
	unsigned int s1 = 0;
};

#endif // !CLOTH_H
