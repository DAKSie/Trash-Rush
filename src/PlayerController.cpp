#include "PlayerController.h"

#include <cmath>

PlayerController::PlayerController(float dimensionsX, float dimensionsY, float maxSpeed, float acceleration)
    : dimensionsX(dimensionsX),
      dimensionsY(dimensionsY),
      maxSpeed(maxSpeed),
      acceleration(acceleration),
      position({dimensionsX / 2.0f, dimensionsY / 2.0f}),
      velocity({0.0f, 0.0f}),
      dashEnabled(false),
    dashMultiplier(2.0f),
      dashDurationFrames(0),
      dashCooldownFrames(0),
      dashFramesRemaining(0),
      dashCooldownRemaining(0),
      invincibilityEnabled(false),
      invincibilityDurationFrames(0),
      invincibilityCooldownFrames(0),
      invincibilityFramesRemaining(0),
      invincibilityCooldownRemaining(0) {}

void PlayerController::Reset() {
    position = {dimensionsX / 2.0f, dimensionsY / 2.0f};
    velocity = {0.0f, 0.0f};
    dashFramesRemaining = 0;
    dashCooldownRemaining = 0;
    invincibilityFramesRemaining = 0;
    invincibilityCooldownRemaining = 0;
}

void PlayerController::SetMovementTuning(float newMaxSpeed, float newAcceleration) {
    maxSpeed = newMaxSpeed;
    acceleration = newAcceleration;
}

void PlayerController::ConfigureSkills(bool allowDash, float dashMultiplierValue, int dashDurationValue, int dashCooldownValue, bool allowInvincibility, int invincibilityDurationValue, int invincibilityCooldownValue) {
    dashEnabled = allowDash;
    dashMultiplier = dashMultiplierValue;
    dashDurationFrames = dashDurationValue;
    dashCooldownFrames = dashCooldownValue;
    dashFramesRemaining = 0;
    dashCooldownRemaining = 0;

    invincibilityEnabled = allowInvincibility;
    invincibilityDurationFrames = invincibilityDurationValue;
    invincibilityCooldownFrames = invincibilityCooldownValue;
    invincibilityFramesRemaining = 0;
    invincibilityCooldownRemaining = 0;
}

void PlayerController::Update() {
    Vector2 movement = {0.0f, 0.0f};
    if (IsKeyDown(KEY_D)) movement.x += 1.0f;
    if (IsKeyDown(KEY_A)) movement.x -= 1.0f;
    if (IsKeyDown(KEY_W)) movement.y -= 1.0f;
    if (IsKeyDown(KEY_S)) movement.y += 1.0f;

    float magnitude = std::sqrt(movement.x * movement.x + movement.y * movement.y);
    if (magnitude != 0.0f) {
        movement.x /= magnitude;
        movement.y /= magnitude;
    }

    if (dashCooldownRemaining > 0) {
        dashCooldownRemaining--;
    }
    if (dashFramesRemaining > 0) {
        dashFramesRemaining--;
    }
    if (invincibilityCooldownRemaining > 0) {
        invincibilityCooldownRemaining--;
    }
    if (invincibilityFramesRemaining > 0) {
        invincibilityFramesRemaining--;
    }

    if (dashEnabled && magnitude > 0.0f && IsKeyPressed(KEY_LEFT_SHIFT) && dashCooldownRemaining == 0) {
        dashFramesRemaining = dashDurationFrames;
        dashCooldownRemaining = dashCooldownFrames;
        float dashSpeed = maxSpeed * dashMultiplier;
        velocity.x = movement.x * dashSpeed;
        velocity.y = movement.y * dashSpeed;
    }

    if (invincibilityEnabled && IsKeyPressed(KEY_SPACE) && invincibilityFramesRemaining == 0 && invincibilityCooldownRemaining == 0) {
        invincibilityFramesRemaining = invincibilityDurationFrames;
        invincibilityCooldownRemaining = invincibilityCooldownFrames;
    }

    velocity.x += movement.x * acceleration;
    velocity.y += movement.y * acceleration;

    float effectiveMaxSpeed = maxSpeed;
    if (dashFramesRemaining > 0) {
        effectiveMaxSpeed *= dashMultiplier;
    }

    float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (currentSpeed > effectiveMaxSpeed) {
        velocity.x = (velocity.x / currentSpeed) * effectiveMaxSpeed;
        velocity.y = (velocity.y / currentSpeed) * effectiveMaxSpeed;
    }

    if (movement.x == 0.0f) velocity.x *= 0.9f;
    if (movement.y == 0.0f) velocity.y *= 0.9f;

    position.x += velocity.x;
    position.y += velocity.y;

    position.x = std::fmax(0.0f, std::fmin(position.x, dimensionsX - 60.0f));
    position.y = std::fmax(0.0f, std::fmin(position.y, dimensionsY - 70.0f));
}

const Vector2 &PlayerController::GetPosition() const {
    return position;
}

bool PlayerController::IsDashActive() const {
    return dashFramesRemaining > 0;
}

bool PlayerController::IsDashEnabled() const {
    return dashEnabled;
}

bool PlayerController::IsInvincibilityEnabled() const {
    return invincibilityEnabled;
}

bool PlayerController::IsInvincible() const {
    return invincibilityFramesRemaining > 0;
}

float PlayerController::GetDashCooldownProgress() const {
    if (dashCooldownFrames <= 0) {
        return 1.0f;
    }

    float remaining = static_cast<float>(dashCooldownRemaining);
    float total = static_cast<float>(dashCooldownFrames);
    float progress = 1.0f - (remaining / total);
    if (progress < 0.0f) {
        return 0.0f;
    }
    if (progress > 1.0f) {
        return 1.0f;
    }
    return progress;
}

float PlayerController::GetInvincibilityCooldownProgress() const {
    if (invincibilityCooldownFrames <= 0) {
        return 1.0f;
    }

    float remaining = static_cast<float>(invincibilityCooldownRemaining);
    float total = static_cast<float>(invincibilityCooldownFrames);
    float progress = 1.0f - (remaining / total);
    if (progress < 0.0f) {
        return 0.0f;
    }
    if (progress > 1.0f) {
        return 1.0f;
    }
    return progress;
}

const Vector2 &PlayerController::GetVelocity() const {
    return velocity;
}

Rectangle PlayerController::GetHitbox() const {
    return {position.x, position.y, 60.0f, 70.0f};
}
