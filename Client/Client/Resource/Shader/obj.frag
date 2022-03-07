#version 450

uniform sampler2D u_texture;

uniform bool u_start_with_dark;
uniform bool u_focus_center;
uniform int u_raw_col;

uniform int u_type = 18; //[0,19]
uniform vec2 u_position = vec2(2, 1);

in vec2 texcoord;

out vec4 FragColor;

void main()
{
	vec2 unit = vec2(0.2, 0.25);

	vec2 tex = texcoord;
	vec2 clamp_sp = vec2(mod(u_type, 5), floor(u_type / 5)) * unit;
	vec2 clamp_ep =  clamp_sp + unit;
	
	vec2 ratio = float(u_raw_col) * unit;
	tex *= ratio;
	
	tex += clamp_sp;
	
	vec2 pos_diff = vec2(-u_position.x, u_position.y - u_raw_col + 1) * unit;
	tex += pos_diff;
	
	tex += (unit/2 * float(u_focus_center));

	if(tex.x < clamp_sp.x || tex.y < clamp_sp.y
	|| tex.x > clamp_ep.x || tex.y > clamp_ep.y)
		discard;

	

//	tex *= float(u_raw_col) / 2;

//	tex += (unit/2 * float(u_focus_center));
//	tex.x += float(u_start_with_dark) * unit;


	vec4 color = texture(u_texture, tex);
	if( color.a < 0.1) discard;

	FragColor = texture(u_texture, tex) + vec4(0.1f);
}
