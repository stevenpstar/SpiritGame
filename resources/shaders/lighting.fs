#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform vec2 u_resolution;
uniform vec2 player_pos;
uniform vec2 u_mouse;

out vec4 finalColor;

void main() {
    vec2 uv = (gl_FragCoord.xy - u_resolution.xy * 0.5) / min(u_resolution.x, u_resolution.y) * 2.5;
    vec2 p_uv = (player_pos.xy - u_resolution.xy * 0.5) / min(u_resolution.x, u_resolution.y) * 2.5;
    vec2 m_uv = (u_mouse.xy - u_resolution.xy * 0.5) / min(u_resolution.x, u_resolution.y) * 2.5;


    float dist_player = distance(uv, p_uv);
    float dist_mouse = distance(uv, m_uv);
    float dist = dist_mouse;
    if (dist_player < dist_mouse ) {
      dist = dist_player;
    }

    vec3 color = vec3(0.);

    color = vec3(0.004, 0.031, 0.102);
    if (dist > 0.9) {
      dist = 0.9;
    }


    finalColor = vec4(color, dist);
}
