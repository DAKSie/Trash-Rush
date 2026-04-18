#include "FlameController.h"

#include <cmath>

FlameController::FlameController()
    : position({200.0f, 200.0f}),
      speed(1.5f),
      attackPattern(FlameAttackPattern::Direct) {}

void FlameController::Reset() {
    position = {200.0f, 200.0f};
    speed = 1.5f;
    attackPattern = FlameAttackPattern::Direct;
}

void FlameController::SetSpeed(float value) {
    speed = value;
}

void FlameController::SetAttackPattern(FlameAttackPattern pattern) {
    attackPattern = pattern;
}

void FlameController::UpdateAI(const Vector2 &characterPosition, const Vector2 &characterVelocity, float leadFactor) {
    Vector2 target = characterPosition;
    if (attackPattern == FlameAttackPattern::Lead) {
        target.x += characterVelocity.x * leadFactor;
        target.y += characterVelocity.y * leadFactor;
    } else if (attackPattern == FlameAttackPattern::Flank) {
        float perpendicularX = -characterVelocity.y;
        float perpendicularY = characterVelocity.x;
        float perpendicularMagnitude = std::sqrt(perpendicularX * perpendicularX + perpendicularY * perpendicularY);

        if (perpendicularMagnitude > 0.0f) {
            perpendicularX /= perpendicularMagnitude;
            perpendicularY /= perpendicularMagnitude;
            target.x += perpendicularX * leadFactor * 22.0f;
            target.y += perpendicularY * leadFactor * 22.0f;
        } else {
            target.x += leadFactor * 15.0f;
        }
    }

    float dx = target.x - position.x;
    float dy = target.y - position.y;
    float magnitude = std::sqrt(dx * dx + dy * dy);
    if (magnitude != 0.0f) {
        dx /= magnitude;
        dy /= magnitude;
    }

    position.x += dx * speed;
    position.y += dy * speed;
}

void FlameController::IncreaseSpeed(float value) {
    speed += value;
}

const Vector2 &FlameController::GetPosition() const {
    return position;
}
