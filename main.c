#include <stdio.h>
#include <SDL3/SDL.h>
#include <math.h>
#include <stdlib.h>

const int SCREEN_HEIGHT = 1000;
const int SCREEN_WIDTH = 1000;

typedef struct{
    float x;
    float y;
} float2D;

typedef struct{
    float2D basePos;
    float2D* vertices;
    float2D scale;
} shape;

typedef struct{
    int r;
    int g;
    int b;
    int a;
} rgba;

shape makeThingy(float xAxisLength, float yAxisLength, int numPoints, float x, float y){
    shape THE_THINGY;
    float2D* points = (float2D*)malloc(sizeof(float2D)*numPoints);

    //plot points relative to basePos
    for(int i=0;i<numPoints;i++){
        //do not ask me how this works (its magic)
        points[i].y = ((float)sin((2*3.14159*i)/(numPoints-1)));
        points[i].x = ((float)cos((2*3.14159*i)/(numPoints-1)));
    }
    //set basePos
    THE_THINGY.basePos.x = x;
    THE_THINGY.basePos.y = y;
    THE_THINGY.scale.x = xAxisLength;
    THE_THINGY.scale.y = yAxisLength;
    //assign points to the shape
    THE_THINGY.vertices = points;

    return THE_THINGY;
}

//renders the vertices of the shape 
void drawShape(shape* input, int count, SDL_Renderer* renderer, rgba color){
    float2D drawLocations[count]; //array with (count) amount of members
    for(int i = 0; i < count; i++){
        //assigns the true coordinates of the vertices (relative offset + base/reference point)
        //to each member the draw locations array 
        drawLocations[i].x = (input->vertices[i].x * input->scale.x) + input->basePos.x;
        drawLocations[i].y = (input->vertices[i].y * input->scale.y) + input->basePos.y;
    }
    //render it lmao
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderLines(renderer, (SDL_FPoint*)drawLocations, count);
    //set color back to default
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
}

float2D findFocalOffset(int xAxisLength,int yAxisLength){
    
    float2D returnValues = {0};
    
    double focalOffset = sqrt(fabs(pow(xAxisLength,2)-pow(yAxisLength,2)));
    if(xAxisLength > yAxisLength){
        returnValues.x = focalOffset;
    } 
    else if(yAxisLength > xAxisLength){
        returnValues.y = focalOffset;
    }
    return returnValues;
}

int main(int argc, char** argv){
    //initialize SDL stuff
    SDL_Init(SDL_INIT_VIDEO);
    //the window (so you can see stuff)
    SDL_Window* myActualWindow = SDL_CreateWindow(
        "Orbiter :333 <3", 
        SCREEN_WIDTH, 
        SCREEN_HEIGHT, 
        SDL_WINDOW_INPUT_FOCUS
    );

    //renderer for window (to make stuff show up)
    SDL_Renderer* myWindowRenderer = SDL_CreateRenderer(myActualWindow, NULL);
    //set colors (renderer, RGBA)
    //SDL_SetRenderScale(myWindowRenderer, 0.5, 0.5);
    SDL_SetRenderDrawColorFloat(myWindowRenderer, 0,0,0,0); //background color
    SDL_SetRenderDrawBlendMode(myWindowRenderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(myWindowRenderer, NULL);
    SDL_SetRenderDrawColor(myWindowRenderer, 255,255,255,255); //default line color

    SDL_SetEventEnabled(SDL_EVENT_WINDOW_CLOSE_REQUESTED, true);
    SDL_Event event;
    

    float xLength = 30;
    float yLength = 30;
    int verticesCount = 33;
    int parentBodyRadius = 25;
    rgba parentBodyColor = {0,255,0,255};
    rgba myShapeColor = {255,255,255,255};
    
    float2D focusOffset = findFocalOffset(xLength, yLength);
    
    //create orbit shape
    shape myShape = makeThingy(
        xLength, 
        yLength, 
        verticesCount, 
        SCREEN_WIDTH/2, 
        SCREEN_HEIGHT/2
    );

    //create parent body, make it go where i want
    shape parentBody = makeThingy(
        parentBodyRadius,
        parentBodyRadius,
        17,
        (SCREEN_WIDTH/2) + focusOffset.x,
        (SCREEN_HEIGHT/2) + focusOffset.y
    );

    //test the stuff

    bool isRunning = true;
    while(isRunning){
        SDL_SetRenderDrawColor(myWindowRenderer, 0,0,0,255);
        SDL_RenderClear(myWindowRenderer);
        drawShape(&myShape, verticesCount, myWindowRenderer, myShapeColor);
        drawShape(&parentBody, 17, myWindowRenderer, parentBodyColor);
        SDL_RenderPresent(myWindowRenderer);
        
        float2D offset = findFocalOffset(myShape.scale.x, myShape.scale.y);

        parentBody.basePos.x = SCREEN_WIDTH/2 + offset.x;
        parentBody.basePos.y = SCREEN_HEIGHT/2 + offset.y;
        
        while(SDL_PollEvent(&event)){
            switch (event.type){
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    isRunning = false;
                    SDL_DestroyRenderer(myWindowRenderer);
                    SDL_DestroyWindow(myActualWindow);
                    break;
                case SDL_EVENT_KEY_DOWN:
                    switch(event.key.key){
                        case SDLK_LEFT:
                            myShape.scale.x -= 1;
                            break;
                        case SDLK_RIGHT:
                            myShape.scale.x += 1;
                            break;
                        case SDLK_DOWN:
                            myShape.scale.y -= 1;
                            break;
                        case SDLK_UP:
                            myShape.scale.y += 1;
                            break;
                    default:
                        break;
                    } 
            default:
                break;
            }
        }
    }
    free(myShape.vertices);
    free(parentBody.vertices);
}