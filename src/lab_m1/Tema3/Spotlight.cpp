#include "lab_m1/Tema3/Spotlight.h"

Spotlight::Spotlight(glm::vec3 position, glm::vec3 scale, glm::vec3 color, glm::vec3 lightDirection, float spotAngle, glm::vec3 axisOfRotation) {
	this->position = position;
	this->scale = scale;
	this->color = color;
	this->lightDirection = lightDirection;

	this->spotAngle = spotAngle;
	this->axisOfRotation = axisOfRotation;

	m_modelMatrix = glm::mat4(1);
	m_modelMatrix = glm::translate(m_modelMatrix, position);
	m_modelMatrix = glm::scale(m_modelMatrix, scale);
}

void Spotlight::Update(float toBeAdded) {
	cummulatedAngle += toBeAdded;

	m_modelMatrix = glm::mat4(1);
	m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(position.x, 6.f, position.z));
	m_modelMatrix = glm::rotate(m_modelMatrix, cummulatedAngle, axisOfRotation);
	m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(0, -scale.y, 0));
	m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(tan(spotAngle), 1, tan(spotAngle)) * scale.y);
}