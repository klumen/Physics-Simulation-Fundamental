#ifndef SOFTBODY_H
#define SOFTBODY_H

#include "Mesh.h"

class SoftBody
{
public:
	SoftBody();
	~SoftBody() = default;

	void update(float deltaTime, Shader& shader);

	Mesh* mesh = nullptr;
	glm::vec3 gravity;

	float damping = 0.5f;
	float mu = 5000.f;
	float lambda = 20000.f;

private:
	unsigned int tetNr = 0;
	std::vector<glm::u32vec4> tets;
	unsigned int particleNr;
	std::vector<float> m;
	std::vector<glm::vec3> x;
	std::vector<glm::vec3> v;
	std::vector<glm::vec3> f;

	std::vector<glm::mat3> Dm_inv;

	void FVM(float deltaTime, Shader& shader);
	void collision(const glm::vec3& P, const glm::vec3& N);
	void smooth_v();

};
#endif // !SOFTBODY_H
