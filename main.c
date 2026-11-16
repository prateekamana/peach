#include<stdio.h>
#include<math.h>
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
    float speed = 0.75f;

    //RAYLIB LOOP
    while(!WindowShouldClose()){
        // Draw at small canvas resolution first
        BeginTextureMode(canvasRenderTexture);
            ClearBackground(BLACK);

            // Get correct position of mouse as canvas is SCALED down
            Vector2 pointerPos = {
                GetMousePosition().x / SCALE - 5,
                GetMousePosition().y / SCALE - 5
            };

            // Draw current posiitions of head and pointer
            DrawPixel(headPos.x, headPos.y, WHITE);
            DrawPixel(pointerPos.x, pointerPos.y, RED);

            // Calculate difference of header and pointer pos and distance
            // Thanks Pythagoras 
            float dY = pointerPos.y - headPos.y;
            float dX = pointerPos.x - headPos.x;
            float dist = sqrtf(dY*dY + dX*dX);

            // update head position by normalizing the distance vector 
            headPos.x += (dX/dist) * speed;
            headPos.y += (dY/dist) * speed;

            //!LOGGING: can leave these commented for now
            // printf("dist = %f \n", dist);
            // printf("dy: %f, dx: %f\n", dY, dX);
            // printf("headPos.x: %f, headPos.y: %f\n", headPos.x, headPos.y);

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