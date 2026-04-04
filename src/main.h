#ifndef MAIN_H_
#define MAIN_H_

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

int main(void);

#endif // MAIN_H_
