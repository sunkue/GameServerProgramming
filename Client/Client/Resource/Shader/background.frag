#version 450

uniform sampler2D u_texture;

uniform bool u_start_with_dark;
uniform bool u_focus_center;
uniform int u_raw_col;

in vec2 texcoord;

out vec4 FragColor;

void main()
{
	float unit = 0.5;

	vec2 tex = texcoord;
	tex *= float(u_raw_col) / 2;
	tex += (unit/2 * float(u_focus_center));
	tex.x += float(u_start_with_dark) * unit;

	FragColor = texture(u_texture, tex) + vec4(0.1f);
}
