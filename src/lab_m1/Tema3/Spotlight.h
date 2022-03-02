#pragma once
#include "utils/glm_utils.h"

class Spotlight {
public:
	glm::mat4 m_modelMatrix;
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 color;
	glm::vec3 lightDirection;

	float spotAngle;
	glm::vec3 axisOfRotation;

	float cummulatedAngle = 0.f;
	float semnTermen = 1.f;

	Spotlight(glm::vec3 position, glm::vec3 scale, glm::vec3 color, glm::vec3 lightDirection, float spotAngle, glm::vec3 axisOfRotation);

	void Update(float toBeAdded);
};