#include "Game.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

namespace {
    constexpr Color COLOR_2F2FE4{47, 47, 228, 255};
    constexpr Color COLOR_162E93{22, 46, 147, 255};
    constexpr Color COLOR_1A1953{26, 25, 83, 255};
    constexpr Color COLOR_080616{8, 6, 22, 255};
    constexpr std::array<FlameAttackPattern, 4> FLAME_ATTACK_PATTERN_ORDER = {
        FlameAttackPattern::Direct,
        FlameAttackPattern::Lead,
        FlameAttackPattern::Flank,
        FlameAttackPattern::Lead
    };

    Vector2 CenteredTextPosition(const Font& font, const std::string& text, float fontSize, float spacing, float y, float containerWidth) {
        Vector2 textSize = MeasureTextEx(font, text.c_str(), fontSize, spacing);
        return { (containerWidth - textSize.x) * 0.5f, y };
    }

    void DrawCenteredText(const Font& font, const std::string& text, float fontSize, float spacing, float y, float containerWidth, Color color) {
        DrawTextEx(font, text.c_str(), CenteredTextPosition(font, text, fontSize, spacing, y, containerWidth), fontSize, spacing, color);
    }
}

Game::Game()
    : score(0),
      highScore(0),
      gameOver(false),
      restartGame(false),
      returnToMainMenu(false),
      scoreChecker(0),
      isBurned(false),
      isMessy(false),
      selectedGameOverOption(0),
            activeDifficulty({MAX_SPEED, ACCELERATION, 1.8f, 5, 0.22f, 8.0f, TRASH_SPEED, SPAWN_INTERVAL, 5, 160, true, 2.0f, 16, 78, true, 50, 420, 560, 760, 300, 2, 3, 1.95f, 0.35f, 54.0f}),
            flamePatternFramesRemaining(0),
            flamePatternIndex(0),
      arcadeClassicFont({}),
      trashcan({}),
      trash({}),
      background({}),
      flame({}),
            shield({}),
      bgMusic({}),
      gameOverSound({}),
      pointSFX({}),
      highScoreStorage("Score.txt"),
      playerController(DIMENSIONS_X, DIMENSIONS_Y, MAX_SPEED, ACCELERATION),
      trashManager(DIMENSIONS_X, DIMENSIONS_Y, TRASH_SPEED, AVOIDANCE_RADIUS, SPAWN_INTERVAL),
            flameController(),
            dashTrailGhosts(),
            dashPulses(),
            shieldParticles(),
            specialFires(),
            visualFrameCounter(0),
            specialFireSpawnTimer(0),
            wasDashActiveLastFrame(false),
            specialFireSpawnWarningIntensity(0.0f),
            trashOverflowWarningIntensity(0.0f),
            currentUncollectedTrashCount(0) {}

void Game::SaveHighScore(int newScore) {
    highScore = highScoreStorage.SaveIfHigher(highScore, newScore);
}

int Game::LoadHighScore() {
    highScore = highScoreStorage.Load(highScore);
    return highScore;
}

void Game::DrawHudPanel() {
    Rectangle hudPanel = {18.0f, 18.0f, 420.0f, 142.0f};
    DrawRectangleRec(hudPanel, ColorAlpha(COLOR_080616, 0.34f));
    DrawRectangleLinesEx(hudPanel, 3.0f, ColorAlpha(COLOR_2F2FE4, 0.75f));
}

void Game::DrawHudText(Font font) {
    std::string scoreText = "Score: " + std::to_string(score);
    std::string highScoreText = "High Score: " + std::to_string(highScore);

    DrawTextEx(font, scoreText.c_str(), {38.0f, 34.0f}, 46.0f, 1.0f, RAYWHITE);
    DrawTextEx(font, highScoreText.c_str(), {38.0f, 86.0f}, 34.0f, 1.0f, ColorAlpha(COLOR_2F2FE4, 0.95f));
    DrawTextEx(font, "TRASH RUSH", {20.0f, 672.0f}, 28.0f, 1.0f, ColorAlpha(COLOR_080616, 0.16f));
}

void Game::DrawSkillCooldownHud(Font font) {
    Rectangle hud = {DIMENSIONS_X * 0.5f - 228.0f, DIMENSIONS_Y - 86.0f, 456.0f, 64.0f};
    DrawRectangleRec(hud, ColorAlpha(COLOR_080616, 0.34f));
    DrawRectangleLinesEx(hud, 2.0f, ColorAlpha(COLOR_2F2FE4, 0.46f));

    Rectangle dashCard = {hud.x + 10.0f, hud.y + 10.0f, 214.0f, 44.0f};
    Rectangle shieldCard = {hud.x + 232.0f, hud.y + 10.0f, 214.0f, 44.0f};

    DrawRectangleRec(dashCard, ColorAlpha(COLOR_1A1953, 0.55f));
    DrawRectangleRec(shieldCard, ColorAlpha(COLOR_1A1953, 0.55f));

    float dashProgress = playerController.GetDashCooldownProgress();
    float shieldProgress = playerController.GetInvincibilityCooldownProgress();

    Rectangle dashBar = {dashCard.x + 88.0f, dashCard.y + 27.0f, 116.0f * dashProgress, 8.0f};
    Rectangle shieldBar = {shieldCard.x + 88.0f, shieldCard.y + 27.0f, 116.0f * shieldProgress, 8.0f};
    DrawRectangle(dashCard.x + 88.0f, dashCard.y + 27.0f, 116, 8, ColorAlpha(RAYWHITE, 0.13f));
    DrawRectangle(shieldCard.x + 88.0f, shieldCard.y + 27.0f, 116, 8, ColorAlpha(RAYWHITE, 0.13f));
    DrawRectangleRec(dashBar, ColorAlpha(COLOR_2F2FE4, 0.95f));
    DrawRectangleRec(shieldBar, ColorAlpha(COLOR_2F2FE4, 0.95f));

    std::string dashStatus;
    if (!playerController.IsDashEnabled()) {
        dashStatus = "OFF";
    } else if (playerController.IsDashActive()) {
        dashStatus = "ACTIVE";
    } else if (dashProgress >= 1.0f) {
        dashStatus = "READY";
    } else {
        dashStatus = std::to_string(static_cast<int>(dashProgress * 100.0f + 0.5f)) + "%";
    }

    std::string shieldStatus;
    if (!playerController.IsInvincibilityEnabled()) {
        shieldStatus = "OFF";
    } else if (playerController.IsInvincible()) {
        shieldStatus = "ACTIVE";
    } else if (shieldProgress >= 1.0f) {
        shieldStatus = "READY";
    } else {
        shieldStatus = std::to_string(static_cast<int>(shieldProgress * 100.0f + 0.5f)) + "%";
    }

    DrawTextEx(font, "DASH", {dashCard.x + 8.0f, dashCard.y + 11.0f}, 20.0f, 1.0f, ColorAlpha(RAYWHITE, 0.95f));
    DrawTextEx(font, dashStatus.c_str(), {dashCard.x + 88.0f, dashCard.y + 8.0f}, 18.0f, 1.0f, ColorAlpha(RAYWHITE, 0.9f));
    DrawTextEx(font, "SHIFT", {dashCard.x + 157.0f, dashCard.y + 8.0f}, 16.0f, 1.0f, ColorAlpha(RAYWHITE, 0.55f));

    DrawTextEx(font, "SHIELD", {shieldCard.x + 8.0f, shieldCard.y + 11.0f}, 20.0f, 1.0f, ColorAlpha(RAYWHITE, 0.95f));
    DrawTextEx(font, shieldStatus.c_str(), {shieldCard.x + 88.0f, shieldCard.y + 8.0f}, 18.0f, 1.0f, ColorAlpha(RAYWHITE, 0.9f));
    DrawTextEx(font, "SPACE", {shieldCard.x + 157.0f, shieldCard.y + 8.0f}, 16.0f, 1.0f, ColorAlpha(RAYWHITE, 0.55f));
}

void Game::ResetGame() {
    score = 0;
    scoreChecker = 0;
    gameOver = false;
    isBurned = false;
    isMessy = false;
    selectedGameOverOption = 0;
    flamePatternFramesRemaining = 0;
    flamePatternIndex = 0;
    visualFrameCounter = 0;
    dashTrailGhosts.clear();
    dashPulses.clear();
    shieldParticles.clear();
    specialFires.clear();
    specialFireSpawnTimer = GetRandomValue(activeDifficulty.specialFireMinSpawnFrames, activeDifficulty.specialFireMaxSpawnFrames);
    wasDashActiveLastFrame = false;
    specialFireSpawnWarningIntensity = 0.0f;
    trashOverflowWarningIntensity = 0.0f;
    currentUncollectedTrashCount = 0;
    trashManager.Reset();
    playerController.Reset();
    flameController.Reset();
    flameController.SetSpeed(activeDifficulty.flameBaseSpeed);
    restartGame = false;
}

int Game::ShowMainMenu(Font font) {
    int selectedOption = 0;
    bool optionSelected = false;
    const std::array<std::string, 3> speedOptions = {"CASUAL", "STANDARD", "INTENSE"};
    float highlightY = 320.0f;

    while (!optionSelected && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(COLOR_1A1953);
        for (int y = 0; y < static_cast<int>(DIMENSIONS_Y); y += 4) {
            DrawLine(0, y, static_cast<int>(DIMENSIONS_X), y, ColorAlpha(COLOR_080616, 0.38f));
        }

        DrawCenteredText(font, "TRASH RUSH", 86.0f, 2.0f, 78.0f, DIMENSIONS_X, RAYWHITE);
        DrawCenteredText(font, "SELECT DIFFICULTY", 40.0f, 1.0f, 194.0f, DIMENSIONS_X, ColorAlpha(RAYWHITE, 0.93f));
        DrawCenteredText(font, "ARROW KEYS TO NAVIGATE  |  ENTER TO SELECT", 26.0f, 1.0f, 246.0f, DIMENSIONS_X, ColorAlpha(COLOR_2F2FE4, 0.95f));

        Rectangle optionsCard = {DIMENSIONS_X * 0.5f - 225.0f, 300.0f, 450.0f, 230.0f};
        DrawRectangleRec(optionsCard, ColorAlpha(COLOR_080616, 0.58f));
        DrawRectangleLinesEx(optionsCard, 3.0f, COLOR_2F2FE4);

        float targetHighlightY = optionsCard.y + 20.0f + static_cast<float>(selectedOption) * 68.0f;
        highlightY += (targetHighlightY - highlightY) * 0.23f;
        if (std::fabs(targetHighlightY - highlightY) < 0.5f) {
            highlightY = targetHighlightY;
        }

        Rectangle movingHighlight = {optionsCard.x + 24.0f, highlightY, optionsCard.width - 48.0f, 54.0f};
        DrawRectangleRec(movingHighlight, COLOR_2F2FE4);
        DrawRectangleLinesEx(movingHighlight, 2.0f, ColorAlpha(RAYWHITE, 0.3f));

        for (int i = 0; i < 3; i++) {
            Rectangle optionRow = {optionsCard.x + 24.0f, optionsCard.y + 20.0f + static_cast<float>(i) * 68.0f, optionsCard.width - 48.0f, 54.0f};
            const bool isSelected = selectedOption == i;

            DrawCenteredText(font, speedOptions[i], 34.0f, 1.0f, optionRow.y + 9.0f, DIMENSIONS_X, isSelected ? RAYWHITE : ColorAlpha(COLOR_2F2FE4, 0.9f));
        }

        if (IsKeyPressed(KEY_UP)) {
            selectedOption = (selectedOption - 1 + 3) % 3;
        } else if (IsKeyPressed(KEY_DOWN)) {
            selectedOption = (selectedOption + 1) % 3;
        } else if (IsKeyPressed(KEY_ENTER)) {
            optionSelected = true;
        }

        EndDrawing();
    }

    return selectedOption + 1;
}

void Game::ShowGameOverScreen(Font font) {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(COLOR_080616);
        for (int y = 0; y < static_cast<int>(DIMENSIONS_Y); y += 4) {
            DrawLine(0, y, static_cast<int>(DIMENSIONS_X), y, ColorAlpha(COLOR_1A1953, 0.45f));
        }

        Rectangle gameOverCard = {DIMENSIONS_X * 0.5f - 330.0f, DIMENSIONS_Y * 0.5f - 185.0f, 660.0f, 370.0f};
        DrawRectangleRec(gameOverCard, ColorAlpha(COLOR_1A1953, 0.9f));
        DrawRectangleLinesEx(gameOverCard, 3.0f, COLOR_2F2FE4);

        std::string gameOverText = "FINAL SCORE " + std::to_string(score);
        DrawCenteredText(font, "GAME OVER", 64.0f, 2.0f, gameOverCard.y + 42.0f, DIMENSIONS_X, RAYWHITE);
        DrawCenteredText(font, gameOverText, 38.0f, 1.0f, gameOverCard.y + 118.0f, DIMENSIONS_X, ColorAlpha(RAYWHITE, 0.9f));

        if (isMessy) {
            DrawCenteredText(font, "AREA OVERFLOW: TOO MUCH TRASH", 28.0f, 1.0f, gameOverCard.y + 164.0f, DIMENSIONS_X, COLOR_2F2FE4);
        } else if (isBurned) {
            DrawCenteredText(font, "YOU HAVE BEEN BURNED", 28.0f, 1.0f, gameOverCard.y + 164.0f, DIMENSIONS_X, COLOR_2F2FE4);
        }

        Rectangle playAgainButton = {gameOverCard.x + 140.0f, gameOverCard.y + 228.0f, gameOverCard.width - 280.0f, 52.0f};
        Rectangle exitButton = {gameOverCard.x + 140.0f, gameOverCard.y + 292.0f, gameOverCard.width - 280.0f, 52.0f};

        DrawRectangleRec(playAgainButton, selectedGameOverOption == 0 ? COLOR_2F2FE4 : ColorAlpha(COLOR_162E93, 0.9f));
        DrawRectangleRec(exitButton, selectedGameOverOption == 1 ? COLOR_2F2FE4 : ColorAlpha(COLOR_162E93, 0.9f));

        DrawCenteredText(font, "PLAY AGAIN", 31.0f, 1.0f, playAgainButton.y + 9.0f, DIMENSIONS_X, RAYWHITE);
        DrawCenteredText(font, "EXIT", 31.0f, 1.0f, exitButton.y + 9.0f, DIMENSIONS_X, RAYWHITE);

        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
            selectedGameOverOption = (selectedGameOverOption + 1) % 2;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (selectedGameOverOption == 0) {
                ShowMainMenu(font);
                break;
            }

            CloseWindow();
            std::exit(0);
        }

        EndDrawing();
    }
}

void Game::ApplyDifficulty(int selectedOption) {
    if (selectedOption == 1) {
        activeDifficulty = {
            10.8f,
            0.52f,
            1.55f,
            6,
            0.17f,
            7.2f,
            1.95f,
            2.35,
            7,
            200,
            true,
            2.0f,
            18,
            76,
            true,
            84,
            340,
            620,
            820,
            260,
            2,
            3,
            2.0f,
            0.45f,
            58.0f
        };
    } else if (selectedOption == 2) {
        activeDifficulty = {
            10.0f,
            0.5f,
            1.8f,
            5,
            0.22f,
            8.0f,
            2.2f,
            1.85,
            5,
            145,
            true,
            2.0f,
            15,
            82,
            true,
            50,
            400,
            540,
            730,
            280,
            3,
            4,
            2.15f,
            0.5f,
            54.0f
        };
    } else {
        activeDifficulty = {
            9.2f,
            0.44f,
            2.15f,
            4,
            0.25f,
            10.0f,
            2.0f,
            1.3,
            4,
            85,
            true,
            2.45f,
            18,
            95,
            true,
            36,
            560,
            460,
            620,
            320,
            3,
            5,
            2.35f,
            0.58f,
            50.0f
        };
    }

    playerController.SetMovementTuning(activeDifficulty.playerMaxSpeed, activeDifficulty.playerAcceleration);
    playerController.ConfigureSkills(
        activeDifficulty.dashEnabled,
        activeDifficulty.dashMultiplier,
        activeDifficulty.dashDurationFrames,
        activeDifficulty.dashCooldownFrames,
        activeDifficulty.invincibilityEnabled,
        activeDifficulty.invincibilityDurationFrames,
        activeDifficulty.invincibilityCooldownFrames
    );

    trashManager.SetTrashSpeed(activeDifficulty.trashSpeed);
    trashManager.SetSpawnInterval(activeDifficulty.spawnInterval);
    flameController.SetSpeed(activeDifficulty.flameBaseSpeed);
    flamePatternFramesRemaining = 0;
    flamePatternIndex = 0;
    specialFireSpawnTimer = GetRandomValue(activeDifficulty.specialFireMinSpawnFrames, activeDifficulty.specialFireMaxSpawnFrames);
}

void Game::UpdateVisualIndicators() {
    visualFrameCounter++;

    bool dashActive = playerController.IsDashActive();

    if (dashActive && !wasDashActiveLastFrame) {
        dashPulses.push_back({playerController.GetPosition(), 18.0f, 1.0f});
    }
    wasDashActiveLastFrame = dashActive;

    if (dashActive) {
        dashTrailGhosts.push_back({playerController.GetPosition(), 1.0f});
    }

    for (auto &ghost : dashTrailGhosts) {
        ghost.life -= 0.08f;
    }

    for (auto &pulse : dashPulses) {
        pulse.radius += 7.2f;
        pulse.life -= 0.09f;
    }

    dashTrailGhosts.erase(
        std::remove_if(
            dashTrailGhosts.begin(),
            dashTrailGhosts.end(),
            [](const DashTrailGhost &ghost) {
                return ghost.life <= 0.0f;
            }),
        dashTrailGhosts.end()
    );

    dashPulses.erase(
        std::remove_if(
            dashPulses.begin(),
            dashPulses.end(),
            [](const DashPulse &pulse) {
                return pulse.life <= 0.0f;
            }),
        dashPulses.end()
    );

    if (playerController.IsInvincible() && visualFrameCounter % 3 == 0) {
        float angle = static_cast<float>(GetRandomValue(0, 360)) * DEG2RAD;
        float radius = static_cast<float>(GetRandomValue(34, 46));
        float speed = static_cast<float>(GetRandomValue(6, 16)) * 0.01f;
        shieldParticles.push_back({angle, radius, speed, 1.0f});
    }

    for (auto &particle : shieldParticles) {
        particle.angle += particle.speed;
        if (playerController.IsInvincible()) {
            particle.life -= 0.04f;
        } else {
            particle.life -= 0.12f;
        }
    }

    shieldParticles.erase(
        std::remove_if(
            shieldParticles.begin(),
            shieldParticles.end(),
            [](const ShieldParticle &particle) {
                return particle.life <= 0.0f;
            }),
        shieldParticles.end()
    );
}

void Game::DrawVisualIndicators() {
    for (const auto &ghost : dashTrailGhosts) {
        float ghostScale = 0.04f * (0.94f + ghost.life * 0.32f);
        DrawTextureEx(trashcan, ghost.position, 0.0f, ghostScale, ColorAlpha(COLOR_2F2FE4, ghost.life * 0.62f));
    }

    Vector2 playerCenter = {playerController.GetPosition().x + 30.0f, playerController.GetPosition().y + 35.0f};
    for (const auto &pulse : dashPulses) {
        DrawRing(playerCenter, pulse.radius - 3.0f, pulse.radius + 2.0f, 0.0f, 360.0f, 36, ColorAlpha(RAYWHITE, pulse.life * 0.38f));
        DrawRing(playerCenter, pulse.radius + 4.0f, pulse.radius + 9.0f, 0.0f, 360.0f, 32, ColorAlpha(COLOR_2F2FE4, pulse.life * 0.56f));
    }

    float pulse = 0.94f + 0.07f * std::sin(static_cast<float>(visualFrameCounter) * 0.18f);
    if (playerController.IsInvincible() || !shieldParticles.empty()) {
        Rectangle source = {0.0f, 0.0f, static_cast<float>(shield.width), static_cast<float>(shield.height)};
        Rectangle dest = {playerCenter.x, playerCenter.y, 124.0f * pulse, 124.0f * pulse};
        Vector2 origin = {dest.width * 0.5f, dest.height * 0.5f};
        float shieldAlpha = playerController.IsInvincible() ? 0.62f : 0.25f;
        DrawTexturePro(shield, source, dest, origin, static_cast<float>(visualFrameCounter) * 0.8f, ColorAlpha(RAYWHITE, shieldAlpha));
    }

    for (const auto &particle : shieldParticles) {
        Vector2 p = {
            playerCenter.x + std::cos(particle.angle) * particle.radius,
            playerCenter.y + std::sin(particle.angle) * particle.radius
        };
        float particleSize = 2.0f + 2.3f * particle.life;
        DrawCircleV(p, particleSize, ColorAlpha(COLOR_2F2FE4, particle.life * 0.9f));
    }
}

Vector2 Game::RandomFireSpawnPosition() const {
    int side = GetRandomValue(0, 3);
    if (side == 0) {
        return {0.0f, static_cast<float>(GetRandomValue(40, static_cast<int>(DIMENSIONS_Y - 40.0f)))};
    }
    if (side == 1) {
        return {DIMENSIONS_X - 56.0f, static_cast<float>(GetRandomValue(40, static_cast<int>(DIMENSIONS_Y - 40.0f)))};
    }
    if (side == 2) {
        return {static_cast<float>(GetRandomValue(40, static_cast<int>(DIMENSIONS_X - 40.0f))), 0.0f};
    }
    return {static_cast<float>(GetRandomValue(40, static_cast<int>(DIMENSIONS_X - 40.0f))), DIMENSIONS_Y - 56.0f};
}

void Game::SpawnSpecialFireWave() {
    int spawnCount = GetRandomValue(activeDifficulty.specialFireMinCount, activeDifficulty.specialFireMaxCount);
    const auto &trashes = trashManager.GetTrashes();
    bool hasTrash = false;
    for (const auto &trashObj : trashes) {
        if (!trashObj.collected) {
            hasTrash = true;
            break;
        }
    }

    for (int i = 0; i < spawnCount; i++) {
        bool guardTrash = hasTrash && (GetRandomValue(0, 100) <= static_cast<int>(activeDifficulty.specialFireGuardChance * 100.0f));
        SpecialFire specialFire = {
            RandomFireSpawnPosition(),
            activeDifficulty.specialFireSpeed,
            activeDifficulty.specialFireLifetimeFrames,
            guardTrash,
            static_cast<float>(GetRandomValue(0, 360)) * DEG2RAD
        };
        specialFires.push_back(specialFire);
    }
}

void Game::UpdateSpecialFires() {
    specialFireSpawnTimer--;
    if (specialFireSpawnTimer <= 0) {
        SpawnSpecialFireWave();
        specialFireSpawnTimer = GetRandomValue(activeDifficulty.specialFireMinSpawnFrames, activeDifficulty.specialFireMaxSpawnFrames);
    }

    const auto &trashes = trashManager.GetTrashes();
    Vector2 playerPosition = playerController.GetPosition();
    Vector2 playerVelocity = playerController.GetVelocity();

    for (auto &specialFire : specialFires) {
        Vector2 target = {playerPosition.x + playerVelocity.x * 8.0f, playerPosition.y + playerVelocity.y * 8.0f};

        if (specialFire.guardTrash) {
            const Trash *closestTrash = nullptr;
            float closestDistance = 1000000.0f;
            for (const auto &trashObj : trashes) {
                if (trashObj.collected) {
                    continue;
                }

                float dx = trashObj.position.x - specialFire.position.x;
                float dy = trashObj.position.y - specialFire.position.y;
                float distanceSq = dx * dx + dy * dy;
                if (distanceSq < closestDistance) {
                    closestDistance = distanceSq;
                    closestTrash = &trashObj;
                }
            }

            if (closestTrash != nullptr) {
                specialFire.orbitAngle += 0.055f;
                target.x = closestTrash->position.x + std::cos(specialFire.orbitAngle) * 78.0f;
                target.y = closestTrash->position.y + std::sin(specialFire.orbitAngle) * 78.0f;
            }
        }

        float dx = target.x - specialFire.position.x;
        float dy = target.y - specialFire.position.y;
        float magnitude = std::sqrt(dx * dx + dy * dy);
        if (magnitude > 0.0f) {
            dx /= magnitude;
            dy /= magnitude;
        }

        specialFire.position.x += dx * specialFire.speed;
        specialFire.position.y += dy * specialFire.speed;
        specialFire.lifetimeFrames--;
    }

    for (size_t i = 0; i < specialFires.size(); i++) {
        for (size_t j = i + 1; j < specialFires.size(); j++) {
            float dx = specialFires[j].position.x - specialFires[i].position.x;
            float dy = specialFires[j].position.y - specialFires[i].position.y;
            float distanceSq = dx * dx + dy * dy;
            if (distanceSq <= 0.0f) {
                continue;
            }

            float distance = std::sqrt(distanceSq);
            float minDistance = activeDifficulty.specialFireSeparationDistance;
            if (distance < minDistance) {
                float overlap = (minDistance - distance) * 0.5f;
                float nx = dx / distance;
                float ny = dy / distance;
                specialFires[i].position.x -= nx * overlap;
                specialFires[i].position.y -= ny * overlap;
                specialFires[j].position.x += nx * overlap;
                specialFires[j].position.y += ny * overlap;
            }
        }
    }

    Vector2 mainFlamePosition = flameController.GetPosition();
    for (auto &specialFire : specialFires) {
        float dx = specialFire.position.x - mainFlamePosition.x;
        float dy = specialFire.position.y - mainFlamePosition.y;
        float distanceSq = dx * dx + dy * dy;
        if (distanceSq > 0.0f) {
            float distance = std::sqrt(distanceSq);
            float minDistance = activeDifficulty.specialFireSeparationDistance * 0.85f;
            if (distance < minDistance) {
                float overlap = (minDistance - distance);
                specialFire.position.x += (dx / distance) * overlap;
                specialFire.position.y += (dy / distance) * overlap;
            }
        }

        specialFire.position.x = std::fmax(0.0f, std::fmin(specialFire.position.x, DIMENSIONS_X - 56.0f));
        specialFire.position.y = std::fmax(0.0f, std::fmin(specialFire.position.y, DIMENSIONS_Y - 62.0f));
    }

    specialFires.erase(
        std::remove_if(
            specialFires.begin(),
            specialFires.end(),
            [](const SpecialFire &specialFire) {
                return specialFire.lifetimeFrames <= 0;
            }),
        specialFires.end()
    );
}

void Game::DrawSpecialFires() const {
    for (const auto &specialFire : specialFires) {
        float fade = static_cast<float>(specialFire.lifetimeFrames) / static_cast<float>(activeDifficulty.specialFireLifetimeFrames);
        if (fade < 0.25f) {
            fade = 0.25f;
        }
        float scale = specialFire.guardTrash ? 0.165f : 0.15f;
        Color tint = specialFire.guardTrash ? ColorAlpha(COLOR_2F2FE4, 0.92f * fade) : ColorAlpha(RAYWHITE, 0.84f * fade);
        DrawTextureEx(flame, specialFire.position, 0.0f, scale, tint);
    }
}

bool Game::CheckSpecialFireCollision(const Rectangle &playerHitbox) const {
    for (const auto &specialFire : specialFires) {
        Rectangle fireHitbox = {specialFire.position.x + 8.0f, specialFire.position.y + 8.0f, static_cast<float>(FLAME_WIDTH) * 0.13f, static_cast<float>(FLAME_HEIGHT) * 0.13f};
        if (CheckCollisionRecs(playerHitbox, fireHitbox)) {
            return true;
        }
    }
    return false;
}

void Game::UpdateWarningOverlays(int uncollectedTrashCount) {
    currentUncollectedTrashCount = uncollectedTrashCount;

    float warningWindow = static_cast<float>(std::max(95, activeDifficulty.specialFireMinSpawnFrames / 3));
    float spawnTarget = 0.0f;
    if (specialFireSpawnTimer <= static_cast<int>(warningWindow) && specialFireSpawnTimer >= 0) {
        spawnTarget = (warningWindow - static_cast<float>(specialFireSpawnTimer)) / warningWindow;
    }
    if (!specialFires.empty() && spawnTarget < 0.38f) {
        spawnTarget = 0.38f;
    }
    if (spawnTarget > 1.0f) {
        spawnTarget = 1.0f;
    }
    specialFireSpawnWarningIntensity += (spawnTarget - specialFireSpawnWarningIntensity) * 0.1f;

    float trashRatio = static_cast<float>(uncollectedTrashCount) / static_cast<float>(activeDifficulty.maxUncollectedTrash);
    float trashTarget = 0.0f;
    if (trashRatio >= 0.62f) {
        trashTarget = (trashRatio - 0.62f) / 0.38f;
    }
    if (trashTarget > 1.0f) {
        trashTarget = 1.0f;
    }
    trashOverflowWarningIntensity += (trashTarget - trashOverflowWarningIntensity) * 0.11f;
}

void Game::DrawWarningOverlays(Font font) const {
    if (specialFireSpawnWarningIntensity > 0.01f) {
        float pulse = 0.74f + 0.26f * std::sin(static_cast<float>(visualFrameCounter) * 0.12f);
        float alpha = specialFireSpawnWarningIntensity * pulse * 0.35f;
        Color edgeColor = ColorAlpha({222, 58, 74, 255}, alpha);
        Color transparentEdge = ColorAlpha({222, 58, 74, 255}, 0.0f);

        DrawRectangleGradientH(0, 0, 168, static_cast<int>(DIMENSIONS_Y), edgeColor, transparentEdge);
        DrawRectangleGradientH(static_cast<int>(DIMENSIONS_X - 168.0f), 0, 168, static_cast<int>(DIMENSIONS_Y), transparentEdge, edgeColor);
        DrawRectangleGradientV(0, 0, static_cast<int>(DIMENSIONS_X), 112, edgeColor, transparentEdge);
        DrawRectangleGradientV(0, static_cast<int>(DIMENSIONS_Y - 112.0f), static_cast<int>(DIMENSIONS_X), 112, transparentEdge, edgeColor);
    }

    float trashRatio = static_cast<float>(currentUncollectedTrashCount) / static_cast<float>(activeDifficulty.maxUncollectedTrash);
    float barProgress = trashRatio;
    if (barProgress > 1.0f) {
        barProgress = 1.0f;
    }

    Rectangle panel = {DIMENSIONS_X - 308.0f, 22.0f, 286.0f, 68.0f};
    float pulse = 0.86f + 0.14f * std::sin(static_cast<float>(visualFrameCounter) * 0.1f);
    float urgency = trashOverflowWarningIntensity * pulse;

    DrawRectangleRec(panel, ColorAlpha(COLOR_080616, 0.28f + urgency * 0.24f));
    DrawRectangleLinesEx(panel, 2.0f, ColorAlpha({214, 103, 82, 255}, 0.2f + urgency * 0.47f));

    Rectangle barBg = {panel.x + 112.0f, panel.y + 38.0f, 156.0f, 9.0f};
    Rectangle barFill = {barBg.x, barBg.y, barBg.width * barProgress, barBg.height};
    DrawRectangleRec(barBg, ColorAlpha(RAYWHITE, 0.11f));
    Color barColor = {static_cast<unsigned char>(92 + barProgress * 154.0f), static_cast<unsigned char>(160 - barProgress * 88.0f), static_cast<unsigned char>(220 - barProgress * 178.0f), 255};
    DrawRectangleRec(barFill, ColorAlpha(barColor, 0.9f));

    std::string label = "TRASH LOAD";
    std::string value = std::to_string(currentUncollectedTrashCount) + "/" + std::to_string(activeDifficulty.maxUncollectedTrash);
    const float labelFontSize = 19.0f;
    const float valueFontSize = 20.0f;
    Vector2 labelSize = MeasureTextEx(font, label.c_str(), labelFontSize, 1.0f);
    Vector2 valueSize = MeasureTextEx(font, value.c_str(), valueFontSize, 1.0f);
    float valueX = barBg.x + barBg.width - valueSize.x;
    float minValueX = panel.x + 12.0f + labelSize.x + 12.0f;
    if (valueX < minValueX) {
        valueX = minValueX;
    }

    DrawTextEx(font, label.c_str(), {panel.x + 12.0f, panel.y + 10.0f}, labelFontSize, 1.0f, ColorAlpha(RAYWHITE, 0.82f + urgency * 0.12f));
    DrawTextEx(font, value.c_str(), {valueX, panel.y + 10.0f}, valueFontSize, 1.0f, ColorAlpha(RAYWHITE, 0.8f + urgency * 0.14f));
}

void Game::RunGameLoop() {
    while (!WindowShouldClose()) {
        int speed = ShowMainMenu(arcadeClassicFont);
        ApplyDifficulty(speed);

        if (!IsMusicStreamPlaying(bgMusic)) {
            PlayMusicStream(bgMusic);
        }

        ResetGame();

        while (!WindowShouldClose() && !returnToMainMenu) {
            UpdateMusicStream(bgMusic);
            BeginDrawing();
            ClearBackground(COLOR_080616);
            DrawTexture(background, 0, 0, COLOR_2F2FE4);

            if (!gameOver) {
                DrawHudPanel();

                playerController.Update();
                UpdateVisualIndicators();
                trashManager.UpdateAI(playerController.GetPosition());
                trashManager.SpawnIfNeeded(GetTime());

                if (flamePatternFramesRemaining <= 0) {
                    flameController.SetAttackPattern(FLAME_ATTACK_PATTERN_ORDER[flamePatternIndex]);
                    flamePatternIndex = (flamePatternIndex + 1) % static_cast<int>(FLAME_ATTACK_PATTERN_ORDER.size());
                    flamePatternFramesRemaining = activeDifficulty.attackPatternSwapFrames;
                }
                flamePatternFramesRemaining--;

                flameController.UpdateAI(playerController.GetPosition(), playerController.GetVelocity(), activeDifficulty.flameLeadFactor);
                UpdateSpecialFires();

                Rectangle trashcanHitbox = playerController.GetHitbox();
                Rectangle flameHitbox = {
                    flameController.GetPosition().x,
                    flameController.GetPosition().y,
                    static_cast<float>(FLAME_WIDTH) * FLAME_SCALE,
                    static_cast<float>(FLAME_HEIGHT) * FLAME_SCALE
                };

                trashManager.RemoveOutOfBounds();

                if (scoreChecker >= activeDifficulty.scorePerFlameRamp) {
                    scoreChecker = 0;
                    flameController.IncreaseSpeed(activeDifficulty.flameRampAmount);
                }

                int uncollectedTrashCount = trashManager.CountUncollected();
                UpdateWarningOverlays(uncollectedTrashCount);
                if (uncollectedTrashCount >= activeDifficulty.maxUncollectedTrash) {
                    gameOver = true;
                    StopMusicStream(bgMusic);
                    PlaySound(gameOverSound);
                    isMessy = true;
                }

                for (auto &trashObj : trashManager.GetTrashes()) {
                    if (!trashObj.collected) {
                        Rectangle trashHitbox = {trashObj.position.x, trashObj.position.y, 60.0f, 80.0f};
                        if (CheckCollisionRecs(trashHitbox, trashcanHitbox)) {
                            PlaySound(pointSFX);
                            trashObj.collected = true;
                            score++;
                            scoreChecker++;
                        }
                    }
                }

                if (!playerController.IsInvincible() && CheckCollisionRecs(trashcanHitbox, flameHitbox)) {
                    gameOver = true;
                    StopMusicStream(bgMusic);
                    PlaySound(gameOverSound);
                    isBurned = true;
                }

                if (!playerController.IsInvincible() && CheckSpecialFireCollision(trashcanHitbox)) {
                    gameOver = true;
                    StopMusicStream(bgMusic);
                    PlaySound(gameOverSound);
                    isBurned = true;
                }

                DrawVisualIndicators();
                DrawTextureEx(trashcan, playerController.GetPosition(), 0.0f, 0.04f, WHITE);
                DrawTextureEx(flame, flameController.GetPosition(), 0.0f, FLAME_SCALE, WHITE);
                DrawSpecialFires();
                trashManager.Draw(trash);
                DrawWarningOverlays(arcadeClassicFont);

                DrawHudText(arcadeClassicFont);
                DrawSkillCooldownHud(arcadeClassicFont);
            } else {
                SaveHighScore(score);
                ShowGameOverScreen(arcadeClassicFont);
                break;
            }

            EndDrawing();
        }

        if (returnToMainMenu) {
            returnToMainMenu = false;
        }
    }
}

void Game::Run() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(static_cast<int>(DIMENSIONS_X), static_cast<int>(DIMENSIONS_Y), "Trash Rush");
    SetTargetFPS(TARGET_FPS);
    InitAudioDevice();

    std::srand(static_cast<unsigned int>(std::time(0)));

    const std::array<const char*, 4> fontCandidates = {
        "C:/Windows/Fonts/segoeuib.ttf",
        "C:/Windows/Fonts/bahnschrift.ttf",
        "C:/Windows/Fonts/calibrib.ttf",
        "assets/ARCADECLASSIC.ttf"
    };

    for (const char* fontPath : fontCandidates) {
        if (!FileExists(fontPath)) {
            continue;
        }

        arcadeClassicFont = LoadFontEx(fontPath, 96, nullptr, 0);
        if (arcadeClassicFont.texture.id != 0) {
            break;
        }
    }

    if (arcadeClassicFont.texture.id == 0) {
        arcadeClassicFont = LoadFont("assets/ARCADECLASSIC.ttf");
    }

    SetTextureFilter(arcadeClassicFont.texture, TEXTURE_FILTER_BILINEAR);

    trashcan = LoadTexture("assets/trashcan.png");
    trash = LoadTexture("assets/trash.png");
    background = LoadTexture("assets/background.png");
    flame = LoadTexture("assets/flame.png");
    shield = LoadTexture("assets/shield.png");

    bgMusic = LoadMusicStream("assets/bgMusic.mp3");
    gameOverSound = LoadSound("assets/game over.wav");
    pointSFX = LoadSound("assets/pointSFX.wav");

    SetMusicVolume(bgMusic, 0.5f);
    PlayMusicStream(bgMusic);
    bgMusic.looping = true;

    highScore = LoadHighScore();
    RunGameLoop();

    UnloadMusicStream(bgMusic);
    UnloadSound(gameOverSound);
    UnloadSound(pointSFX);
    UnloadTexture(shield);
    CloseAudioDevice();
    CloseWindow();
}
