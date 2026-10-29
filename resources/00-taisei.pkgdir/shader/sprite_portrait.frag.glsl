#version 330 core

#include "lib/sprite_main.frag.glslh"
#include "lib/util.glslh"

// 캐릭터 선택 화면 초상화 연출. 그림 파일은 그대로 두고 그릴 때만 보정한다.
//
// customParams.x = 바닥 페이드 시작 위치(0~1, 1 이상이면 페이드 없음)
// customParams.y = 림라이트 세기(0이면 계산하지 않음)
// customParams.z = 채도 배율
// customParams.w = 대비 배율
// customMatrix[0].rgb = 림라이트 색, [0].w = 림 두께(텍셀)
// customMatrix[1].xy  = 림 방향(텍스처 좌표계에서 빛이 오는 쪽)

float sample_alpha(vec2 uv, vec4 region) {
	// 아틀라스에서 이웃 스프라이트가 비치지 않도록 자기 영역 안에서만 샘플링한다.
	uv = clamp(uv, region.xy, region.xy + region.zw);
	return texture(tex, uv).a;
}

void spriteMain(out vec4 fragColor) {
	vec4 c = texture(tex, texCoord);
	mat4 cm = customMatrix;

	// 텍스처는 알파를 미리 곱한 상태이므로 보정 전에 되돌린다.
	vec3 rgb = c.a > 0.0 ? c.rgb / c.a : vec3(0.0);

	float l = lum(rgb);
	rgb = mix(vec3(l), rgb, customParams.z);
	rgb = (rgb - vec3(0.5)) * customParams.w + vec3(0.5);
	rgb = clamp(rgb, 0.0, 1.0);

	if(customParams.y > 0.0) {
		vec2 texel = 1.0 / vec2(textureSize(tex, 0));
		vec2 offset = cm[1].xy * texel * cm[0].w;
		float a_far = sample_alpha(texCoord + offset, texRegion);
		float a_near = sample_alpha(texCoord + offset * 0.5, texRegion);
		float rim = clamp(c.a - 0.5 * (a_far + a_near), 0.0, 1.0);
		rim = smoothstep(0.05, 0.6, rim) * customParams.y;
		rgb += cm[0].rgb * rim;
	}

	float fade = 1.0 - smoothstep(customParams.x, 1.0, texCoordRaw.y);

	fragColor = vec4(rgb * c.a, c.a) * color * fade;
}
