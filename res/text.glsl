
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
out vec4 color;

void main() {
	vec4 sample = texture(font, uv);
	color = mix(vec4(sample.rgb,0), sample, sample.r * 2);
}

#endif
