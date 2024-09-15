#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include <thread>
#include <mutex>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Math.h"
#include "Shader.h"
#include "Mesh.h"

class RigidBody
{
public:
	RigidBody(Mesh* mesh);
	~RigidBody();

	void update(float deltaTime, Shader& shader);

	Mesh* mesh;

	glm::vec3 gravity;
	float mass;
	glm::mat3 I_ref;
	
	glm::vec3 v, w;
	glm::vec3 x;
	glm::quat q;

	float vDamping;
	float wDamping;
	float restitution;

private:
	void euler(float deltaTime, Shader& shader);
	void collision(const glm::vec3& P, const glm::vec3& N);

};

#endif // !RIGID_BODY_H
