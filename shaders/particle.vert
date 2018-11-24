#version 330 core
layout (location = 0) in vec3 vertex;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
uniform float factor;

smooth out vec2 texCoords;

void main(){

    mat4 ModelView = view * model;

    // Column 0:
    ModelView[0][0] = 1;
    ModelView[0][1] = 0;
    ModelView[0][2] = 0;

    // Column 1:
    ModelView[1][0] = 0;
    ModelView[1][1] = 1;
    ModelView[1][2] = 0;

    // Column 2:
    ModelView[2][0] = 0;
    ModelView[2][1] = 0;
    ModelView[2][2] = 1;

    texCoords = vertex.xy/factor;
    vec4 view_vertex = ModelView * vec4(vertex, 1);
    gl_Position = projection * view_vertex;
}
