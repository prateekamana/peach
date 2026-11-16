#include<stdio.h>
#include<math.h>
#include "raylib.h"

// NES Resolution for canvas
#define CANVAS_W 240
#define CANVAS_H 160
#define SCALE 4

typedef struct {
    int nodesAmount;
    float linkLength;
    float speed;

    //!TODO: Need to see how to make this array below dynamic
    Vector2 nodes[108]; 
} Rope;

int main() {
    // Init window and set FPS to upscaled res (4x the Canvas)
    InitWindow(CANVAS_W * SCALE, CANVAS_H * SCALE, "Peach 🍑");
    SetTargetFPS(60);

    //Create render texture, basically our little canvas
    RenderTexture2D canvasRenderTexture = LoadRenderTexture(CANVAS_W, CANVAS_H);
    //Nearest Neighbor filter for our canvas, maybe to handle upscaling !DOUBT
    SetTextureFilter(canvasRenderTexture.texture, TEXTURE_FILTER_POINT);

    // Rope instantiate
    Rope rope1 = {
        .nodesAmount = 84, 
        .linkLength = 8.0f,
        .speed = 1.25f,
        .nodes = {0}
    };

    // Set node[0] to be head position in the center
    rope1.nodes[0] = (Vector2){CANVAS_W/2, CANVAS_H/2};

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

            // Loop to calculate the positions of rope nodes
            //!TODO: Need to see if code repetition can be dealth with here
            for(int i=0; i<rope1.nodesAmount; i++){
                if(i==0){
                    // HEAD NODE
                    // Calculate difference of HEADER and POINTER pos and distance
                    Vector2 headNode = rope1.nodes[i];

                    float dY = pointerPos.y - headNode.y;
                    float dX = pointerPos.x - headNode.x;
                    float dist = sqrtf(dY*dY + dX*dX);

                    rope1.nodes[0] = (Vector2) {
                        headNode.x + (dX/dist)*rope1.speed,
                        headNode.y + (dY/dist)*rope1.speed    
                    };
                } else {
                    // CHILDREN NODES
                    Vector2 currentNode = rope1.nodes[i];
                    Vector2 previousNode = rope1.nodes[i-1];

                    float dY = previousNode.y - currentNode.y;
                    float dX = previousNode.x - currentNode.x;
                    float dist = sqrtf(dY*dY + dX*dX);

                    if(dist > rope1.linkLength){
                        rope1.nodes[i] = (Vector2) {
                            currentNode.x + (dX/dist) * rope1.speed,
                            currentNode.y + (dY/dist) * rope1.speed
                        };
                    }

                }
            }

            // Draw current posiitions of head, node and pointer
            for(int n=0; n<rope1.nodesAmount; n++){
                DrawPixel(rope1.nodes[n].x, rope1.nodes[n].y, WHITE);
            }
            DrawPixel(pointerPos.x, pointerPos.y, RED);

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