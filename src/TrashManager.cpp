#include "TrashManager.h"

#include <cmath>

TrashManager::TrashManager(float dimensionsX, float dimensionsY, float trashSpeed, float avoidanceRadius, double spawnInterval)
    : dimensionsX(dimensionsX),
      dimensionsY(dimensionsY),
      trashSpeed(trashSpeed),
      avoidanceRadius(avoidanceRadius),
      spawnInterval(spawnInterval),
      lastTrashSpawnTime(0.0) {}

void TrashManager::Reset() {
    trashes.clear();
    lastTrashSpawnTime = GetTime();
}

void TrashManager::SetTrashSpeed(float newTrashSpeed) {
    trashSpeed = newTrashSpeed;
}

void TrashManager::SetSpawnInterval(double newSpawnInterval) {
    spawnInterval = newSpawnInterval;
}

void TrashManager::UpdateAI(const Vector2 &characterPosition) {
    for (auto &currentTrash : trashes) {
        if (!currentTrash.collected) {
            float dx = characterPosition.x - currentTrash.position.x;
            float dy = characterPosition.y - currentTrash.position.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance < avoidanceRadius) {
                dx /= distance;
                dy /= distance;
                currentTrash.position.x -= dx * trashSpeed;
                currentTrash.position.y -= dy * trashSpeed;
            }
        }
    }
}

void TrashManager::SpawnIfNeeded(double currentTime) {
    if (currentTime - lastTrashSpawnTime >= spawnInterval) {
        trashes.push_back({{static_cast<float>(GetRandomValue(50, static_cast<int>(dimensionsX - 50.0f))),
                           static_cast<float>(GetRandomValue(50, static_cast<int>(dimensionsY - 50.0f)))},
                          false});
        lastTrashSpawnTime = currentTime;
    }
}

void TrashManager::RemoveOutOfBounds() {
    for (auto it = trashes.begin(); it != trashes.end();) {
        if (it->position.x < 0.0f || it->position.x > dimensionsX || it->position.y < 0.0f || it->position.y > dimensionsY) {
            it = trashes.erase(it);
        } else {
            ++it;
        }
    }
}

int TrashManager::CountUncollected() const {
    int uncollectedTrashCount = 0;
    for (const auto &trashObj : trashes) {
        if (!trashObj.collected) {
            uncollectedTrashCount++;
        }
    }
    return uncollectedTrashCount;
}

std::vector<Trash> &TrashManager::GetTrashes() {
    return trashes;
}

const std::vector<Trash> &TrashManager::GetTrashes() const {
    return trashes;
}

void TrashManager::Draw(const Texture2D &trashTexture) const {
    for (const auto &trashObj : trashes) {
        if (!trashObj.collected) {
            DrawTextureEx(trashTexture, trashObj.position, 0.0f, 0.2f, WHITE);
        }
    }
}
