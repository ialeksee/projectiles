#include <optional>
#define OLC_PGE_APPLICATION
#include <cassert>
#include "olcPixelGameEngine.h"

class Projectiles : public olc::PixelGameEngine
{
public:
    Projectiles()
    {
        sAppName = "Projectiles";
    }

    olc::vf2d vSource = { 160.0f, 300.0f };
    olc::vf2d vTarget = { 480.0f, 300.0f };

    float fMuzzleVelocity = 40.0f;
    float fGravity = 4.0f;

public: 
    bool OnUserCreate() override
    {
        return true;
    }

    std::optional<std::pair<float,float>> CalculateTrajectory(
        const olc::vf2d& source,
        const olc::vf2d& target,
        const float gravity,
        const float muzzle
    )
    {
        //displacement
        olc::vf2d D = target - source;
        float u = muzzle;
        float G = gravity;

        float a = (0.5f * G * (D.x / u) * (D.x / u));
        float b = D.x;
        float c = (0.5f * G * (D.x / u) * (D.x / u)) - D.y;
        
        float d = b * b - 4.0f * a * c;

        if(d < 0)
        {
            return std::nullopt;
        }

        float angle1 = atan((-b + sqrt(d)) / (2.0f * a));
        float angle2 = atan((-b - sqrt(d)) / (2.0f * a));

        //Preserve the sign of the direction
        float dir = (D.x >= 0 ? 0.0f : 3.14159f);

        return std::pair {dir + angle1, dir + angle2};
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        if(GetMouse(0).bHeld)
        {
            vSource = GetMousePos();
        }

        if(GetMouse(1).bHeld)
        {
            vTarget = GetMousePos();
        }
        
        if(GetMouseWheel() > 0)
            fMuzzleVelocity += 1.0f;
        if(GetMouseWheel() < 0)
            fMuzzleVelocity -= 1.0f;

        Clear(olc::VERY_DARK_MAGENTA);

        //Draw Source
        FillCircle(vSource, 10, olc::YELLOW);

        FillCircle(vTarget, 10, olc::CYAN);

        olc::vf2d vLine = (vTarget - vSource).norm();
        //last parameter is dashed pattern...
        DrawLine(vSource - vLine * 2000.0f, vSource + vLine * 2000.0f, olc::MAGENTA, 0xf0f0f0f0);

        auto angles = CalculateTrajectory(vSource, vTarget, fGravity, fMuzzleVelocity);

        if(!angles.has_value())
        {
            return true;
        }
        
        olc::vf2d p1_vel = olc::vf2d(fMuzzleVelocity, angles->first).cart();
        olc::vf2d p1_pos = vSource;
        olc::vf2d p2_vel = olc::vf2d(fMuzzleVelocity, angles->second).cart();
        olc::vf2d p2_pos = vSource;
        float dt = 0.1f;
        
        olc::vf2d vGravity = { 0.0f, fGravity};

        for(int n = 0; n < 1000; n++)
        {
            p1_pos = p1_vel * (n * dt) + 0.5f * vGravity * (n * dt) * (n * dt);
            p2_pos = p2_vel * (n * dt) + 0.5f * vGravity * (n * dt) * (n * dt);

            Draw(p1_pos + vSource, olc::GREEN);
            Draw(p2_pos + vSource, olc::WHITE);
        }

        return true;
    }
};

int main()
{
    Projectiles demo;
    if(demo.Construct(640, 480, 2, 2))
        demo.Start();
    return 0;
}
