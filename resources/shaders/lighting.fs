#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform vec2 u_resolution;
uniform vec2 player_pos;
uniform vec2 u_mouse;
uniform float max_alpha;
uniform vec3 u_colour;

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

    if (dist > max_alpha) {
      dist = max_alpha;
    }


    finalColor = vec4(u_colour, dist);
}
