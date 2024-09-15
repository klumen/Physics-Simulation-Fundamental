#include "Camera.h"

Camera::Camera(const glm::vec3& position /*= glm::vec3(0.f)*/, float pitch /*= 0.f*/, float yaw /*= -90.f*/) : position(position), front(0.f, 0.f, -1.f), up(0.f, 1.f, 0.f), pitch(pitch), yaw(yaw), fov(FOV), moveSpeed(SPEED), mouseSensitivity(SENSITIVITY)
{
	update_orientation();
}

Camera::Camera(float posX, float posY, float posZ, float pitch, float yaw) : position(posX, posY, posZ), front(0.f, 0.f, -1.f), up(0.f, 1.f, 0.f), pitch(pitch), yaw(yaw), fov(FOV), moveSpeed(SPEED), mouseSensitivity(SENSITIVITY)
{
	update_orientation();
}

glm::mat4 Camera::get_view_matrix() const
{
	return glm::lookAt(position, position + front, up);
}

void Camera::process_keyboard(CameraMovement direction, float deltaTime)
{
	float distance = deltaTime * moveSpeed;
	if (direction == FORWARD)
		position += front * distance;
	if (direction == BACKWARD)
		position -= front * distance;
	if (direction == LEFT)
		position -= cameraX * distance;
	if (direction == RIGHT)
		position += cameraX * distance;
}

void Camera::process_mouse_movement(float xOffset, float yOffset)
{
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	pitch += yOffset;
	yaw += xOffset;

	if (pitch > 89.f)
		pitch = 89.f;
	if (pitch < -89.f)
		pitch = -89.f;

	update_orientation();
}

void Camera::update_orientation()
{
	glm::vec3 tfront;
	tfront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	tfront.y = sin(glm::radians(pitch));
	tfront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(tfront);
	cameraZ = -front;
	cameraX = glm::normalize(glm::cross(front, up));
	cameraY = glm::normalize(glm::cross(cameraZ, cameraX));
}