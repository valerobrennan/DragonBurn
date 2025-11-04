#pragma once
#include "Aimbot.h"

// More explicit namespace, config, and documentation
namespace RCS
{
    // Recoil Control System configuration
    struct Config {
        int bulletsToActivate = 1;
        Vec2 scale = {1.4f, 1.4f};
        bool enabled = true;
    };

    // Allows configuration at runtime
    extern Config config;

    // Core RCS functionality
    void UpdateAngles(const CEntity& local, Vec2& outAngles);
    void RecoilControl(const CEntity& local);

    // Utility: Clamp and normalize angle
    void NormalizeAngles(Vec2& angles);
}
