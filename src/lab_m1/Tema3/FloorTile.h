#pragma once
#include "utils/glm_utils.h"

class FloorTile {
public:
	glm::mat4 m_modelMatrix;
	glm::vec3 position;
	glm::vec3 scale;

	glm::vec3 emissive_color;

	FloorTile(glm::vec3 position, glm::vec3 scale, glm::vec3 emissive_color);
};