#pragma once
#include "utils/glm_utils.h"
#include <vector>

class Walls {
public:
	glm::mat4 m_modelMatrix;
	glm::vec3 position;
	glm::vec3 scale;

	std::vector<glm::vec3> lightFromFloor;

	Walls(glm::vec3 position, glm::vec3 scale);
};