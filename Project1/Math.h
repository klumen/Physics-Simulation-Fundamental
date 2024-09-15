#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Math
{
public:
	Math() = default;
	~Math() = default;

	static glm::mat3 get_cross_matrix(const glm::vec3& v);
	static glm::mat3 polar_decomposition(const glm::mat3& A);

private:

};


#endif // !MATH_H