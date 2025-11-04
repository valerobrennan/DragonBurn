#include "RCS.h"

// Implementation namespace-internal config instance
namespace RCS {
    Config config;
}

static Vec2 lastPunch{0.f, 0.f};

void RCS::NormalizeAngles(Vec2& angles) {
    // Clamp pitch
    if (angles.x > 89.f) angles.x = 89.f;
    if (angles.x < -89.f) angles.x = -89.f;

    // Normalize yaw
    while (angles.y > 180.f) angles.y -= 360.f;
    while (angles.y < -180.f) angles.y += 360.f;
}

void RCS::UpdateAngles(const CEntity& local, Vec2& outAngles)
{
    const int shotsFired = local.Pawn.ShotsFired;

    if (shotsFired > 0)
    {
        // Compute corrected aiming angle
        const Vec2 viewAngles = local.Pawn.ViewAngle;
        const Vec2 aimPunch = local.Pawn.AimPunchAngle;
        Vec2 newAngles = {
            viewAngles.x + lastPunch.x - aimPunch.x * 2.0f,
            viewAngles.y + lastPunch.y - aimPunch.y * 2.0f
        };

        NormalizeAngles(newAngles);
        outAngles = newAngles;
        lastPunch = aimPunch;
    }
    else
    {
        lastPunch = Vec2{0.f, 0.f};
        outAngles = local.Pawn.ViewAngle;
    }

    // Optionally, refine with latest punch value from memory cache
    // Only if punch cache is valid (defensive check)
    if (local.Pawn.AimPunchCache.Count > 0 && local.Pawn.AimPunchCache.Count <= 0xFFFF) {
        Vec2 punchAngle;
        const uintptr_t offset = local.Pawn.AimPunchCache.Data +
            (local.Pawn.AimPunchCache.Count - 1) * sizeof(Vec3);
        if (memoryManager.ReadMemory<Vec2>(offset, punchAngle)) {
            outAngles = punchAngle;
        }
    }
}

void RCS::RecoilControl(const CEntity& local)
{
    if (!config.enabled)
        return;

    static Vec2 prevPunch{0.f, 0.f};

    if (local.Pawn.ShotsFired > config.bulletsToActivate)
    {
        Vec2 viewAngles = local.Pawn.ViewAngle;
        Vec2 punch = local.Pawn.AimPunchAngle * 2.0f;

        Vec2 delta = viewAngles + (prevPunch - punch);
        Vec2 scaledDelta = {
            delta.y * config.scale.x / local.Client.Sensitivity,
            delta.x * config.scale.y / local.Client.Sensitivity
        };

        int mouseX = static_cast<int>(std::round(scaledDelta.x / 0.011f));
        int mouseY = static_cast<int>(std::round(scaledDelta.y / 0.011f));

        if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000))
        {
            mouse_event(MOUSEEVENTF_MOVE, mouseX, -mouseY, NULL, NULL);
        }

        prevPunch = punch;
    }
    else
    {
        prevPunch = Vec2{0.f, 0.f};
    }
}
