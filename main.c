#include<stdio.h>
#include "raylib.h"

// NES Resolution for canvas
#define CANVAS_W 240
#define CANVAS_H 160
#define SCALE 4

int main() {
    // Init window and set FPS to upscaled res (4x the Canvas)
    InitWindow(CANVAS_W * SCALE, CANVAS_H * SCALE, "Peach 🍑");
    SetTargetFPS(60);

    //Create render texture, basically our little canvas
    RenderTexture2D canvasRenderTexture = LoadRenderTexture(CANVAS_W, CANVAS_H);
    //Nearest Neighbor filter for our canvas, maybe to handle upscaling !DOUBT
    SetTextureFilter(canvasRenderTexture.texture, TEXTURE_FILTER_POINT);

    //Rect properties
    float rotation = 0.0f;

    //RAYLIB LOOP
    while(!WindowShouldClose()){
        // Draw at small canvas resolution first
        BeginTextureMode(canvasRenderTexture);
            ClearBackground(WHITE);
            Rectangle rect = { CANVAS_W / 2.0f, CANVAS_H/2.0f, 50, 50 };            
            Vector2 origin = { rect.width/2, rect.height/2 };
            DrawRectanglePro(rect, origin, rotation, RED);
            rotation += 1.0f;
        EndTextureMode();

        // Draw Upscaled Canvas
        BeginDrawing();
            // ClearBackground(WHITE);
            Rectangle src = {0, 0, CANVAS_W, -CANVAS_H};
            Rectangle dest = {0, 0, CANVAS_W * SCALE, CANVAS_H * SCALE};
            DrawTexturePro(canvasRenderTexture.texture, src, dest, (Vector2){0, 0}, 0.0f, WHITE);

        EndDrawing();
    }


    UnloadRenderTexture(canvasRenderTexture);
    CloseWindow();
    return 0;
}