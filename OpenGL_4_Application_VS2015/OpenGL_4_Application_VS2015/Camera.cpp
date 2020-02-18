//
//  Camera.cpp
//  Lab5
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#include "Camera.hpp"

namespace gps {

	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget)
	{
		this->cameraPosition = cameraPosition;
		this->cameraTarget = cameraTarget;
		this->cameraDirection = glm::normalize(cameraTarget - cameraPosition);
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
	}



	glm::mat4 Camera::getViewMatrix()
	{
		return glm::lookAt(cameraPosition, cameraPosition + cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f));
		//return glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraTarget);
	}

	void Camera::setCameraDirection(glm::vec3 cameraDirection)
	{
		this->cameraDirection = cameraDirection;
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	void Camera::setCameraPosition(glm::vec3 cameraPos)
	{
		this->cameraPosition = cameraPos;
	}

	glm::vec3 Camera::getCameraDirection()
	{
		return cameraDirection;
	}

	glm::vec3 Camera::getCameraPosition()
	{
		return cameraPosition;
	}

	glm::vec3 Camera::getCameraTarget()
	{
		return cameraTarget;
	}

	void Camera::move(MOVE_DIRECTION direction, float speed)
	{
		switch (direction) {
		case MOVE_FORWARD:
			cameraPosition += cameraDirection * speed;
			break;

		case MOVE_BACKWARD:
			cameraPosition -= cameraDirection * speed;
			break;

		case MOVE_RIGHT:
			cameraPosition += cameraRightDirection * speed;
			break;

		case MOVE_LEFT:
			cameraPosition -= cameraRightDirection * speed;
			break;
		case MOVE_UP:
			cameraPosition += glm::vec3(0.0f, 1.0f, 0.0f) * speed;
			break;
		case MOVE_DOWN:
			cameraPosition -= glm::vec3(0.0f, 1.0f, 0.0f) * speed;
			break;
		}
	}

	void Camera::rotate(float pitch, float yaw)
	{

	}

}
