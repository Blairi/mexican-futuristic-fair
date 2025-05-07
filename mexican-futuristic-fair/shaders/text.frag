#version 330 core
in vec2 TexCoord;
uniform sampler2D text;
uniform vec3 textColor;
uniform vec2 offset;
out vec4 FragColor;
void main() {
    vec4 sampled = texture(text, TexCoord + offset);
    FragColor = vec4(textColor, 1.0) * sampled.a;
}
