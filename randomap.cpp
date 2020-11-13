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

float BilinearInterpolation(float q11, float q12, float q21, float q22, float x1, float x2, float y1, float y2, float x, float y)
{
    float x2x1, y2y1, x2x, y2y, yy1, xx1;
    x2x1 = x2 - x1;
    y2y1 = y2 - y1;
    x2x = x2 - x;
    y2y = y2 - y;
    yy1 = y - y1;
    xx1 = x - x1;
    cout << "" << (q11 * x2x * y2y + q21 * xx1 * y2y + q12 * x2x * yy1 + q22 * xx1 * yy1) << endl;
    return (
               q11 * x2x * y2y +
               q21 * xx1 * y2y +
               q12 * x2x * yy1 +
               q22 * xx1 * yy1) /
           (x2x1 * y2y1);
}

float interpolatePosition(Vector3 position, unsigned char *data)
{
    float textureX = (position.x + 8) / 16 * 128;
    float textureY = (position.z + 8) / 16 * 128;
    int minX = floor(textureX);
    int maxX = ceil(textureX);
    int minY = floor(textureY);
    int maxY = ceil(textureY);

    // cout << "Int: " << (int)data[(minY * 128 + minX) * 4] << endl;
    // cout << "Int: " << position.x << endl;
    // cout << "Int: " << minX << endl;
    // cout << "Int: " << maxX << endl;

    return(BilinearInterpolation(
                (float)(int)data[(minY * 128 + minX) * 4],
                (float)(int)data[(minY * 128 + maxX) * 4],
                (float)(int)data[(maxY * 128 + minX) * 4],
                (float)(int)data[(maxY * 128 + maxX) * 4],
                (float)minX,
                (float)maxX,
                (float)minY,
                (float)maxY,
                position.x, position.y));
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

    // Main game loop
    while (!WindowShouldClose() && IsKeyUp(KEY_L)) // Detect window close button or ESC key
    {
        // Convert image to texture (VRAM)
        int vertexCount = mesh.vertexCount;
        // Update
        //----------------------------------------------------------------------------------
        // Update camera

        if (IsKeyPressed(KEY_R))
        {
            x += 1;

            image = GenImagePerlinNoise(128, 128, x, rand() % 100000, 1); // Load heightmap image (RAM)
            texture = LoadTextureFromImage(image);                        // Convert image to texture (VRAM)

            mesh = GenMeshHeightmap(image, (Vector3){16, 8, 16}); // Generate heightmap mesh (RAM and VRAM)
            model = LoadModelFromMesh(mesh);
            UnloadImage(image);

            model.materials[0].maps[MAP_DIFFUSE].texture = texture;
        }

        UpdateCamera(&camera);
        
        mapPosition.y = -2 + interpolatePosition(camera.position, data)/100;

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

        // DrawGrid(20, 1.0f);

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