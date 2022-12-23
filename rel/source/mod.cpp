#include "evt_cmd.h"
#include "mod.h"
#include "patch.h"

#include <common.h>
#include <spm/camdrv.h>
#include <spm/dispdrv.h>
#include <spm/evtmgr_cmd.h>
#include <spm/evt_mario.h>
#include <spm/evt_msg.h>
#include <spm/fontmgr.h>
#include <spm/mario_pouch.h>
#include <spm/msgdrv.h>
#include <spm/seqdef.h>
#include <spm/seqdrv.h>
#include <spm/seq_game.h>
#include <spm/spmario.h>
#include <spm/spmario_snd.h>
#include <wii/gx.h>
#include <wii/os.h>
#include <msl/string.h>

namespace mod {

/*
	Handle win screen
*/

#define FRAMES_FADE_START (60 * 5)
#define STAGE_WIN_START 2
#define STAGE_FADE_START (STAGE_WIN_START + FRAMES_FADE_START)
#define FADE_INTERVAL 2
#define STAGE_FADE_MAX (STAGE_FADE_START + (FADE_INTERVAL * 0x80))

static spm::seqdef::SeqFunc * seq_mapChangeMainReal;

static void drawStr(const char * s, int y, int scale, bool rainbow, u8 alpha)
{
	// Set font code settings
	wii::gx::GXColor white {0xff, 0xff, 0xff, 0xff};
	spm::fontmgr::FontDrawStart_alpha(alpha);
	spm::fontmgr::FontDrawScale(scale);
	spm::fontmgr::FontDrawColor(&white);
	spm::fontmgr::FontDrawEdge();
	spm::fontmgr::FontDrawNoiseOff();
	if (rainbow)
		spm::fontmgr::FontDrawRainbowColor();
	else
		spm::fontmgr::FontDrawRainbowColorOff();

	// Centre align text
	f32 x = -(spm::fontmgr::FontGetMessageWidth(s) * scale / 2);

	// Draw string
	spm::fontmgr::FontDrawString(x, y, s);
}

static void winScreenPatch()
{
	seq_mapChangeMainReal = spm::seqdef::seq_data[spm::seqdrv::SEQ_MAPCHANGE].main;
	spm::seqdef::seq_data[spm::seqdrv::SEQ_MAPCHANGE].main = [](spm::seqdrv::SeqWork * wp)
	{
		// If the player has won, stop the game and display the messages
		if ((wp->stage >= STAGE_WIN_START) && (spm::mario_pouch::pouchGetXp() >= 500))
		{
			// Play credits music on first run
			if (wp->stage == STAGE_WIN_START)
				spm::spmario_snd::spsndBGMOn(0, "BGM_EVT_STAFF1");

			spm::dispdrv::dispEntry(spm::camdrv::CAM_ID_DEBUG_3D, 2, 0.0f, 
				[](s32 cameraId, void * param)
				{
					// Draw main message
					drawStr("You win!", 100, 5, true, 0xff);

					// Slowly fade in second message
					spm::seqdrv::SeqWork * wp = (spm::seqdrv::SeqWork *) param;
					if (wp->stage++ >= STAGE_FADE_START)
					{
						if (wp->stage >= STAGE_FADE_MAX)
							wp->stage = STAGE_FADE_MAX;
						
						drawStr("...was it worth it?", -50, 2, false,
 						        (wp->stage - STAGE_FADE_START) / FADE_INTERVAL);
					}
				},
				wp
			);
		}
		else
		{
			// Otherwise run the game as normal
			seq_mapChangeMainReal(wp);
		}
	};
}

/*
	Redirect maps to he3_04
*/

static void (*seqSetSeqReal)(s32 seq, const char * p1, const char * p2);

static bool firstLoad = true;

static void mapRedirectPatch()
{
	// Hijack map changing
	seqSetSeqReal = patch::hookFunction(spm::seqdrv::seqSetSeq,
		[](s32 seq, const char * p1, const char * p2)
		{
			// If the seqSetSeq is a map change and not the opening cutscene, override it
			if ((seq == spm::seqdrv::SEQ_MAPCHANGE) && (msl::string::strcmp(p1, "aa4_01") != 0))
			{
				if (firstLoad)
				{
					// Update sequence value on first run
					firstLoad = false;
					spm::spmario::gp->gsw0 = 419;
				}
				else
				{
					// If they went in the wrong door, reset score
					if (msl::string::strcmp(p1, "he3_03") == 0)
						spm::mario_pouch::pouchAddXp(spm::mario_pouch::pouchGetXp() * -1);
					else
						// Otherwise increment by 1
						spm::mario_pouch::pouchAddXp(1);
				}

				// Override map name to be the O'Chunks fight room and door name to be the left door
				p1 = "he3_04";
				p2 = "doa1_l";
			}

			// Run the actual function
			seqSetSeqReal(seq, p1, p2);
		}
	);
}

/*
	Displays a tutorial when gameplay starts
*/

static void (*seq_gameInitReal)(spm::seqdrv::SeqWork * wp);
static bool infoShown = false;

// Script to display the tutorial message box
EVT_DECLARE(tutorial_evt);
EVT_BEGIN(tutorial_evt)

// Disable player movement
USER_FUNC(spm::evt_mario::evt_mario_key_off, 0)

// Display text
USER_FUNC(spm::evt_msg::evt_msg_print, 0, PTR("desert_bus"), 0, 0)

// Hide window when finished
USER_FUNC(spm::evt_msg::evt_msg_continue)

// Enable player movement
USER_FUNC(spm::evt_mario::evt_mario_key_on)

RETURN()
EVT_END()

void infoPatch()
{
	seq_gameInitReal = patch::hookFunction(spm::seq_game::seq_gameInit,
		[](spm::seqdrv::SeqWork * wp)
		{
			// The first time gameplay starts, display the tutorial text
			if ((msl::string::strcmp(spm::spmario::gp->mapName, "he3_04") == 0) && !infoShown)
			{
				infoShown = true;
				spm::evtmgr::evtEntry(tutorial_evt, 0, 0);
			}

			seq_gameInitReal(wp);
		}
	);
}

/*
	Handle custom messages
*/

// Custom intro text
const char * opening_000 =
"<se 1>\n"
"<edge>\n"
"<dkey>\n"
"<clear>\n"
"<wpos -275 80 576 176>\n"
"<col ff9933ff>\n"
"         SPM Desert Bus\n"
"<col 9a74d9ff>\n"
"             By Seeky\n"
"<col ffffff80>\n"
"         Press 2 to start\n"
"<wait 36000000>\n" // a bit lazy, just waits for 10 hours in the hope they'll press a button by then
"</dkey>";

// Tutorial text
const char * desert_bus = 
"<system>\n"
"Increase your score by entering\n"
"the door on the right.\n"
"<k>\n<p>\n"
"Reach 500 to win!\n"
"<k>\n<p>\n"
"Enter the left door for a\n"
"fun surprise... (the higher\n"
"your score, the better)\n"
"<k>\n<o>\n";

static const char * (*msgSearchReal)(const char * msgName);

static void messagePatch()
{
	msgSearchReal = patch::hookFunction(spm::msgdrv::msgSearch,
		[](const char * msgName)
		{
			if (msl::string::strcmp(msgName, "opening_000") == 0)
				// Override intro cutscene's text
				return opening_000;
			else if (msl::string::strcmp(msgName, "desert_bus") == 0)
				// Add custom tutorial message
				return desert_bus;
			else
				return msgSearchReal(msgName);
		}
	);
}

/*
	Pouch handling
*/

void wipePouch()
{
	spm::mario_pouch::MarioPouchWork * wp = spm::mario_pouch::pouchGetPtr();
	for (int i = 0; i < 32; i++)
		wp->keyItem[i] = 0;
	
	for (int i = 0; i < 10; i++)
		wp->useItem[i] = 0;
	
	for (int i = 0; i < 4; i++)
	{
		wp->characters[i].itemType = 0;
		wp->characters[i].selectable = false;
		wp->characters[i].selected = false;
	}
	
	for (int i = 0; i < 16; i++)
	{
		wp->pixls[i].itemType = 0;
		wp->pixls[i].selectable = false;
		wp->pixls[i].selected = false;
	}

	wp->characters[0].itemType = 0xd8; // mario
	wp->characters[0].selectable = true;
	wp->characters[0].selected = true;
}

/*
	Script variable setting
*/

void initVars()
{
	// Set first 3d flip flag
	spm::evtmgr_cmd::evtSetValue(0, GSWF(386), 1);

	// Set first HP critical flag
	spm::evtmgr_cmd::evtSetValue(0, GSWF(420), 1);
}

/*
	Don't let the player die
*/

void (*pouchAddHpReal)(int increase);

void hpLossPatch()
{
	pouchAddHpReal = patch::hookFunction(spm::mario_pouch::pouchAddHp,
		[](int increase)
		{
			// If mario is going to die, leave him on 1 hp
			int curHp = spm::mario_pouch::pouchGetHp();
			if ((curHp + increase) < 1)
				increase = 1 - curHp;
			
			// Do change
			pouchAddHpReal(increase);
		}
	);
}

/*
	Override intro screen music
*/

bool (*spsndBGMOnReal)(u32 flags, const char * name);

void introMusicPatch()
{
	spsndBGMOnReal = patch::hookFunction(spm::spmario_snd::spsndBGMOn,
		[](u32 flags, const char * name)
		{
			// If the BGM is the intro music, replace it with fracktail's intro
			if (msl::string::strcmp(name, "BGM_EVT_OPENING1") == 0)
				name = "BGM_EVT_STG1_BOSS_APPEAR1";
			
			return spsndBGMOnReal(flags, name);
		}
	);
}

/*
	Entrypoint
*/

void main()
{
	wii::os::OSReport("SPM Desert Bus v1.0: main running\n");

	mapRedirectPatch();
	winScreenPatch();
	messagePatch();
	wipePouch();
	initVars();
	infoPatch();
	hpLossPatch();
	introMusicPatch();
}

}
