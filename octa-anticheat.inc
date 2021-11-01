/*
* Octa AntiCheat
* Developed by Atom, Heix
* Thanks to: Deru, Vinter, Nahuel37
* GitHub: github.com/RealAtom
* Powered by Hyaxe
*/

#if defined _OCTA_AC_INCLUDED
	#endinput
#endif
#define _OCTA_AC_INCLUDED

#include <a_samp>
#include <Pawn.RakNet>

/* Config */
#define OCTA_VERSION "0.0.2"

#define OCTA_DEBUG

#if !defined MAX_CHAT_FLOOD
	#define MAX_CHAT_FLOOD			3
#endif

#if !defined MAX_DEATH_FLOOD
	#define MAX_DEATH_FLOOD			3
#endif

#if !defined MAX_ENTER_VEH_FLOOD
	#define MAX_ENTER_VEH_FLOOD		2
#endif

#if !defined MAX_EXIT_VEH_FLOOD
	#define MAX_EXIT_VEH_FLOOD		2
#endif

#if !defined MAX_CLASS_FLOOD
	#define MAX_CLASS_FLOOD			2
#endif

#if !defined MAX_SPAWN_FLOOD
	#define MAX_SPAWN_FLOOD			2
#endif

#if !defined MAX_PLAYERS_ON_IP
	#define MAX_PLAYERS_ON_IP		3
#endif

/* Core */
forward OnCheatDetected(playerid, cheat_id, const cheat_name[], cheat_detections);

#if !defined isnull
    #define isnull(%1) \
                ((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif

static const // RakNet packets
	PLAYER_SYNC = 207,
	AIM_SYNC = 203,
	WEAPONS_UPDATE_SYNC = 204,
	BULLET_SYNC = 206,
	UNOCCUPIED_SYNC = 209,
	VEHICLE_SYNC = 200,
	SPECTATING_SYNC = 212,
	VEHICLE_DESTROYED = 136,
	CHAT_RPC = 101,
	DEATH_RPC = 53,
	ENTER_VEHICLE_RPC = 26,
	EXIT_VEHICLE_RPC = 154,
	REQUEST_CLASS_RPC = 128,
	REQUEST_SPAWN_RPC = 129
;

enum _:ac_cheats // Cheats types
{
	CHEAT_SPECIAL_ACTION,
	CHEAT_FLY,
	CRASHER_CAMERA,
	CRASHER_BULLET,
	CHEAT_SPEED,
	FLOOD_CHAT,
	FLOOD_DEATH,
	FLOOD_ENTER_VEH,
	FLOOD_EXIT_VEH,
	FLOOD_CLASS,
	FLOOD_SPAWN,
	CHEAT_TELEPORT,
	CHEAT_UNFREEZE,
	CHEAT_INVISIBLE_SURFING,
	CHEAT_INVISIBLE_AIM,
	CHEAT_SPEC,
	CHEAT_WEAPON,
	CHEAT_AMMO,
	CHEAT_FAKEBULLET,
	CRASHER_UNOCCUPIED,
	CRASHER_ENTER,
	CHEAT_MONEY,
	CHEAT_LIMIT_SESSION,
	CHEAT_CARJACK,
	CRASHER_BULLET_POS,
	CHEAT_DIALOGS,
	CHEAT_RAPIDFIRE,
	CHEAT_NORELOAD
};

enum ac_info
{
	ac_name[64], // Cheat name
	bool:ac_enabled // Global activation
};

new ANTICHEAT_INFO[][ac_info] =
{
	{ !"special action", true },
	{ !"fly", true },
	{ !"invalid camera crasher", false },
	{ !"bullet crasher", true },
	{ !"speed", true },
	{ !"chat flood", true },
	{ !"death flood", true },
	{ !"enter vehicle flood", true },
	{ !"exit vehicle flood", true },
	{ !"request class flood", true },
	{ !"request spawn flood", true },
	{ !"teleport", true },
	{ !"unfreeze", true },
	{ !"invisible surfing", true },
	{ !"invisible aim", true },
	{ !"spec", true },
	{ !"weapon", true },
	{ !"ammo", true },
	{ !"fake bullet", true },
	{ !"unoccupied crasher", true },
	{ !"enter crasher", true },
	{ !"money", true },
	{ !"exceed maximum players in one ip", true },
	{ !"car jack", true },
	{ !"fake bullet position", true },
	{ !"dialog spoofing", true },
	{ !"rapidfire", true },
	{ !"noreload", true }
};

enum _:ac_player
{
	pac_detections, 	// Total of anticheat detections
	pac_last_detection, // Time of last detection
	bool:pac_immunity 	// true = skip, false = call OnCheatDetected 
};
static ANTICHEAT_PLAYER[MAX_PLAYERS][ sizeof(ANTICHEAT_INFO) ][ac_player];
static ANTICHEAT_DELAY[MAX_PLAYERS][ac_cheats];

enum _:data_player // Local playe data
{
	bool:p_kicked,
	Float:p_position[3],
	p_interior,
	p_freeze,
	p_special_action,
	p_spectating,
	p_vehicleid,
	p_last_chat_message,
	p_chat_flood_attemps,
	p_last_death,
	p_death_flood_attemps,
	p_last_enter_veh,
	p_enter_veh_attemps,
	p_last_exit_veh,
	p_exit_veh_attemps,
	p_last_request_class,
	p_class_attemps,
	p_last_request_spawn,
	p_spawn_attemps,
	p_money,
	p_dialog,
	p_last_shot_tick,
	p_last_weapon_fired,
	p_bullets_fired
}
static PLAYER_DATA[MAX_PLAYERS][data_player];

enum _:weapons_player
{
	bool:w_valid,
	w_id,
	w_ammo
};
static PLAYER_WEAPONS[MAX_PLAYERS][13][weapons_player];

// Weapons info
new const
	gMinWeaponFiringTime[][] = {
        { 150, 150 }, // 22 (Colt45)
        { 340, 150 }, // 23 (Colt45 con silenciador)
        { 150, 150 }, // 24 (Deagle)
        { 800, 800 }, // 25 (Escopeta)
        { 115, 115 }, // 26 (Escopetas recortadas)
        { 200, 150 }, // 27 (Escopetas de combate)
        { 20, 100 },  // 28 (Uzi)
        { 70, 140 },  // 29 (MP5)
        { 60, 140 },  // 30 (AK-47)
        { 60, 140 },  // 31 (M4)
        { 30, 140 },  // 32 (Tec-9)
        { 800, 800 }, // 33 (Rifle)
        { 800, 800 }  // 34 (Sniper)
    },
	gMaxBulletsPerRound[] = {
        34, // 22 (Colt45)
        17, // 23 (Colt45 con silenciador)
        7,  // 24 (Desert Eagle)
        1,  // 25 (Escopeta)
        4,  // 26 (Escopetas recortadas)
        7,  // 27 (Escopeta de combate)
        100, // 28 (Uzi)
        30, // 29 (MP5)
        30, // 30 (AK-47)
        50, // 31 (M4)
        100, // 32 (Tec-9)
        1, // 33 (Rifle)
        2, // 34 (Sniper)
        0, // Arma inválida
    }
;

static bool:ot_pedAnims = false; // Server animation type (ped or CJ)

// Tick difference
static GetTickDiff(newtick, oldtick)
{
	if (oldtick < 0 && newtick >= 0) {
		return newtick - oldtick;
	} else if (oldtick >= 0 && newtick < 0 || oldtick > newtick) {
		return (cellmax - oldtick + 1) - (cellmin - newtick);
	}
	return newtick - oldtick;
}

// Reset all player data
stock ot_ResetPlayerVariables(playerid)
{
	new temp_PLAYER_DATA[data_player]; PLAYER_DATA[playerid] = temp_PLAYER_DATA;
	PLAYER_DATA[playerid][p_kicked] = false;

	for(new i = 0; i < sizeof(ANTICHEAT_PLAYER[]); i ++)
	{
		ANTICHEAT_PLAYER[playerid][i][pac_detections] = 0;
		ANTICHEAT_PLAYER[playerid][i][pac_last_detection] = 0;
		ANTICHEAT_PLAYER[playerid][i][pac_immunity] = false;
	}

	for(new i = 0; i < sizeof(ANTICHEAT_DELAY[]); i ++)
	{
		ANTICHEAT_DELAY[playerid][i] = 0;
	}

	for(new i = 0; i < sizeof(PLAYER_WEAPONS[]); i ++)
	{
		PLAYER_WEAPONS[playerid][i][w_valid] = false;
		PLAYER_WEAPONS[playerid][i][w_id] = 0;
		PLAYER_WEAPONS[playerid][i][w_ammo] = 0;
	}
	return 1;
}

/* Init */
public OnGameModeInit()
{
	printf("\n\t  ____  ______________ ");
	printf("\t / __ \\/ ___/_  __/ _ |");
	printf("\t/ /_/ / /__  / / / __ |");
	printf("\t\\____/\\___/ /_/ /_/ |_|\n");

	printf("Octa AntiCheat v"OCTA_VERSION" initialized successfully!\n");
	
	#if defined OT_OnGameModeInit
		return OT_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit OT_OnGameModeInit
#if defined OT_OnGameModeInit
	forward OT_OnGameModeInit();
#endif

/* Player Callbacks */
public OnPlayerDisconnect(playerid, reason)
{
	ot_ResetPlayerVariables(playerid);

	#if defined OT_OnPlayerDisconnect
		return OT_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect OT_OnPlayerDisconnect
#if defined OT_OnPlayerDisconnect
	forward OT_OnPlayerDisconnect(playerid, reason);
#endif

/* Utils */
static GetWeaponSlot(weaponid)
{
   	switch(weaponid)
   	{
    	case 0, 1: return 0;
      	case 2..9: return 1;
      	case 10..15: return 10;
      	case 16..18: return 8;
      	case 22..24: return 2;
      	case 25..27: return 3;
      	case 28, 29, 32: return 4;
      	case 30, 31: return 5;
      	case 33, 34: return 6;
      	case 35..38: return 7;
      	case 39: return 8;
      	case 40: return 12;
      	case 41..43: return 9;
      	case 44..46: return 11;
   	}
   	return -1;
}

static Float:GetDistanceBetweenPoints3D(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2)
{
    return VectorSize(x1 - x2, y1 - y2, z1 - z2);
}

// Kick player with delay
stock ot_CheatKick(playerid, cheat_id)
{
	if (!PLAYER_DATA[playerid][p_kicked])
	{
		static const fmt_str[] = "[OCTA]{FFFFFF} You were kicked out of the server for: %s";
    	new string[sizeof(fmt_str) + (-2 + 64)];

		strunpack(string, ANTICHEAT_INFO[cheat_id][ac_name], sizeof(string));
    	format(string, sizeof(string), fmt_str, string);

		SendClientMessage(playerid, 0x0090FFFF, string);

		SetTimerEx("ot_Kick", 100, false, "i", playerid);
		PLAYER_DATA[playerid][p_kicked] = true;
	}
	return 1;
}

forward ot_Kick(playerid);
public ot_Kick(playerid)
{
	return Kick(playerid);
}

// Logs a warning but does nothing
stock ot_RegisterDetection(playerid, cheat_id)
{
	ANTICHEAT_PLAYER[playerid][cheat_id][pac_last_detection] = gettime();
	++ ANTICHEAT_PLAYER[playerid][cheat_id][pac_detections];
	return 1;
}

// Register a warning and call the corresponding callback, it is recommended to kick the player in this case
stock ot_ReturnCallback(playerid, cheat_id)
{
	new detections = ANTICHEAT_PLAYER[playerid][cheat_id][pac_detections];
	ANTICHEAT_PLAYER[playerid][cheat_id][pac_detections] = 0;
	ANTICHEAT_PLAYER[playerid][cheat_id][pac_last_detection] = gettime();

	#if defined OCTA_DEBUG
		new
			name[MAX_PLAYER_NAME],
			cheat_name[64]
		;
		strunpack(cheat_name, ANTICHEAT_INFO[cheat_id][ac_name], sizeof(cheat_name));
		GetPlayerName(playerid, name, sizeof(name));
		printf("[OCTA DETECTION] %s(%i): %s (%d detections)", name, playerid, cheat_name, detections);

		// This line is only to facilitate testing of TP Hack
		GetPlayerPos(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]);
	#endif

	#if defined OnCheatDetected
		return OnCheatDetected(playerid, cheat_id, ANTICHEAT_INFO[cheat_id][ac_name], detections);
	#else
		ot_CheatKick(playerid, cheat_id);
		return 1;
	#endif
}

// Enable global detection
stock ToggleCheatDetection(cheat_id, bool:enable)
{
	ANTICHEAT_INFO[cheat_id][ac_enabled] = enable;
	return 1;
}

// Per player detection
stock SetCheatImmunity(playerid, cheat_id, bool:immunity)
{
	ANTICHEAT_PLAYER[playerid][cheat_id][pac_immunity] = immunity;
	return 1;
}

stock bool:GetCheatImmunity(playerid, cheat_id)
{
	return ANTICHEAT_PLAYER[playerid][cheat_id][pac_immunity];
}

stock GetPlayerDetections(playerid, cheat_id)
{
	return ANTICHEAT_PLAYER[playerid][cheat_id][pac_detections];
}

stock GetPlayerLastDetection(playerid, cheat_id)
{
	return ANTICHEAT_PLAYER[playerid][cheat_id][pac_last_detection];
}

/* Cheat Detection */
public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	++PLAYER_DATA[playerid][p_bullets_fired];

	/* NoReload */
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_NORELOAD][pac_immunity] && ANTICHEAT_INFO[CHEAT_NORELOAD][ac_enabled])
	{
		if (22 <= weaponid <= 34)
        {
            if (PLAYER_DATA[playerid][p_bullets_fired] > gMaxBulletsPerRound[weaponid - 22])
            {
                if ((ANTICHEAT_PLAYER[playerid][CHEAT_NORELOAD][pac_last_detection] + 2) < gettime())
                    ANTICHEAT_PLAYER[playerid][CHEAT_NORELOAD][pac_detections] = 0;

				ot_RegisterDetection(playerid, CHEAT_NORELOAD);

				if (ANTICHEAT_PLAYER[playerid][CHEAT_NORELOAD][pac_detections] >= 3)
					ot_ReturnCallback(playerid, CHEAT_NORELOAD);

                return 0;
            }
        }
	}

	/* RapidFire */
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_RAPIDFIRE][pac_immunity] && ANTICHEAT_INFO[CHEAT_RAPIDFIRE][ac_enabled])
	{
		if (PLAYER_DATA[playerid][p_last_weapon_fired] != weaponid)
        	PLAYER_DATA[playerid][p_last_shot_tick] = 0;

        PLAYER_DATA[playerid][p_last_weapon_fired] = weaponid;

        if (weaponid < 35)
        {
			new diff = GetTickDiff(GetTickCount(), PLAYER_DATA[playerid][p_last_shot_tick]);
			
            if (GetPlayerState(playerid) == PLAYER_STATE_PASSENGER)
            {
                if (diff < gMinWeaponFiringTime[weaponid - 22][1])
                {
                    if ((ANTICHEAT_PLAYER[playerid][CHEAT_RAPIDFIRE][pac_last_detection] + 2) < gettime())
                    	ANTICHEAT_PLAYER[playerid][CHEAT_RAPIDFIRE][pac_detections] = 0;
                    
                    ot_RegisterDetection(playerid, CHEAT_RAPIDFIRE);

					if (ANTICHEAT_PLAYER[playerid][CHEAT_RAPIDFIRE][pac_detections] >= 12)
						ot_ReturnCallback(playerid, CHEAT_RAPIDFIRE);

                    return 0;
                }
            }
            else
            {
                if (diff < gMinWeaponFiringTime[weaponid - 22][0])
                {
                    if ((ANTICHEAT_PLAYER[playerid][CHEAT_RAPIDFIRE][pac_last_detection] + 2) < gettime())
                    	ANTICHEAT_PLAYER[playerid][CHEAT_RAPIDFIRE][pac_detections] = 0;
                    
					ot_RegisterDetection(playerid, CHEAT_RAPIDFIRE);

					if (ANTICHEAT_PLAYER[playerid][CHEAT_RAPIDFIRE][pac_detections] >= 10)
						ot_ReturnCallback(playerid, CHEAT_RAPIDFIRE);

                    return 0;
                }
            }
        }
	}

	PLAYER_DATA[playerid][p_last_shot_tick] = GetTickCount();

	#if defined OT_OnPlayerWeaponShot
		return OT_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerWeaponShot
	#undef OnPlayerWeaponShot
#else
	#define _ALS_OnPlayerWeaponShot
#endif

#define OnPlayerWeaponShot OT_OnPlayerWeaponShot
#if defined OT_OnPlayerWeaponShot
	forward OT_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
#endif

IPacket:PLAYER_SYNC(playerid, BitStream:bs)
{
	if (PLAYER_DATA[playerid][p_kicked]) return 0;

	new onFootData[PR_OnFootSync];
	BS_IgnoreBits(bs, 8);
	BS_ReadOnFootSync(bs, onFootData);

	/* Special Action Hack */
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_SPECIAL_ACTION][pac_immunity] && ANTICHEAT_INFO[CHEAT_SPECIAL_ACTION][ac_enabled])
	{
		if (onFootData[PR_specialAction] == SPECIAL_ACTION_USEJETPACK && onFootData[PR_specialAction] != PLAYER_DATA[playerid][p_special_action])
		{
			ot_ReturnCallback(playerid, CHEAT_SPECIAL_ACTION);
			return 0;
		}
	}

	/* FlyHack */
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_FLY][pac_immunity] && ANTICHEAT_INFO[CHEAT_FLY][ac_enabled])
	{

		if (onFootData[PR_specialAction] == SPECIAL_ACTION_USEJETPACK)
		{
			ot_ReturnCallback(playerid, CHEAT_FLY);
			return 0;
		}
		
		switch(onFootData[PR_animationId])
        {
			case 157, 159, 161, 1058:
	        {
	            if (!IsPlayerInAnyVehicle(playerid))
	            {
					ot_ReturnCallback(playerid, CHEAT_FLY);
					return 0;
	            }
	        }

	        case 958, 959:
	        {
	        	if (onFootData[PR_weaponId] != WEAPON_PARACHUTE)
				{
					ot_ReturnCallback(playerid, CHEAT_FLY);
					return 0;
				}
	        }

	        case 1538, 1539, 1543:
	        {
	        	if (onFootData[PR_position][2] > 1.0)
				{
					ot_ReturnCallback(playerid, CHEAT_FLY);
					return 0;
				}
	        }
	    }
	}

	/* Bullet Crasher */
	if (!ANTICHEAT_PLAYER[playerid][CRASHER_BULLET][pac_immunity] && ANTICHEAT_INFO[CRASHER_BULLET][ac_enabled] && onFootData[PR_position][2] == -5.5)
	{
		ot_ReturnCallback(playerid, CRASHER_BULLET);
		return 0;
	}

	/* Speed Hack */
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_SPEED][pac_immunity] && ANTICHEAT_INFO[CHEAT_SPEED][ac_enabled] && onFootData[PR_surfingVehicleId] == 0)
	{
		if (onFootData[PR_specialAction] == SPECIAL_ACTION_USEJETPACK || IsPlayerInAnyVehicle(playerid)) return 1;

		// Animations with these IDs tend to increase speed for nothing, doing so will trigger the speedhack detection.
		if (onFootData[PR_animationId] == 1130 || onFootData[PR_animationId] == 1062 || onFootData[PR_animationId] == 1132) return 1;

		new animlib[16], animname[32], bool:is_cheating;
		GetAnimationName(onFootData[PR_animationId], animlib, sizeof(animlib), animname, sizeof(animname));

		if (!strcmp(animname, "GUNCROUCHFWD") && (floatabs(onFootData[PR_velocity][0]) > 0.08 || floatabs(onFootData[PR_velocity][1]) > 0.08))
			is_cheating = true;

		if (!ot_pedAnims && (floatabs(onFootData[PR_velocity][0]) > 0.23 || floatabs(onFootData[PR_velocity][1]) > 0.23)) 
			is_cheating = true;
		
		else if (ot_pedAnims && (floatabs(onFootData[PR_velocity][0]) > 0.29 || floatabs(onFootData[PR_velocity][1]) > 0.29))
			is_cheating = true;

		if (is_cheating)
		{
			ot_RegisterDetection(playerid, CHEAT_SPEED);

			if (ANTICHEAT_PLAYER[playerid][CHEAT_SPEED][pac_detections] >= 4)
				ot_ReturnCallback(playerid, CHEAT_SPEED);

			return 0;
		}
	}

	/* Teleport Hack */
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_TELEPORT][pac_immunity] && ANTICHEAT_INFO[CHEAT_TELEPORT][ac_enabled])
	{
		if (gettime() > ANTICHEAT_DELAY[playerid][CHEAT_TELEPORT])
		{
			new 
				Float:distance = GetPlayerDistanceFromPoint(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]),
				Float:limit_distance = 15.0,
				player_state = GetPlayerState(playerid)
			;

			if (Player_IsDead(playerid)) distance = 3.0;
			else
			{
				switch(player_state)
				{
					case PLAYER_STATE_ONFOOT: limit_distance = 35.0;
					case PLAYER_STATE_DRIVER, PLAYER_STATE_PASSENGER: limit_distance = 50.0;
				}
			}

			if (distance > limit_distance || GetPlayerInterior(playerid) != PLAYER_DATA[playerid][p_interior])
			{
				ot_RegisterDetection(playerid, CHEAT_TELEPORT);

				// Back to original position
				SetPlayerPos(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]);
				SetPlayerInterior(playerid, PLAYER_DATA[playerid][p_interior]);

				if (ANTICHEAT_PLAYER[playerid][CHEAT_TELEPORT][pac_detections] >= 5)
					ot_ReturnCallback(playerid, CHEAT_TELEPORT);
				return 0;
			}

			// Update position
			GetPlayerPos(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]);
			ANTICHEAT_DELAY[playerid][CHEAT_TELEPORT] = (gettime() + 1);
		}
	}

	/* UnFreeze Hack */
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_UNFREEZE][pac_immunity] && ANTICHEAT_INFO[CHEAT_UNFREEZE][ac_enabled])
	{
		if ((ANTICHEAT_PLAYER[playerid][CHEAT_UNFREEZE][pac_last_detection] + 1) < gettime())
		{
			if (PLAYER_DATA[playerid][p_freeze] == 1)
			{
				new Float:distance = GetPlayerDistanceFromPoint(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]);
			
				if (distance > 3.0)
				{
					ot_RegisterDetection(playerid, CHEAT_UNFREEZE);

					if (ANTICHEAT_PLAYER[playerid][CHEAT_UNFREEZE][pac_detections] >= 3)
						ot_ReturnCallback(playerid, CHEAT_UNFREEZE);
					return 0;
				}
			}
		}
	}

	/* Invisible vehicle surfing */
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_INVISIBLE_SURFING][pac_immunity] && ANTICHEAT_INFO[CHEAT_INVISIBLE_SURFING][ac_enabled])
	{
		if (onFootData[PR_surfingVehicleId] != 0 && onFootData[PR_surfingVehicleId] != INVALID_VEHICLE_ID)
		{
            if ((floatabs(onFootData[PR_surfingOffsets][0]) >= 50.0) || (floatabs(onFootData[PR_surfingOffsets][1]) >= 50.0) || (floatabs(onFootData[PR_surfingOffsets][2]) >= 50.0))
            {
				ot_ReturnCallback(playerid, CHEAT_INVISIBLE_SURFING);
				return 0;
            } 
        }
	}

	/* Spec hack */
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_SPEC][pac_immunity] && ANTICHEAT_INFO[CHEAT_SPEC][ac_enabled])
	{
		if (GetPlayerState(playerid) == PLAYER_STATE_SPECTATING && PLAYER_DATA[playerid][p_spectating] == 0)
		{
			ot_ReturnCallback(playerid, CHEAT_SPEC);
			return 0;
		}
	}

	// Ammo hack
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_AMMO][pac_immunity] && ANTICHEAT_INFO[CHEAT_AMMO][ac_enabled])
	{
		if ((ANTICHEAT_PLAYER[playerid][CHEAT_AMMO][pac_last_detection] + 1) < gettime())
		{
			if (onFootData[PR_weaponId] && onFootData[PR_weaponId] != 46)
			{
				new 
					weapon_slot = GetWeaponSlot(onFootData[PR_weaponId]),
					weapon_id,
					weapon_ammo
				;

				GetPlayerWeaponData(playerid, weapon_slot, weapon_id, weapon_ammo);
				if (weapon_ammo > (PLAYER_WEAPONS[playerid][weapon_slot][w_ammo] + 10) || weapon_ammo < -1)
				{
					ot_RegisterDetection(playerid, CHEAT_AMMO);

					if (ANTICHEAT_PLAYER[playerid][CHEAT_AMMO][pac_detections] >= 2)
						ot_ReturnCallback(playerid, CHEAT_AMMO);

					return 0;
				}

				PLAYER_WEAPONS[playerid][weapon_slot][w_ammo] = weapon_ammo;
			}
		}
	}
	return 1;
}

IPacket:AIM_SYNC(playerid, BitStream:bs)
{
	if (PLAYER_DATA[playerid][p_kicked]) return 0;

    new aimData[PR_AimSync];
    BS_IgnoreBits(bs, 8);
    BS_ReadAimSync(bs, aimData);

    /* Invalid Camera */
	if (!ANTICHEAT_PLAYER[playerid][CRASHER_CAMERA][pac_immunity] && ANTICHEAT_INFO[CRASHER_CAMERA][ac_enabled] && aimData[PR_camMode] == 45 || aimData[PR_camMode] == 49)
	{
		ot_ReturnCallback(playerid, CRASHER_CAMERA);
		return 0;
	}

	/* Invisible Aim */
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_INVISIBLE_AIM][pac_immunity] && ANTICHEAT_INFO[CHEAT_INVISIBLE_AIM][ac_enabled])
	{
		if (aimData[PR_aimZ] != aimData[PR_aimZ])
    	{
			ot_ReturnCallback(playerid, CHEAT_INVISIBLE_AIM);
			return 0;
    	}
	}

	/* NoReload */
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_NORELOAD][pac_immunity] && ANTICHEAT_INFO[CHEAT_NORELOAD][ac_enabled])
	{
		if (aimData[PR_weaponState] == WEAPONSTATE_RELOADING || aimData[PR_weaponState] == WEAPONSTATE_NO_BULLETS)
			PLAYER_DATA[playerid][p_bullets_fired] = 0;
	}
    return 1;
}

IPacket:WEAPONS_UPDATE_SYNC(playerid, BitStream:bs)
{
	if (PLAYER_DATA[playerid][p_kicked]) return 0;

    new weaponsUpdate[PR_WeaponsUpdate];
    BS_IgnoreBits(bs, 8);
    BS_ReadWeaponsUpdate(bs, weaponsUpdate);

	/* Weapons Fixes */
    for (new i = 0; i < sizeof(PLAYER_WEAPONS[]); i++)
	{
		// Weapon hack
		if (!ANTICHEAT_PLAYER[playerid][CHEAT_WEAPON][pac_immunity] && ANTICHEAT_INFO[CHEAT_WEAPON][ac_enabled])
		{
			if (weaponsUpdate[PR_slotWeaponId][i] > 0)
			{
				if (!PLAYER_WEAPONS[playerid][i][w_valid])
				{
					SetPlayerArmedWeapon(playerid, 0);
					ot_ReturnCallback(playerid, CHEAT_WEAPON);
					return 0;
				}
			}
		}
	}
    return 1;
}

IPacket:BULLET_SYNC(playerid, BitStream:bs)
{
	if (PLAYER_DATA[playerid][p_kicked]) return 0;

    new bulletData[PR_BulletSync];
    BS_IgnoreBits(bs, 8);
    BS_ReadBulletSync(bs, bulletData);

	/* Fake bullets (malformed packets) */
    if (!ANTICHEAT_PLAYER[playerid][CHEAT_FAKEBULLET][pac_immunity] && ANTICHEAT_INFO[CHEAT_FAKEBULLET][ac_enabled])
	{
		new slot = GetWeaponSlot(bulletData[PR_weaponId]);
		if (!PLAYER_WEAPONS[playerid][slot][w_valid])
		{
			ot_ReturnCallback(playerid, CHEAT_FAKEBULLET);
			return 0;
		}
	}

	/* Damager */
	if (bulletData[PR_hitType] == BULLET_HIT_TYPE_PLAYER && (bulletData[PR_weaponId] < WEAPON_COLT45 || bulletData[PR_weaponId] > WEAPON_SNIPER))
	{
		Kick(playerid);
		return 0;
	}

	/* Bullet crasher */
	if (!ANTICHEAT_PLAYER[playerid][CRASHER_BULLET_POS][pac_immunity] && ANTICHEAT_INFO[CRASHER_BULLET_POS][ac_enabled])
	{
		if (!(-35000.0 <= bulletData[PR_origin][0] <= 35000.0) || !(-35000.0 <= bulletData[PR_origin][1] <= 35000.0) || !(-35000.0 <= bulletData[PR_origin][2] <= 35000.0))
		{
			ot_ReturnCallback(playerid, CRASHER_BULLET_POS);
			return 0;	
		}

		if (!(-35000.0 <= bulletData[PR_hitPos][0] <= 35000.0) || !(-35000.0 <= bulletData[PR_hitPos][1] <= 35000.0) || !(-35000.0 <= bulletData[PR_hitPos][2] <= 35000.0))
		{
			ot_ReturnCallback(playerid, CRASHER_BULLET_POS);
			return 0;	
		}
	}
    return 1;
}

IPacket:UNOCCUPIED_SYNC(playerid, BitStream:bs)
{
	if (PLAYER_DATA[playerid][p_kicked]) return 0;

    new unoccupiedData[PR_UnoccupiedSync];
    BS_IgnoreBits(bs, 8);
    BS_ReadUnoccupiedSync(bs, unoccupiedData);

    /* Unoccupied crasher (thx https://vk.com/franzesco and https://vk.com/dataflex) */
    if (!ANTICHEAT_PLAYER[playerid][CRASHER_UNOCCUPIED][pac_immunity] && ANTICHEAT_INFO[CRASHER_UNOCCUPIED][ac_enabled])
	{
	    if ((floatabs(unoccupiedData[PR_roll][0]) > 1.0) ||
	        (floatabs(unoccupiedData[PR_roll][1]) > 1.0) ||
	        (floatabs(unoccupiedData[PR_roll][2]) > 1.0) ||
	        (floatabs(unoccupiedData[PR_direction][0]) > 1.0) ||
	        (floatabs(unoccupiedData[PR_direction][1]) > 1.0) ||
	        (floatabs(unoccupiedData[PR_direction][2]) > 1.0) ||
	        (floatabs(unoccupiedData[PR_position][0]) > 20000.0) ||
	        (floatabs(unoccupiedData[PR_position][1]) > 20000.0) ||
	        (floatabs(unoccupiedData[PR_position][2]) > 20000.0) ||
	        (floatabs(unoccupiedData[PR_angularVelocity][0]) > 1.0) ||
	        (floatabs(unoccupiedData[PR_angularVelocity][1]) > 1.0) ||
	        (floatabs(unoccupiedData[PR_angularVelocity][2]) > 1.0) ||
	        (floatabs(unoccupiedData[PR_velocity][0]) > 100.0) ||
	        (floatabs(unoccupiedData[PR_velocity][1]) > 100.0) ||
	        (floatabs(unoccupiedData[PR_velocity][2]) > 100.0)
	    )
	    {
	        ot_ReturnCallback(playerid, CRASHER_UNOCCUPIED);
			return 0;
	    }
	}

	/* Vehicle enter crasher (thx f0Re3t and Nexius) */
	if (!ANTICHEAT_PLAYER[playerid][CRASHER_ENTER][pac_immunity] && ANTICHEAT_INFO[CRASHER_ENTER][ac_enabled])
	{
		if ((unoccupiedData[PR_roll][0] == unoccupiedData[PR_direction][0]) && (unoccupiedData[PR_roll][1] == unoccupiedData[PR_direction][1]) && (unoccupiedData[PR_roll][2] == unoccupiedData[PR_direction][2]))
		{
			ot_ReturnCallback(playerid, CRASHER_ENTER);
	        return 0;
	    }
	}
    return 1;
}

IPacket:VEHICLE_SYNC(playerid, BitStream:bs)
{
	if (PLAYER_DATA[playerid][p_kicked]) return 0;

    new inCarData[PR_InCarSync];
    BS_IgnoreBits(bs, 8);
    BS_ReadInCarSync(bs, inCarData);

	/* Teleport Hack */
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_TELEPORT][pac_immunity] && ANTICHEAT_INFO[CHEAT_TELEPORT][ac_enabled])
	{
		if (gettime() > ANTICHEAT_DELAY[playerid][CHEAT_TELEPORT])
		{
			// Get distance between old position and actual position
			new Float:distance = GetDistanceBetweenPoints3D(
				inCarData[PR_position][0], inCarData[PR_position][1], inCarData[PR_position][2],
				PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]
			);
			
			if (distance > 120.0)
			{
				ot_RegisterDetection(playerid, CHEAT_TELEPORT);

				// Back to original position
				SetVehiclePos(inCarData[PR_vehicleId], PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]);
				SetPlayerInterior(playerid, PLAYER_DATA[playerid][p_interior]);

				if (ANTICHEAT_PLAYER[playerid][CHEAT_TELEPORT][pac_detections] >= 5)
					ot_ReturnCallback(playerid, CHEAT_TELEPORT);
				return 0;
			}

			// Update position
			PLAYER_DATA[playerid][p_position][0] = inCarData[PR_position][0];
    		PLAYER_DATA[playerid][p_position][1] = inCarData[PR_position][1];
    		PLAYER_DATA[playerid][p_position][2] = inCarData[PR_position][2];

			ANTICHEAT_DELAY[playerid][CHEAT_TELEPORT] = (gettime() + 1);
		}
	}

	/* Car stealer (used in troll hacks) */
    if (!ANTICHEAT_PLAYER[playerid][CHEAT_CARJACK][pac_immunity] && ANTICHEAT_INFO[CHEAT_CARJACK][ac_enabled])
	{
	    if (PLAYER_DATA[playerid][p_vehicleid] != inCarData[PR_vehicleId])
	    {
			ot_ReturnCallback(playerid, CHEAT_CARJACK);
			return 0;
		}
	}
	return 1;
}

IPacket:SPECTATING_SYNC(playerid, BitStream:bs)
{
    if (PLAYER_DATA[playerid][p_kicked])
		return 0;
    return 1;
}

IRPC:VEHICLE_DESTROYED(playerid, BitStream:bs)
{
    new vehicleid;

    BS_ReadUint16(bs, vehicleid);

    if (GetVehicleModel(vehicleid) < 400)
    {
        return 0;
    }

    return OnVehicleRequestDeath(vehicleid, playerid);
}

IRPC:CHAT_RPC(playerid, BitStream:bs)
{
	// Rate-limited OnPlayerText
    if (!ANTICHEAT_PLAYER[playerid][FLOOD_CHAT][pac_immunity] && ANTICHEAT_INFO[FLOOD_CHAT][ac_enabled])
	{
		if ((GetTickCount() - PLAYER_DATA[playerid][p_last_chat_message]) < 500)
		{
			++ PLAYER_DATA[playerid][p_chat_flood_attemps];

			if (PLAYER_DATA[playerid][p_chat_flood_attemps] >= MAX_CHAT_FLOOD)
			{
				ot_ReturnCallback(playerid, FLOOD_CHAT);
				return 0;
			}
		}
		PLAYER_DATA[playerid][p_last_chat_message] = GetTickCount();
	}
    return 1;
}

IRPC:DEATH_RPC(playerid, BitStream:bs)
{
	// Rate-limited Death
	if (!ANTICHEAT_PLAYER[playerid][FLOOD_DEATH][pac_immunity] && ANTICHEAT_INFO[FLOOD_DEATH][ac_enabled])
	{
		if ((GetTickCount() - PLAYER_DATA[playerid][p_last_death]) < 500)
		{
			++ PLAYER_DATA[playerid][p_death_flood_attemps];

			if (PLAYER_DATA[playerid][p_death_flood_attemps] >= MAX_DEATH_FLOOD)
			{
				ot_ReturnCallback(playerid, FLOOD_DEATH);
				return 0;
			}
		}
		PLAYER_DATA[playerid][p_last_death] = GetTickCount();
	}
	return 1;
}

IRPC:ENTER_VEHICLE_RPC(playerid, BitStream:bs)
{
	// Rate-limited OnPlayerEnterVehicle
	if (!ANTICHEAT_PLAYER[playerid][FLOOD_ENTER_VEH][pac_immunity] && ANTICHEAT_INFO[FLOOD_ENTER_VEH][ac_enabled])
	{
		if ((GetTickCount() - PLAYER_DATA[playerid][p_last_enter_veh]) < 500)
		{
			++ PLAYER_DATA[playerid][p_enter_veh_attemps];

			if (PLAYER_DATA[playerid][p_enter_veh_attemps] >= MAX_ENTER_VEH_FLOOD)
			{
				ot_ReturnCallback(playerid, FLOOD_ENTER_VEH);
				return 0;
			}
		}
		PLAYER_DATA[playerid][p_last_enter_veh] = GetTickCount();
	}

	// Update position
	GetPlayerPos(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]);
	return 1;
}

IRPC:EXIT_VEHICLE_RPC(playerid, BitStream:bs)
{
	// Rate-limited OnPlayerExitVehicle
	if (!ANTICHEAT_PLAYER[playerid][FLOOD_EXIT_VEH][pac_immunity] && ANTICHEAT_INFO[FLOOD_EXIT_VEH][ac_enabled])
	{
		if ((GetTickCount() - PLAYER_DATA[playerid][p_last_exit_veh]) < 500)
		{
			++ PLAYER_DATA[playerid][p_exit_veh_attemps];

			if (PLAYER_DATA[playerid][p_exit_veh_attemps] >= MAX_EXIT_VEH_FLOOD)
			{
				ot_ReturnCallback(playerid, FLOOD_EXIT_VEH);
				return 0;
			}
		}
		PLAYER_DATA[playerid][p_last_exit_veh] = GetTickCount();
	}

	// Update position
	GetPlayerPos(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]);
	return 1;
}

IRPC:REQUEST_CLASS_RPC(playerid, BitStream:bs)
{
	// Rate-limited OnPlayerRequestClass
	if (!ANTICHEAT_PLAYER[playerid][FLOOD_CLASS][pac_immunity] && ANTICHEAT_INFO[FLOOD_CLASS][ac_enabled])
	{
		if ((GetTickCount() - PLAYER_DATA[playerid][p_last_request_class]) < 500)
		{
			++ PLAYER_DATA[playerid][p_class_attemps];

			if (PLAYER_DATA[playerid][p_class_attemps] >= MAX_CLASS_FLOOD)
			{
				ot_ReturnCallback(playerid, FLOOD_CLASS);
				return 0;
			}
		}
		PLAYER_DATA[playerid][p_last_request_class] = GetTickCount();
	}
	return 1;
}

IRPC:REQUEST_SPAWN_RPC(playerid, BitStream:bs)
{
	// Rate-limited OnPlayerRequestSpawn
	if (!ANTICHEAT_PLAYER[playerid][FLOOD_SPAWN][pac_immunity] && ANTICHEAT_INFO[FLOOD_SPAWN][ac_enabled])
	{
		if ((GetTickCount() - PLAYER_DATA[playerid][p_last_request_spawn]) < 500)
		{
			++ PLAYER_DATA[playerid][p_spawn_attemps];

			if (PLAYER_DATA[playerid][p_spawn_attemps] >= MAX_SPAWN_FLOOD)
			{
				ot_ReturnCallback(playerid, FLOOD_SPAWN);
				return 0;
			}
		}
		PLAYER_DATA[playerid][p_last_request_spawn] = GetTickCount();
	}
	return 1;
}

public OnIncomingConnection(playerid, ip_address[], port)
{
	// Disable all anticheat for NPCs
	if (IsPlayerNPC(playerid))
	{
		for(new i = 0; i < sizeof(ANTICHEAT_PLAYER[]); i ++)
		{
			ANTICHEAT_PLAYER[playerid][i][pac_detections] = 0;
			ANTICHEAT_PLAYER[playerid][i][pac_last_detection] = 0;
			ANTICHEAT_PLAYER[playerid][i][pac_immunity] = true;
		}
		return 0;
	}

	// Invalid IDs
	if (!( 0 <= playerid < MAX_PLAYERS ))
	{
		#if defined OCTA_DEBUG
			printf("[OCTA DETECTION] %s:%d > Invalid PlayerID", ip_address, port);
		#endif

		BlockIpAddress(ip_address, 120000);
	}

	// Limit of users per ip
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_LIMIT_SESSION][pac_immunity] && ANTICHEAT_INFO[CHEAT_LIMIT_SESSION][ac_enabled])
	{
		new 
			temp_ip[16],
			count = 0
		;

		for(new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
		{
			if (IsPlayerConnected(i))
			{
			    GetPlayerIp(i, temp_ip, 16);
			    if (!strcmp(temp_ip, ip_address)) count ++;
			}
		}

		if (count > MAX_PLAYERS_ON_IP)
		{
			ot_ReturnCallback(playerid, CHEAT_LIMIT_SESSION);
			return 0;
		}
	}
	return 1;
}

forward OnVehicleRequestDeath(vehicleid, killerid);
public OnVehicleRequestDeath(vehicleid, killerid)
{
    new Float:health;

    GetVehicleHealth(vehicleid, health);

	// Fix for car respawning hack
    if (health >= 250.0)
    {
    	#if defined OCTA_DEBUG
			new name[MAX_PLAYER_NAME];
			GetPlayerName(killerid, name, sizeof(name));
			printf("[OCTA DETECTION] %s(%i): Vehicle Spawn", name, killerid);
		#endif
        return 0;
    }

    return 1;
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	PLAYER_DATA[playerid][p_vehicleid] = vehicleid;
	
	#if defined OT_OnPlayerEnterVehicle
		return OT_OnPlayerEnterVehicle();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerEnterVehicle
	#undef OnPlayerEnterVehicle
#else
	#define _ALS_OnPlayerEnterVehicle
#endif

#define OnPlayerEnterVehicle OT_OnPlayerEnterVehicle
#if defined OT_OnPlayerEnterVehicle
	forward OT_OnPlayerEnterVehicle();
#endif

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	// Reset player vehicle
	if (oldstate == PLAYER_STATE_PASSENGER || oldstate == PLAYER_STATE_DRIVER)
		PLAYER_DATA[playerid][p_vehicleid] = -1;
	
	#if defined OT_OnPlayerStateChange
		return OT_OnPlayerStateChange(playerid, newstate, oldstate);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerStateChange
	#undef OnPlayerStateChange
#else
	#define _ALS_OnPlayerStateChange
#endif

#define OnPlayerStateChange OT_OnPlayerStateChange
#if defined OT_OnPlayerStateChange
	forward OT_OnPlayerStateChange(playerid, newstate, oldstate);
#endif

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	/* Dialog Crasher */
	if (!isnull(inputtext))
    {
        for(new i = 0; i != strlen(inputtext); i ++)
        {
            if (inputtext[i] == '%') inputtext[i] = '\0';
        }
    }

	/* Dialog spoofing */
	if (!ANTICHEAT_PLAYER[playerid][CHEAT_DIALOGS][pac_immunity] && ANTICHEAT_INFO[CHEAT_DIALOGS][ac_enabled])
	{
		if (PLAYER_DATA[playerid][p_dialog] != dialogid)
		{
			ot_ReturnCallback(playerid, CHEAT_DIALOGS);
			return 0;
		}
	}

	PLAYER_DATA[playerid][p_dialog] = -1;

	#if defined OT_OnDialogResponse
		return OT_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);
	#else
		return 1;
	#endif
}

#if defined _ALS_OnDialogResponse
	#undef OnDialogResponse
#else
	#define _ALS_OnDialogResponse
#endif
#define OnDialogResponse OT_OnDialogResponse
#if defined OT_OnDialogResponse
	forward OT_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]);
#endif

/* Fixed Functions */
stock ot_SetPlayerPos(playerid, Float:x, Float:y, Float:z)
{
	PLAYER_DATA[playerid][p_position][0] = x;
	PLAYER_DATA[playerid][p_position][1] = y;
	PLAYER_DATA[playerid][p_position][2] = z;
	SetPlayerPos(playerid, x, y, z);
	return 1;
}

stock ot_SetPlayerInterior(playerid, interiorid)
{
	PLAYER_DATA[playerid][p_interior] = interiorid;
	SetPlayerInterior(playerid, interiorid);
	return 1;
}

stock ot_SetSpawnInfo(playerid, team, skin, Float:x, Float:y, Float:z, Float:rotation, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo)
{
	PLAYER_DATA[playerid][p_position][0] = x;
	PLAYER_DATA[playerid][p_position][1] = y;
	PLAYER_DATA[playerid][p_position][2] = z;
	SetSpawnInfo(playerid, team, skin, x, y, z, rotation, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
	return 1;
}

stock ot_TogglePlayerControllable(playerid, toggle)
{
	PLAYER_DATA[playerid][p_freeze] = toggle;
	GetPlayerPos(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]);
	PLAYER_DATA[playerid][p_interior] = GetPlayerInterior(playerid);
	TogglePlayerControllable(playerid, toggle);
	return 1;
}

stock ot_SetPlayerSpecialAction(playerid, actionid)
{
	PLAYER_DATA[playerid][p_special_action] = actionid;
	SetPlayerSpecialAction(playerid, actionid);
	return 1;
}

stock ot_TogglePlayerSpectating(playerid, toggle)
{
	PLAYER_DATA[playerid][p_spectating] = toggle;
	TogglePlayerSpectating(playerid, toggle);
	return 1;
}

stock ot_GivePlayerWeapon(playerid, weaponid, ammo)
{
	new slot = GetWeaponSlot(weaponid);
	
	if (PLAYER_WEAPONS[playerid][slot][w_valid] && PLAYER_WEAPONS[playerid][slot][w_id] == weaponid)
		PLAYER_WEAPONS[playerid][slot][w_ammo] += ammo;
	else PLAYER_WEAPONS[playerid][slot][w_ammo] = ammo;
	
	PLAYER_WEAPONS[playerid][slot][w_valid] = true;
	PLAYER_WEAPONS[playerid][slot][w_id] = weaponid;
	
	PLAYER_DATA[playerid][p_bullets_fired] = 0;
	ANTICHEAT_PLAYER[playerid][CHEAT_AMMO][pac_last_detection] = gettime();

	GivePlayerWeapon(playerid, weaponid, ammo);
	return 1;
}

stock ot_SetPlayerAmmo(playerid, weaponslot, ammo)
{
	PLAYER_DATA[playerid][p_bullets_fired] = 0;
	ANTICHEAT_PLAYER[playerid][CHEAT_AMMO][pac_last_detection] = gettime();
	PLAYER_WEAPONS[playerid][weaponslot][w_ammo] = ammo;
	SetPlayerAmmo(playerid, weaponslot, ammo);
	return 1;
}

stock ot_ResetPlayerWeapons(playerid)
{
	for (new i = 0; i < sizeof(PLAYER_WEAPONS[]); i++)
	{
		PLAYER_WEAPONS[playerid][i][w_valid] = false;
		PLAYER_WEAPONS[playerid][i][w_id] = 0;
		PLAYER_WEAPONS[playerid][i][w_ammo] = 0;
	}

	ResetPlayerWeapons(playerid);
	return 1;
}

stock ot_SetPlayerArmedWeapon(playerid, weaponid)
{
	new slot = GetWeaponSlot(weaponid);
	PLAYER_WEAPONS[playerid][slot][w_valid] = true;
	PLAYER_WEAPONS[playerid][slot][w_id] = weaponid;

	SetPlayerArmedWeapon(playerid, weaponid);
	return 1;
}

stock ot_GivePlayerMoney(playerid, money)
{
	ResetPlayerMoney(playerid);
	PLAYER_DATA[playerid][p_money] += money;
	GivePlayerMoney(playerid, PLAYER_DATA[playerid][p_money]);
	return 1;
}

stock ot_GetPlayerMoney(playerid)
{
	return PLAYER_DATA[playerid][p_money];	
}

stock ot_ResetPlayerMoney(playerid)
{
	PLAYER_DATA[playerid][p_money] = 0;
	ResetPlayerMoney(playerid);
	return 1;
}

stock ot_UsePlayerPedAnims()
{
	ot_pedAnims = true;
	return UsePlayerPedAnims();
}

stock ot_ShowPlayerDialog(playerid, dialogid, style, caption[], info[], button1[], button2[])
{
	if (ShowPlayerDialog(playerid, dialogid, style, caption, info, button1, button2))
	{
		PLAYER_DATA[playerid][p_dialog] = dialogid;
		return 1;
	}
	return 0;
}

/* y_dialog hooked function */
#if defined _INC_y_dialog
	stock ot_Dialog_Show(playerid, style, string:title[], string:caption[], string:button1[], string:button2[] = "", dialog = -1)
	{
		if (0 <= playerid < MAX_PLAYERS)
		{
			PLAYER_DATA[playerid][p_dialog] = dialog;
		}

		return Dialog_Show(playerid, style, title, caption, button1, button2, dialog);
	}

	#if defined _ALS_Dialog_Show
		#undef Dialog_Show
	#else
		#define _ALS_Dialog_Show
	#endif
	#define Dialog_Show ot_Dialog_Show
#endif

// ALS Hooks
#if defined _ALS_SetPlayerPos
	#undef SetPlayerPos
#else
	#define _ALS_SetPlayerPos
#endif
#define SetPlayerPos ot_SetPlayerPos


#if defined _ALS_SetPlayerInterior
	#undef SetPlayerInterior
#else
	#define _ALS_SetPlayerInterior
#endif
#define SetPlayerInterior ot_SetPlayerInterior


#if defined _ALS_SetSpawnInfo
	#undef SetSpawnInfo
#else
	#define _ALS_SetSpawnInfo
#endif
#define SetSpawnInfo ot_SetSpawnInfo


#if defined _ALS_TogglePlayerControllable
	#undef TogglePlayerControllable
#else
	#define _ALS_TogglePlayerControllable
#endif
#define TogglePlayerControllable ot_TogglePlayerControllable


#if defined _ALS_SetPlayerSpecialAction
	#undef SetPlayerSpecialAction
#else
	#define _ALS_SetPlayerSpecialAction
#endif
#define SetPlayerSpecialAction ot_SetPlayerSpecialAction


#if defined _ALS_TogglePlayerSpectating
	#undef TogglePlayerSpectating
#else
	#define _ALS_TogglePlayerSpectating
#endif
#define TogglePlayerSpectating ot_TogglePlayerSpectating


#if defined _ALS_GivePlayerWeapon
	#undef GivePlayerWeapon
#else
	#define _ALS_GivePlayerWeapon
#endif
#define GivePlayerWeapon ot_GivePlayerWeapon


#if defined _ALS_SetPlayerAmmo
	#undef SetPlayerAmmo
#else
	#define _ALS_SetPlayerAmmo
#endif
#define SetPlayerAmmo ot_SetPlayerAmmo


#if defined _ALS_ResetPlayerWeapons
	#undef ResetPlayerWeapons
#else
	#define _ALS_ResetPlayerWeapons
#endif
#define ResetPlayerWeapons ot_ResetPlayerWeapons


#if defined _ALS_SetPlayerArmedWeapon
	#undef SetPlayerArmedWeapon
#else
	#define _ALS_SetPlayerArmedWeapon
#endif
#define SetPlayerArmedWeapon ot_SetPlayerArmedWeapon


#if defined _ALS_GivePlayerMoney
	#undef GivePlayerMoney
#else
	#define _ALS_GivePlayerMoney
#endif
#define GivePlayerMoney ot_GivePlayerMoney


#if defined _ALS_GetPlayerMoney
	#undef GetPlayerMoney
#else
	#define _ALS_GetPlayerMoney
#endif
#define GetPlayerMoney ot_GetPlayerMoney


#if defined _ALS_ResetPlayerMoney
	#undef ResetPlayerMoney
#else
	#define _ALS_ResetPlayerMoney
#endif
#define ResetPlayerMoney ot_ResetPlayerMoney


#if defined _ALS_UsePlayerPedAnims
	#undef UsePlayerPedAnims
#else
	#define _ALS_UsePlayerPedAnims
#endif
#define UsePlayerPedAnims ot_UsePlayerPedAnims


#if defined _ALS_ShowPlayerDialog
	#undef ShowPlayerDialog
#else
	#define _ALS_ShowPlayerDialog
#endif
#define ShowPlayerDialog ot_ShowPlayerDialog
