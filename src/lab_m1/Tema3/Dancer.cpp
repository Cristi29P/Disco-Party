#include "lab_m1/Tema3/Dancer.h"

Dancer::Dancer(glm::vec3 initialPosition, glm::vec3 scale, glm::vec3 temp_destination, float speed) {
	this->initialPosition = initialPosition;
	this->scale = scale;
	this->temp_destination = temp_destination;
	this->speed = speed;

	this->temp_direction = glm::normalize(temp_destination - initialPosition);

	m_modelMatrix = glm::mat4(1);
	m_modelMatrix = glm::translate(m_modelMatrix, initialPosition);
	m_modelMatrix = glm::scale(m_modelMatrix, scale);
}

void Dancer::Update(float deltaTimeSeconds) {
	translateX += deltaTimeSeconds * temp_direction.x * speed;
	translateZ += deltaTimeSeconds * temp_direction.z * speed;

	// ModelMatrix update
	m_modelMatrix = glm::mat4(1);
	m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(translateX, 0, translateZ));
	m_modelMatrix = glm::translate(m_modelMatrix, initialPosition);
	m_modelMatrix = glm::scale(m_modelMatrix, scale);

	// Update internal position
	position = m_modelMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f);
	position.y = 0.f;
}