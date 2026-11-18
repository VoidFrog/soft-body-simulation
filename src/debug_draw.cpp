#include "debug_draw.h"

void drawBlobDebug(const Blob &blob)
{
    size_t n = blob.pos.size();
    Color color = {90, 230, 90, 255};
    Color jointColor = {190, 230, 210, 255};
    for (size_t i = 0; i < n; i++)
    {
        DrawLineV(blob.pos[i], blob.pos[(i + 1) % n], color);
    }
    for (size_t i = 0; i < n; i++)
    {
        DrawCircleV(blob.pos[i], 4.0f, jointColor);
    }
}