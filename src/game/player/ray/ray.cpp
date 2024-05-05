#include "ray.hpp"
#include "../../game.hpp"

Ray::Ray(glm::vec3 _position, glm::vec3 _direction, float _moveScalar):
    startPosition{_position},
    direction{glm::normalize(_direction)},
    moveScalar{_moveScalar} {}

Ray::~Ray() {}

bool Ray::tryBreakBlock(World& world, float maxDist) {
    glm::vec3 cPos = startPosition;
    glm::vec3 moveVec = direction * moveScalar;
    double distTraveled = 0;

    auto airBlockIndex = world.getBlockDBRef().getIndexByName("air");

    while(distTraveled < maxDist) {
        cPos += moveVec;
        distTraveled += moveScalar;
        // Check if the ray is in world bounds

        if (world.coordinatesInWorld(cPos)
         && !world.getBlockDBRef().isAir(world.getBlock(cPos).getIndex())) {
            world.setBlock(cPos, Block(airBlockIndex));
            return true;
        }
    }
    return false;
}

bool Ray::tryPlaceBlock(World& world, Block block, float maxDist) {
    double distTraveled = 0;

    using namespace glm;

    while(distTraveled < maxDist) {
        auto beforePos = startPosition + direction * glm::vec3(distTraveled - moveScalar);
        auto pos = startPosition + direction * glm::vec3(distTraveled);
        if (!world.coordinatesInWorld(pos)) continue;

        if (!world.getBlockDBRef().isAir(world.getBlock(pos).getIndex())) {
            world.setBlock(beforePos, block);
            return true;
        }

        distTraveled += moveScalar;
    }

    return false;
}