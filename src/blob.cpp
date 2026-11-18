#include "blob.h"
#include "raymath.h"

void Blob::init(Vector2 center)
{
    int n = params.pointCount;
    float radius = params.segmentLen / (2.0f * sinf(PI / n));
    bendLen = 2.0f * radius * sinf(2.0f * PI / n);

    // this is needed here so the function can be called many times to resize the blob
    pos.resize(n);
    prev.resize(n);
    grads.resize(n);

    for (int i = 0; i < n; i++)
    {
        float angle = (2.0f * PI / n) * i;
        pos[i] = center + Vector2{radius * cosf(angle), radius * sinf(angle)};
        prev[i] = pos[i]; //- Vector2{1, -20};
    }

    restArea = area();
}

void Blob::step(float dt)
{
    Vector2 acc = {0, params.gravity};
    float dt2 = dt * dt;

    for (size_t i = 0; i < pos.size(); i++)
    {
        Vector2 velocity = (pos[i] - prev[i]) * params.damping;
        prev[i] = pos[i];
        pos[i] = pos[i] + velocity + acc * dt2;
    }
}

void Blob::collide(Rectangle bounds)
{
    float left = bounds.x;
    float right = bounds.x + bounds.width;
    float top = bounds.y;
    float bottom = bounds.y + bounds.height;

    for (size_t i = 0; i < pos.size(); i++)
    {
        if (pos[i].y > bottom)
        {
            pos[i].y = bottom;
            prev[i].x += (pos[i].x - prev[i].x) * params.friction;
        }
        if (pos[i].y < top)
        {
            pos[i].y = top;
            prev[i].x += (pos[i].x - prev[i].x) * params.friction;
        }

        if (pos[i].x < left)
        {
            pos[i].x = left;
            prev[i].y += (pos[i].y - prev[i].y) * params.friction;
        }
        if (pos[i].x > right)
        {
            pos[i].x = right;
            prev[i].y += (pos[i].y - prev[i].y) * params.friction;
        }
    }
}

void Blob::solveConstraints()
{
    size_t n = pos.size();
    for (int i = 0; i < params.iterations; i++)
    {
        bool forward = (i % 2 == 0);
        for (size_t s = 0; s < n; s++)
        {
            size_t j = forward ? s : n - 1 - s;
            size_t k = (j + 1) % n;
            Vector2 dist = pos[k] - pos[j];

            float len = Vector2Length(dist);
            if (len < 0.0001f)
                continue;

            float diff = params.segmentLen - len;
            Vector2 correction = dist * 0.5f * diff / len;

            pos[j] = pos[j] - correction;
            pos[k] = pos[k] + correction;
        }
        for (size_t s = 0; s < n; s++)
        {
            size_t j = forward ? s : n - 1 - s;
            size_t k = (j + 2) % n;
            Vector2 dist = pos[k] - pos[j];

            float len = Vector2Length(dist);
            if (len < 0.0001f)
                continue;

            float diff = bendLen - len;
            Vector2 correction = dist * 0.5f * params.bendStiffness * diff / len;

            pos[j] = pos[j] - correction;
            pos[k] = pos[k] + correction;
        }

        solvePressure();
    }
}

float Blob::area() const
{
    float area = 0.0f;
    size_t n = pos.size();
    for (size_t i = 0; i < n; i++)
    {
        area += 0.5f * (pos[i].x * pos[(i + 1) % n].y - pos[(i + 1) % n].x * pos[i].y);
    }

    return area;
}

void Blob::solvePressure()
{
    size_t n = pos.size();
    float sum = 0.0f;

    for (size_t i = 0; i < n; i++)
    {
        Vector2 next = pos[(i + 1) % n];
        Vector2 prev = pos[(i + n - 1) % n];
        Vector2 e = next - prev;

        grads[i] = Vector2{e.y, -e.x} * 0.5f;
        sum += Vector2LengthSqr(grads[i]);
    }

    if (sum < 0.0001f)
        return;

    float lambda = (restArea * params.pressure - area()) / sum;

    for (size_t i = 0; i < n; i++)
    {
        pos[i] = pos[i] + grads[i] * lambda;
    }
}

void Blob::grab(Vector2 mouse)
{
    int closestIndex = -1;
    float lowestDistance = INFINITY;
    for (size_t i = 0; i < pos.size(); i++)
    {
        float distance = Vector2DistanceSqr(mouse, pos[i]);
        if (distance < lowestDistance)
        {
            lowestDistance = distance;
            closestIndex = i;
        }
    }

    if (lowestDistance > 40.0f * 40.0f)
        return;
    grabbed = closestIndex;
    grabTarget = mouse;
}

void Blob::release()
{
    grabbed = -1;
}

void Blob::applyGrab()
{
    if (grabbed != -1)
    {
        pos[grabbed] = grabTarget;
    }
}