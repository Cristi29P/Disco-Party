#pragma once
#include "utils/glm_utils.h"

class FloorLight {
public:
	glm::vec3 position;
	glm::vec3 light_color;

	FloorLight(glm::vec3 position, glm::vec3 light_color);
};