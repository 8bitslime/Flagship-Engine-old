
#ifdef VERTEX

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 text_coord;
uniform mat4 perspective;

out vec3 normal;
out vec2 uv;

void main() {
	gl_Position = perspective * vec4(pos, 1);
	normal = (perspective * vec4(norm, 0)).xyz;
	uv = text_coord;
}

#endif

#ifdef FRAGMENT

in vec3 normal;
in vec2 uv;

uniform sampler2D image;

out vec4 out_color;

void main() {
	out_color = texture(image, uv) * dot(normal, vec3(0, 0, -1));
	out_color.a = 1;
}

#endif
