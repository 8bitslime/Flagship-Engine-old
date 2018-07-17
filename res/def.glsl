
#ifdef VERTEX

layout (location = 0) in vec3 pos;
uniform mat4 perspective;

void main() {
	gl_Position = perspective * vec4(pos, 1);
}

#endif

#ifdef FRAGMENT

out vec4 out_color;

void main() {
	out_color = vec4(0, 1, 1, 1);
}

#endif
