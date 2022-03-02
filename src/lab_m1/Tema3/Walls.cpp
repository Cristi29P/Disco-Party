#include "lab_m1/Tema3/Walls.h"

Walls::Walls(glm::vec3 position, glm::vec3 scale) {
	this->position = position;
	this->scale = scale;

	m_modelMatrix = glm::mat4(1);
	m_modelMatrix = glm::translate(m_modelMatrix, position);
	m_modelMatrix = glm::scale(m_modelMatrix, scale);
}