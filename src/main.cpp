#include "raylib.h"
#include "rlgl.h"
#include "blob.h"
#include "debug_draw.h"

enum CurrentColor{ COLOR_NONE, COLOR_RED, COLOR_GREEN, COLOR_BLUE };


int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "soft-body");

    Shader shader = LoadShader(0, SHADER_DIR "blob.frag");
    int resolutionLoc = GetShaderLocation(shader, "resolution");
    int mouseLoc = GetShaderLocation(shader, "mouse");
    int countLoc = GetShaderLocation(shader, "count");
    int pointsLoc = GetShaderLocation(shader, "points");
    int baseColorLoc = GetShaderLocation(shader, "baseColor");

    // Bez kontekstu 4.3 nie będzie compute shaderów ani SSBO.
    if (rlGetVersion() != RL_OPENGL_43)
        TraceLog(LOG_WARNING, "missing OpenGL 4.3 context - compute shaders wont work");

    Blob blob;
    blob.init({GetScreenWidth() / 2.0f, 200.0f});

    const float dt = 1.0f / 240.0f;
    float accumulator = 0.0f;
    bool debugMode = false;
    CurrentColor currentColor = COLOR_NONE;

    while (!WindowShouldClose())
    {
        switch (GetKeyPressed())
        {
        case KEY_R:
            blob.init({GetScreenWidth() / 2.0f, 200.0f});
            break;
        case KEY_LEFT:
            blob.params.pressure -= 0.1f;
            break;
        case KEY_RIGHT:
            blob.params.pressure += 0.1f;
            break;
        case KEY_UP:
            blob.params.iterations += 1;
            break;
        case KEY_DOWN:
            blob.params.iterations -= 1;
            break;
        case KEY_D:
            debugMode = !debugMode;
            break;
        case KEY_Q:
            currentColor = COLOR_RED;
            break;
        case KEY_W:
            currentColor = COLOR_GREEN;
            break;
        case KEY_E:
            currentColor = COLOR_BLUE;
            break;
        case KEY_N:
            currentColor = COLOR_NONE;
            break;
        case KEY_EQUAL:
            if (currentColor == COLOR_RED)
                blob.params.baseColor.x += 0.05f;
            else if (currentColor == COLOR_GREEN)
                blob.params.baseColor.y += 0.05f;
            else if (currentColor == COLOR_BLUE)
                blob.params.baseColor.z += 0.05f;
            break;
        case KEY_MINUS:
            if (currentColor == COLOR_RED)
                blob.params.baseColor.x -= 0.05f;
            else if (currentColor == COLOR_GREEN)
                blob.params.baseColor.y -= 0.05f;
            else if (currentColor == COLOR_BLUE)
                blob.params.baseColor.z -= 0.05f;
            break;
        }

        if (IsKeyPressedRepeat(KEY_EQUAL)){
            
            if (currentColor == COLOR_RED)
                blob.params.baseColor.x += 0.05f;
            else if (currentColor == COLOR_GREEN)
                blob.params.baseColor.y += 0.05f;
            else if (currentColor == COLOR_BLUE)
                blob.params.baseColor.z += 0.05f;
        }

        if (IsKeyPressedRepeat(KEY_MINUS)){
            
            if (currentColor == COLOR_RED)
                blob.params.baseColor.x -= 0.05f;
            else if (currentColor == COLOR_GREEN)
                blob.params.baseColor.y -= 0.05f;
            else if (currentColor == COLOR_BLUE)
                blob.params.baseColor.z -= 0.05f;
        }

        // if (IsKeyPressed(KEY_R))
        //     blob.init({GetScreenWidth() / 2.0f, 200.0f});

        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            blob.grab(mouse);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            blob.grabTarget = mouse;
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            blob.release();

        accumulator += GetFrameTime();
        if (accumulator > 0.1f)
            accumulator = 0.1f;
        while (accumulator >= dt)
        {
            blob.step(dt);
            blob.solveConstraints();
            blob.applyGrab();
            blob.collide({0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()});
            accumulator -= dt;
        }

        BeginDrawing();
        ClearBackground({18, 18, 24, 255});

        Vector2 res = {(float)GetScreenWidth(), (float)GetScreenHeight()};
        SetShaderValue(shader, resolutionLoc, &res, SHADER_UNIFORM_VEC2);
        Vector2 m = GetMousePosition();
        SetShaderValue(shader, mouseLoc, &m, SHADER_UNIFORM_VEC2);
        int count = (int)blob.pos.size();
        SetShaderValue(shader, countLoc, &count, SHADER_UNIFORM_INT);
        SetShaderValueV(shader, pointsLoc, blob.pos.data(), SHADER_UNIFORM_VEC2, count);
        SetShaderValue(shader, baseColorLoc,&blob.params.baseColor, SHADER_UNIFORM_VEC3);

        BeginShaderMode(shader);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
        EndShaderMode();

        if (debugMode)
            drawBlobDebug(blob);
        DrawText(TextFormat("A = %.0f", blob.area()), 10, 40, 20, WHITE);
        DrawText(TextFormat("Iterations = %i", blob.params.iterations), 10, 70, 20, WHITE);
        DrawText(TextFormat("Pressure = %0.2f", blob.params.pressure), 10, 100, 20, WHITE);
        DrawText(TextFormat("Base Color = (%0.2f, %0.2f, %0.2f)", blob.params.baseColor.x, blob.params.baseColor.y, blob.params.baseColor.z), 10, 130, 20, WHITE);
        DrawText(TextFormat("Current Color = %s", currentColor == COLOR_NONE ? "None" : currentColor == COLOR_RED ? "Red" : currentColor == COLOR_GREEN ? "Green" : "Blue"), 10, 160, 20, WHITE);

        DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadShader(shader);
    CloseWindow();
    return 0;
}
