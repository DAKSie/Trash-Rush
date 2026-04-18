#pragma once

#include <raylib.h>

class PlayerController {
public:
    PlayerController(float dimensionsX, float dimensionsY, float maxSpeed, float acceleration);
    void Reset();
    void SetMovementTuning(float newMaxSpeed, float newAcceleration);
    void ConfigureSkills(bool allowDash, float dashMultiplierValue, int dashDurationValue, int dashCooldownValue, bool allowInvincibility, int invincibilityDurationValue, int invincibilityCooldownValue);
    void Update();
    bool IsDashActive() const;
    bool IsDashEnabled() const;
    bool IsInvincibilityEnabled() const;
    bool IsInvincible() const;
    float GetDashCooldownProgress() const;
    float GetInvincibilityCooldownProgress() const;
    const Vector2 &GetPosition() const;
    const Vector2 &GetVelocity() const;
    Rectangle GetHitbox() const;

private:
    float dimensionsX;
    float dimensionsY;
    float maxSpeed;
    float acceleration;
    Vector2 position;
    Vector2 velocity;
    bool dashEnabled;
    float dashMultiplier;
    int dashDurationFrames;
    int dashCooldownFrames;
    int dashFramesRemaining;
    int dashCooldownRemaining;
    bool invincibilityEnabled;
    int invincibilityDurationFrames;
    int invincibilityCooldownFrames;
    int invincibilityFramesRemaining;
    int invincibilityCooldownRemaining;
};
