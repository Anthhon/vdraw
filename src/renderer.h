#ifndef RENDERER_H_
#define RENDERER_H_

#include <stdio.h>
#include <raylib.h>

// Graph rendering parameters
#define VD_MAX_FPS 240
#define VD_RANGE_START -20 // Starting point of the graph (can be negative)
#define VD_RANGE_END    20 // Ending point of the graph
#define VD_GRAPH_RESOLUTION 1024 // Number of points to plot (higher = smoother curve)
#define VD_VECTOR3_NULL (Vector3){ 0.0f, 0.0f, 0.0f }

// UI parameters
#define VD_DEBUG_TEXT_SIZE    20 // Font size for debug text
#define VD_DEBUG_TEXT_MARGIN  20 // Margin from screen edges
#define VD_DEBUG_TEXT_PADDING 25 // Padding from left edge

// Camera parameters
#define VD_CAMERA_INITIAL_POS ((Vector3){ 0.0f, 10.0f, 10.0f })
#define VD_CAMERA_INITIAL_TARGET ((Vector3){ 0.0f, 0.0f, 0.0f })
#define VD_CAMERA_FOV 90.0f // Field of view in degrees
#define VD_CAMERA_ZOOM_MIN 2.0f // Minimum zoom distance
#define VD_CAMERA_ZOOM_MAX 50.0f // Maximum zoom distance
#define VD_CAMERA_ZOOM_SPEED 1.0f // Zoom sensitivity
#define VD_CAMERA_PAN_SPEED 0.02f // Panning sensitivity
#define VD_CAMERA_ROTATION_SENSITIVITY 0.005f // Mouse rotation sensitivity
#define VD_CAMERA_INERTIA_FACTOR 0.15f // Smoothing factor (0 = max smoothing, 1 = no smoothing)
#define VD_CINEMATIC_ROTATION_SPEED 0.002f // Auto-rotation speed in cinematic mode

// Custom font parameters
#define VD_FONT_PATH "/usr/share/fonts/truetype/freefont/FreeSans.ttf" // Path to your custom file
#define VD_FONT_SIZE 32  // Base font size for the font atlas
#define VD_FONT_SPACING 2  // Character spacing

// Window configuration structure
typedef struct {
    int display_id;          // Monitor identifier
    int screen_width;        // Window width in pixels
    int screen_height;       // Window height in pixels
    char *title;             // Window title
    Font custom_font;         // Window font
} VdManagerWindow;

Vector3 VdCalculatePointValue(float t);
void VdDrawControlText(VdManagerWindow* window);
void VdDrawDebugInfo(VdManagerWindow* window);
void VdPointsTraceAll(Vector3* points);
void VdPointsDrawAll(Vector3* points);
void VdPointsCalculateValue(Vector3* points);
void VdGetUserInput(Camera3D* camera, Vector3* orbitTarget, bool* cinematicMode, bool* hideUI);
float VdCalculateAxisSize(Vector3 x, Vector3 y);
void VdDrawAxis(Camera3D* camera);

#endif // RENDERER_H_
