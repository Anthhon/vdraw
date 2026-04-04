#include "renderer.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

int main(void)
{
    int screen_id = GetCurrentMonitor();                    // Use primary monitor
    bool hideUI = false;                                    // Start showing UI
    bool cinematicMode = false;                             // Start without auto-rotation
    Vector3 orbitTarget = (Vector3){0, 0, 0};               // Center of orbit
    
    // Allocate memory for graph points
    Vector3* points = malloc(sizeof(*points) * VD_GRAPH_RESOLUTION);
    if (points == NULL) {
        fprintf(stderr, "[ERROR] Could not allocate enough memory to store points.\n");
        return 1;
    }
    
    // Configure window
    VdManagerWindow window = {
        .screen_width = GetMonitorWidth(screen_id),
        .screen_height = GetMonitorHeight(screen_id),
        .title = "vdraw - 3D Helix Visualizer",
    };
    
    // Initialize camera
    Camera3D camera = {
        .position   = VD_CAMERA_INITIAL_POS,
        .target     = VD_CAMERA_INITIAL_TARGET,
        .up         = (Vector3){ 0.0f, 1.0f, 0.0f },       // Y-up orientation
        .fovy       = VD_CAMERA_FOV,
        .projection = CAMERA_PERSPECTIVE,
    };
    
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_WINDOW_MAXIMIZED);
    InitWindow(window.screen_width, window.screen_height, window.title);
    SetTargetFPS(VD_MAX_FPS);

    // Check if font file exists before loading
    if (FileExists(VD_FONT_PATH)) {
        int fontChars[256];
        for (int i = 0; i < 256; i++) {
            fontChars[i] = 32 + i;  // Basic ASCII characters
        }
        
        // Load the font with extended parameters
        // LoadFontEx( filePath, fontSize, codepointsArray, codepointsCount )
        window.custom_font = LoadFontEx(VD_FONT_PATH, VD_FONT_SIZE, fontChars, 256);
        GenTextureMipmaps(&window.custom_font.texture);
        SetTextureFilter(window.custom_font.texture, TEXTURE_FILTER_ANISOTROPIC_4X);
        
        TraceLog(LOG_INFO, TextFormat("Font loaded: %s, base size: %d", VD_FONT_PATH, window.custom_font.baseSize));
    } else {
        // Fall back to default font
        TraceLog(LOG_WARNING, TextFormat("Font file not found: %s", VD_FONT_PATH));
        window.custom_font = GetFontDefault();
    }
    
    while (!WindowShouldClose()) {
        // Update logic
        VdGetUserInput(&camera, &orbitTarget, &cinematicMode, &hideUI);
        VdPointsCalculateValue(points);
        
        // Rendering
        BeginDrawing();
        {
            ClearBackground(BLACK);
            if (!hideUI) {
                VdDrawDebugInfo(&window);
                VdDrawControlText(&window);
            }
            
            BeginMode3D(camera);
            {
                if (!hideUI) {
                    DrawGrid(10, 1.0f);
                    VdDrawAxis(&camera);
                }
                //VdPointsDrawAll(points);
                VdPointsTraceAll(points);
            }
            EndMode3D();
        }
        EndDrawing();
    }
    
    // Cleanup
    CloseWindow();
    free(points);
    points = NULL;
    return 0;
}
