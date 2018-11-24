#version 330 core

out vec4 frag_color;
uniform vec3 color;
uniform bool amICube;

smooth in float normalColor;

void main (void) {
   // write Total Color:
    if (amICube) frag_color = vec4(1, 1, 1, 1) - vec4(color*normalColor, 0);
    else frag_color = vec4(sqrt(color*normalColor), 1);
}
