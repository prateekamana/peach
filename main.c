#include<stdio.h>
#include<math.h>
#include "raylib.h"
#include "raymath.h"

// NES Resolution for canvas
#define CANVAS_W 256
#define CANVAS_H 224
#define SCALE 3.5
#define MAX_NODES 18
#define MAX_LEGS 18
#define BASE_FOOT_SPEED 8.0f

typedef struct {
    Vector2 targetPos;
    Vector2 currentPos; 
    float legLength;
    float angleOffset;
    int parentNodeNumber;
    bool stepTriggered;
    bool gaitFlag; 
} Leg;

typedef struct {
    int nodesAmount; // NEVER greater than 18
    float linkLength;
    float speed;
    Color color;
    int legsAmount;

    //!TODO: Need to see how to make these arrays below dynamic
    Leg legs[MAX_LEGS];
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

void updateLegs(Rope* rope){
    for(int legNumber=0; legNumber < rope->legsAmount; legNumber++){
        // Current leg in the for loop 
        Leg *currentLeg = &rope->legs[legNumber];

        // The other leg of the current pair
        Leg *theOtherLegOfCurrentPair;
        bool isRightLeg = (legNumber % 2 == 0); // Our program draws first the right leg, then the left leg, so even ones are right leg
        if(isRightLeg){
            // Right leg is first of the legs[] divided into two, therefore add 1 to it to reach the other leg
            theOtherLegOfCurrentPair = &rope->legs[legNumber+1];
        } else {
            // Left leg is always the second of the legs[] divided into two, therefore subtract 1 
            theOtherLegOfCurrentPair = &rope->legs[legNumber-1];
        }

        // Need to find how far the other leg is from its target, to adjust the current leg speed accordingly
        Vector2 otherLegDistVect = Vector2Subtract(theOtherLegOfCurrentPair->targetPos, theOtherLegOfCurrentPair->currentPos);
        float otherLegDist = sqrtf(otherLegDistVect.x*otherLegDistVect.x + otherLegDistVect.y*otherLegDistVect.y);
        float otherLegDistFactor = otherLegDist / theOtherLegOfCurrentPair->legLength;

        // Range of otherLegDistFactor is 0 to 1 depending on how far the other leg is
        // 1 when leg is farthest, 0 when leg is closest
        float footSpeed = rope->speed * otherLegDistFactor * otherLegDistFactor * otherLegDistFactor * otherLegDistFactor * BASE_FOOT_SPEED;

        // If node = 1 that'd just be the head node, can't add legs
        if(currentLeg->parentNodeNumber < 2){
            return;
        } else {
            // Offset by -1 so that the correct node can be referenced (arrays start with 0 not 1)
            int nodeNumber = currentLeg->parentNodeNumber - 1;
            
            // Source node is main leg node, target is the one ahead of it
            Vector2 sourceNode = rope->nodes[nodeNumber];    
            Vector2 targetNode = rope->nodes[nodeNumber-1];  

            // Find the angle between source and target nodes
            float dY = targetNode.y-sourceNode.y;
            float dX = targetNode.x-sourceNode.x;
            float dAngle =  atan2(dY, dX);

            // Leg pos vector is calculated by first finding out the vector between the two nodes
            // and then adding the offset angle to them, times leg length. 

            // This is the target pos for implementing walking
            Vector2 legTargetPos = {                
                sourceNode.x + cosf(dAngle + currentLeg->angleOffset) * currentLeg->legLength, 
                sourceNode.y + sinf(dAngle + currentLeg->angleOffset) * currentLeg->legLength
            };
            // targetPos is being set 60 times per second
            currentLeg->targetPos = legTargetPos;

            // When initialized, currentpos is 0, so the first frame of raylib sets the legs currentPos as targetPos, commenting this causes the current pos to go to top left corner (0,0) and then as walking happens it becomes alright
            if(currentLeg->currentPos.x == 0 &&  currentLeg->currentPos.y == 0){
                currentLeg->currentPos = legTargetPos;
            }

            // Find legDist between currentPos and currentTargetPos
            Vector2 currentLegTargetPos = currentLeg->targetPos;
            Vector2 currentLegCurrentPos = currentLeg->currentPos;
            float legDY = currentLegTargetPos.y - currentLegCurrentPos.y;
            float legDX = currentLegTargetPos.x - currentLegCurrentPos.x;
            float legDist = sqrtf(legDY*legDY + legDX*legDX);

            // If current step is gait enabled, 
            // && distance has exceeded leg length,
            // && stepTriggered is false, then mark stepTriggered to be true
            if(currentLeg->stepTriggered == false){
                if(legDist >= currentLeg->legLength && currentLeg->gaitFlag == true){
                    currentLeg->stepTriggered=true;
                }

            } 
            // If stepTriggered is true, calculate step and add step displacement to currentPos
            //!TODO: Add more comments here
            else if (currentLeg->stepTriggered == true){
                // pixel per frame distance exceeds how far it has to be, clamped to how far it has to be 
                if(footSpeed > legDist) {
                    footSpeed = legDist;
                }
                float xDisplacement = currentLeg->currentPos.x + (legDX/legDist) * footSpeed;
                float yDisplacement = currentLeg->currentPos.y + (legDY/legDist) * footSpeed;
                currentLeg->currentPos.x = xDisplacement;
                currentLeg->currentPos.y = yDisplacement;

                // Prevent overshooting
                if(legDist <= footSpeed){
                    currentLeg->currentPos = currentLeg->targetPos;
                    currentLeg->stepTriggered = false;
                    currentLeg->gaitFlag = false;
                    theOtherLegOfCurrentPair->gaitFlag = true;
                }
            }

            DrawLineV(
                sourceNode,
                currentLeg->currentPos,
                RED
            );
            DrawCircleV(currentLeg->currentPos, 2.7f, RED);
            
            // Target foot pos
            // DrawCircleV(currentLeg->targetPos, 2.7f, BLUE);

            //!LOGGING: Dist step and length
            // DrawText(TextFormat("Dist: %.2f", legDist), 10, 10, 8, WHITE);
            // DrawText(TextFormat("Step?: %i",  currentLeg->stepTriggered), 70, 10, 8, WHITE);
            // DrawText(TextFormat("Len: %.2f",  currentLeg->legLength), 120, 10, 8, WHITE);

            //!LOGGING: Misc
            // printf("currentLeg->angleOffset: %f\n", currentLeg->angleOffset);
            // printf("currentLeg->legLength: %f\n", currentLeg->legLength);
            // printf("dAngle + currentLeg->angleOffset %f\n", dAngle + currentLeg->angleOffset);
            // printf("cosf(dAngle + currentLeg->angleOffset): %f\n", cosf(dAngle + currentLeg->angleOffset));

            //!LOGGING: 
            // printf("atan2 : %f\n", atan2f(dY, dX));
            // printf("final angle: %f\n", angle);
            // printf("source : %f , %f \n ", sourceNode.x, sourceNode.y);
            // printf("target : %f , %f \n", sourceNode.x + cosf(dAngle + currentLeg->angleOffset) * currentLeg->legLength, sourceNode.y + sinf(dAngle + currentLeg->angleOffset) * currentLeg->legLength);
  
        }
    }
}

// Function to init/make rope 
Rope initRope(int nodesAmount, float linkLength, float speed, Color color, int legsAmount, Vector2 startPos){
    Rope r = {
        .nodesAmount = nodesAmount,
        .linkLength = linkLength,
        .speed = speed,
        .color = color,
        .legsAmount = legsAmount
    };

    for(int i=0; i<nodesAmount; i++){
        r.nodes[i] = startPos;
    }

    return r;
}

void initLegs(Rope* r, float legLength, float angleOffset, int legNodes[]){
    for(int i=0; i < r->legsAmount; i++){
        Leg l = {
            .targetPos={0},
            .currentPos={0},
            .legLength=legLength,
            .stepTriggered=false
        };

        if(i%2){
            l.angleOffset = -angleOffset * DEG2RAD;
        } else {
            l.angleOffset = +angleOffset * DEG2RAD;
        }
        l.gaitFlag = ((i/2) + (i%2)) % 2;
        l.parentNodeNumber = legNodes[i/2];
        r->legs[i] = l;
    }
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
    Rope rope1 = initRope(18, 9.0f, 1.7f, lightPink, 4, (Vector2){100, 100});
    initLegs(&rope1, 20.0f, 15.0f, (int[]){2, 7});
    Rope rope2 = initRope(18, 9.0f, 0.25f, lightSageGreen, 4, (Vector2){50, 50});

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
            updateLegs(&rope1);
            // drawLegs(&rope1, 2, 20.0f, 30);
            // drawLegs(&rope1, 7, 18.0f, 30);

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