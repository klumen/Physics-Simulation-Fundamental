#include "RigidBody.h"

RigidBody::RigidBody(Mesh* mesh) : mesh(mesh), gravity(0.f, -9.8f, 0.f), mass(0.f), I_ref(0.f), v(0.f), w(0.f), x(0.f), q(1.f, 0.f, 0.f, 0.f), vDamping(0.2f), wDamping(0.3f), restitution(0.5f)
{
	if (mesh == nullptr)
	{
		std::cerr << "RIGIDBODY::NO_MESH" << std::endl;
		return;
	}

	float m = 0.001f;
	for (unsigned int i = 0; i < mesh->vertices.size(); i++)
	{
		mass += m;
		glm::vec3& r_i = mesh->vertices[i].position;
		float diag = m * (r_i[0] * r_i[0] + r_i[1] * r_i[1] + r_i[2] * r_i[2]);
		I_ref[0][0] += diag;
		I_ref[1][1] += diag;
		I_ref[2][2] += diag;
		I_ref[0][0] -= m * r_i[0] * r_i[0];
		I_ref[0][1] -= m * r_i[0] * r_i[1];
		I_ref[0][2] -= m * r_i[0] * r_i[2];
		I_ref[1][0] -= m * r_i[1] * r_i[0];
		I_ref[1][1] -= m * r_i[1] * r_i[1];
		I_ref[1][2] -= m * r_i[1] * r_i[2];
		I_ref[2][0] -= m * r_i[2] * r_i[0];
		I_ref[2][1] -= m * r_i[2] * r_i[1];
		I_ref[2][2] -= m * r_i[2] * r_i[2];
	}
}

RigidBody::~RigidBody()
{
	mesh = nullptr;
}

void RigidBody::update(float deltaTime, Shader& shader)
{
	if (!mesh)
		return;

	euler(deltaTime, shader);
}

void RigidBody::euler(float deltaTime, Shader& shader)
{
	v += deltaTime * gravity;
	v *= exp(-vDamping * deltaTime);
	w *= exp(-wDamping * deltaTime);

	collision(glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	collision(glm::vec3(-2.f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f));

	x += deltaTime * v;
	q += glm::quat(0.f, (deltaTime / 2.f) * w) * q;
	q = glm::normalize(q);

	shader.use();
	glm::mat4 model(1.f);
	model = glm::translate(model, x);
	model = model * glm::mat4_cast(q);
	shader.set_mat4("model", model);
}

void RigidBody::collision(const glm::vec3& P, const glm::vec3& N)
{
	std::vector<unsigned int> colList;
	glm::mat3 R = glm::mat3_cast(q);

	unsigned int numThreads = std::thread::hardware_concurrency();
	if (numThreads == 0)
		numThreads = 4;
	unsigned int verticesPerThread = mesh->vertices.size() / numThreads;
	std::vector<std::thread> threads;
	std::mutex detectMutex;

	for (unsigned int i = 0; i < numThreads; i++)
	{
		unsigned int start = i * verticesPerThread;
		unsigned int end = (i == numThreads - 1) ? mesh->vertices.size() : (i + 1) * verticesPerThread;

		threads.emplace_back([start, end, &R, &P, &N, &colList, &detectMutex, this]() {
			std::vector<unsigned int> colList_local;

			for (unsigned int j = start; j < end; j++)
			{
				glm::vec3& r_i = mesh->vertices[j].position;
				glm::vec3 x_i = R * r_i + x;
				glm::vec3 vector = x_i - P;
				if (glm::dot(vector, N) < 0)
					colList_local.push_back(j);
			}
			if (!colList_local.empty())
			{
				std::lock_guard<std::mutex> lock(detectMutex);
				colList.insert(colList.end(), colList_local.begin(), colList_local.end());
			}
			});
	}
	for (auto& thread : threads)
		thread.join();
	std::vector<std::thread>().swap(threads);

	if (colList.empty())
		return;

	glm::vec3 r_i(0.f);
	for (auto i : colList)
		r_i += mesh->vertices[i].position / float(colList.size());
	r_i = R * r_i;
	glm::vec3 v_i = v + glm::cross(w, r_i);
	if (glm::dot(v_i, N) > 0)
		return;

	glm::vec3 v_n = glm::dot(v_i, N) * N;
	glm::vec3 v_t = v_i - v_n;
	float a = std::max(1.f - restitution * (1.f + restitution) * glm::length(v_n) / glm::length(v_t), 0.f);
	v_n *= -restitution;
	v_t *= a;

	glm::mat3 I = R * I_ref * glm::transpose(R);
	glm::mat3 I_inverse = glm::inverse(I);
	glm::mat3 M_r_i = Math::get_cross_matrix(r_i);
	glm::mat3 K = glm::mat3(1.f / mass) - M_r_i * I_inverse * M_r_i;
	glm::vec3 j = glm::inverse(K) * (v_n + v_t - v_i);

	v += j / mass;
	w += I_inverse * (glm::cross(r_i, j));
}