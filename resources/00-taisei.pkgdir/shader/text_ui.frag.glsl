#version 330 core

#include "lib/sprite_main.frag.glslh"
#include "lib/util.glslh"

// 메뉴 글자용. 글리프의 바깥 테두리 채널(g)을 어두운 번짐으로 깔아
// 밝은 배경 위에서도 글자가 읽히게 한다.
void spriteMain(out vec4 fragColor) {
	vec3 glyph = texture(tex, texCoord).rgb;
	vec4 halo = vec4(vec3(0.0), 0.6 * glyph.g * color.a);
	vec4 fill = color * glyph.r;
	fragColor = alphaCompose(halo, fill);
}
