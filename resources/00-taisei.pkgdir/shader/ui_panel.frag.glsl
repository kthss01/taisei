#version 330 core

#include "lib/render_context.glslh"
#include "lib/util.glslh"
#include "interface/standard.glslh"

// 메뉴용 둥근 사각형 패널.
// r_draw_quad()로 그린 단위 사각형을 size(px)만큼 늘려 쓴다.
// 반경을 크기의 절반 이상으로 주면 원이 되므로 점 인디케이터에도 쓴다.
UNIFORM(1) vec2 size;          // 패널 크기(px)
UNIFORM(2) float radius;       // 모서리 반경(px)
UNIFORM(3) float border;       // 테두리 두께(px), 0이면 없음
UNIFORM(4) vec4 fill_color;    // 채움색(그라데이션 시작), 알파를 곱하지 않은 값
UNIFORM(5) vec4 fill_color2;   // 채움색(그라데이션 끝)
UNIFORM(6) vec4 border_color;  // 테두리색
UNIFORM(7) vec2 gradient_dir;  // 그라데이션 방향 (0,1) = 위→아래, (1,0) = 왼→오른

float rounded_box_sdf(vec2 p, vec2 half_size, float r) {
	vec2 q = abs(p) - half_size + vec2(r);
	return length(max(q, vec2(0.0))) + min(max(q.x, q.y), 0.0) - r;
}

void main(void) {
	vec2 p = (texCoordRaw - vec2(0.5)) * size;
	float r = min(radius, 0.5 * min(size.x, size.y));
	float d = rounded_box_sdf(p, 0.5 * size, r);

	// 1px 폭 안티앨리어싱
	float coverage = 1.0 - smoothstep(-1.0, 0.0, d);

	if(coverage <= 0.0) {
		discard;
	}

	float t = clamp(dot(texCoordRaw - vec2(0.5), gradient_dir) + 0.5, 0.0, 1.0);
	vec4 fill = mix(fill_color, fill_color2, t);

	vec4 c = fill;

	if(border > 0.0) {
		float inner = 1.0 - smoothstep(-border - 1.0, -border, d);
		c = mix(border_color, fill, inner);
	}

	// 기본 블렌드 모드에 맞춰 알파를 미리 곱해 출력한다.
	fragColor = r_color * color_mul_alpha(c) * coverage;
}
