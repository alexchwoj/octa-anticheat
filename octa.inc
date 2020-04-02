/*
####################################
	      Octa Anti-Cheat
	      Created by Atom
	GitHub: github.com/RealAtom
####################################
*/

#if defined _OCTA_AC_INCLUDED
	#endinput
#endif
#define _OCTA_AC_INCLUDED

#include <a_samp>
#include <Pawn.RakNet>

/* Config */
#define OCTA_VERSION "0.0.1"

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

/* Core */
const 
	PLAYER_SYNC = 207,
	AIM_SYNC = 203
;

enum
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
	CHEAT_SPEC
};

enum ac_info
{
	ac_name[64],
	bool:ac_enabled
};

new ANTICHEAT_INFO[][ac_info] =
{
	{"special action", true},
	{"fly", true},
	{"invalid camera crasher", false},
	{"bullet crasher", true},
	{"speed", true},
	{"chat flood", true},
	{"death flood", true},
	{"enter vehicle flood", true},
	{"exit vehicle flood", true},
	{"request class flood", true},
	{"request spawn flood", true},
	{"teleport", true},
	{"unfreeze", true},
	{"invisible surfing", true},
	{"invisible aim", true},
	{"spec", true}
};

enum ac_player
{
	pac_detections, // Total of anticheat detections
	pac_last_detection, // time of last detection
	bool:pac_immunity // true = skip, false = call OnCheatDetected 
};
new ANTICHEAT_PLAYER[MAX_PLAYERS][sizeof(ANTICHEAT_INFO)][ac_player];

enum data_player
{
	bool:p_kicked,
	Float:p_position[3],
	p_interior,
	p_virtual_world,
	p_freeze,
	p_special_action,
	p_spectating,
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
	p_spawn_attemps
}
new PLAYER_DATA[MAX_PLAYERS][data_player];

/* Kick */
stock ot_CheatKick(playerid, cheat_id)
{
	if(PLAYER_DATA[playerid][p_kicked] == false)
	{
		new str_text[128];
		format(str_text, sizeof(str_text), "[OCTA]{FFFFFF} You were kicked out of the server for: %s", ANTICHEAT_INFO[cheat_id][ac_name]);
		SendClientMessage(playerid, 0x0090FFFF, str_text);
		SetTimerEx("ot_Kick", 500, false, "i", playerid);
		PLAYER_DATA[playerid][p_kicked] = true;
	}
	return 1;
}

forward ot_Kick(playerid);
public ot_Kick(playerid)
{
	return Kick(playerid);
}

stock ot_ReturnCallback(playerid, cheat_id)
{
	ANTICHEAT_PLAYER[playerid][CHEAT_SPEC][pac_last_detection] = gettime();
	ANTICHEAT_PLAYER[playerid][CHEAT_SPEC][pac_detections] ++;

	#if defined OnCheatDetected
		return OnCheatDetected(playerid, cheat_id, ANTICHEAT_INFO[cheat_id][ac_name], ANTICHEAT_PLAYER[playerid][cheat_id][pac_detections]);
	#else
		return ot_CheatKick(playerid, cheat_id);
	#endif
}

/* Cheat Detection */
stock SetImmunityCheat(playerid, cheat_id, bool:immunity)
{
	ANTICHEAT_PLAYER[playerid][cheat_id][pac_immunity] = immunity;
	return 1;
}

stock bool:GetImmunityCheat(playerid, cheat_id)
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

IPacket:PLAYER_SYNC(playerid, BitStream:bs)
{
	new onFootData[PR_OnFootSync];

	BS_IgnoreBits(bs, 8);
	BS_ReadOnFootSync(bs, onFootData);

	/* Special Action Hack */
	if(ANTICHEAT_PLAYER[playerid][CHEAT_SPECIAL_ACTION][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_SPECIAL_ACTION][ac_enabled] == true)
	{
		if(onFootData[PR_specialAction] == SPECIAL_ACTION_USEJETPACK)
		{
			if(onFootData[PR_specialAction] != PLAYER_DATA[playerid][p_special_action])
			{
				ot_ReturnCallback(playerid, CHEAT_SPECIAL_ACTION);
				return 0;
			}
		}
	}

	/* FlyHack */
	if(ANTICHEAT_PLAYER[playerid][CHEAT_FLY][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_FLY][ac_enabled] == true)
	{
		switch(onFootData[PR_animationId])
        {
			case 157, 159, 161:
	        {
	            if(!IsPlayerInAnyVehicle(playerid))
	            {
					ot_ReturnCallback(playerid, CHEAT_FLY);
					return 0;
	            }
	        }
	        case 958, 959:
	        {
	        	if(onFootData[PR_weaponId] != WEAPON_PARACHUTE)
				{
					ot_ReturnCallback(playerid, CHEAT_FLY);
					return 0;
				}
	        }
	        case 1538, 1539:
	        {
	        	if(onFootData[PR_position][2] > 1.0)
				{
					ot_ReturnCallback(playerid, CHEAT_FLY);
					return 0;
				}
	        }
	    }
	}

	/* Bullet Crasher */
	if(ANTICHEAT_PLAYER[playerid][CRASHER_BULLET][pac_immunity] == false && ANTICHEAT_INFO[CRASHER_BULLET][ac_enabled] == true && onFootData[PR_position][2] == -5.5)
	{
		ot_ReturnCallback(playerid, CRASHER_BULLET);
		return 0;
	}

	/* Speed Hack */
	if(ANTICHEAT_PLAYER[playerid][CHEAT_SPEED][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_SPEED][ac_enabled] == true && onFootData[PR_surfingVehicleId] > 0)
	{
		if(onFootData[PR_specialAction] == SPECIAL_ACTION_USEJETPACK || IsPlayerInAnyVehicle(playerid)) return 1;

		new Float:speed = floatsqroot( floatpower(floatabs(onFootData[PR_position][0]), 2.0) + floatpower(floatabs(onFootData[PR_position][1]), 2.0) + floatpower(floatabs(onFootData[PR_position][2]), 2.0)) * 253.3;

		if(speed > 41.0)
		{
			ot_ReturnCallback(playerid, CHEAT_SPEED);
			return 0;
		}
	}

	/* Teleport Hack */
	if(ANTICHEAT_PLAYER[playerid][CHEAT_TELEPORT][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_TELEPORT][ac_enabled] == true)
	{
		new Float:distance = GetPlayerDistanceFromPoint(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]);

		if(distance > 20.0)
		{
			ot_ReturnCallback(playerid, CHEAT_TELEPORT);
			return 0;
		}

		if(GetPlayerInterior(playerid) != PLAYER_DATA[playerid][p_interior])
		{
			ot_ReturnCallback(playerid, CHEAT_TELEPORT);
			return 0;
		}

		if(GetPlayerVirtualWorld(playerid) != PLAYER_DATA[playerid][p_virtual_world])
		{
			ot_ReturnCallback(playerid, CHEAT_TELEPORT);
			return 0;
		}

		GetPlayerPos(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]);
	}

	/* UnFreeze Hack */
	if(ANTICHEAT_PLAYER[playerid][CHEAT_UNFREEZE][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_UNFREEZE][ac_enabled] == true)
	{
		if(PLAYER_DATA[playerid][p_freeze] == 1)
		{
			new Float:distance = GetPlayerDistanceFromPoint(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]);
		
			if(distance > 5.0)
			{
				ot_ReturnCallback(playerid, CHEAT_UNFREEZE);
				return 0;
			}
		}
	}

	/* Invisible vehicle surfing */
	if(ANTICHEAT_PLAYER[playerid][CHEAT_INVISIBLE_SURFING][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_INVISIBLE_SURFING][ac_enabled] == true)
	{
		if(onFootData[PR_surfingVehicleId] != 0 && onFootData[PR_surfingVehicleId] != INVALID_VEHICLE_ID)
		{
            if((floatabs(onFootData[PR_surfingOffsets][0]) >= 50.0) || (floatabs(onFootData[PR_surfingOffsets][1]) >= 50.0) || (floatabs(onFootData[PR_surfingOffsets][2]) >= 50.0))
            {
				ot_ReturnCallback(playerid, CHEAT_INVISIBLE_SURFING);
				return 0;
            } 
        }
	}

	if(ANTICHEAT_PLAYER[playerid][CHEAT_SPEC][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_SPEC][ac_enabled] == true)
	{
		if(GetPlayerState(playerid) == PLAYER_STATE_SPECTATING && PLAYER_DATA[playerid][p_spectating] == 0)
		{
			ot_ReturnCallback(playerid, CHEAT_SPEC);
			return 0;
		}
	}
	return 1;
}

IPacket:AIM_SYNC(playerid, BitStream:bs)
{
    new aimData[PR_AimSync];
    
    BS_IgnoreBits(bs, 8);
    BS_ReadAimSync(bs, aimData);

    /* Invalid Camera */
	if(ANTICHEAT_PLAYER[playerid][CRASHER_CAMERA][pac_immunity] == false && ANTICHEAT_INFO[CRASHER_CAMERA][ac_enabled] == true && aimData[PR_camMode] == 45 || aimData[PR_camMode] == 49)
	{
		ot_ReturnCallback(playerid, CRASHER_CAMERA);
		return 0;
	}

	/* Invisible Aim */
	if(ANTICHEAT_PLAYER[playerid][CHEAT_INVISIBLE_AIM][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_INVISIBLE_AIM][ac_enabled] == true)
	{
		if(aimData[PR_aimZ] != aimData[PR_aimZ])
    	{
			ot_ReturnCallback(playerid, CHEAT_INVISIBLE_AIM);
			return 0;
    	}
	}
    return 1;
}

public OnIncomingRPC(playerid, rpcid, BitStream:bs)
{
	/* Chat Flood */
	if(rpcid == 101)
	{
		if(ANTICHEAT_PLAYER[playerid][FLOOD_CHAT][pac_immunity] == false && ANTICHEAT_INFO[FLOOD_CHAT][ac_enabled] == true)
		{
			if((GetTickCount() - PLAYER_DATA[playerid][p_last_chat_message]) < 500)
			{
				PLAYER_DATA[playerid][p_chat_flood_attemps] ++;

				if(PLAYER_DATA[playerid][p_chat_flood_attemps] >= MAX_CHAT_FLOOD)
				{
					ot_ReturnCallback(playerid, FLOOD_CHAT);
					return 0;
				}
			}
			PLAYER_DATA[playerid][p_last_chat_message] = GetTickCount();
		}
	}

	/* Death Flood */
	if(rpcid == 53)
	{
		if(ANTICHEAT_PLAYER[playerid][FLOOD_DEATH][pac_immunity] == false && ANTICHEAT_INFO[FLOOD_DEATH][ac_enabled] == true)
		{
			if((GetTickCount() - PLAYER_DATA[playerid][p_last_death]) < 500)
			{
				PLAYER_DATA[playerid][p_death_flood_attemps] ++;

				if(PLAYER_DATA[playerid][p_death_flood_attemps] >= MAX_DEATH_FLOOD)
				{
					ot_ReturnCallback(playerid, FLOOD_DEATH);
					return 0;
				}
			}
			PLAYER_DATA[playerid][p_last_death] = GetTickCount();
		}
	}

	/* Enter Vehicle Flood */
	if(rpcid == 26)
	{
		if(ANTICHEAT_PLAYER[playerid][FLOOD_ENTER_VEH][pac_immunity] == false && ANTICHEAT_INFO[FLOOD_ENTER_VEH][ac_enabled] == true)
		{
			if((GetTickCount() - PLAYER_DATA[playerid][p_last_enter_veh]) < 500)
			{
				PLAYER_DATA[playerid][p_enter_veh_attemps] ++;

				if(PLAYER_DATA[playerid][p_enter_veh_attemps] >= MAX_ENTER_VEH_FLOOD)
				{
					ot_ReturnCallback(playerid, FLOOD_ENTER_VEH);
					return 0;
				}
			}
			PLAYER_DATA[playerid][p_last_enter_veh] = GetTickCount();
		}
	}

	/* Exit Vehicle Flood */
	if(rpcid == 154)
	{
		if(ANTICHEAT_PLAYER[playerid][FLOOD_EXIT_VEH][pac_immunity] == false && ANTICHEAT_INFO[FLOOD_EXIT_VEH][ac_enabled] == true)
		{
			if((GetTickCount() - PLAYER_DATA[playerid][p_last_exit_veh]) < 500)
			{
				PLAYER_DATA[playerid][p_exit_veh_attemps] ++;

				if(PLAYER_DATA[playerid][p_exit_veh_attemps] >= MAX_EXIT_VEH_FLOOD)
				{
					ot_ReturnCallback(playerid, FLOOD_EXIT_VEH);
					return 0;
				}
			}
			PLAYER_DATA[playerid][p_last_exit_veh] = GetTickCount();
		}
	}

	/* Request Class Flood */
	if(rpcid == 128)
	{
		if(ANTICHEAT_PLAYER[playerid][FLOOD_CLASS][pac_immunity] == false && ANTICHEAT_INFO[FLOOD_CLASS][ac_enabled] == true)
		{
			if((GetTickCount() - PLAYER_DATA[playerid][p_last_request_class]) < 500)
			{
				PLAYER_DATA[playerid][p_class_attemps] ++;

				if(PLAYER_DATA[playerid][p_class_attemps] >= MAX_CLASS_FLOOD)
				{
					ot_ReturnCallback(playerid, FLOOD_CLASS);
					return 0;
				}
			}
			PLAYER_DATA[playerid][p_last_request_class] = GetTickCount();
		}
	}

	/* Request Spawn Flood */
	if(rpcid == 129)
	{
		if(ANTICHEAT_PLAYER[playerid][FLOOD_SPAWN][pac_immunity] == false && ANTICHEAT_INFO[FLOOD_SPAWN][ac_enabled] == true)
		{
			if((GetTickCount() - PLAYER_DATA[playerid][p_last_request_spawn]) < 500)
			{
				PLAYER_DATA[playerid][p_spawn_attemps] ++;

				if(PLAYER_DATA[playerid][p_spawn_attemps] >= MAX_SPAWN_FLOOD)
				{
					ot_ReturnCallback(playerid, FLOOD_SPAWN);
					return 0;
				}
			}
			PLAYER_DATA[playerid][p_last_request_spawn] = GetTickCount();
		}
	}
	return 1;
}

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
	SetPlayerInterior(playerid, interiorid)
	return 1;
}

stock ot_SetPlayerVirtualWorld(playerid, worldid)
{
	PLAYER_DATA[playerid][p_virtual_world] = worldid
	SetPlayerVirtualWorld(playerid, worldid);
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
	PLAYER_DATA[playerid][p_virtual_world] = GetPlayerVirtualWorld(playerid);
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
	PLAYER_DATA[playerid][p_spectating] = toggle
	TogglePlayerSpectating(playerid, toggle);
	return 1;
}