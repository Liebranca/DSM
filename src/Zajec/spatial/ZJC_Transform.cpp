#include "ZJC_Transform.h"

#include "../../../include/glm/gtx/transform.hpp"
#include "../../../include/glm/gtc/quaternion.hpp"
#include "../../../include/glm/gtc/matrix_inverse.hpp"

//  - --- - --- - --- - --- -

glm::mat4 TRANSFORM3D::getModel
                        (bool ignoreParent)     {

    glm::mat4 parentMatrix;
    glm::mat4 wPosMatrix   = glm::translate(position);
    glm::mat4 wRotMatrix   = glm::mat4_cast(glm::normalize(orientation));
    glm::mat4 wScaleMatrix = glm::scale(scaling);

    if (!parent || ignoreParent)                { parentMatrix = IDENTITY;                                              }
    else                                        { parentMatrix = parent->getModel();                                    }

    return parentMatrix * (wPosMatrix * wRotMatrix * wScaleMatrix);                                                     }

glm::mat3 TRANSFORM3D::getNormal
                        (glm::mat4& model)      { return glm::inverseTranspose(model);                                  }

//  - --- - --- - --- - --- -

bool TRANSFORM3D::faceTo(glm::vec3 v,
                 float elapsed,
                 float rfac,
                 bool fullRot)                  {

    glm::vec3 vecTo;
    if (!fullRot) { vecTo = -(xzvec(v) - xzvec(this->position)); }
    else { vecTo = -(v - this->position); }
    glm::quat delta = glm::quatLookAt(vecTo, Y_AXIS);
    glm::quat& rot = this->orientation;

    if (delta != rot)                            
    {
        this->tick += elapsed;

        float p = this->tick / rfac;
        rot = glm::slerp(rot, delta, p);
        this->rotate({ 1,0,0,0 });

        if (p * rfac * 10.0f > 0.8f)            { rot = delta;                                                          }

        return p * rfac * 10.0f > 0.3f;
    }

    this->tick = 0.0f;
    return true;                                                                                                        }

//  - --- - --- - --- - --- -

void TRANSFORM3D::rotate(glm::quat delta)       {

    this->orientation = glm::normalize(glm::cross(orientation, delta));

    fwd = (orientation.y * orientation.w) * 2;
    if (fabs(orientation.w) < 0.5) { dirn.y = -1; }
    else { dirn.y = 1; }

    if (fabs(orientation.y) > 0.75) { dirn.x = -1; }
    else { dirn.x = 1; }
                                                                                                                        }

//  - --- - --- - --- - --- -

