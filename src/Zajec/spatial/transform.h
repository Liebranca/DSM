#ifndef TRANSFORM_H
#define TRANSFORM_H

//  - --- - --- - --- - --- -

#include "../../../include/glm/glm.hpp"
#include "../../../include/glm/gtx/quaternion.hpp"
#include "../lymath/vops.h"

#define IDENTITY glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

//  - --- - --- - --- - --- -
namespace zjc {
    // 3D transform class
    class Trans3D
    {
    public:
        Trans3D(const glm::vec3& pos = glm::vec3(0, 0, 0),
            const glm::quat& rot = glm::quat(1, 0, 0, 0),
            const glm::vec3& scale = glm::vec3(1, 1, 1)) {
            wPos = pos; wRot = rot; wScale = scale;
        }

        virtual ~Trans3D() {};

        inline glm::mat4 getModel(bool ignoreParent = false) const;
        inline glm::mat4 getNormal(bool ignoreParent = false) const;

//      - --- - --- - --- - --- -

        inline glm::vec3& worldPosition() { return wPos; }
        inline glm::quat& worldOrientation() { return wRot; }
        inline glm::vec3& worldScale() { return wScale; }

        void setParent(Trans3D* parent) { this->parent = parent; }

//      - --- - --- - --- - --- -

            // Overcomplicated func to have an object face towards another.
        bool faceTo(glm::vec3 v, float elapsed, float rfac = 10.0f, bool fullRot = false);

        void rotate(glm::quat twat);
        void move(glm::vec3& pos) { wPos += pos; }

        glm::vec3 localizePos(glm::vec3& mvec, glm::vec3 fvec, glm::vec3 lvec);

//      - --- - --- - --- - --- -

    private:
        Trans3D* parent = nullptr;

        glm::vec3 wPos;
        glm::quat wRot;
        glm::vec3 wScale;

        glm::vec3 yAxis = glm::vec3(0, 1, 0);
        glm::vec2 dirn = glm::vec2(1, 1);

        float fwd = 0.0f;
        float tick = 0.0f;

    };
}

#endif // TRANSFORM_H