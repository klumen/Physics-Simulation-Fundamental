#ifndef WAVE_H
#define WAVE_H

#include "Global.h"
#include "Mesh.h"

class Wave
{
public:
	Wave();
	~Wave() = default;

	void update(float deltaTime, Shader& shader);

	Mesh* mesh = nullptr;
	float rate = 0.005f;
	float damping = 0.996f;

	bool createWave = false;

private:
	unsigned int size;
	std::vector<std::vector<float>> h_old;

	void shallow_wave(float deltaTime, Shader& shader);

};

#endif // !WAVE_H