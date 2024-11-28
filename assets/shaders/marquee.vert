#version 150

in vec4 p3d_Vertex; // Vertex position in object space
uniform mat4 p3d_ModelViewProjectionMatrix; // Combined model-view-projection matrix

void main() {
    // Transform vertex to clip space (NDC)
    gl_Position = p3d_ModelViewProjectionMatrix * p3d_Vertex;
}
