#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

class DirLight
{
public:
	DirLight(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular);
	~DirLight() = default;

	void setup(Shader& shader) const;

	glm::vec3 direction;

	glm::vec3 ambient, diffuse, specular;

private:

};

class PointLight
{
public:
	PointLight(const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float constant, float linear, float quadratic);
	~PointLight() = default;

	void setup(Shader& shader) const;

	glm::vec3 position;

	glm::vec3 ambient, diffuse, specular;

	float constant, linear, quadratic;

private:

};

class SpotLight
{
public:
	SpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float constant, float linear, float quadratic, float cutoff, float outerCutoff);
	~SpotLight() = default;

	void setup(Shader& shader) const;

	glm::vec3 position, direction;

	glm::vec3 ambient, diffuse, specular;

	float constant, linear, quadratic;

	float cutoff, outerCutoff;

private:

};

#endif // !LIGHT_H
