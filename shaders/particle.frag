#version 330 core
smooth in vec2 texCoords;

out vec4 frag_color;
uniform vec3 color;


void main(){
    float dist = pow(texCoords.x, 2.0f) + pow(texCoords.y, 2.0f);
    if (pow(texCoords.x, 2.0f) + pow(texCoords.y, 2.0f) < pow(.5, 2.0f)) {
        frag_color = vec4(color*(1-dist), 1);
    }
    else {discard;}
}
