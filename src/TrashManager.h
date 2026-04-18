#pragma once

#include <raylib.h>

#include <vector>

struct Trash {
    Vector2 position;
    bool collected;
};

class TrashManager {
public:
    TrashManager(float dimensionsX, float dimensionsY, float trashSpeed, float avoidanceRadius, double spawnInterval);
    void Reset();
    void SetTrashSpeed(float newTrashSpeed);
    void SetSpawnInterval(double newSpawnInterval);
    void UpdateAI(const Vector2 &characterPosition);
    void SpawnIfNeeded(double currentTime);
    void RemoveOutOfBounds();
    int CountUncollected() const;
    std::vector<Trash> &GetTrashes();
    const std::vector<Trash> &GetTrashes() const;
    void Draw(const Texture2D &trashTexture) const;

private:
    float dimensionsX;
    float dimensionsY;
    float trashSpeed;
    float avoidanceRadius;
    double spawnInterval;
    double lastTrashSpawnTime;
    std::vector<Trash> trashes;
};
