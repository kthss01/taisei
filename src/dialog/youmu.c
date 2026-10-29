/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2026, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2026, Andrei Alexeyev <akari@taisei-project.org>.
 */

#include "dialog_macros.h"
#include "i18n/i18n.h"

/*
 * Stage 1
 */

DIALOG_TASK(youmu, Stage1PreBoss) {
	DIALOG_BEGIN(Stage1PreBoss);

	ACTOR_LEFT(youmu);
	ACTOR_RIGHT(cirno);
	HIDE(cirno);

	FACE(youmu, happy);
	MSG(youmu, "I suppose falling snow can be just as pretty as cherry blossoms…");

	EVENT(boss_appears);
	// 자전거 의인화 테마 1차 시험: 스테이지 1 보스는 MINIVELO(임시 이름). 내부 id cirno는 유지한다.
	MSG(cirno, "Hey!");
	SHOW(cirno);

	MSG(cirno, "This path’s mine today! Got a ticket, sword girl?");
	FACE(youmu, unamused);
	MSG(youmu, "A ticket? For a lake?");
	FACE(youmu, smug);
	MSG(youmu, "I tend to dislike dueling those weaker than myself, so I’d appreciate it if you’d step aside.");

	TITLE_CIRNO(cirno);
	MSG(cirno, "Hah! Maybe YOU should step aside, then!");

	FACE(youmu, eyes_closed);
	MSG(youmu, "I suppose it’s as good a time as any to practice my Snowflake-Destruction Technique.");

	EVENT(music_changes);
	FACE(cirno, angry);
	MSG(cirno, "Then I’ll bury you in snow and ride right over you!");

	DIALOG_END();
}

DIALOG_TASK(youmu, Stage1PostBoss) {
	DIALOG_BEGIN(Stage1PostBoss);

	ACTOR_LEFT(youmu);
	ACTOR_RIGHT(cirno);
	VARIANT(cirno, defeated);
	FACE(cirno, defeated);

	FACE(youmu, sigh);
	MSG(youmu, "The cold is no good for such fine blades…");

	MSG(cirno, "Y-you should be careful with t-those!");

	FACE(youmu, unamused);
	MSG(youmu, "Yes, that’s what I was just saying.");

	DIALOG_END();
}

/*
 * Stage 2
 */

// 자전거 테마: 클라이머 대사
DIALOG_TASK(youmu, Stage2PreBoss) {
	DIALOG_BEGIN(Stage2PreBoss);

	ACTOR_LEFT(youmu);
	ACTOR_RIGHT(hina);
	HIDE(hina);

	FACE(youmu, normal);
	MSG(youmu, "So this is Yōkai Mountain…");
	MSG(youmu, "A shame, I’ve heard this is usually quite a lovely place.");

	EVENT(boss_appears);
	MSG(hina, "…Shh. Listen.");
	SHOW(hina);
	MSG(hina, "The mountain is breathing hard today. Who are you?");

	MSG(youmu, "Konpaku Yōmu. I am on a mission from Lady Yuyuko to investigate—");

	TITLE_HINA(hina);
	MSG(hina, "Two swords. A half-phantom. That’s a lot of weight for this climb.");
	FACE(youmu, normal);
	FACE(hina, serious);
	MSG(hina, "Turn back. The summit doesn’t forgive extra grams.");

	FACE(youmu, embarrassed);
	MSG(youmu, "‘E-Extra grams’?! I’ll have you know these blades are perfectly balanced—");
	MSG(youmu, "—forged and honed over *decades*—");
	MSG(youmu, "—dare I say the finest steel in Gensōkyō—");

	FACE(hina, normal);
	MSG(hina, "Steel is heavy.");
	MSG(hina, "And the ghost. Does it count as luggage?");

	FACE(youmu, unamused);
	MSG(youmu, "It is NOT luggage! Both halves of me together weigh barely forty ki—");
	FACE(youmu, embarrassed);
	MSG(youmu, "…!!");

	EVENT(music_changes);

	DIALOG_END();
}

// 자전거 테마: 클라이머 대사
DIALOG_TASK(youmu, Stage2PostBoss) {
	DIALOG_BEGIN(Stage2PostBoss);

	ACTOR_LEFT(youmu);
	ACTOR_RIGHT(hina);
	VARIANT(hina, defeated);
	FACE(hina, defeated);

	MSG(hina, "Did you just tell me your weight…?");

	FACE(youmu, eyes_closed);
	MSG(youmu, "I did not.");
	MSG(youmu, "It must be the thin mountain air playing tricks on your ears.");
	MSG(youmu, "Yes, indeed.");

	FACE(hina, normal);
	MSG(hina, "…Good power-to-weight ratio.");

	FACE(youmu, unamused);
	MSG(youmu, "Don’t you dare write that down.");

	DIALOG_END();
}

/*
 * Stage 3
 */

// 자전거 테마: 랑도너 대사
DIALOG_TASK(youmu, Stage3PreBoss) {
	DIALOG_BEGIN(Stage3PreBoss);

	ACTOR_LEFT(youmu);
	ACTOR_RIGHT(wriggle);
	HIDE(wriggle);
	FACE(youmu, relaxed);
	MSG(youmu, "The further I go into this forest, the more restless I feel.");
	FACE(youmu, normal);
	MSG(youmu, "Something is definitely amiss.");

	EVENT(boss_appears);
	MSG(wriggle, "Amiss? Nope!");
	SHOW(wriggle);
	MSG(wriggle, "You’re right on course! This is checkpoint three of the Great Night Brevet!");

	FACE(youmu, eyes_closed);
	MSG(youmu, "Races do not concern me at the moment.");

	FACE(wriggle, outraged);
	MSG(wriggle, "A race?! A brevet is NOT a race, it’s a journey! I see how it is!");
	TITLE_WRIGGLE(wriggle);
	MSG(wriggle, "Ever heard of control cards? Night lamps? Napping in a ditch at 3 AM? Show some respect!");

	FACE(youmu, normal);
	MSG(youmu, "Apologies, I meant no offense.");
	FACE(youmu, eyes_closed);
	MSG(youmu, "I am on a mission from Lady Yuyuko to end whatever is causing the disturbance—");

	FACE(wriggle, proud);
	MSG(wriggle, "Oh, oh, I get it now!");
	FACE(wriggle, calm);
	MSG(wriggle, "A thousand riders rolling through the night by starlight would surely raise some concerns.");
	FACE(wriggle, proud);
	MSG(wriggle, "Simply put, we’re charting a route that goes all the way up to the stars!");
	MSG(wriggle, "Of course folks down the mountain get nervous when that many headlamps come their way!");
	FACE(wriggle, calm);
	MSG(wriggle, "Don’t worry, I’ll stamp your card too. You’re welcome to ride along—");

	FACE(youmu, unamused);
	MSG(youmu, "… no, no, a few cyclists wouldn’t cause this kind of disturbance…");
	FACE(youmu, eyes_closed);
	MSG(youmu, "All they’re capable of is leaving tire tracks across my carefully raked garden paths.");

	MSG(wriggle, "Did you just say you…");
	FACE(wriggle, outraged);
	MSG(wriggle, "… don’t want your card stamped?!");

	EVENT(music_changes);

	FACE(youmu, puzzled);
	MSG(wriggle, "Skipping a checkpoint is the one thing a randonneur can never forgive!");
	MSG(wriggle, "My lamp shines brighter than ever now! I’ll guide you to every control point by force!");
	MSG(wriggle, "Follow my map, and no one will ever get lost under these stars again!");

	DIALOG_END();
}

// 자전거 테마: 랑도너 대사
DIALOG_TASK(youmu, Stage3PostBoss) {
	DIALOG_BEGIN(Stage3PostBoss);

	ACTOR_LEFT(youmu);
	ACTOR_RIGHT(wriggle);
	VARIANT(wriggle, defeated);
	FACE(wriggle, defeated);

	FACE(youmu, eyes_closed);
	MSG(youmu, "Are you aware of the legend of the Tonbokiri?");
	MSG(youmu, "It was said its blade could sever anything that so much as touched its edge.");
	FACE(youmu, smug);
	MSG(youmu, "Undoubtedly, your map met the same fate today.");

	MSG(wriggle, "That’s not the route I planned…");

	MSG(youmu, "How unfortunate for you.");
	FACE(youmu, chuuni);
	MSG(youmu, "Fear not. I will end your delusional madness and restore justice and order to the—");
	FACE(youmu, eeeeh);
	MSG(youmu, "… ugh, that’s a little much, even for me, isn’t it?");

	MSG(wriggle, "Seems about right, honestly… I’ll stamp your card anyway.");

	FACE(youmu, unamused);
	MSG(youmu, "…");

	DIALOG_END();
}

/*
 * Stage 4
 */

// 자전거 테마: 클래식 대사
DIALOG_TASK(youmu, Stage4PreBoss) {
	DIALOG_BEGIN(Stage4PreBoss);

	ACTOR_LEFT(youmu);
	ACTOR_RIGHT(kurumi);
	HIDE(kurumi);

	MSG(youmu, "A-ha, I must be close to the true entrance now.");
	FACE(youmu, eyes_closed);
	MSG(youmu, "This tower… I can already feel it…");
	FACE(youmu, chuuni);
	MSG(youmu, "Could this be a challenge worthy of my skills…?!");

	EVENT(boss_appears);
	SHOW(kurumi);
	MSG(kurumi, "Oh my, such a hurry. You’ll never enjoy the ride at that pace, dear.");
	FACE(youmu, relaxed);
	MSG(youmu, "Who might you be?");

	TITLE_KURUMI(kurumi);
	FACE(kurumi, tsun);
	MSG(kurumi, "I am CLASSIC. Steel frame, wicker basket, and a bell I polish every morning.");
	MSG(kurumi, "Do remember that, won’t you?");

	FACE(youmu, eyes_closed);
	MSG(youmu, "Perhaps I will.");
	FACE(youmu, normal);
	MSG(youmu, "Are you an associate of this… mansion-tower’s denizens, then?");

	FACE(kurumi, dissatisfied);
	MSG(kurumi, "Goodness, no. Whatever gave you that idea?");
	MSG(kurumi, "I would never associate myself with the sort who run this mansion.");
	MSG(kurumi, "They ride in clip-in shoes and drink their tea from plastic bottles. Unthinkable.");

	FACE(youmu, eyes_closed);
	MSG(youmu, "I see. I have no immediate reason to distrust you.");
	FACE(youmu, puzzled);
	MSG(youmu, "… what is a ‘clip-in shoe’, though, if I may ask?");

	FACE(kurumi, normal);
	MSG(kurumi, "I haven’t the faintest idea, dear.");
	MSG(kurumi, "The lady in the tower speaks of them constantly, so they must be dreadful indeed.");

	FACE(youmu, unamused);
	MSG(youmu, "Didn’t you just say you weren’t associated with them?");

	MSG(kurumi, "Ah, yes. That was a little fib, to see how gullible you were. More tea?");

	FACE(youmu, embarrassed);
	MSG(youmu, "O-of course. I-I knew that.");
	FACE(youmu, normal);
	MSG(youmu, "I wanted to see if you’d come clean.");
	FACE(youmu, eyes_closed);
	MSG(youmu, "My true challenge lies beyond you. Step aside.");

	EVENT(music_changes);

	MSG(kurumi, "I’m afraid not. You have a slow waltz around the garden scheduled first.");
	MSG(kurumi, "Mind the bell, hurried swordsgirl!");

	DIALOG_END();
}

// 자전거 테마: 클래식 대사
DIALOG_TASK(youmu, Stage4PostBoss) {
	DIALOG_BEGIN(Stage4PostBoss);

	ACTOR_LEFT(youmu);
	ACTOR_RIGHT(kurumi);
	VARIANT(kurumi, defeated);
	FACE(kurumi, defeated);

	FACE(youmu, eyes_closed);
	MSG(youmu, "Despite your leisurely technique, I must thank you.");
	MSG(youmu, "‘Waltzing’ with you has been a pleasant distraction on the path to completing my mission.");
	FACE(youmu, chuuni);
	MSG(youmu, "Tell your ‘leader’ that I am coming for them.");

	MSG(kurumi, "(She speaks like the hero of a penny novel… how charming.)");
	FACE(kurumi, normal);
	MSG(kurumi, "Oh, very well, dear.");
	FACE(kurumi, defeated);
	MSG(kurumi, "I’m quite weary of being parked in this creepy, spooky mansion anyway.");

	FACE(youmu, unamused);
	MSG(youmu, "S-… spooky?");

	FACE(kurumi, puzzled);
	MSG(kurumi, "…?");
	FACE(kurumi, normal);
	MSG(kurumi, "… why, yes! I expected a proper bicycle shed, but they made it haunted, with ghosts and such!");

	FACE(youmu, eeeeh);
	MSG(youmu, "G-…");
	MSG(youmu, "GHOSTS?!");

	MSG(kurumi, "Dreadful vengeful ghosts! They ring their bells in the dark, even!");
	MSG(kurumi, "But surely a strong swordsgirl like yourself could handle it.");

	FACE(youmu, embarrassed);
	MSG(youmu, "B-Bring it on! Nothing will stand in my way!");

	FACE(kurumi, tsun);
	MSG(kurumi, "(Hehe, so gullible. Now, where did I leave the teapot?)");

	DIALOG_END();
}

/*
 * Stage 5
 */

// 자전거 테마: 에어로 대사
DIALOG_TASK(youmu, Stage5PreBoss) {
	DIALOG_BEGIN(Stage5PreBoss);

	ACTOR_LEFT(youmu);
	ACTOR_RIGHT(iku);
	HIDE(iku);

	FACE(youmu, relaxed);
	MSG(youmu, "Incredible. Truly astounding.");
	FACE(youmu, eyes_closed);
	MSG(youmu, "Yes, the final battle approaches. I can feel it.");
	FACE(youmu, chuuni);
	MSG(youmu, "Will it be a legendary duel? One to go down in the history of Gensōkyō?!");

	EVENT(boss_appears);
	MSG(iku, "Oh? The gardener from the Netherworld, finally catching up?");
	SHOW(iku);
	MSG(iku, "Took you long enough. I’ve been up here since sunrise.");
	MSG(iku, "Let me guess. Your mistress got blown off course by the Tower’s winds?");

	FACE(youmu, normal);
	MSG(youmu, "Lady Yuyuko is in a safe, undisclosed location.");

	MSG(iku, "The Hakurei Shrine. Downhill, sheltered, free tea. Obviously.");

	FACE(youmu, surprised);
	MSG(youmu, "Wh-who told you?! Are you in league with them too?!");

	FACE(iku, smile);
	MSG(iku, "Nobody. Where else would anyone sit out a headwind like this?");

	FACE(youmu, unamused);
	MSG(youmu, "Hmph. Perhaps we have become too predictable.");
	MSG(youmu, "I am here to settle this matter. You need not concern yourself—");

	FACE(iku, normal);
	MSG(iku, "Wrong. This is my stretch of road.");
	TITLE_IKU(iku);
	FACE(youmu, normal);
	MSG(iku, "I scouted ahead. First one up the tower, of course.");
	MSG(iku, "This tower is driven by a machine…");
	FACE(iku, serious);
	MSG(iku, "… and its wind is blowing well past Gensōkyō’s boundaries.");

	FACE(youmu, normal);
	MSG(youmu, "Including the heavens, I presume? I see.");
	FACE(youmu, eyes_closed);
	MSG(youmu, "Fear not. I will allow nothing to stand in my way.");
	FACE(youmu, chuuni);
	MSG(youmu, "Konpaku Yōmu, the greatest swordswoman to live and die, will settle this incident once and—");

	FACE(iku, smile);
	MSG(iku, "Looks like the machine pumped your ego way past its rated pressure, Swordswoman of Hakugyokurō.");
	FACE(iku, serious);
	MSG(iku, "Blow out on a descent like this and nobody’s catching you.");

	FACE(youmu, eyes_closed);
	MSG(youmu, "Overinflated ego? Nonsense.");
	MSG(youmu, "During my recent battles, I have achieved a flow state most blademasters only dream of!");
	FACE(youmu, chuuni);
	MSG(youmu, "Through that, I have realized the true form of my previously-incomplete yōkai-vanquishing technique!");

	FACE(iku, smile);
	MSG(iku, "Is that so?");

	FACE(youmu, sigh);
	MSG(youmu, "More doubt from ignorant skeptics, I see.");
	FACE(youmu, chuuni);
	MSG(youmu, "Shall we duel so that I may demonstrate to your satisfaction?!");

	FACE(iku, serious);
	MSG(iku, "W-wait, I said ease off the pace, not sprint for the line—");

	MSG(youmu, "I can see it in your expression! The doubt! The fear!");
	MSG(youmu, "But I have become one with my swords, achieving a perfection that no other before me has been able—");

	EVENT(music_changes);

	MSG(iku, "Ugh, fine! You want a sprint? You’ve got one!");
	MSG(iku, "Stay on my wheel if you can! I won’t let you crash out before the top!");

	MSG(youmu, "Finally! Then, witness the overwhelming strength of my Rōkanken!");

	DIALOG_END();
}

DIALOG_TASK(youmu, Stage5PostMidBoss) {
	DIALOG_BEGIN(Stage5PostMidBoss);

	ACTOR_LEFT(youmu);
	FACE(youmu, smug);

	// should be only one message with a fixed 180-frames timeout
	MSG_UNSKIPPABLE(youmu, 180, "Lightning? Hmph. I can outrun it with ease.");

	DIALOG_END();
}

// 자전거 테마: 에어로 대사
DIALOG_TASK(youmu, Stage5PostBoss) {
	DIALOG_BEGIN(Stage5PostBoss);

	ACTOR_LEFT(youmu);
	ACTOR_RIGHT(iku);
	VARIANT(iku, defeated);
	FACE(iku, defeated);

	FACE(youmu, eyes_closed);
	MSG(youmu, "All according to plan.");

	MSG(iku, "You didn’t even draft once… Who rides like that…");
	MSG(iku, "Whatever. The machine’s wind probably got to you long before you got here.");
	MSG(iku, "I don’t like it, but a loss is a loss. The road’s yours.");
	MSG(iku, "Maybe there’s a decent rider hiding under all that ego.");

	MSG(youmu, "And perhaps I truly have gone mad. What of it?");
	MSG(youmu, "My mission is crystal clear:");
	FACE(youmu, chuuni);
	MSG(youmu, "… to carry out Lady Yuyuko’s mission, and to bring peace to Gensōkyō and all connected realms!");
	MSG(youmu, "I will not fail!");

	MSG(iku, "O-okay then.");
	MSG(iku, "Climb the staircase. The one behind all this is waiting at the top.");
	MSG(iku, "Heads up. Her machine is built from parts out of a so-called ‘parallel universe’.");
	MSG(iku, "Nothing I’ve ever raced against. Even I can’t read its line.");

	FACE(youmu, eyes_closed);
	MSG(youmu, "Fear not, I will slice her infernal machine in half like an invasive weed.");

	DIALOG_END();
}

/*
 * Stage 6
 */

DIALOG_TASK(youmu, Stage6PreBoss) {
	DIALOG_BEGIN(Stage6PreBoss);

	ACTOR_LEFT(youmu);
	ACTOR_RIGHT(elly);
	HIDE(elly);

	FACE(youmu, eyes_closed);
	MSG(youmu, "Ah.");
	FACE(youmu, happy);
	MSG(youmu, "Ahaha.");
	FACE(youmu, chuuni);
	MSG(youmu, "Ah-hahaha!");
	MSG(youmu, "Yes. Yes! This is it!");
	MSG(youmu, "What a magnificent arena for a heroic final battle!!");

	EVENT(boss_appears);
	MSG(elly, "Nothing seems to slow you down, hmm?");

	SHOW(elly);
	MSG(elly, "Such an annoyance. But that’s to be expected of the folk of this Gensōkyō.");

	MSG(youmu, "And I see that you’re a master of the blade, a shinigami no less!");
	FACE(youmu, happy);
	MSG(youmu, "Absolutely splendid!");

	TITLE_ELLY(elly);
	MSG(elly, "Ah, I’m no shinigami, especially not now.");

	FACE(youmu, eyes_closed);
	MSG(youmu, "The scythe is a rather particular weapon, you must understand. Forgive me presumptiveness.");

	MSG(elly, "I see. I used to rely on this scythe as a gatekeeper, in the real Gensōkyō.");
	FACE(elly, angry);
	MSG(elly, "Now, it will be the sigil of a new era!");
	FACE(elly, shouting);
	MSG(elly, "One that we, the ‘forgotten’, will reign over!");

	MSG(youmu, "Such an unwieldy weapon… for anyone to choose it willingly…");
	MSG(youmu, "Either you’re incredibly skilled, or incredibly foolish.");
	FACE(youmu, chuuni);
	MSG(youmu, "I wish to find that out!");
	MSG(youmu, "After our duel, let us meet and discuss our techniques!");

	FACE(elly, angry);
	MSG(elly, "Techniques? Hah!");
	MSG(elly, "I’m sure our existence means nothing to you! And the feeling is mutual!");

	FACE(youmu, chuuni);
	MSG(youmu, "On the contrary, scythe-bearer!");
	MSG(youmu, "You’ve created an impeccable stage for our final confrontation.");
	FACE(youmu, eyes_closed);
	MSG(youmu, "But first, please, tell me: what is your mission? What is it you desire?");
	FACE(youmu, chuuni);
	MSG(youmu, "For us to share this… intimate moment, I desire to know everything about you.");

	FACE(elly, blush);
	MSG(elly, "…!");
	MSG(elly, "O-Our mission, is t-to—");

	FACE(elly, eyes_closed);
	MSG(elly, "…");
	FACE(elly, angry);
	MSG(elly, "… is to avenge our forgotten f-friends!");
	MSG(elly, "We will create a new era of Gensōkyō, where the truly forgotten can live another day!");
	MSG(elly, "Soon, the residents of this land will be driven mad beyond comprehension, and be ripe to conquer!");
	FACE(youmu, eyes_closed);
	MSG(youmu, "So you wish to avenge your fallen compatriots… how noble…");
	MSG(youmu, "I can feel it move me to tears…");
	FACE(youmu, chuuni);
	MSG(youmu, "Alas, my mission, given to me by Lady Yuyuko of the Netherworld, directly conflicts with yours!");
	MSG(youmu, "I cannot allow you to drive my compatriots mad, or to subjugate my home!");
	FACE(youmu, eyes_closed);
	MSG(youmu, "Perhaps, in another time and place, we could have been friends!");

	FACE(elly, angry);
	MSG(elly, "B-Being friends with you h-heartless folk would be a sin! I could never!");

	FACE(youmu, chuuni);
	MSG(youmu, "What a tragedy!");

	FACE(elly, shouting);
	MSG(elly, "You’re aiming to be the big ‘hero’ of this Gensōkyō, aren’t you?!");

	MSG(youmu, "Naturally! No different than how you wish to be the hero of ‘your’ Gensōkyō!");
	FACE(youmu, eyes_closed);
	MSG(youmu, "We’re not so different, you and I.");

	FACE(elly, smug);
	MSG(elly, "Your courage is admirable, I’ll give you that.");
	FACE(elly, angry);
	MSG(elly, "But there’s an enormous difference between us, swordswoman.");

	EVENT(music_changes);

	FACE(elly, shouting);
	FACE(youmu, normal);
	MSG(elly, "For I possess the knowledge of science and rationality!");
	MSG(elly, "In a magic-filled world like this, they cut through delusional opponents with unparalleled precision!");
	MSG(elly, "Let’s see what good your swords do you when you fall prey to true enlightenment!");

	DIALOG_END();
}

DIALOG_TASK(youmu, Stage6PreFinal) {
	DIALOG_BEGIN(Stage6PreFinal);

	ACTOR_LEFT(youmu);
	ACTOR_RIGHT(elly);
	VARIANT(elly, beaten);
	FACE(elly, angry);

	FACE(youmu, chuuni);
	MSG(youmu, "Impressive! You’ve pushed me to 10% of my true power!");

	MSG(elly, "Your despicable pride will be your downfall!");
	FACE(elly, shouting);
	MSG(elly, "Behold, your judgement before the true nature of reality!");

	DIALOG_END();
}

/*
 * Register the tasks
 */

#define EXPORT_DIALOG_TASKS_CHARACTER youmu
#include "export_dialog_tasks.inc.h"
