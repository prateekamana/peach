#include<stdio.h>
#include<math.h>
#include "raylib.h"

// NES Resolution for canvas
#define CANVAS_W 256
#define CANVAS_H 224
#define SCALE 3.5
#define MAX_NODES 18

typedef struct {
    int nodesAmount; // NEVER greater than 18
    float linkLength;
    float speed;
    Color color;

    //!TODO: Need to see how to make this array below dynamic
    Vector2 nodes[MAX_NODES]; 
} Rope;

//!TODO: Wonder if it's the best way to handle this.
float nodeWidths[18] = {
    5.0f, //1
    3.0f, //2 Arms 
    4.0f, //3
    5.0f, //4
    5.0f, //5
    5.0f, //6
    4.5f, //7 Legs
    4.25f, //8
    4.0f, //9
    3.8f, //10
    3.5f, //11
    3.2f, //12
    3.0f, //13
    2.0f, //14
    1.5f, //15
    1.25f, //16
    1.0f, //17
    0.85f, //18
};

// Function to update rope nodes movement with respect to a target point 
void updateRopeNodes(Rope* rope, Vector2 targetPoint){
    for(int i=0; i<rope->nodesAmount; i++){
        if(i==0){
            // HEAD NODE
            // Calculate difference of HEADER and POINTER pos and distance
            Vector2 headNode = rope->nodes[i];

            float dY = targetPoint.y - headNode.y;
            float dX = targetPoint.x - headNode.x;
            float dist = sqrtf(dY*dY + dX*dX);
            if(dist > nodeWidths[i]){
                rope->nodes[0] = (Vector2) {
                    headNode.x + (dX/dist)*rope->speed,
                    headNode.y + (dY/dist)*rope->speed    
                };
            }
        } else {
            // CHILDREN NODES
            Vector2 currentNode = rope->nodes[i];
            Vector2 previousNode = rope->nodes[i-1];
            //!TODO: search by float dY and use the DRY principle to encapsulate code
            float dY = previousNode.y - currentNode.y;
            float dX = previousNode.x - currentNode.x;
            float dist = sqrtf(dY*dY + dX*dX);

            if(dist > rope->linkLength){
                rope->nodes[i] = (Vector2) {
                    currentNode.x + (dX/dist) * rope->speed,
                    currentNode.y + (dY/dist) * rope->speed
                };
            }
        }
    }
}
// Function to draw all the rope nodes, to be called from raylib loop
void drawRopeNodes(Rope* rope){
    for(int n=0; n<rope->nodesAmount; n++){
        Vector2 currentNode = rope->nodes[n];
        DrawPixelV(currentNode, rope->color);
        DrawCircleLinesV(currentNode, nodeWidths[n], rope->color);
        if(n!=0){
            Vector2 previousNode = rope->nodes[n-1];
            DrawLine(currentNode.x, currentNode.y, previousNode.x, previousNode.y, rope->color);
        }
    }
}

void drawLegs(Rope* rope, int nodeNumber, float legLength, float angle){
    // First convert degrees to radians
    angle = angle * DEG2RAD;
    // just added this because in the parameter sectoin vscode was greying out leglength and ts pmo fr
    legLength = legLength; 
    // Radius to draw feet circles
    float footRadius = 2.7f;

    // If node = 1 that'd just be the head node, can't add legs
    if(nodeNumber<2){
        return;
    }
    else {
        // Offset by -1 so that the correct node can be referenced (arrays start with 0 not 1)
        nodeNumber-=1;
        
        // Source node is main leg node, target is the one ahead of it
        Vector2 sourceNode = rope->nodes[nodeNumber];    
        Vector2 targetNode = rope->nodes[nodeNumber-1];  
        // Find the angle between source and target nodes
        float dY = targetNode.y-sourceNode.y;
        float dX = targetNode.x-sourceNode.x;
        float dAngle =  atan2(dY, dX);

        // Leg pos vector is calculated by first finding out the vector between the two nodes
        // and then adding the offset angle to them, times leg length. 
        Vector2 rightLegPos = {                
            sourceNode.x + cosf(dAngle + angle) * legLength, 
            sourceNode.y + sinf(dAngle + angle) * legLength
        };
        Vector2 leftLegPos = {                
            sourceNode.x + cosf(dAngle - angle) * legLength, 
            sourceNode.y + sinf(dAngle - angle) * legLength
        };

        // RIGHT LEG AND FOOT
        DrawLineV(
            sourceNode,
            rightLegPos,
            RED
        );
        DrawCircleV(rightLegPos, footRadius, RED);
        
        //LEFT LEG AND FOOT 
        DrawLineV(
            sourceNode,
            leftLegPos,
            RED
        );
        DrawCircleV(leftLegPos, footRadius, RED);

        //!LOGGING: 
        // printf("atan2 : %f\n", atan2f(dY, dX));
        // printf("final angle: %f\n", angle);
        // printf("source : %f , %f \n ", sourceNode.x, sourceNode.y);
        // printf("target : %f , %f \n", sourceNode.x + sinf(angle) * length, sourceNode.y + cosf(angle) * length);
    }
}

// Function to init/make rope 
Rope initRope(int nodesAmount, float linkLength, float speed, Color color, Vector2 startPos){
    Rope r = {
        .nodesAmount = nodesAmount,
        .linkLength = linkLength,
        .speed = speed,
        .color = color
    };

    for(int i=0; i<nodesAmount; i++){
        r.nodes[i] = startPos;
    }

    return r;
}

// Colors
Color lightPink      = (Color){ 255, 228, 235, 255 }; // really light pink
Color lightSageGreen = (Color){ 214, 232, 208, 255 }; // really light sage green


int main() {
    // Init window and set FPS to upscaled res (4x the Canvas)
    InitWindow(CANVAS_W * SCALE, CANVAS_H * SCALE, "Peach 🍑");
    SetTargetFPS(60);

    //Create render texture, basically our little canvas
    RenderTexture2D canvasRenderTexture = LoadRenderTexture(CANVAS_W, CANVAS_H);
    //Nearest Neighbor filter for our canvas, maybe to handle upscaling !DOUBT
    SetTextureFilter(canvasRenderTexture.texture, TEXTURE_FILTER_POINT);

    // Rope instantiate
    Rope rope1 = initRope(18, 9.0f, 1.25f, lightPink, (Vector2){100, 100});
    Rope rope2 = initRope(18, 9.0f, 0.25f, lightSageGreen, (Vector2){50, 50});

    // Centering head node code commented for now
    // Set node[0] to be head position in the center
    // rope1.nodes[0] = (Vector2){CANVAS_W/2, CANVAS_H/2};

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

            // Update nodes of rope 60 times per second
            updateRopeNodes(&rope1, pointerPos);
            updateRopeNodes(&rope2, pointerPos);
            // Draw updated nodes 60 times per second 
            drawRopeNodes(&rope1);
            // drawRopeNodes(&rope2);
            drawLegs(&rope1, 2, 20.0f, 30);
            drawLegs(&rope1, 7, 18.0f, 30);

            // Draw Mouse Pointer
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
    printf("cos %f \n", cosf(0.0f)) ;
    CloseWindow();
    
    return 0;
}