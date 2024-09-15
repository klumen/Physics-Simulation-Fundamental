#include "Math.h"

glm::mat3 Math::get_cross_matrix(const glm::vec3& v)
{
	glm::mat3 M(0.f);

	M[0][1] = -v[2];
	M[0][2] = v[1];
	M[1][0] = v[2];
	M[1][2] = -v[0];
	M[2][0] = -v[1];
	M[2][1] = v[0];

	return M;
}

glm::mat3 Math::polar_decomposition(const glm::mat3& A)
{
	glm::mat3 C = A * glm::transpose(A);
	glm::mat3 C2 = C * C;
	float A_det = glm::determinant(A);

	float I_C = C[0][0] + C[1][1] + C[2][2];
	float II_C = 0.5f * (I_C * I_C - (C2[0][0] + C2[1][1] + C2[2][2]));
	float III_C = A_det * A_det;

	glm::mat3 U_inv(0.f);
	float k = I_C * I_C - 3.f * II_C;
	if (k < 1e-6f)
	{
		float lambda = sqrtf(I_C / 3.f);

		U_inv[0][0] = U_inv[1][1] = U_inv[2][2] = 1.f / lambda;
	}
	else
	{
		float l = I_C * I_C * (I_C - 4.5f * II_C) + 13.5f * III_C;
		float k_root = sqrtf(k);
		float value = std::clamp(l / (k * k_root), -1.f, 1.f);
		float phi = acosf(value);
		float lambda2 = (I_C + 2 * k_root * cosf(phi / 3.f)) / 3.f;
		float lambda = sqrtf(lambda2);

		float III_U = sqrtf(III_C);
		if (A_det < 0.f) III_U = -III_U;
		float I_U = lambda + sqrtf((-lambda2 + I_C + 2.f * III_U / lambda));
		float II_U = (I_U * I_U - I_C) * 0.5f;

		glm::mat3 U = 1.f / (I_U * II_U - III_U) * (glm::mat3(I_U * III_U) + (I_U * I_U - II_U) * C - C2);
		U_inv = 1.f / III_U * (glm::mat3(II_U) - I_U * U + C);
	}

	return U_inv * A;
}