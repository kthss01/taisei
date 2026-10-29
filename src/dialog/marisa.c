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

DIALOG_TASK(marisa, Stage1PreBoss) {
	DIALOG_BEGIN(Stage1PreBoss);

	ACTOR_LEFT(marisa);
	ACTOR_RIGHT(cirno);
	HIDE(cirno);

	FACE(marisa, puzzled);
	MSG(marisa, "Snow again? But I just put away my winter coat.");
	FACE(marisa, sweat_smile);
	MSG(marisa, "…even though it’s half-way through summer already.");

	EVENT(boss_appears);
	// 자전거 의인화 테마 1차 시험: 스테이지 1 보스는 MINIVELO(임시 이름). 내부 id cirno는 유지한다.
	MSG(cirno, "Beep beep! Move it, broom girl!");

	SHOW(cirno);
	FACE(cirno, normal);
	MSG(cirno, "I brought this cold air straight down from the mountain pass. Pretty neat, huh?");

	FACE(marisa, normal);
	MSG(marisa, "Mind paying to get my coat cleaned, then?");
	FACE(marisa, sweat_smile);
	MSG(marisa, "I was gonna get it done in the fall, but now…");

	TITLE_CIRNO(cirno);
	MSG(cirno, "Pay YOU? Nope! Around here, *you* pay the toll!");
	EVENT(music_changes);
	FACE(cirno, angry);
	MSG(cirno, "Let’s race! Loser pushes the bike home!");

	FACE(marisa, smug);
	MSG(marisa, "Race a broom? You’re on.");

	DIALOG_END();
}

DIALOG_TASK(marisa, Stage1PostBoss) {
	DIALOG_BEGIN(Stage1PostBoss);

	ACTOR_LEFT(marisa);
	ACTOR_RIGHT(cirno);
	VARIANT(cirno, defeated);
	FACE(cirno, defeated);

	FACE(marisa, smug);
	MSG(marisa, "Thought a bike could outrun a broom?");
	MSG(marisa, "Come back when you’ve got a tailwind.");

	MSG(cirno, "Ouch…");
	MSG(cirno, "I-I only lost ’cause my chain slipped! Next time you’re toast!");

	MSG(marisa, "Sure, sure.");

	DIALOG_END();
}

/*
 * Stage 2
 */

DIALOG_TASK(marisa, Stage2PreBoss) {
	// 자전거 테마: 클라이머 대사
	DIALOG_BEGIN(Stage2PreBoss);

	ACTOR_LEFT(marisa);
	ACTOR_RIGHT(hina);
	HIDE(hina);
	FACE(marisa, unamused);
	MSG(marisa, "Geez, this mountain road just keeps goin’ up ‘n up.");
	MSG(marisa, "What’s got the yōkai around here all twitchy, anyways?");

	EVENT(boss_appears);
	MSG(hina, "…You’re climbing without gears.");
	SHOW(hina);
	FACE(hina, normal);
	MSG(hina, "The grade gets steeper past here. Turn back.");

	FACE(marisa, happy);
	MSG(marisa, "Gears? I’ve got a broom. Brooms don’t do ‘steep’.");
	MSG(marisa, "See ya at the top.");

	TITLE_HINA(hina);
	FACE(hina, serious);
	MSG(hina, "…You skipped the warm-up.");
	MSG(hina, "And you laugh at the mountain.");

	FACE(marisa, smug);
	MSG(marisa, "I laugh at everything. It’s part of my charm.");

	EVENT(music_changes);
	MSG(hina, "The mountain doesn’t forgive those who sprint the first switchback.");
	MSG(hina, "Lowest gear. Full climb. Try to keep up.");

	DIALOG_END();
}

DIALOG_TASK(marisa, Stage2PostBoss) {
	// 자전거 테마: 클라이머 대사
	DIALOG_BEGIN(Stage2PostBoss);

	ACTOR_LEFT(marisa);
	ACTOR_RIGHT(hina);
	VARIANT(hina, defeated);
	FACE(hina, defeated);
	FACE(marisa, unamused);
	MSG(marisa, "You’re way quieter than a bike oughta be. Not even a bell?");
	MSG(marisa, "Kinda spooky, honestly.");

	MSG(hina, "…Out of breath.");
	MSG(hina, "I only wanted you to pace yourself… The summit is still far.");

	MSG(marisa, "Don’t go all coach on me now.");
	FACE(marisa, sweat_smile);
	MSG(marisa, "I’ve had enough folks tellin’ me to slow down for a lifetime.");

	DIALOG_END();
}

/*
 * Stage 3
 */

DIALOG_TASK(marisa, Stage3PreBoss) {
	// 자전거 테마: 랑도너 대사
	DIALOG_BEGIN(Stage3PreBoss);

	ACTOR_LEFT(marisa);
	ACTOR_RIGHT(wriggle);
	HIDE(wriggle);
	FACE(marisa, normal);
	MSG(marisa, "Geez, everyone’s getting even more reckless the further up I go. What gives?");

	EVENT(boss_appears);
	MSG(wriggle, "Checkpoint ahead! Stop and get your card stamped!");
	SHOW(wriggle);

	MSG(marisa, "Card? I don’t remember signin’ up for anything.");

	FACE(wriggle, proud);
	MSG(wriggle, "Everyone on this road is riding the brevet! Six hundred kilometers, no excuses!");

	MSG(marisa, "Oh, so you’re the gatekeeper?");
	FACE(marisa, happy);
	MSG(marisa, "Mind wavin’ me through?");

	TITLE_WRIGGLE(wriggle);
	FACE(wriggle, outraged);
	MSG(wriggle, "Gatekeeper?! I’m not some checkpoint volunteer!");
	FACE(wriggle, proud);
	MSG(wriggle, "I’m RANDONNEUR! I ride the whole route, start to finish!");

	FACE(marisa, smug);
	MSG(marisa, "The whole route? What, on purpose? For fun?");

	FACE(wriggle, outraged);
	MSG(wriggle, "Yes, for fun! Rain, night, headwinds — that’s the best part!");
	FACE(wriggle, calm);
	MSG(wriggle, "Anyway, you had a question?");

	FACE(marisa, normal);
	MSG(marisa, "Hmm, guess I forgot.");
	MSG(marisa, "What’s yer deal, anyways?");

	FACE(wriggle, proud);
	MSG(wriggle, "A true randonneur lives by three rules:");
	MSG(wriggle, "First: always carry your own spare tube!");
	MSG(wriggle, "Second: when the map gets soggy, navigate by the stars!");
	FACE(wriggle, outraged);
	MSG(wriggle, "Third: never, EVER skip a checkpoint!");
	MSG(wriggle, "Fourth—!");

	FACE(marisa, unamused);
	MSG(marisa, "Okay okay, this is great ‘n all, but I’ve honestly got more important stuff to do.");
	MSG(marisa, "… and ya don’t got a clue about what’s goin’ on, do ya?");

	EVENT(music_changes);
	FACE(wriggle, proud);
	MSG(wriggle, "Maybe my map shows more than you think.");
	FACE(wriggle, outraged);
	MSG(wriggle, "But I’m NOT sharing my route with someone who cuts corners on a broom!");

	DIALOG_END();
}

DIALOG_TASK(marisa, Stage3PostBoss) {
	// 자전거 테마: 랑도너 대사
	DIALOG_BEGIN(Stage3PostBoss);

	ACTOR_LEFT(marisa);
	ACTOR_RIGHT(wriggle);
	VARIANT(wriggle, defeated);
	FACE(wriggle, defeated);

	FACE(marisa, smug);
	MSG(marisa, "Tougher than usual, but not tough enough.");
	MSG(marisa, "Where’d ya pick up those new moves, anyway?");

	MSG(wriggle, "Aw! I, um… plotted them from the constellations!");
	MSG(wriggle, "Follow the Great Bicycle in the sky and you never get lost!");

	FACE(marisa, puzzled);
	MSG(marisa, "‘Great Bicycle’? In the sky?");
	FACE(marisa, normal);
	MSG(marisa, "Oh, ya must mean that old star chart with the wheels drawn in. Two big circles ‘n a handlebar.");
	MSG(marisa, "I remember that doctor at Eientei goin’ off about star charts once.");
	FACE(marisa, sweat_smile);
	MSG(marisa, "Only time I’d seen her so excited…");

	MSG(wriggle, "Yeah, that!");
	FACE(wriggle, proud);
	MSG(wriggle, "Wanna ride the next brevet with me? I’ll lend you a spare map!");

	FACE(marisa, smug);
	MSG(marisa, "Maybe later.");
	FACE(marisa, happy);
	MSG(marisa, "Probably never.");

	DIALOG_END();
}

/*
 * Stage 4
 */

DIALOG_TASK(marisa, Stage4PreBoss) {
	// 자전거 테마: 클래식 대사
	DIALOG_BEGIN(Stage4PreBoss);

	ACTOR_LEFT(marisa);
	ACTOR_RIGHT(kurumi);
	HIDE(kurumi);

	FACE(marisa, surprised);
	MSG(marisa, "Oh wow, a mysterious, magical mansion!");
	FACE(marisa, smug);
	MSG(marisa, "… how generic can you get?");

	EVENT(boss_appears);
	MSG(kurumi, "And a witch arriving on a broom, no less. How very traditional.");
	SHOW(kurumi);
	MSG(kurumi, "Do mind the gravel, dear. It scratches the paintwork.");

	FACE(marisa, happy);
	MSG(marisa, "Traditional? I’m the fastest thing in Gensōkyō. Ask any bike girl.");

	FACE(kurumi, dissatisfied);
	MSG(kurumi, "Fastest… Everyone is in such a hurry these days.");
	FACE(kurumi, normal);
	MSG(kurumi, "What comes next? Frames as light as a feather?");
	FACE(marisa, normal);
	MSG(kurumi, "Twenty-two gears, and not a single basket to carry flowers home in?");
	MSG(kurumi, "Tires so thin they cannot even manage a cobblestone lane?");

	FACE(marisa, smug);
	MSG(marisa, "Maybe they’ll paint ‘em red ‘n black to make ‘em look even faster.");

	FACE(kurumi, dissatisfied);
	MSG(kurumi, "Oh my. Perhaps I have been reading too many of those tengu racing magazines.");

	FACE(marisa, puzzled);
	MSG(marisa, "The tengu write about bike races now?");
	FACE(marisa, sweat_smile);
	MSG(marisa, "Well, at least they can’t fake the finish times. Probably.");

	TITLE_KURUMI(kurumi);
	FACE(kurumi, normal);
	MSG(kurumi, "Indeed they do.");
	MSG(kurumi, "Pages and pages of riders panting up hills, and not a single teacup in sight!");
	FACE(kurumi, dissatisfied);
	MSG(kurumi, "And do not even get me started on those frames. Not one has a proper chain guard!");

	FACE(marisa, normal);
	MSG(marisa, "Maybe they oughta hire someone with some class.");

	FACE(kurumi, normal);
	MSG(kurumi, "That would be me, I suppose.");
	MSG(kurumi, "A step-through frame, a wicker basket, a bell that rings like a spoon on porcelain…");

	FACE(marisa, unamused);
	MSG(marisa, "Eh? Sounds kinda slow…");

	FACE(kurumi, dissatisfied);
	MSG(kurumi, "I beg your pardon?!");

	FACE(marisa, smug);
	MSG(marisa, "Your ride’s missin’ somethin’… y’know, speed.");

	FACE(kurumi, tsun_blush);
	MSG(kurumi, "H-Hmph! I could say the same about your broom!");
	FACE(kurumi, normal);
	MSG(kurumi, "Honestly, it is just a stick with bristles! No fenders, no lamp, not even a kickstand!");

	FACE(marisa, unamused);
	MSG(marisa, "Who needs fenders? It’s summer!");
	FACE(marisa, happy);
	MSG(marisa, "My broom’s legendary. Girls across Gensōkyō talk about it.");
	FACE(marisa, normal);
	MSG(marisa, "In fact, there’s a few books on broom-ridin’, uh, deeper inside this mansion.");
	FACE(marisa, happy);
	MSG(marisa, "So I’ll be headin’ in, then.");

	EVENT(music_changes);
	FACE(kurumi, tsun);
	MSG(kurumi, "Oh, I shall not let you pass before teatime is over!");
	FACE(kurumi, normal);
	MSG(kurumi, "Shall we take a leisurely little ride, witch? Do try to mind your manners.");

	DIALOG_END();
}

DIALOG_TASK(marisa, Stage4PostBoss) {
	// 자전거 테마: 클래식 대사
	DIALOG_BEGIN(Stage4PostBoss);

	ACTOR_LEFT(marisa);
	ACTOR_RIGHT(kurumi);
	VARIANT(kurumi, defeated);
	FACE(kurumi, defeated);

	FACE(marisa, smug);
	MSG(marisa, "Whoever gets there first gets the tea. It’s a scientific fact.");
	FACE(marisa, sweat_smile);
	MSG(marisa, "… but since when do I care about scientific facts?");

	MSG(kurumi, "Oh dear, where was I riding to again?");
	MSG(kurumi, "(Yes, that’s it, play it off as a gentle afternoon daze…)");
	MSG(kurumi, "(But… have I not seen this nerdy-looking girl somewhere before…?)");
	MSG(kurumi, "Goodness, I do not remember a thing!");

	FACE(marisa, normal);
	MSG(marisa, "Ah, sorry, sorry. I think ya hit a pothole ‘n bumped your head.");
	FACE(marisa, sweat_smile);
	MSG(marisa, "I’m Gensōkyō’s… bicycle inspector.");
	FACE(marisa, smug);
	MSG(marisa, "I’m here to inspect the, uh, bike manuals in this here mansion.");
	FACE(marisa, happy);
	MSG(marisa, "Ya were just lettin’ me in, so I’ll be off, then.");

	FACE(kurumi, normal);
	MSG(kurumi, "Hmm, I rather doubt that…");
	FACE(kurumi, defeated);
	MSG(kurumi, "Not that I could ever catch you at my pace…");
	FACE(marisa, normal);
	MSG(kurumi, "Oh, I did not enter this ride to be overtaken like this!");

	FACE(marisa, smug);
	MSG(marisa, "‘Enter this ride’? I thought you didn’t remember anything.");

	FACE(kurumi, tsun_blush);
	MSG(kurumi, "N-Never mind that!");

	DIALOG_END();
}

/*
 * Stage 5
 */

DIALOG_TASK(marisa, Stage5PreBoss) {
	// 자전거 테마: 에어로 대사
	DIALOG_BEGIN(Stage5PreBoss);

	ACTOR_LEFT(marisa);
	ACTOR_RIGHT(iku);
	HIDE(iku);

	FACE(marisa, inquisitive);
	MSG(marisa, "This place is fascinatin’!");
	MSG(marisa, "I gotta tell Nitori!");

	EVENT(boss_appears);
	MSG(iku, "Eyes forward, witch! Don’t stop in the middle of the lane!");
	SHOW(iku);
	MSG(iku, "Didn’t expect to see you this high up. Nice climb — for a broom.");

	MSG(marisa, "What grimoire would this be in…?");
	MSG(marisa, "Hmm… probably a major illusion of some kind…?");
	MSG(marisa, "That’d be easier than making the bricks weightless, ‘n it’s a good disguise…");

	TITLE_IKU(iku);
	FACE(iku, smile);
	MSG(iku, "… hey. Are you talking to yourself?");

	FACE(marisa, normal);
	MSG(marisa, "Huh? No, I was askin’ ya a question.");

	MSG(iku, "Interesting theory. Wrong, but interesting.");

	MSG(marisa, "It’d make sense though, right?");
	FACE(marisa, inquisitive);
	MSG(marisa, "It appeared outta nowhere. If it were real brick, the air displacement alone woulda caused a huge noise…");
	MSG(marisa, "And it’d take so much power, too.");
	MSG(marisa, "Definitely an illusion, then.");

	MSG(iku, "Your theory’s got more drag than a flapping raincoat.");
	FACE(iku, eyes_closed);
	FACE(marisa, normal);
	MSG(iku, "Whatever built this tower is way past anything we understand. Feel the wind? Perfectly smooth.");

	FACE(marisa, surprised);
	MSG(marisa, "Technology? Whoa, so yer sayin’ this is REAL brick?!");
	FACE(marisa, normal);
	MSG(marisa, "*tap tap*");
	FACE(marisa, surprised);
	MSG(marisa, "It’s so cold it’s almost creepy, like it was suckin’ magic right outta my fingers.");
	FACE(marisa, inquisitive);
	MSG(marisa, "But that means the tower… has corporeal form?!");
	MSG(marisa, "Dang! Now I GOTTA meet the owner!");
	MSG(marisa, "What grimoires does she got to pull somethin’ like this off?!");

	FACE(iku, smile);
	MSG(iku, "I-I doubt she uses grimoires. Or magic, for that matter.");
	FACE(iku, serious);
	MSG(iku, "This is serious engineering. Smooth airflow, precise geometry, advanced computational—");

	MSG(marisa, "But then, what’s the power source?");
	MSG(marisa, "Not even nuclear fusion could sustain somethin’ like this!");

	FACE(iku, smile);
	MSG(iku, "I-I haven’t worked out every detail yet—");

	FACE(marisa, happy);
	MSG(marisa, "Where’d y’say she was?");

	FACE(iku, normal);
	MSG(iku, "You mean…?");

	FACE(marisa, inquisitive);
	MSG(marisa, "The owner, o’ course! Upstairs, right?");

	FACE(iku, serious);
	MSG(iku, "The thin air up here must be getting to you. I’m not telling you anything else, for your own good.");

	FACE(marisa, happy);
	MSG(marisa, "What’s her name?");

	MSG(iku, "Why won’t you listen to me…?");

	FACE(marisa, inquisitive);
	MSG(marisa, "Do ya got any tips on talkin’ to her, maybe make her loosen up a bit?");
	FACE(marisa, happy);
	MSG(marisa, "I don’t wanna blow my chance.");

	FACE(iku, smile);
	MSG(iku, "I’m trying to keep you out of a crash, and you just keep talking over me. Give me some room!");
	MSG(iku, "I figured you’d hold a clean line and sprint this incident to the finish, but it looks like—");

	FACE(marisa, normal);
	MSG(marisa, "Ah, I see, I see. I’ll have to turn on the ol’ Kirisame charm.");
	FACE(marisa, smug);
	FACE(iku, serious);
	MSG(marisa, "I get it, I get it.");
	MSG(marisa, "Haven’t done ‘femme fatale’ in a while, but…");

	EVENT(music_changes);

	MSG(iku, "Enough!");
	MSG(iku, "They say a good, strong headwind clears the mind.");
	FACE(iku, eyes_closed);
	MSG(iku, "Point that broom forward — I’ll give you one you won’t forget!");

	DIALOG_END();
}

DIALOG_TASK(marisa, Stage5PostMidBoss) {
	DIALOG_BEGIN(Stage5PostMidBoss);

	ACTOR_LEFT(marisa);
	FACE(marisa, surprised);

	// should be only one message with a fixed 180-frame (3 second) timeout
	MSG_UNSKIPPABLE(marisa, 180, "So this place runs on electricity, eh? Hmm…");

	DIALOG_END();
}

DIALOG_TASK(marisa, Stage5PostBoss) {
	// 자전거 테마: 에어로 대사
	DIALOG_BEGIN(Stage5PostBoss);

	ACTOR_LEFT(marisa);
	ACTOR_RIGHT(iku);
	VARIANT(iku, defeated);
	FACE(iku, defeated);

	FACE(marisa, unamused);
	MSG(marisa, "Oof, that headwind made my hair a total mess.");
	FACE(marisa, happy);
	MSG(marisa, "Y’know, for my big first impression ‘n all.");

	FACE(iku, eyes_closed);
	MSG(iku, "L-listen. She doesn’t do spells. She won’t teach you any…");
	MSG(iku, "Her tower runs on technology and the scientific method. I’d bet my best wheels on it.");

	FACE(marisa, unamused);
	MSG(marisa, "The one time Sanae’s not around to geek out about all the sciencey stuff, eh?");
	FACE(iku, normal);
	FACE(marisa, normal);
	MSG(marisa, "Ah well, I know how to operate one of those Intelligent Phones, and that’s plenty advanced, so I should be fine.");
	FACE(marisa, inquisitive);
	MSG(marisa, "By the way, have ya been hearing that strange voice?");

	MSG(iku, "Strange voice?");

	FACE(marisa, smug);
	MSG(marisa, "Yeah, it keeps tellin’ me that I’m not thinkin’ straight or whatever.");
	MSG(marisa, "Probably just those weird madness rays tryin’ to get me.");

	FACE(iku, eyes_closed);
	MSG(iku, "Ugh. Hopeless.");
	FACE(iku, normal);
	MSG(iku, "But maybe you’re riding just straight enough to finish this incident after all…");

	FACE(marisa, surprised);
	MSG(marisa, "Ah! There’s that voice again!");
	FACE(marisa, smug);
	MSG(marisa, "So strange…");

	DIALOG_END();
}

/*
 * Stage 6
 */

DIALOG_TASK(marisa, Stage6PreBoss) {
	DIALOG_BEGIN(Stage6PreBoss);

	ACTOR_LEFT(marisa);
	ACTOR_RIGHT(elly);
	HIDE(elly);

	EVENT(boss_appears);
	MSG(elly, "I’ve been waiting for you to make your move.");
	SHOW(elly);
	MSG(elly, "Sneaking around as you are, and yet still so loud somehow.");

	FACE(marisa, puzzled);
	MSG(marisa, "Hmm…");

	FACE(elly, smug);
	MSG(elly, "Did you really think I wouldn’t notice all the commotion you caused inside the tower?");

	MSG(marisa, "Hmmmmmmm…");

	FACE(elly, angry);
	MSG(elly, "Stop being an insolent brat and speak up!");

	FACE(marisa, unamused);
	MSG(marisa, "… sigh.");

	FACE(elly, normal);
	MSG(elly, "… w-what is it?");

	MSG(marisa, "Ya don’t look like a magician after all.");
	MSG(marisa, "I’m pretty disappointed.");
	MSG(marisa, "Honestly, this might be the most disappointing incident yet.");

	TITLE_ELLY(elly);
	MSG(elly, "You’re after… magic?");
	FACE(elly, smug);
	MSG(elly, "Hah! You’ll regret wasting your time on such nonsense.");
	MSG(elly, "Soon, you’ll see the error of—");

	MSG(marisa, "Do ya know how boring it is to be a magician in Gensōkyō these days?");

	FACE(elly, eyes_closed);
	MSG(elly, "I couldn’t possibly care about—");

	MSG(marisa, "Seriously! Recently it’s just been ‘God this’ and ‘Otherworld that’.");
	MSG(marisa, "Reimu’s been pickin’ up some sweet skills, but me? Nothin’!");
	MSG(marisa, "Gods use magic too, but they’re like her! They don’t need books!");
	MSG(marisa, "But I do! And I end up empty handed every dang time!");

	FACE(elly, angry);
	MSG(elly, "I SAID I don’t CARE about your stupid little sorc—");

	MSG(marisa, "I saw this place ‘n I was all, ‘Whoa! Is this all enchantments ‘n stuff?!’");
	FACE(marisa, inquisitive);
	MSG(marisa, "I got really excited!");
	MSG(marisa, "‘Maybe I could get a new trump-card outta this’ is what I thought.");
	FACE(marisa, unamused);
	MSG(marisa, "But obviously yer a shinigami or somethin’. And I didn’t see a library…");
	MSG(marisa, "I guess that oarfish from before was right.");
	MSG(marisa, "Master Spark’s good ‘n all, but it’ll only get me so far in life…");

	FACE(elly, eyes_closed);
	MSG(elly, "…");
	MSG(marisa, "*sigh*");
	MSG(elly, "Are you finally done?");
	FACE(elly, angry);
	MSG(elly, "Is this some kind of joke?!");
	FACE(elly, shouting);
	MSG(elly, "How does every version of you just keep mocking me?!");

	MSG(marisa, "What, you mean last time? At least then you were guardin’ some powerful thingy I coulda used.");
	MSG(marisa, "I’m tryin’ to remember, was that timeline a bust, too?");
	FACE(marisa, happy);
	MSG(marisa, "At least I met Yūka, right? Did ya know she ended up givin’ me my trademark spell?");
	MSG(marisa, "Not right then, ‘n not voluntarily, but—");

	FACE(elly, angry);
	MSG(elly, "Y-you remember that?! But how—?!");
	MSG(elly, "… ugh, nevermind! It’s not as if you could possibly grasp the Tower’s true power.");
	FACE(elly, smug);
	MSG(elly, "After all, I can already sense your feeble mind succumbing to its might!");

	FACE(marisa, unamused);
	MSG(marisa, "Eh? I’m not depressed or anythin’. I’m just disappointed.");

	FACE(elly, shouting);
	MSG(elly, "I-It’s not about being ‘depressed’! It’s about being overwhelmed with the vast knowledge of the universe!");
	MSG(elly, "Once you realize the true potential of reality, you’ll go mad with knowledge!");
	MSG(elly, "Don’t you see?! Nothing can stop us now! For we are—!");

	FACE(marisa, surprised);
	MSG(marisa, "But I’m already mad with knowledge! Why do ya think I drink all the time?!");
	MSG(marisa, "My mind’s always racin’ with this ‘n that…");
	MSG(marisa, "Ya think this is any different than how I usually live?! Get over y’erself!");
	FACE(marisa, inquisitive);
	MSG(marisa, "Now, I gotta ask… ya really, seriously don’t got anything?");
	MSG(marisa, "A single bookshelf?! A few loose scribbled notes?!");
	MSG(marisa, "Heck, I’ll even take one of those little glowy tablet thingies with all those ‘Pee Dee Effs’ on ‘em! They're so bright they hurt my eyes, but—");

	FACE(elly, eyes_closed);
	MSG(elly, "Since you’re OBVIOUSLY treating this as a joke, I REFUSE to speak with you any longer!");

	FACE(marisa, happy);
	MSG(marisa, "Oh c’mon, turn the madness-whatever off ‘n we’ll go out for drinks! There’s this nice new bar in town with a cute—");

	MSG(elly, "There’s no force in this world that would make me turn back now!");
	EVENT(music_changes);
	FACE(elly, smug);
	MSG(elly, "Someone as ‘ordinary’ as you getting in our way may be impressive…");
	FACE(elly, angry);
	MSG(elly, "But there’s no place for sorcery in our enlightened vision of Gensōkyō!");
	FACE(elly, shouting);
	MSG(elly, "Now, succumb to the madness!");

	DIALOG_END();
}

DIALOG_TASK(marisa, Stage6PreFinal) {
	DIALOG_BEGIN(Stage6PreFinal);

	ACTOR_LEFT(marisa);
	ACTOR_RIGHT(elly);
	VARIANT(elly, beaten);
	FACE(elly, normal);
	FACE(marisa, unamused);
	MSG(marisa, "Are ya sure ya don’t got a spellbook laying around?! Nothin’ at all?!");
	FACE(elly, angry);
	MSG(elly, "Are you still on about that?! Fine, see what good it does you!");
	FACE(elly, shouting);
	MSG(elly, "Your pitiful magic amounts to nothing against the true nature of reality!");

	DIALOG_END();
}

/*
 * Register the tasks
 */

#define EXPORT_DIALOG_TASKS_CHARACTER marisa
#include "export_dialog_tasks.inc.h"
