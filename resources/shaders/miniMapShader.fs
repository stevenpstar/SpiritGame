#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform int[1024] u_map;
uniform int pp;

out vec4 finalColor;

void main() {

    vec2 relPos = vec2(0, 0);
    vec2 size = vec2(256, 256);

    vec4 col = vec4(1.0, 1.0, 0.0, 1.0);

    if (gl_FragCoord.x > 255 || gl_FragCoord.y > 255) {
      col = vec4(0.0, 0.0, 0.0, 0.0); 
    } else {

    int indx = int(gl_FragCoord.x / 8);
    int indy = int((gl_FragCoord.y / 8));
    int index = indx + indy * 32;

    if (u_map[index] >= 0 && u_map[index] < 32) {
      col = vec4(0.494, 0.639, 0.243, 1.0);
    } else if (u_map[index] == 32) {
      col = vec4(0.259, 0.259, 0.259, 1.0);
    } else if (u_map[index] == 50) {
      col = vec4(0.278, 0.439, 0.0, 1.0);
    } 
  }

    finalColor = col;
}
