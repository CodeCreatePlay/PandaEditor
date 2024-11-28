#version 150

uniform vec2 start;       // Start position of the marquee (in normalized screen space [-1, 1])
uniform vec2 end;         // End position of the marquee (in normalized screen space [-1, 1])
uniform vec2 viewport;    // Viewport dimensions in pixels

const float BORDER_WIDTH = 0.0025; // Border width in NDC units

out vec4 fragColor;

void main() {
    // Convert gl_FragCoord.xy to normalized device coordinates (NDC)
    vec2 fragCoordNDC = (gl_FragCoord.xy / viewport) * 2.0 - 1.0;

    // Calculate rectangle bounds in NDC space
    float x_min = min(start.x, end.x);
    float y_min = min(start.y, end.y);
    float x_max = max(start.x, end.x);
    float y_max = max(start.y, end.y);

    // Check if the fragment is inside the rectangle
    bool inside = fragCoordNDC.x >= x_min && fragCoordNDC.x <= x_max &&
                  fragCoordNDC.y >= y_min && fragCoordNDC.y <= y_max;

    // Check if the fragment is within the border region
    bool in_left_border   = fragCoordNDC.x >= x_min && fragCoordNDC.x <= x_min + BORDER_WIDTH;
    bool in_right_border  = fragCoordNDC.x <= x_max && fragCoordNDC.x >= x_max - BORDER_WIDTH;
    bool in_bottom_border = fragCoordNDC.y >= y_min && fragCoordNDC.y <= y_min + BORDER_WIDTH;
    bool in_top_border    = fragCoordNDC.y <= y_max && fragCoordNDC.y >= y_max - BORDER_WIDTH;

    bool in_border = in_left_border || in_right_border || in_bottom_border || in_top_border;

    if (inside) {
        if (in_border) {
            fragColor = vec4(0.25, 0.25, 0.25, 1.0); // Red for the border
        } else {
            fragColor = vec4(1.0, 1.0, 1.0, 0.2); // White with transparency for the interior
        }
    } else {
        discard; // Discard fragments outside the rectangle
    }
}
