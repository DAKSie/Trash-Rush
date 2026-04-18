#pragma once

#include <raylib.h>

enum class FlameAttackPattern {
    Direct,
    Lead,
    Flank
};

class FlameController {
public:
    FlameController();
    void Reset();
    void SetSpeed(float value);
    void SetAttackPattern(FlameAttackPattern pattern);
    void UpdateAI(const Vector2 &characterPosition, const Vector2 &characterVelocity, float leadFactor);
    void IncreaseSpeed(float value);
    const Vector2 &GetPosition() const;

private:
    Vector2 position;
    float speed;
    FlameAttackPattern attackPattern;
};
