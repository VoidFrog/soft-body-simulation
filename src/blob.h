#pragma once
#include <vector>
#include "raylib.h"

struct BlobParams
{
    int pointCount = 48;
    int iterations = 4;
    float segmentLen = 10.0f;
    float gravity = 1500.0f;
    float damping = 0.999f;
    float friction = 0.02f;
    float pressure = 1.0f;
    float bendStiffness = 0.2f;
    Vector3 baseColor = {0.35f, 0.9f, 0.55f};
};

struct Blob
{
    BlobParams params;
    std::vector<Vector2> pos;
    std::vector<Vector2> prev;
    std::vector<Vector2> grads;

    Vector2 grabTarget = {0, 0};
    int grabbed = -1;
    float restArea = 0.0f;
    float bendLen = 0.0f;

    void init(Vector2 center);
    void step(float dt);
    void collide(Rectangle bounds);
    void solveConstraints();
    float area() const;
    void solvePressure();
    void grab(Vector2 mouse);
    void release();
    void applyGrab();
};