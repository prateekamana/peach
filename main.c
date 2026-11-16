#include<stdio.h>
#include<math.h>
#include "raylib.h"

// NES Resolution for canvas
#define CANVAS_W 240
#define CANVAS_H 160
#define SCALE 4

typedef struct {
    int nodes;
    float linkLength;
    float speed;
} Rope;

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
    Vector2 node1 = {headPos.x, headPos.y-2};

    // Rope initialize //!TODO: need to change comment term, initialize doesnt seem to be c term 
    Rope rope1 = {
        .nodes = 2, 
        .linkLength = 15.0f,
        .speed = 0.75f
    };

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

            // Draw current posiitions of head, node and pointer
            DrawPixel(headPos.x, headPos.y, WHITE);
            DrawPixel(node1.x, node1.y, WHITE);
            DrawPixel(pointerPos.x, pointerPos.y, RED);

            // Calculate difference of HEADER and POINTER pos and distance
            // Thanks Pythagoras 
            float dY = pointerPos.y - headPos.y;
            float dX = pointerPos.x - headPos.x;
            float dist = sqrtf(dY*dY + dX*dX);

            //Added single node for proof of concept of link length following logic
            //!TODO: Iteratively draw pixel based on rope struct
            float nodeDY = headPos.y - node1.y;
            float nodeDX = headPos.x - node1.x;
            float nodeDistance = sqrtf(nodeDY*nodeDY + nodeDX*nodeDX);

            if(nodeDistance > rope1.linkLength){
                node1.y += (nodeDY/nodeDistance) * rope1.speed;
                node1.x += (nodeDX/nodeDistance) * rope1.speed;
            }

            // update head position by normalizing the distance vector 
            headPos.x += (dX/dist) * rope1.speed;
            headPos.y += (dY/dist) * rope1.speed;

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