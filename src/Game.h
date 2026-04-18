#pragma once

#include <raylib.h>

#include <vector>

#include "FlameController.h"
#include "HighScoreStorage.h"
#include "PlayerController.h"
#include "TrashManager.h"

class Game {
public:
    Game();
    void Run();

private:
    struct DashTrailGhost {
        Vector2 position;
        float life;
    };

    struct DashPulse {
        Vector2 center;
        float radius;
        float life;
    };

    struct ShieldParticle {
        float angle;
        float radius;
        float speed;
        float life;
    };

    struct SpecialFire {
        Vector2 position;
        float speed;
        int lifetimeFrames;
        bool guardTrash;
        float orbitAngle;
    };

    struct DifficultyProfile {
        float playerMaxSpeed;
        float playerAcceleration;
        float flameBaseSpeed;
        int scorePerFlameRamp;
        float flameRampAmount;
        float flameLeadFactor;
        float trashSpeed;
        double spawnInterval;
        int maxUncollectedTrash;
        int attackPatternSwapFrames;
        bool dashEnabled;
        float dashMultiplier;
        int dashDurationFrames;
        int dashCooldownFrames;
        bool invincibilityEnabled;
        int invincibilityDurationFrames;
        int invincibilityCooldownFrames;
        int specialFireMinSpawnFrames;
        int specialFireMaxSpawnFrames;
        int specialFireLifetimeFrames;
        int specialFireMinCount;
        int specialFireMaxCount;
        float specialFireSpeed;
        float specialFireGuardChance;
        float specialFireSeparationDistance;
    };

    static constexpr float DIMENSIONS_X = 1080.0f;
    static constexpr float DIMENSIONS_Y = 720.0f;
    static constexpr int TARGET_FPS = 60;
    static constexpr float MAX_SPEED = 10.0f;
    static constexpr float ACCELERATION = 0.5f;
    static constexpr float TRASH_SPEED = 2.0f;
    static constexpr float AVOIDANCE_RADIUS = 250.0f;
    static constexpr float FLAME_SCALE = 0.2f;
    static constexpr int FLAME_WIDTH = 322;
    static constexpr int FLAME_HEIGHT = 400;
    static constexpr double SPAWN_INTERVAL = 2.0;

    int score;
    int highScore;
    bool gameOver;
    bool restartGame;
    bool returnToMainMenu;

    int scoreChecker;
    bool isBurned;
    bool isMessy;
    int selectedGameOverOption;
    DifficultyProfile activeDifficulty;
    int flamePatternFramesRemaining;
    int flamePatternIndex;

    Font arcadeClassicFont;
    Texture2D trashcan;
    Texture2D trash;
    Texture2D background;
    Texture2D flame;
    Texture2D shield;
    Music bgMusic;
    Sound gameOverSound;
    Sound pointSFX;

    HighScoreStorage highScoreStorage;
    PlayerController playerController;
    TrashManager trashManager;
    FlameController flameController;
    std::vector<DashTrailGhost> dashTrailGhosts;
    std::vector<DashPulse> dashPulses;
    std::vector<ShieldParticle> shieldParticles;
    std::vector<SpecialFire> specialFires;
    int visualFrameCounter;
    int specialFireSpawnTimer;
    bool wasDashActiveLastFrame;
    float specialFireSpawnWarningIntensity;
    float trashOverflowWarningIntensity;
    int currentUncollectedTrashCount;

    void SaveHighScore(int newScore);
    int LoadHighScore();
    void DrawHudPanel();
    void DrawHudText(Font font);
    void DrawSkillCooldownHud(Font font);
    void ResetGame();
    int ShowMainMenu(Font font);
    void ShowGameOverScreen(Font font);
    void ApplyDifficulty(int selectedOption);
    void UpdateVisualIndicators();
    void DrawVisualIndicators();
    Vector2 RandomFireSpawnPosition() const;
    void SpawnSpecialFireWave();
    void UpdateSpecialFires();
    void DrawSpecialFires() const;
    bool CheckSpecialFireCollision(const Rectangle &playerHitbox) const;
    void UpdateWarningOverlays(int uncollectedTrashCount);
    void DrawWarningOverlays(Font font) const;
    void RunGameLoop();
};
