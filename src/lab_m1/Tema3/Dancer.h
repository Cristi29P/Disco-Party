#pragma once
#include "utils/glm_utils.h"

class Dancer {
public:
	glm::mat4 m_modelMatrix;
	glm::vec3 position = glm::vec3(0.f);
	glm::vec3 initialPosition;
	glm::vec3 scale;

	glm::vec3 temp_direction;
	glm::vec3 temp_destination;

	float speed, translateX = 0.f, translateZ = 0.f;

	Dancer(glm::vec3 initialPosition, glm::vec3 scale, glm::vec3 temp_destination, float speed);

	void Update(float deltaTimeSeconds);
};