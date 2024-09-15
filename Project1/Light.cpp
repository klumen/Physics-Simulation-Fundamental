#include "Light.h"

DirLight::DirLight(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular) : direction(direction), ambient(ambient), diffuse(diffuse), specular(specular)
{
}

void DirLight::setup(Shader& shader) const
{
	shader.set_vec3("dirLight.direction", direction);

	shader.set_vec3("dirLight.ambient", ambient);
	shader.set_vec3("dirLight.diffuse", diffuse);
	shader.set_vec3("dirLight.specular", specular);
}

PointLight::PointLight(const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float constant, float linear, float quadratic) : position(position), ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadratic)
{
}

void PointLight::setup(Shader& shader) const
{
	shader.set_vec3("pointLight.position", position);

	shader.set_vec3("pointLight.ambient", ambient);
	shader.set_vec3("pointLight.diffuse", diffuse);
	shader.set_vec3("pointLight.specular", specular);

	shader.set_float("pointLight.constant", constant);
	shader.set_float("pointLight.linear", linear);
	shader.set_float("pointLight.quadratic", quadratic);
}

SpotLight::SpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float constant, float linear, float quadratic, float cutoff, float outerCutoff) : position(position), direction(direction), ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadratic), cutoff(cutoff), outerCutoff(outerCutoff)
{
}

void SpotLight::setup(Shader& shader) const
{
	shader.set_vec3("spotLight.position", position);
	shader.set_vec3("spotLight.direction", direction);

	shader.set_vec3("spotLight.ambient", ambient);
	shader.set_vec3("spotLight.diffuse", diffuse);
	shader.set_vec3("spotLight.specular", specular);

	shader.set_float("spotLight.constant", constant);
	shader.set_float("spotLight.linear", linear);
	shader.set_float("spotLight.quadratic", quadratic);

	shader.set_float("spotLight.cutoff", cutoff);
	shader.set_float("spotLight.outerCutoff", outerCutoff);
}