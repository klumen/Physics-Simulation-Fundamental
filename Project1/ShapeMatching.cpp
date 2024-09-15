#include "ShapeMatching.h"

ShapeMatching::ShapeMatching(Mesh* mesh) : mesh(mesh), mass(0.f), particlesNr(0), x0_cm(0.f), qq_inv(0.f), vDamping(0.2f), gravity(0.f, -9.8f, 0.f), alpha(1.f)
{
	if (!mesh)
		return;

	particlesNr = mesh->vertices.size();
	m.resize(particlesNr);
	v.resize(particlesNr);
	x.resize(particlesNr);
	q.resize(particlesNr);

	for (unsigned int i = 0; i < particlesNr; i++)
	{
		m[i] = 1.f;
		v[i] = glm::vec3(-5.f, 2.f, 0.f);
		x[i] = mesh->vertices[i].position;
	}

	glm::vec3 x0_sum(0.f);
	for (unsigned int i = 0; i < particlesNr; i++)
	{
		x0_sum += m[i] * mesh->vertices[i].position;
		mass += m[i];
	}
	x0_cm = x0_sum / mass;

	for (unsigned int i = 0; i < particlesNr; i++)
	{
		q[i] = mesh->vertices[i].position - x0_cm;

		qq_inv[0][0] += m[i] * q[i].x * q[i].x;
		qq_inv[0][1] += m[i] * q[i].x * q[i].y;
		qq_inv[0][2] += m[i] * q[i].x * q[i].z;
		qq_inv[1][0] += m[i] * q[i].y * q[i].x;
		qq_inv[1][1] += m[i] * q[i].y * q[i].y;
		qq_inv[1][2] += m[i] * q[i].y * q[i].z;
		qq_inv[2][0] += m[i] * q[i].z * q[i].x;
		qq_inv[2][1] += m[i] * q[i].z * q[i].y;
		qq_inv[2][2] += m[i] * q[i].z * q[i].z;
	}
	qq_inv = glm::inverse(qq_inv);
}

ShapeMatching::~ShapeMatching()
{
	mesh = nullptr;
}

void ShapeMatching::update(float deltaTime, Shader& shader)
{
	shape_matching(deltaTime, shader);
}

void ShapeMatching::shape_matching(float deltaTime, Shader& shader)
{
	unsigned int numThreads = std::thread::hardware_concurrency();
	if (numThreads == 0)
		numThreads = 4;
	unsigned int verticesPerThread = particlesNr / numThreads;
	std::vector<std::thread> threads;
	std::mutex yMutex;

	for (unsigned int i = 0; i < numThreads; i++)
	{
		unsigned int start = i * verticesPerThread;
		unsigned int end = (i == numThreads - 1) ? mesh->vertices.size() : (i + 1) * verticesPerThread;

		threads.emplace_back([start, end, &yMutex, &deltaTime, this]() {
			glm::vec3 local_y_sum(0.f);

			for (unsigned int j = start; j < end; j++)
			{
				v[j] += deltaTime * gravity;
				v[j] *= exp(-vDamping * deltaTime);
			}
			});
	}
	for (auto& thread : threads)
		thread.join();
	std::vector<std::thread>().swap(threads);

	std::vector<glm::vec3> y(particlesNr);
	glm::vec3 y_sum(0.f);
	glm::vec3 y_cm(0.f);

	for (unsigned int i = 0; i < numThreads; i++)
	{
		unsigned int start = i * verticesPerThread;
		unsigned int end = (i == numThreads - 1) ? mesh->vertices.size() : (i + 1) * verticesPerThread;

		threads.emplace_back([start, end, &yMutex, &y, &y_sum, &deltaTime, this]() {
			glm::vec3 local_y_sum(0.f);

			for (unsigned int j = start; j < end; j++)
			{
				y[j] = x[j] + deltaTime * v[j];
				local_y_sum += m[j] * y[j];
			}
			{
				std::lock_guard<std::mutex> lock(yMutex);
				y_sum += local_y_sum;
			}
			});
	}
	for (auto& thread : threads)
		thread.join();
	std::vector<std::thread>().swap(threads);

	y_cm = y_sum / mass;
	glm::mat3 A(0.f);

	for (unsigned int i = 0; i < numThreads; i++)
	{
		unsigned int start = i * verticesPerThread;
		unsigned int end = (i == numThreads - 1) ? mesh->vertices.size() : (i + 1) * verticesPerThread;

		threads.emplace_back([start, end, &yMutex, &y, &deltaTime, &y_cm, &A, this]() {
			glm::mat3 local_A(0.f);

			for (unsigned int j = start; j < end; j++)
			{
				local_A[0][0] += m[j] * (y[j] - y_cm)[0] * q[j][0];
				local_A[0][1] += m[j] * (y[j] - y_cm)[0] * q[j][1];
				local_A[0][2] += m[j] * (y[j] - y_cm)[0] * q[j][2];
				local_A[1][0] += m[j] * (y[j] - y_cm)[1] * q[j][0];
				local_A[1][1] += m[j] * (y[j] - y_cm)[1] * q[j][1];
				local_A[1][2] += m[j] * (y[j] - y_cm)[1] * q[j][2];
				local_A[2][0] += m[j] * (y[j] - y_cm)[2] * q[j][0];
				local_A[2][1] += m[j] * (y[j] - y_cm)[2] * q[j][1];
				local_A[2][2] += m[j] * (y[j] - y_cm)[2] * q[j][2];
			}
			{
				std::lock_guard<std::mutex> lock(yMutex);
				A += local_A;
			}
			});
	}
	for (auto& thread : threads)
		thread.join();
	std::vector<std::thread>().swap(threads);

	A = qq_inv * A;
	glm::mat3 R = Math::polar_decomposition(A);

	for (unsigned int i = 0; i < numThreads; i++)
	{
		unsigned int start = i * verticesPerThread;
		unsigned int end = (i == numThreads - 1) ? mesh->vertices.size() : (i + 1) * verticesPerThread;

		threads.emplace_back([start, end, &y, &y_cm, &R, &deltaTime, this]() {
			for (unsigned int j = start; j < end; j++)
			{
				glm::vec3 g = R * q[j] + y_cm;
				v[j] += (g - y[j]) / deltaTime * alpha;
				x[j] += v[j] * deltaTime;
				mesh->vertices[j].position = x[j];
			}
			});
	}
	for (auto& thread : threads)
		thread.join();
	std::vector<std::thread>().swap(threads);
}