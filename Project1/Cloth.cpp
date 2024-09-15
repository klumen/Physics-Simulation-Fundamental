#include "Cloth.h"

Cloth::Cloth(Mesh* mesh) : mesh(mesh)
{
	if (!mesh)
		return;

	particlesNr = mesh->vertices.size();
	std::vector<glm::u32vec3> triple(mesh->indices.size());
	for (unsigned int i = 0; i < triple.size() / 3; i++)
	{
		triple[3 * i + 0] = glm::u32vec3(mesh->indices[3 * i + 0], mesh->indices[3 * i + 1], i);
		triple[3 * i + 1] = glm::u32vec3(mesh->indices[3 * i + 1], mesh->indices[3 * i + 2], i);
		triple[3 * i + 2] = glm::u32vec3(mesh->indices[3 * i + 0], mesh->indices[3 * i + 2], i);
	}
	for (auto& t : triple)
		if (t[0] > t[1])
			std::swap(t[0], t[1]);

	std::sort(triple.begin(), triple.end(), [](const glm::u32vec3& a, const glm::u32vec3& b) {
		for (unsigned int i = 0; i < 3; i++)
		{
			if (a[i] < b[i])
				return true;
			else if (a[i] > b[i])
				return false;
		}

		return false;
		});

	springs.emplace_back(glm::vec2(triple[0][0], triple[0][1]));
	std::vector<glm::u32vec2> nbrTri;
	for (unsigned int i = 1; i < triple.size(); i++)
	{
		if (triple[i - 1][0] == triple[i][0] && triple[i - 1][1] == triple[i][1])
		{
			nbrTri.emplace_back(glm::u32vec2(triple[i - 1][2], triple[i][2]));
			continue;
		}
		springs.emplace_back(glm::u32vec2(triple[i][0], triple[i][1]));
	}
	/*
	for (auto& nbr : nbrTri)
	{
		unsigned int s0 = 3 * nbr[0];
		unsigned int s1 = 3 * nbr[1];

		std::set<unsigned int> id0 = { mesh->indices[s0], mesh->indices[s0 + 1], mesh->indices[s0 + 2] };
		std::set<unsigned int> id1 = { mesh->indices[s1], mesh->indices[s1 + 1], mesh->indices[s1 + 2] };
		std::set<unsigned int> unionSet;
		std::set<unsigned int> intersectionSet;
		std::set<unsigned int> res;
		std::set_union(id0.begin(), id0.end(), id1.begin(), id1.end(), std::inserter(unionSet, unionSet.begin()));
		std::set_intersection(id0.begin(), id0.end(), id1.begin(), id1.end(), std::inserter(intersectionSet, intersectionSet.begin()));
		std::set_difference(unionSet.begin(), unionSet.end(), intersectionSet.begin(), intersectionSet.end(), std::inserter(res, res.begin()));
		if (res.size() != 2)
			std::cerr << "CLOTH::CLOTH::ERROR" << std::endl;

		auto it0 = res.begin();
		unsigned int u = *it0;
		unsigned int v = *(++it0);
		springs.emplace_back(u, v);
	}
	*/
	length.resize(springs.size());
	for (unsigned int i = 0; i < length.size(); i++)
	{
		glm::vec3 l = mesh->vertices[springs[i][0]].position - mesh->vertices[springs[i][1]].position;
		length[i] = sqrtf(glm::dot(l, l));
	}

	x.resize(particlesNr), v.resize(particlesNr), mass.resize(particlesNr);
	for (unsigned int i = 0; i < particlesNr; i++)
	{
		mass[i] = 1.f;
		x[i] = mesh->vertices[i].position;
		v[i] = glm::vec3(0.f);
		
		if (x[i].z == -1.f)
		{
			if (x[i].x == 1.f )
				s0 = i;

			if (x[i].x == -1.f)
				s1 = i;
		}
	}
}

Cloth::~Cloth()
{
	mesh = nullptr;
}

void Cloth::update(float deltaTime, Shader& shader)
{
	if (!mesh)
		return;

	particle_system(deltaTime, shader);
}

void Cloth::particle_system(float deltaTime, Shader& shader)
{
	std::vector<glm::vec3> x_hat(particlesNr);
	std::vector<glm::vec3> g(particlesNr);

	for (unsigned int j = 0; j < particlesNr; j++)
	{
		if (j == s0 || j == s1)
			continue;

		v[j] *= expf(-damping * deltaTime);
		x[j] += v[j] * deltaTime;
		x_hat[j] = x[j];
	}

	unsigned int iterationFre = 32;
	for (unsigned int k = 0; k < iterationFre; k++)
	{
		get_gradient(1.f / deltaTime, g, x_hat);

		for (unsigned int j = 0; j < particlesNr; j++)
		{
			if (j == s0 || j == s1)
				continue;

			glm::mat3 H(mass[j] / deltaTime / deltaTime + 4 * spring_k);
			x[j] += glm::inverse(H) * -g[j];
		}
	}

	for (unsigned int j = 0; j < particlesNr; j++)
	{
		if (j == s0 || j == s1)
			continue;

		v[j] = (x[j] - x_hat[j]) / deltaTime;
		mesh->vertices[j].position = x[j];
	}
	mesh->recalculate_normals();
}

void Cloth::get_gradient(float dt_inv, std::vector<glm::vec3>& g, const std::vector<glm::vec3>& x_hat)
{
	for (unsigned int j = 0; j < particlesNr; j++)
		g[j] = dt_inv * dt_inv * mass[j] * (x[j] - x_hat[j]);

	for (unsigned int j = 0; j < springs.size(); j++)
	{
		unsigned int u = springs[j][0];
		unsigned int v = springs[j][1];
		glm::vec3 l = (x[u] - x[v]);
		glm::vec3 f = -spring_k * (1.f - length[j] / sqrtf(glm::dot(l, l))) * l;

		g[u] -= f;
		g[v] -= -f;
		g[u] -= mass[u] * gravity;
		g[v] -= mass[v] * gravity;
	}
}