
#ifdef VERTEX

in vec2 pos;
out vec2 uv;
layout (location = 1) in vec2 character;
layout (location = 2) in vec2 position;
uniform mat4 ortho;

void main() {
	gl_Position = ortho * vec4(pos*16 + position, 0, 1);
	uv = pos*(1./16.) + (character/16.);
}

#endif

#ifdef FRAGMENT

in vec2 uv;
uniform sampler2D font;
uniform vec3 color;
out vec4 out_color;

void main() {
	vec4 sample = texture(font, uv);
	vec4 textColor = sample * vec4(color.rgb, 1);
	out_color = mix(vec4(textColor.rgb, 0), textColor, sample.r * 2);
}

#endif
