#include "lab_m1/Tema3/FloorTile.h"

FloorTile::FloorTile(glm::vec3 position, glm::vec3 scale, glm::vec3 emissive_color) {
	this->position = position;
	this->scale = scale;
	this->emissive_color = emissive_color;

	m_modelMatrix = glm::mat4(1);
	m_modelMatrix = glm::translate(m_modelMatrix, position);
	m_modelMatrix = glm::scale(m_modelMatrix, scale);
}