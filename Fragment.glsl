#version 400

in vec3 color;
out vec4 frag_color;

void main() {
    // offset 0.5 to prevent lower left corner from being black
    frag_color = vec4(color.r + 0.5, color.g + 0.5, color.b + 0.5, 1.0);
}