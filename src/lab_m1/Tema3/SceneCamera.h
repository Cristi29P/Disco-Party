#pragma once
#include "utils/glm_utils.h"

class SceneCamera
{
public:
	float distanceToTarget;
	glm::vec3 position;
	glm::vec3 forward;
	glm::vec3 right;
	glm::vec3 up;
	glm::mat4 projectionMatrix;

	SceneCamera(const glm::vec3 position, const glm::vec3 center, const float distanceToTarget, const glm::vec3 up, const glm::mat4 projectionMatrix)
	{
		Set(position, center, distanceToTarget, up, projectionMatrix);
	}

	void Set(const glm::vec3 position, const glm::vec3 center, const float distanceToTarget, const glm::vec3 up, const glm::mat4 projectionMatrix)
	{
		this->position = position;
		this->forward = glm::normalize(center - position);
		this->right = glm::cross(forward, up);
		this->distanceToTarget = distanceToTarget;
		this->up = glm::cross(right, forward);
		this->projectionMatrix = projectionMatrix;
	}

	void MoveForward(const float distance)
	{
		glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0.f, forward.z));
		position += dir * distance;
	}

	void TranslateUpward(const float distance)
	{
		position += glm::vec3(0.f, 1.f, 0.f) * distance;
	}

	void TranslateRight(const float distance)
	{
		glm::vec3 dir = glm::normalize(glm::vec3(right.x, 0.f, right.z));
		position += dir * distance;
	}

	void RotateFirstPerson_OX(const float angle)
	{
		forward = glm::rotate(glm::mat4(1.f), angle, right) * glm::vec4(glm::vec3(forward), 0.f);
		forward = glm::normalize(forward);

		up = glm::cross(right, forward);
	}

	void RotateFirstPerson_OY(const float angle)
	{
		forward = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0.f, 1.f, 0.f)) * glm::vec4(glm::vec3(forward), 0.f);
		forward = glm::normalize(forward);
		right = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0.f, 1.f, 0.f)) * glm::vec4(glm::vec3(right), 0.f);
		right = glm::normalize(right);
		up = glm::cross(right, forward);
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(position, position + forward, up);
	}
};
