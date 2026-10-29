
.name = N_("Introduction"),
.bgm = "intro",

.phases = (CutscenePhase[]) {
	// 자전거 의인화 테마: 프롤로그를 바퀴 자국·방울 소리를 남기는 '라이더' 이야기로 다시 씀.
	// 라이더의 정체·역할은 미확정이라 색과 소리로만 암시한다. 이미지는 새 그림 수령 전까지 기존 그림을 쓴다.
	{ "cutscenes/locations/hakurei", {
		T_NARRATOR("— The Hakurei Shrine\n"),
		T_NARRATOR("The shrine at the border of fantasy and reality.\n"),
		T_NARRATOR("Three heroines of Gensōkyō sat drinking tea on a calm day at the end of summer.\n"),
		T_NARRATOR("There was something unsettling in the air—"),
		{},
	}},
	{ "cutscenes/opening/01", {
		T_NARRATOR("*Ring-ring.* A bright little bell, then the whir of spinning wheels racing past the torii.\n"),
		T_MARISA("Whoa! Somethin’ just zoomed right past the gate!"),
		T_REIMU("And it didn’t even stop to make a donation."),
		{},
	}},
	{ "cutscenes/opening/01", {
		T_YOUMU("There are tracks in the dirt. Two thin lines, perfectly straight."),
		T_REIMU("Too narrow for a cart… What leaves a trail like that?"),
		T_MARISA("Beats me. But whoever it was, they sure were in a hurry."),
		{},
	}},
	{ "cutscenes/opening/02", {
		T_NARRATOR("Over the next few days, the same tracks appeared all across Gensōkyō.\n"),
		T_NARRATOR("Along the shore of Misty Lake, through the Forest of Magic, up mountain passes no human dared to climb.\n"),
		T_NARRATOR("Those who glimpsed the riders could describe only a blur of color— red and black, white and lavender, navy and gold, mint and cream, burgundy and ivory."),
		{},
	}},
	{ "cutscenes/opening/02", {
		T_REIMU("Wheels, huh? Since when do yōkai need wheels to get around?"),
		T_YOUMU("Perhaps they aren’t yōkai at all."),
		T_MARISA("Ghosts, gods, whatever— if it’s fast, I wanna race it!"),
		{},
	}},
	{ "cutscenes/opening/03", {
		T_NARRATOR("Every set of tracks led the same way— up toward Yōkai Mountain.\n"),
		T_NARRATOR("And the closer one came to the mountain, the stronger that unsettling feeling grew.\n"),
		T_REIMU("So it all points to the mountain again. Figures."),
		T_YOUMU("Then the riders and this strange air may be connected."),
		{},
	}},
	{ "cutscenes/opening/04", {
		T_NARRATOR("*Ring-ring.* The bell sounded once more— from far down the shrine road, as if waiting for them.\n"),
		T_MARISA("Hear that? They’re callin’ us out!"),
		T_YOUMU("Then we shouldn’t keep them waiting."),
		T_REIMU("Fine. Let’s go see who’s been tearing up my road."),
		{},
	}},
	{ "cutscenes/locations/hakurei", {
		T_NARRATOR("And so the three heroines set out, following two thin lines in the dirt…\n"),
		T_NARRATOR("toward a race that would carry them to the very top of Yōkai Mountain."),
		{},
	}},
	{}
}
