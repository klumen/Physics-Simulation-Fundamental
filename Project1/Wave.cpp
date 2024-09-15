#include "Wave.h"

Wave::Wave() : size(100)
{
	std::vector<glm::vec3> vertices(size * size);
	for (unsigned int i = 0; i < size; i++)
		for (unsigned int j = 0; j < size; j++)
		{
			vertices[i * size + j][0] = i * 0.1f - size * 0.05f;
			vertices[i * size + j][1] = 0.f;
			vertices[i * size + j][2] = j * 0.1f - size * 0.05f;
		}

	std::vector<unsigned int> indices((size - 1) * (size - 1) * 6);
	int index = 0;
	for (unsigned int i = 0; i < size - 1; i++)
		for (unsigned int j = 0; j < size - 1; j++)
		{
			indices[index * 6 + 0] = (i + 0) * size + (j + 0);
			indices[index * 6 + 1] = (i + 0) * size + (j + 1);
			indices[index * 6 + 2] = (i + 1) * size + (j + 1);
			indices[index * 6 + 3] = (i + 0) * size + (j + 0);
			indices[index * 6 + 4] = (i + 1) * size + (j + 1);
			indices[index * 6 + 5] = (i + 1) * size + (j + 0);
			index++;
		}

	mesh = new Mesh("wave", vertices, indices, std::vector<Texture>(), {});
	mesh->recalculate_normals();

	h_old.resize(size);
	for (unsigned int i = 0; i < size; i++)
	{
		h_old[i].resize(size);
		for (unsigned int j = 0; j < size; j++)
			h_old[i][j] = 0.f;
	}
}

void Wave::update(float deltaTime, Shader& shader)
{
	if (mesh == nullptr)
		return;

	shallow_wave(deltaTime, shader);
}

void Wave::shallow_wave(float deltaTime, Shader& shader)
{
	std::vector<std::vector<float>> h(size);
	for (unsigned int i = 0; i < size; i++)
	{
		h[i].resize(size);
		for (unsigned int j = 0; j < size; j++)
			h[i][j] = mesh->vertices[i * size + j].position.y;
	}

	if (createWave)
	{
		unsigned int u = random_uint(1, size - 2);
		unsigned int v = random_uint(1, size - 2);
		float w = random_float(0.f, 0.1f);
		h[u][v] = w;
		h[u - 1][v] = h[u][v - 1] = h[u + 1][v] = h[u][v + 1] = -w / 4.f;
		createWave = false;
	}

	std::vector<std::vector<float>> h_new(size);
	for (unsigned int i = 0; i < size; i++)
	{
		h_new[i].resize(size);
		for (unsigned int j = 0; j < size; j++)
		{
			h_new[i][j] = h[i][j] + damping * (h[i][j] - h_old[i][j]);
			if (i > 0) h_new[i][j] += rate * (h[i - 1][j] - h[i][j]);
			if (i < size - 1) h_new[i][j] += rate * (h[i + 1][j] - h[i][j]);
			if (j > 0) h_new[i][j] += rate * (h[i][j - 1] - h[i][j]);
			if (j < size - 1) h_new[i][j] += rate * (h[i][j + 1] - h[i][j]);
		}
	}

	for (unsigned int i = 0; i < size; i++)
		for (unsigned int j = 0; j < size; j++)
		{
			h_old[i][j] = h[i][j];
			h[i][j] = h_new[i][j];
		}

	for (unsigned int i = 0; i < size; i++)
		for (unsigned int j = 0; j < size; j++)
			mesh->vertices[i * size + j].position.y = h[i][j];
	mesh->recalculate_normals();
}