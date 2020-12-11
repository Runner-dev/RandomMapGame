/*******************************************************************************************
*
*   raylib [models] example - Heightmap loading and drawing
*
*   This example has been created using raylib 1.8 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "random"
#include "string"
#include "iostream"

using namespace std;

float bilinear_interpolation(int x, int y, int points[4][3])
{
    return (points[0][2] * (points[2][0] - x) * (points[1][1] - y) +
            points[2][2] * (x - points[0][0]) * (points[1][1] - y) +
            points[1][2] * (points[2][0] - x) * (y - points[0][1]) +
            points[3][2] * (x - points[0][0]) * (y - points[0][1])) /
           ((points[2][0] - points[0][0]) * (points[1][1] - points[0][1]) + 0.0);
}

float interpolatePosition(Vector3 position, unsigned char *data)
{
    float textureX = (position.x + 8) / 16 * 128;
    float textureY = (position.z + 8) / 16 * 128;
    int minX = floor(textureX);
    int maxX = ceil(textureX);
    int minY = floor(textureY);
    int maxY = ceil(textureY);

    int points[4][3] = {
        {minX, minY, (int)data[(minY * 128 + minX) * 4]},
        {minX, maxY, (int)data[(maxY * 128 + minX) * 4]},
        {maxX, minY, (int)data[(minY * 128 + maxX) * 4]},
        {maxX, maxY, (int)data[(maxY * 128 + maxX) * 4]}};

    return (bilinear_interpolation(textureX, textureY, points));
};

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1600;
    const int screenHeight = 900;

    int x = rand() % 100000;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - heightmap loading and drawing");

    // Define our custom camera to look into our 3d world
    // Camera camera = { 0 };
    // camera.position = (Vector3){18.0f, 18.0f, 18.0f};
    // camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    // camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    // camera.fovy = 60.0f;
    // camera.type = CAMERA_PERSPECTIVE;
    Camera camera = {0};
    camera.position = (Vector3){4.0f, 2.0f, 4.0f};
    camera.target = (Vector3){0.0f, 1.8f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.type = CAMERA_PERSPECTIVE;
    // Set map diffuse texture
    Vector3 mapPosition = {-8.0f, 0.0f, -8.0f}; // Define model position

    SetCameraMode(camera, CAMERA_FIRST_PERSON);

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    Image image = GenImagePerlinNoise(128, 128, x, rand() % 100000, 1); // Load heightmap image (RAM)
    Texture2D texture = LoadTextureFromImage(image);
    unsigned char *data = (unsigned char *)image.data;

    Mesh mesh = GenMeshHeightmap(image, (Vector3){16, 8, 16}); // Generate heightmap mesh (RAM and VRAM)
    Model model = LoadModelFromMesh(mesh);

    // UnloadImage(image);

    model.materials[0].maps[MAP_DIFFUSE].texture = texture;

    float lastI = 0;

    Vector3 ballPos = {(rand() % 800) / 100.0, 0, (rand() % 800) / 100.0};

    bool showBall = false;

    // Main game loop
    while (!WindowShouldClose() && IsKeyUp(KEY_L)) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Update camera

        if (IsKeyPressed(KEY_R))
        {
            x += 1;

            UnloadImage(image);

            image = GenImagePerlinNoise(128, 128, x, rand() % 100000, 1); // Load heightmap image (RAM)
            texture = LoadTextureFromImage(image);                        // Convert image to texture (VRAM)

            data = (unsigned char *)image.data;

            mesh = GenMeshHeightmap(image, (Vector3){16, 8, 16}); // Generate heightmap mesh (RAM and VRAM)
            model = LoadModelFromMesh(mesh);

            model.materials[0].maps[MAP_DIFFUSE].texture = texture;
            ballPos = {(rand() % 700) / 100.0 + 0.5,
                       0,
                       (rand() % 700) / 100.0 + 0.5};
            showBall = false;
        }

        if(IsKeyPressed(KEY_H)) {
            showBall = !showBall;
        }

        UpdateCamera(&camera);

        if (interpolatePosition(camera.position, data) != lastI)
        {
            lastI = interpolatePosition(camera.position, data);
        }
        mapPosition.y = -0.5 - interpolatePosition(camera.position, data) / 50;
        if (isnan(interpolatePosition(camera.position, data)))
        {
            mapPosition.y = interpolatePosition({0.1, 0, 0.1}, data);
            if(showBall) {
            ballPos.y = 2 - interpolatePosition({0.1, 0, 0.1}, data) / 50 + interpolatePosition(ballPos, data) / 50;
            } else {
            ballPos.y = 1.2 - interpolatePosition({0.1, 0, 0.1}, data) / 50 + interpolatePosition(ballPos, data) / 50;
            }
        }
        else
        {
            mapPosition.y = -0.5 - interpolatePosition(camera.position, data) / 50;
            if(showBall) {
            ballPos.y = 2 - interpolatePosition(camera.position, data) / 50 + interpolatePosition(ballPos, data) / 50;
            } else {
            ballPos.y = 1.2 - interpolatePosition(camera.position, data) / 50 + interpolatePosition(ballPos, data) / 50;
            }
        }

        if (IsKeyDown(KEY_LEFT_CONTROL))
        {
            mapPosition.y += 0.1f;
        }
        if (IsKeyDown(KEY_LEFT_SHIFT))
        {
            mapPosition.y -= 0.1f;
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawModel(model, mapPosition, 1.0f, RED);

        DrawSphere(ballPos, 0.2, DARKGRAY);

        EndMode3D();

        DrawTexture(texture, screenWidth - texture.width - 20, 20, WHITE);
        DrawRectangleLines(screenWidth - texture.width - 20, 20, texture.width, texture.height, GREEN);

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture); // Unload texture
    UnloadModel(model);     // Unload model

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}