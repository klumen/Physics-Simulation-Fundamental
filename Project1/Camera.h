#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr float FOV = 45.f;
constexpr float SPEED = 2.5f;
constexpr float SENSITIVITY = 0.1f;

enum CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
public:
	Camera(const glm::vec3& position = glm::vec3(0.f), float pitch = 0.f, float yaw = -90.f);
	Camera(float posX, float posY, float posZ, float pitch, float yaw);
	~Camera() = default;

	glm::mat4 get_view_matrix() const;
	void process_keyboard(CameraMovement direction, float deltaTime);
	void process_mouse_movement(float xOffset, float yOffset);

	glm::vec3 cameraX, cameraY, cameraZ;
	glm::vec3 position, front, up;
	float pitch, yaw;
	float fov;
	float moveSpeed, mouseSensitivity;

private:
	void update_orientation();

};

#endif // !CAMERA_H