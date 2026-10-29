/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2026, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2026, Andrei Alexeyev <akari@taisei-project.org>.
 */

#include "dialog_macros.h"

/*
 * Stage 1
 */

DIALOG_TASK(reimu, Stage1PreBoss) {
	PRELOAD {
		PRELOAD_CHAR(reimu) {
			PRELOAD_FACE(normal);
			PRELOAD_FACE(unamused);
			PRELOAD_FACE(sigh);
		}

		PRELOAD_CHAR(cirno) {
			PRELOAD_FACE(normal);
			PRELOAD_FACE(angry);
		}
	}

	// Initialization, must be at the very top (after PRELOAD).
	DIALOG_BEGIN(Stage1PreBoss);

	ACTOR_LEFT(reimu);
	ACTOR_RIGHT(cirno);

	// Hide cirno for now, to be revealed later.
	HIDE(cirno);

	// Let’s wait a bit, then change Reimu’s expression.
	// All timings are in frames (60 = 1 sec).
	WAIT_SKIPPABLE(60);
	// "normal" is the default face.
	FACE(reimu, sigh);

	// MSG() makes the actor say a line, and then waits an unspecified amount of time (skippable).
	// The timeout is determined by the dialog_util_estimate_wait_timeout_from_text() function in dialog.c
	MSG(reimu, "Snow during summer? This is certainly familiar…");

	// EVENT()s are handled by stage code.
	// You can find the list of events per dialogue in dialog_interface.h
	// All of them should be signaled eventually.
	EVENT(boss_appears);
	// 자전거 의인화 테마 1차 시험: 스테이지 1 보스는 MINIVELO(임시 이름). 내부 id cirno는 유지한다.
	MSG(cirno, "Ding-ding! Coming through~!");
	// Reveal the boss portrait
	SHOW(cirno);
	FACE(cirno, normal);
	MSG(cirno, "Whoa, a shrine maiden! You’re just in time for my grand tour!");

	FACE(reimu, puzzled);
	MSG(reimu, "A tour? Who are you supposed to be?");
	// Titles are not yet implemented, but this should work once they are.
	// Right now this does nothing.
	TITLE_CIRNO(cirno);
	MSG(cirno, "MINIVELO, at your service! This lake is my new shortcut, so I’m collecting tolls!");

	MSG(reimu, "Tolls? On a frozen lake?");
	FACE(reimu, sigh);
	MSG(reimu, "Anyway, I don’t really care. So if you could just—");
	EVENT(music_changes);

	FACE(reimu, annoyed);
	FACE(cirno, angry);
	MSG(cirno, "No skipping the line! Catch me if you can~!");
	// Teardown, must be at the very bottom.
	DIALOG_END();
}

DIALOG_TASK(reimu, Stage1PostBoss) {
	DIALOG_BEGIN(Stage1PostBoss);

	ACTOR_LEFT(reimu);
	ACTOR_RIGHT(cirno);

	// VARIANT() changes the base body sprite.
	// Bosses currently only have a "defeated" variant with some clothing damage, to be used in post-battle scenes.
	// Elly’s is called "beaten" and is slightly subdued.
	VARIANT(cirno, defeated);

	// Bosses also have a "defeated" face to go along with the variant, but all the other faces can be used as well.
	// It’s best to set the face to "defeated" in the beginning of a post-battle dialogue, and change it later if needed.
	FACE(cirno, defeated);
	MSG(cirno, "Owww… I folded up a little too fast there…");
	FACE(reimu, smug);
	MSG(reimu, "You could have just let me through, you know.");
	MSG(cirno, "Not fair! You didn’t even buy a ticket…");

	DIALOG_END();
}

/*
 * Stage 2
 */

// 자전거 테마: 클라이머 대사
DIALOG_TASK(reimu, Stage2PreBoss) {
	DIALOG_BEGIN(Stage2PreBoss);

	ACTOR_LEFT(reimu);
	ACTOR_RIGHT(hina);

	HIDE(hina);
	WAIT_SKIPPABLE(60);

	FACE(reimu, normal);
	MSG(reimu, "With those loudmouthed kappa gone, the mountain feels practically deserted.");
	FACE(reimu, sigh);
	MSG(reimu, "I almost miss them.");

	EVENT(boss_appears);
	MSG(hina, "…Footsteps. On my climb.");
	SHOW(hina);
	FACE(reimu, normal);
	FACE(hina, normal);
	MSG(hina, "Ms. Hakurei. I thought you might come.");
	FACE(hina, concerned);
	MSG(hina, "But this pass is steeper than it looks.");
	MSG(hina, "And you don’t even have low gears…");

	FACE(reimu, sigh);
	MSG(reimu, "So much for ‘deserted’…");
	FACE(reimu, normal);
	MSG(reimu, "Gears? Who are you, and why are you guarding the road?");

	TITLE_HINA(hina);

	FACE(hina, normal);
	MSG(hina, "CLIMBER. I ride this mountain every morning.");
	MSG(hina, "Today the wind up top feels wrong. The summit is dangerous.");
	FACE(hina, concerned);
	MSG(hina, "Turn back. Coast down while your legs are still fresh.");

	FACE(reimu, sigh);
	MSG(reimu, "(She talks like she’s saving her breath for the hill.)");
	FACE(reimu, puzzled);
	MSG(reimu, "Does it matter that the Gods asked me to come this time?");

	EVENT(music_changes);

	FACE(hina, serious);
	MSG(hina, "The Gods don’t climb. I do.");
	MSG(hina, "If you won’t turn back, I’ll drop you on the next switchback.");

	FACE(reimu, sigh);
	MSG(reimu, "So annoying…");

	DIALOG_END();
}

// 자전거 테마: 클라이머 대사
DIALOG_TASK(reimu, Stage2PostBoss) {
	DIALOG_BEGIN(Stage2PostBoss);

	ACTOR_LEFT(reimu);
	ACTOR_RIGHT(hina);

	WAIT_SKIPPABLE(30);

	VARIANT(hina, defeated);
	FACE(hina, defeated);
	FACE(reimu, normal);

	MSG(hina, "So I lost… go on, then. The summit is yours.");
	FACE(reimu, puzzled);
	MSG(reimu, "I can handle a hill. Why are you being so overbearing?");
	MSG(hina, "Just don’t say I didn’t warn you, when you run out of legs halfway up…");

	FACE(reimu, unamused);
	MSG(reimu, "(Ignored again…)");

	DIALOG_END();
}

/*
 * Stage 3
 */

// 자전거 테마: 랑도너 대사
DIALOG_TASK(reimu, Stage3PreBoss) {
	DIALOG_BEGIN(Stage3PreBoss);

	ACTOR_LEFT(reimu);
	ACTOR_RIGHT(wriggle);
	HIDE(wriggle);
	FACE(reimu, normal);
	MSG(reimu, "Aside from everyone being whipped up into a frenzy, I can’t see anything abnormal yet.");

	EVENT(boss_appears);
	MSG(wriggle, "Nothing abnormal? Then you haven’t checked the stars tonight!");

	SHOW(wriggle);
	FACE(wriggle, proud);
	MSG(wriggle, "The whole route is written up there, checkpoint by checkpoint!");

	FACE(reimu, puzzled);
	MSG(reimu, "Eh? Who are you? Aren’t you a bit far from home?");
	MSG(reimu, "Besides, a route in the sky? Nobody draws roads on constellations.");

	TITLE_WRIGGLE(wriggle);
	FACE(wriggle, outraged);
	MSG(wriggle, "‘Far from home’ is my whole job! I’m RANDONNEUR, and you’re on my checkpoint!");
	FACE(wriggle, proud);
	MSG(wriggle, "Haven’t you heard of the great Bre—…");
	MSG(wriggle, "Brev—…");
	FACE(reimu, puzzled);
	MSG(wriggle, "That really, really long ride?");
	MSG(wriggle, "Twelve hundred kilometers, barely any sleep! Everyone should’ve heard of it by now!");

	MSG(reimu, "Seems pretty far-fetched to me, honestly. Who rides that far for fun?");

	FACE(wriggle, outraged);
	MSG(wriggle, "Don’t lump us long-haulers in with weekend cruisers!");
	FACE(wriggle, proud);
	MSG(wriggle, "I’m going to stamp every checkpoint in Gensōkyō, one constellation at a time!");

	FACE(reimu, puzzled);
	MSG(reimu, "Every checkpoint? But you’re just one girl with a map.");
	FACE(reimu, unamused);
	MSG(reimu, "And I still don’t know what any of this has to do with me.");

	EVENT(music_changes);

	MSG(wriggle, "Nobody passes this control without a stamp. Those are the rules!");
	MSG(wriggle, "Allow me to be your checkpoint!");
	DIALOG_END();
}

// 자전거 테마: 랑도너 대사
DIALOG_TASK(reimu, Stage3PostBoss) {
	DIALOG_BEGIN(Stage3PostBoss);

	ACTOR_LEFT(reimu);
	ACTOR_RIGHT(wriggle);

	VARIANT(wriggle, defeated);
	FACE(wriggle, defeated);

	FACE(reimu, smug);
	MSG(reimu, "Stamping the whole night sky, huh?");
	MSG(reimu, "Sounds like one of Sanae’s ‘weekend projects.’");

	MSG(wriggle, "B-but I plotted it so carefully… every star a checkpoint…");

	FACE(reimu, unamused);
	MSG(reimu, "Don’t you have a pretty sweet deal as it is, though?");
	MSG(reimu, "You already know every road in Gensōkyō by heart.");

	MSG(wriggle, "But can’t I ever hope for one more kilometer…?");

	DIALOG_END();
}

/*
 * Stage 4
 */

// 자전거 테마: 클래식 대사
DIALOG_TASK(reimu, Stage4PreBoss) {
	DIALOG_BEGIN(Stage4PreBoss);

	ACTOR_LEFT(reimu);
	ACTOR_RIGHT(kurumi);

	HIDE(kurumi);

	FACE(reimu, puzzled);
	MSG(reimu, "Huh? There’s a mansion jutting out of this tower?");
	FACE(reimu, sigh);
	MSG(reimu, "This makes even less sense than usual.");

	EVENT(boss_appears);
	WAIT_SKIPPABLE(30);

	FACE(reimu, normal);
	MSG(reimu, "And who are you supposed to be?");

	SHOW(kurumi);
	FACE(kurumi, tsun);
	MSG(kurumi, "Goodness, a guest barging in before teatime? I should be asking you that.");

	FACE(reimu, puzzled);
	MSG(reimu, "…?");
	FACE(reimu, unamused);
	MSG(reimu, "And what are you doing, anyways?");
	MSG(reimu, "Are you behind all these… strange thoughts everyone’s having?");

	FACE(kurumi, normal);
	TITLE_KURUMI(kurumi);
	MSG(kurumi, "I am CLASSIC. I know nothing of ‘strange thoughts’, but I must be honest…");
	MSG(kurumi, "Your attire is giving me some rather strange thoughts.");
	MSG(kurumi, "Those enormous sleeves! They would tangle in a chain before the first corner.");

	FACE(reimu, unamused);
	MSG(reimu, "Ugh, another prissy mansion-dweller.");
	FACE(reimu, sigh);
	MSG(reimu, "I thought I remembered you from somewhere, but no. Absolutely not.");
	FACE(reimu, assertive);
	MSG(reimu, "Just… stop whatever it is you’re doing, right now.");

	FACE(kurumi, dissatisfied);
	MSG(kurumi, "Once you learn to dress for a proper ride, perhaps I shall.");

	FACE(reimu, unamused);
	MSG(reimu, "… so you *are* behind this, huh?");
	FACE(reimu, sigh);
	MSG(reimu, "Besides, haven’t you ever seen a shrine maiden before?");

	FACE(kurumi, normal);
	MSG(kurumi, "A shrine maiden? Without a basket? Without a bell?");
	FACE(reimu, unamused);
	MSG(kurumi, "However do you carry the offerings home, dear?");

	FACE(reimu, assertive);
	MSG(reimu, "T-this is what a shrine maiden wears. It’s a uniform.");

	MSG(kurumi, "A uniform? Oh, surely you jest.");
	MSG(kurumi, "I once met a real shrine maiden, and she wore a long skirt with a proper skirt guard!");
	MSG(kurumi, "And those patterns! No mudguards, no lamp, not even a kickstand! Most unorthodox!");
	FACE(kurumi, tsun);
	MSG(kurumi, "Do go home and change, Miss Imitation.");

	FACE(reimu, irritated);
	MSG(reimu, "… ARGH!");
	FACE(reimu, outraged);
	MSG(reimu, "Listen!! It’s HARD to find clothes I can wear every day on the job!");
	MSG(reimu, "It has to fit a certain style!");
	MSG(reimu, "And only a few fabrics feel good to wear!");
	MSG(reimu, "I can’t stand scratchy wool or slippery silk!");
	MSG(reimu, "Everything else feels terrible against my skin—");
	FACE(reimu, irritated);

	MSG(kurumi, "Oh dear, I didn’t mean to upset you so— shall I put the kettle on?");

	EVENT(music_changes);

	MSG(reimu, "I *hate* prissy mansion dwellers!");
	FACE(reimu, outraged);
	MSG(reimu, "Let’s just get this over with!");

	DIALOG_END();
}

// 자전거 테마: 클래식 대사
DIALOG_TASK(reimu, Stage4PostBoss) {
	DIALOG_BEGIN(Stage4PostBoss);

	ACTOR_LEFT(reimu);
	ACTOR_RIGHT(kurumi);
	VARIANT(kurumi, defeated);
	FACE(kurumi, defeated);

	FACE(reimu, irritated);
	MSG(reimu, "That’s what you get for insulting my outfit like that.");

	MSG(kurumi, "Ouch ouch ouch… my poor spokes…");
	MSG(kurumi, "W-wait, what happened?");

	MSG(reimu, "What do you mean ‘what happened’?");

	MSG(kurumi, "(P-perhaps if I pretend a bolt came loose in my memory… yes, quite…!)");
	FACE(kurumi, normal);
	MSG(kurumi, "O-oh, my head! Wherever am I? Has someone forgotten to oil me?");
	MSG(kurumi, "W-wait! Aren’t you that girl from a long while back?");

	FACE(reimu, sigh);
	MSG(reimu, "I don’t remember you at all.");
	FACE(reimu, unsettled);
	MSG(reimu, "But somehow, so familiar…");
	FACE(reimu, sigh);
	MSG(reimu, "This feeling is annoying, whatever it is. Make it stop, or tell me how to make it stop.");

	FACE(kurumi, defeated);
	MSG(kurumi, "Huh? I-I couldn’t say! I barely remember a thing!");
	MSG(kurumi, "(Those sleeves really would catch in a chain, though…)");

	MSG(reimu, "(Just another victim…?)");
	FACE(reimu, normal);
	MSG(reimu, "I’ll deal with you after I figure out whatever's happening here.");

	DIALOG_END();
}

/*
 * Stage 5
 */

// 자전거 테마: 에어로 대사
DIALOG_TASK(reimu, Stage5PreBoss) {
	DIALOG_BEGIN(Stage5PreBoss);

	ACTOR_LEFT(reimu);
	ACTOR_RIGHT(iku);
	HIDE(iku);

	FACE(reimu, unsettled);
	MSG(reimu, "I’ve got a bad feeling about this…");
	FACE(reimu, puzzled);
	MSG(reimu, "Or maybe I’m just feeling nauseous with this tower’s constant spinning.");

	EVENT(boss_appears);
	WAIT_SKIPPABLE(60);
	SHOW(iku);
	FACE(iku, normal);
	MSG(iku, "Or maybe you just can’t handle the speed up here?");

	MSG(reimu, "Oh, it’s you. Why did you attack me earlier, anyway?");

	TITLE_IKU(iku);
	FACE(iku, smile);
	MSG(iku, "Ha! Sorry, you drifted into my line. Racing incident.");
	FACE(iku, eyes_closed);
	MSG(iku, "Things get hectic in a crosswind like this. No hard feelings.");
	MSG(iku, "But it’s been a while, hasn’t it? AERO, remember?");

	FACE(reimu, normal);
	MSG(reimu, "Err, right, yes, it has been a while. Nice to see you again.");

	FACE(iku, serious);
	MSG(iku, "This tower is unlike any course I’ve ever ridden.");
	MSG(iku, "Word travels fast in the clouds. The culprit is tied to ‘parallel universes.’");

	FACE(reimu, puzzled);
	MSG(reimu, "You mean like Otherworlds? That’s not too out of the ordinary these days.");
	FACE(reimu, unsettled);
	MSG(reimu, "This feels different though. It’s like the walls are sucking up faith into a void.");
	MSG(reimu, "I’m surprised the fairies are surviving at all…");
	FACE(reimu, normal);
	MSG(reimu, "But what are you even doing here?");

	FACE(iku, eyes_closed);
	MSG(iku, "It’s not just the Netherworld and Gensōkyō feeling the drag.");
	FACE(iku, serious);
	MSG(iku, "In Bhava-agra, the Eldest Daughter’s personality has shifted, too.");

	FACE(reimu, surprised);
	MSG(reimu, "It’s spreading that fast?!");

	FACE(iku, smile);
	MSG(iku, "Honestly? I wasn’t planning to get involved.");
	FACE(reimu, normal);
	FACE(iku, serious);
	MSG(iku, "But the tower’s pull kept getting stronger, and nobody was taking a turn at the front.");

	FACE(reimu, sigh);
	MSG(reimu, "(Do people have this little faith in me…?)");

	FACE(iku, eyes_closed);
	MSG(iku, "As more minds got dropped from the pack, I came up with a theory…");
	FACE(reimu, normal);
	MSG(iku, "This machine is a headwind against fantasy itself.");
	FACE(iku, normal);
	MSG(iku, "That’s why this place feels like riding straight into a gale.");

	FACE(reimu, normal);
	MSG(reimu, "Well, that settles it. Where’s the owner, then?");

	FACE(iku, smile);
	MSG(iku, "Straight ahead. But frankly? I’m not sure you can hold the pace.");
	MSG(iku, "You’re still composed, sure, but you’re starting to wobble.");

	FACE(reimu, irritated);
	MSG(reimu, "Wobble? But I just got here!");

	EVENT(music_changes);
	MSG(iku, "Sit on my wheel and I’ll tow you home before you crack.");

	FACE(reimu, irritated);
	MSG(reimu, "No!");

	FACE(iku, serious);
	MSG(iku, "Come again?");

	MSG(reimu, "Don’t keep telling me how little you think of me. I’m sick of it!");
	FACE(reimu, assertive);
	MSG(reimu, "I’m going to give everyone a reason to have faith in me!");

	MSG(iku, "Ha! Now that’s a rider’s answer. Fine.");
	MSG(iku, "Show me you can hold your line against their machine — starting with me!");

	DIALOG_END();
}

DIALOG_TASK(reimu, Stage5PostMidBoss) {
	DIALOG_BEGIN(Stage5PostMidBoss);

	ACTOR_LEFT(reimu);
	FACE(reimu, surprised);

	// should be only one message with a fixed 120-frames timeout
	// UNSKIPPABLE is required here to keep music in sync
	MSG_UNSKIPPABLE(reimu, 120, "Thunder clouds? Are we really that high up?");

	DIALOG_END();
}

// 자전거 테마: 에어로 대사
DIALOG_TASK(reimu, Stage5PostBoss) {
	DIALOG_BEGIN(Stage5PostBoss);

	ACTOR_LEFT(reimu);
	ACTOR_RIGHT(iku);
	VARIANT(iku, defeated);
	FACE(iku, defeated);

	MSG(iku, "Ah…");
	MSG(iku, "Guess I shouldn’t have doubted you…");

	FACE(reimu, unamused);
	MSG(reimu, "Somehow I don’t feel too satisfied by that.");

	FACE(iku, eyes_closed);
	MSG(iku, "Truth is, I’m glad you showed up after all.");
	MSG(iku, "I couldn’t have taken her down solo. Not into this headwind.");
	FACE(reimu, normal);
	MSG(iku, "But you’re still riding straight. Maybe Gensōkyō will make it after all.");

	FACE(reimu, sigh);
	MSG(reimu, "Finally.");
	FACE(reimu, normal);
	MSG(reimu, "But you’re right. This is bigger than Gensōkyō.");
	MSG(reimu, "I’ll need to take this seriously.");

	FACE(iku, normal);
	MSG(iku, "Good. The road ahead is yours.");

	DIALOG_END();
}

/*
 * Stage 6
 */

DIALOG_TASK(reimu, Stage6PreBoss) {
	DIALOG_BEGIN(Stage6PreBoss);

	ACTOR_LEFT(reimu);
	ACTOR_RIGHT(elly);
	HIDE(elly);
	EVENT(boss_appears);
	MSG(elly, "Well, well, well. It seems a faithful rat trespassed on our domain.");
	SHOW(elly);

	FACE(elly, smug);
	MSG(elly, "How strange. You should’ve lost your mind by now.");

	FACE(reimu, unsettled);
	MSG(reimu, "Just turn it off already.");
	MSG(reimu, "I’m not in the mood to argue, so don’t push me!");

	TITLE_ELLY(elly);
	FACE(elly, angry);
	MSG(elly, "Bold of you to barge into MY tower and give ME orders!");
	MSG(elly, "I still remember what you did to Lady Kazami and I back then!");

	FACE(reimu, sigh);
	MSG(reimu, "And what was that, again?");
	MSG(reimu, "I’m getting a familiar feeling just looking at you, but…");

	FACE(elly, shouting);
	MSG(elly, "Is this a joke?! Are you trying to mock me?!");

	FACE(reimu, assertive);
	MSG(reimu, "I think I would’ve remembered a giant tower that makes people go mad with power!");
	MSG(reimu, "The least you can do when accusing someone is to tell them what they’ve supposedly done!");

	FACE(elly, smug);
	MSG(elly, "Ah. I see.");
	FACE(elly, normal);
	MSG(elly, "Fine. I will.");
	FACE(reimu, unamused);
	MSG(elly, "This tower is a recent invention. Its mere existence is revolutionary in several scientific fields.");
	MSG(elly, "The denizens of this Gensōkyō ought to have been thankful for its vast scientific knowledge…");
	MSG(elly, "But alas, we have other plans.");

	FACE(reimu, unsettled);
	MSG(reimu, "‘This’ Gensōkyō…?");

	FACE(reimu, unamused);
	MSG(reimu, "Somehow, it feels like more of a punishment.");
	MSG(reimu, "Ever heard of ‘ignorance is bliss’?");
	MSG(reimu, "There won’t be anyone to rule over, or whatever, if you make them all lose it.");

	MSG(elly, "The Tower of Babel is capable of ‘enlightening’ those fortunate to be around it.");
	FACE(elly, smug);
	MSG(elly, "Anyone unable to comprehend its brilliance will be driven to madness.");
	FACE(elly, shouting);
	MSG(elly, "But this is simply retribution for the crimes that have been committed against us!");

	FACE(reimu, unsettled);
	MSG(reimu, "‘Parallel universes’, is that it?");

	FACE(elly, normal);
	MSG(elly, "Hmm? Is that what you think this Gensōkyō is?");

	FACE(reimu, irritated);
	MSG(reimu, "You tell me! You’re the megalomaniac!");
	FACE(reimu, assertive);
	MSG(reimu, "Do you hate me because some other Reimu did something? Is that it?");
	MSG(reimu, "But then what does that have to do with any of us?!");
	FACE(reimu, irritated);
	MSG(reimu, "For a ‘scientist’, you’re not making a whole lot of sense!");

	FACE(elly, angry);
	MSG(elly, "Perhaps you’ve simply forgotten, like you have of the rest of us!");
	FACE(reimu, unamused);
	MSG(elly, "Only the privileged, such as Lady Kazami, made it out! The rest of us were lost!");

	FACE(reimu, unsettled);
	MSG(reimu, "You mean Kazami Yūka? And I suppose… Alice, too?");
	MSG(reimu, "I remember when I first met Alice, and she said some weird things too, just like you are now.");
	FACE(reimu, unamused);
	MSG(reimu, "But she didn’t go around threatening everyone!");

	FACE(elly, angry);
	MSG(elly, "We weren’t as lucky! And as reparation, we will take control of this Gensōkyō!");
	MSG(elly, "We will make it so we will NEVER be forgotten!");

	FACE(reimu, sigh);
	MSG(reimu, "Even in Gensōkyō, you can be forgotten, huh?");
	MSG(reimu, "What a miserable thought. No wonder I felt terrible just seeing this place!");
	FACE(reimu, assertive);
	MSG(reimu, "Turn it off, or being forgotten will be the least of your worries!");

	EVENT(music_changes);
	FACE(elly, shouting);
	MSG(elly, "It’s too late for that! Soon, all of Gensōkyō will become enlightened!");
	FACE(reimu, unsettled);
	MSG(elly, "Don’t you understand, pitiful shrine maiden? Science is an unfathomable power, surpassed by nothing!");
	MSG(elly, "We won’t be forgotten again!");

	DIALOG_END();
}

DIALOG_TASK(reimu, Stage6PreFinal) {
	DIALOG_BEGIN(Stage6PreFinal);

	ACTOR_LEFT(reimu);
	ACTOR_RIGHT(elly);
	VARIANT(elly, beaten);
	FACE(elly, shouting);
	FACE(reimu, assertive);
	MSG(reimu, "Is that it? Are you finally done?");
	MSG(elly, "Y-you think this is over?! The time of reckoning is upon you!");
	MSG(elly, "Gaze upon the ultimate truth of the universe, and tremble!");

	DIALOG_END();
}

/*
 * Register the tasks
 */

#define EXPORT_DIALOG_TASKS_CHARACTER reimu
#include "export_dialog_tasks.inc.h"
