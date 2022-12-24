// 등가속도(포물선 계산)
// s = s0 + vt
// a = dv / dt    v = ds / dt
// s = s0 + v0t + 1a(t^2)/2

// velocity x: 10 m/s
// accel x: 0
// velocity y: 5 m/s
// accel y: -9.8 m/s^2

static float gravityAccel = 9.80665f;

struct PositionVelocity
{
    float4 pos;
    float4 velo;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;
    
    PositionVelocity curPosVelo;

    curPosVelo.pos.x = 0.0f;
    curPosVelo.pos.y = 0.0f;
    curPosVelo.pos.z = 0.0f;
    curPosVelo.pos.w = 0.0f;

    curPosVelo.velo.x = 10.0f;
    curPosVelo.velo.y = 5.0f;
    curPosVelo.velo.z = 1.0f;
    curPosVelo.velo.w = 0.0f;

    result.position = parabolicMotion(curPosVelo);
};

// 3 Dimension
float4 parabolicMotion(PositionVelocity input)
{
    float4 result;

    // Sx = s0 + 10m/s * t
    result.x = input.pos.x + input.velo.x * deltaTime;

    // Sy = s0 + 5m/s * t + (1/2) * (-9.8) * t^2
    result.y = input.pos.y + input.velo.y * deltaTime + (1/2) * gravityAccel * deltaTime * deltaTime;

    result.z = input.pos.z + input.velo.z * deltaTime;
    result.w = input.pos.w;

    return result;
}