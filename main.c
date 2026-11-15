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

    //Head properties

    Vector2 headPos = {CANVAS_W/2, CANVAS_H/2};

    //RAYLIB LOOP
    while(!WindowShouldClose()){
        // Draw at small canvas resolution first
        BeginTextureMode(canvasRenderTexture);
            ClearBackground(BLACK);
            Vector2 pointerPos = {
                GetMousePosition().x / SCALE - 5,
                GetMousePosition().y / SCALE - 5
            };

            float diffPointerPosToHeadPosX = pointerPos.x - headPos.x;
            float diffPointerPosToHeadPosY = pointerPos.y - headPos.y;


            if(diffPointerPosToHeadPosX > 0){
                headPos.x += 1;
            } else {
                headPos.x -= 1;
            }

            if(diffPointerPosToHeadPosY > 0){
                headPos.y += 1;
            } else {
                headPos.y -= 1;
            }
            DrawPixel(headPos.x, headPos.y, WHITE);
            DrawPixel(pointerPos.x, pointerPos.y, RED);



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