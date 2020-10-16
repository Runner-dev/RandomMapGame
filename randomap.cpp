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
    Texture2D texture = LoadTextureFromImage(image);                    // Convert image to texture (VRAM)

    Mesh mesh = GenMeshHeightmap(image, (Vector3){16, 8, 16}); // Generate heightmap mesh (RAM and VRAM)
    Model model = LoadModelFromMesh(mesh);

    UnloadImage(image);

    model.materials[0].maps[MAP_DIFFUSE].texture = texture;

    // Main game loop
    while (!WindowShouldClose() && IsKeyUp(KEY_L)) // Detect window close button or ESC key
    {
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