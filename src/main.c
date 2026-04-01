#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GLFW_INCLUDE_VULKAN
#define RAYMATH_IMPLEMENTATION

/////////////////////////////////
// CHANGE FUNCTIONS INPUT HERE //
// Obs.: Use 't' as the scalar //
// and 'm' as controllable val //
/////////////////////////////////
#define X_FUNC_INPUT cosf(t)
#define Y_FUNC_INPUT m * t
#define Z_FUNC_INPUT sinf(t)
// To visualize different functions, modify the return expression.
// Examples:
// - Circle: (cos(t), sin(t), 0)
// - Sine wave: (t, sin(t), 0) 
// - Spiral: (t * cos(t), t, t * sin(t))
// - Lissajous curve: (sin(3t), sin(2t), 0)

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

// Initialize graph resolution (can be ONLY DECREASED at runtime)
// Warning: if u increase the value of actual_resolution 
// above VD_GRAPH_RESOLUTION the points buffer is gonna segfault!
static size_t actual_resolution = VD_GRAPH_RESOLUTION;

// Window configuration structure
typedef struct {
    int display_id;          // Monitor identifier
    int screen_width;        // Window width in pixels
    int screen_height;       // Window height in pixels
    char *title;             // Window title
    Font custom_font;         // Window font
} VdManagerWindow;

// Change the values for the formula here, 
// use 't' as the function scalar input
static float custom_param = 1.0f;
Vector3 VdCalculatePointValue(float t)
{
    float m = custom_param; // Obs.: this variable links the global value and defined custom parameter
    return (Vector3){
        .x = X_FUNC_INPUT,      // Circular motion in X-Z plane
        .y = Y_FUNC_INPUT,      // Linear progression along Y axis
        .z = Z_FUNC_INPUT,      // Circular motion in X-Z plane
    };
}

// Draw control information overlay on screen
void VdDrawControlText(VdManagerWindow* window)
{
    const char* info[] = {
        "ESC to quit",
        "",
        "F - Toggle cinematic mode",
        "G - Reset view",
        "D - Toggle UI",
        "",
        "E, R - Control point plot quantity",
        "Q, W - Control graph custom parameter",
        "",
        "Mouse scroll - Zoom in/out",
        "Mouse middle button - Pan camera",
        "Mouse left click - Rotate camera",
    };
    const int screen_h = GetScreenHeight();

    for (size_t i = 0; i < (sizeof(info) / sizeof(info[0])); ++i) {
        int y = screen_h - (VD_DEBUG_TEXT_SIZE * (i + 1)) - VD_DEBUG_TEXT_MARGIN;
        DrawTextEx(window->custom_font, info[i], (Vector2){VD_DEBUG_TEXT_PADDING, (float)y}, VD_DEBUG_TEXT_SIZE, VD_FONT_SPACING, LIME);
    }
}

// Draw FPS counter with color coding
void VdDrawDebugInfo(VdManagerWindow* window)
{
    Color color = LIME;                         // Good FPS
    int fps = GetFPS();

    if ((fps < 30) && (fps >= 15)) color = ORANGE;  // Warning FPS
    else if (fps < 15) color = RED;                 // Low FPS

        // Format the text strings
    char fpsText[32];
    char pointsText[64];
    sprintf(fpsText, "%2i FPS", fps);
    sprintf(pointsText, "%zu POINTS BEING PLOTTED", actual_resolution);
    
    // Draw text with custom font using DrawTextEx
    // DrawTextEx(font, text, position, fontSize, spacing, color)
    DrawTextEx(window->custom_font, fpsText, (Vector2){ 20.0f, 20.0f }, 20.0f, 2.0f, color);
    DrawTextEx(window->custom_font, pointsText, (Vector2){ 20.0f, 45.0f }, 20.0f, 2.0f, color);
}

// Draw lines connecting all points to form a continuous curve
void VdPointsTraceAll(Vector3* points)
{
    for (size_t i = 1; i < actual_resolution; ++i) {
        DrawLine3D(points[i-1], points[i], YELLOW);
    }
}

// Draw individual points for debugging (commented out by default for performance)
void VdPointsDrawAll(Vector3* points)
{
    for (size_t i = 0; i < actual_resolution; ++i) {
        DrawPoint3D(points[i], YELLOW);
    }
}

// Calculate all points for the graph based on current range and resolution
void VdPointsCalculateValue(Vector3* points) {
    float range = VD_RANGE_END - VD_RANGE_START;  // Total parameter range
    float step = range / actual_resolution;       // Step size between points
    
    for (size_t i = 0; i < actual_resolution; ++i) {
        float t = VD_RANGE_START + step * i;       // Current parameter value
        points[i] = VdCalculatePointValue(t);
    }
}

// Handle user input for orbital camera control with smooth inertia
void VdGetUserInput(Camera3D* camera, Vector3* orbitTarget, bool* cinematicMode, bool* hideUI)
{
    // Static variables maintain state between frames for smooth motion
    static float angleX = 0;                    // Current horizontal angle (radians)
    static float angleY = M_PI / 3;             // Current vertical angle (radians)
    static float targetAngleX = 0;              // Target horizontal angle
    static float targetAngleY = M_PI / 3;       // Target vertical angle
    static float currentRadius = 15.0f;         // Current zoom distance
    static float targetRadius = 15.0f;          // Target zoom distance
    
    // Zoom control
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        targetRadius -= wheel * VD_CAMERA_ZOOM_SPEED;
        // Clamp zoom limits to prevent going through target or getting too far
        if (targetRadius < VD_CAMERA_ZOOM_MIN) targetRadius = VD_CAMERA_ZOOM_MIN;
        if (targetRadius > VD_CAMERA_ZOOM_MAX) targetRadius = VD_CAMERA_ZOOM_MAX;
    }
    
    // Smoothly interpolate radius (easing effect)
    currentRadius += (targetRadius - currentRadius) * VD_CAMERA_INERTIA_FACTOR;
    
    // Rotation control
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mouseDelta = GetMouseDelta();   // Get mouse movement since last frame
        
        // Apply sensitivity and update target angles
        targetAngleX += mouseDelta.x * VD_CAMERA_ROTATION_SENSITIVITY;
        targetAngleY += mouseDelta.y * VD_CAMERA_ROTATION_SENSITIVITY;
        
        // Clamp vertical angle to prevent gimbal lock and camera flipping
        // Limit to just below 90 degrees in both directions
        if (targetAngleY > M_PI / 2 - 0.01f) targetAngleY = M_PI / 2 - 0.01f;
        if (targetAngleY < -M_PI / 2 + 0.01f) targetAngleY = -M_PI / 2 + 0.01f;
    }
    
    // Smooth angle interpolation (inertia/damping effect)
    angleX += (targetAngleX - angleX) * VD_CAMERA_INERTIA_FACTOR;
    angleY += (targetAngleY - angleY) * VD_CAMERA_INERTIA_FACTOR;
    
    // Spherical coordinates to Cartesian conversion
    camera->position.x = orbitTarget->x + currentRadius * cosf(angleY) * cosf(angleX);
    camera->position.y = orbitTarget->y + currentRadius * sinf(angleY);
    camera->position.z = orbitTarget->z + currentRadius * cosf(angleY) * sinf(angleX);
    camera->target = *orbitTarget;
    
    // Enable cinematic mode
    if (IsKeyPressed(KEY_F)) {
        *cinematicMode = !(*cinematicMode);
    }
    if (*cinematicMode) {
        targetAngleX += VD_CINEMATIC_ROTATION_SPEED;
    }

    // Toggle UI
    if (IsKeyPressed(KEY_D)) {
        *hideUI = !(*hideUI);
    }
    
    // Reset camera
    if (IsKeyPressed(KEY_G)) {
        targetAngleX = 0;
        targetAngleY = M_PI / 3;
        targetRadius = 15.0f;
        angleX = targetAngleX;
        angleY = targetAngleY;
        currentRadius = targetRadius;
        camera->position = VD_CAMERA_INITIAL_POS;
        orbitTarget->x = 0;
        orbitTarget->y = 0;
        orbitTarget->z = 0;
    }
    
    // Pan Control 
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || 
        (IsKeyDown(KEY_LEFT_ALT) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))) {
        Vector2 mouseDelta = GetMouseDelta();
        float panSpeed = VD_CAMERA_PAN_SPEED * currentRadius;
        
        // Calculate camera-relative movement vectors
        Vector3 forward = Vector3Normalize(Vector3Subtract(camera->target, camera->position));
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera->up));
        Vector3 up = camera->up;
        
        // Move orbit target based on mouse movement
        orbitTarget->x -= (mouseDelta.x * right.x + mouseDelta.y * up.x) * panSpeed;
        orbitTarget->y += (mouseDelta.x * right.y + mouseDelta.y * up.y) * panSpeed;
        orbitTarget->z -= (mouseDelta.x * right.z + mouseDelta.y * up.z) * panSpeed;
    }

    // Points-to-plot control
    if (IsKeyDown(KEY_R) && actual_resolution < VD_GRAPH_RESOLUTION) ++actual_resolution;
    if (IsKeyDown(KEY_E) && actual_resolution > 1) --actual_resolution;

    // Custom parameter 'm'
    if (IsKeyDown(KEY_W)) custom_param += 0.01f;
    if (IsKeyDown(KEY_Q) && custom_param > 0.01f) custom_param -= 0.01f;

    // Exit
    if (IsKeyDown(KEY_ESCAPE)) CloseWindow();
}

// Calculate appropriate axis size based on camera distance
float VdCalculateAxisSize(Vector3 x, Vector3 y)
{
    return Vector3Distance(x, y) * 0.5f;
}

// Draw RGB coordinate axes at origin
void VdDrawAxis(Camera3D* camera)
{
    float axisSize = VdCalculateAxisSize(VD_VECTOR3_NULL, camera->position);
    
    DrawLine3D(VD_VECTOR3_NULL, (Vector3){axisSize, 0, 0}, RED);    // X-axis
    DrawLine3D(VD_VECTOR3_NULL, (Vector3){0, axisSize, 0}, GREEN);  // Y-axis
    DrawLine3D(VD_VECTOR3_NULL, (Vector3){0, 0, axisSize}, BLUE);   // Z-axis
}

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
