#include "Camera.h"

namespace CG
{
    void Camera::updateVectors(const glm::quat deltaOrientation)
    {
        glm::vec3 direction = pos - target;
        glm::vec3 rotatedDirection = deltaOrientation * direction;

        pos = rotatedDirection + target;
        up = deltaOrientation * up;
        LookAt();
        
    }
    Camera::Camera()
    {
        projection = glm::mat4(1.0);
        view = glm::mat4(1.0);

        fov = 80.0f;
        aspect = 4.0f / 3.0f;
        clipNear = 0.01f;
        clipFar = 1000.0f;

        UpdateProjectionMatrix();
    }

    Camera::~Camera()
    {

    }

    void Camera::LookAt()
    {
        view = glm::lookAt(pos, target, up);
    }

    void Camera::LookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 _up)
    {
        pos = eye;
        target = center;
        up = _up;

        LookAt();
    }

    void Camera::rotateAround(float angle, glm::vec3 axis)
    {
        // Because the camara position use its current position as reference point, so it need to use deltaOrientation to to transform.
        // While the camara up vector
        glm::quat deltaOrientation = glm::angleAxis(glm::radians(angle), axis);

        updateVectors(deltaOrientation);
    }


    void Camera::setPos(glm::vec3 newPos)
    {
        pos = newPos;
        LookAt();
    }

    void Camera::setTarget(glm::vec3 newTarget)
    {
        target = newTarget;
        LookAt();
    }

    void Camera::setUpVector(glm::vec3 newUp)
    {
        up = newUp;
        LookAt();
    }

    void Camera::UpdateProjectionMatrix()
    {
        projection = glm::perspective(glm::radians(fov), aspect, clipNear, clipFar);
    }
}