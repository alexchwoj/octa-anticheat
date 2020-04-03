/*
* Octa AntiCheat
* Developed by Atom
* GitHub: github.com/RealAtom
*/

#if defined _OCTA_AC_INCLUDED
	#endinput
#endif
#define _OCTA_AC_INCLUDED

#include <a_samp>
#include <Pawn.RakNet>

/* Config */
#define OCTA_VERSION "0.0.1"

#if !defined DEBUG
	#define DEBUG 
#endif

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

const 
	PLAYER_SYNC = 207,
	AIM_SYNC = 203,
	WEAPONS_UPDATE_SYNC = 204,
	BULLET_SYNC = 206,
	UNOCCUPIED_SYNC = 209,
	VEHICLE_SYNC = 200,
	SPECTATING_SYNC = 212,
	VEHICLE_DESTROYED = 136
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
	CHEAT_SPEC,
	CHEAT_WEAPON,
	CHEAT_AMMO,
	CHEAT_FAKEBULLET,
	CRASHER_UNOCCUPIED,
	CRASHER_ENTER,
	CHEAT_MONEY,
	CHEAT_LIMIT_SESSION,
	CHEAT_CARJACK,
	CRASHER_BULLET_POS
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
	{"spec", true},
	{"weapon", true},
	{"ammo", true},
	{"fake bullet", true},
	{"unoccupied crasher", true},
	{"enter crasher", true},
	{"money", true},
	{"exceed maximum players in one ip", true},
	{"car jack", true},
	{"fake bullet position", true}
};

enum ac_player
{
	pac_detections, // Total of anticheat detections
	pac_last_detection, // time of last detection
	bool:pac_immunity // true = skip, false = call OnCheatDetected 
};
new ANTICHEAT_PLAYER[MAX_PLAYERS][ sizeof(ANTICHEAT_INFO) ][ac_player];

enum data_player
{
	bool:p_kicked,
	Float:p_position[3],
	p_interior,
	p_virtual_world,
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
	p_spawn_attemps
}
new PLAYER_DATA[MAX_PLAYERS][data_player];

enum weapons_player
{
	bool:w_valid,
	w_id,
	w_ammo
};
new PLAYER_WEAPONS[MAX_PLAYERS][13][weapons_player];

stock ot_ResetPlayerVariables(playerid)
{
	new temp_PLAYER_DATA[data_player]; PLAYER_DATA[playerid] = temp_PLAYER_DATA;
	PLAYER_DATA[playerid][p_kicked] = false;

	for(new i = 0; i < sizeof(ANTICHEAT_INFO[]); i ++)
	{
		ANTICHEAT_PLAYER[playerid][i][pac_detections] = 0;
		ANTICHEAT_PLAYER[playerid][i][pac_last_detection] = 0;
		ANTICHEAT_PLAYER[playerid][i][pac_immunity] = false;
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
	printf("[-] Octa anti-cheat v"OCTA_VERSION" initialized successfully [-]");
	
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
public OnPlayerDisconnect(playerid)
{
	ot_ResetPlayerVariables(playerid);

	#if defined OT_OnPlayerDisconnect
		return OT_OnPlayerDisconnect(playerid);
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
	forward OT_OnPlayerDisconnect(playerid);
#endif

/* Utils */
stock GetWeaponSlot(weaponid)
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

stock ot_CheatKick(playerid, cheat_id)
{
	if (PLAYER_DATA[playerid][p_kicked] == false)
	{
		new str_text[128];
		format(str_text, sizeof(str_text), "[OCTA]{FFFFFF} You were kicked out of the server for: %s", ANTICHEAT_INFO[cheat_id][ac_name]);
		SendClientMessage(playerid, 0x0090FFFF, str_text);
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

stock ot_ReturnCallback(playerid, cheat_id)
{
	ANTICHEAT_PLAYER[playerid][cheat_id][pac_last_detection] = gettime();
	ANTICHEAT_PLAYER[playerid][cheat_id][pac_detections] ++;

	#if defined DEBUG
		new name[MAX_PLAYER_NAME];
		GetPlayerName(playerid, name, sizeof(name));
		printf("[OCTA DETECTION] %s(%i): %s (%d detections)", name, playerid, ANTICHEAT_INFO[cheat_id][ac_name], ANTICHEAT_PLAYER[playerid][cheat_id][pac_detections]);
	#endif

	#if defined OnCheatDetected
		return CallLocalFunction("OnCheatDetected", "iis[64]d", playerid, cheat_id, ANTICHEAT_INFO[cheat_id][ac_name], ANTICHEAT_PLAYER[playerid][cheat_id][pac_detections]);
	#endif
}

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

/* Cheat Detection */
IPacket:PLAYER_SYNC(playerid, BitStream:bs)
{
	new onFootData[PR_OnFootSync];

	BS_IgnoreBits(bs, 8);
	BS_ReadOnFootSync(bs, onFootData);

	if (PLAYER_DATA[playerid][p_kicked] == true) return 0;

	/* Special Action Hack */
	if (ANTICHEAT_PLAYER[playerid][CHEAT_SPECIAL_ACTION][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_SPECIAL_ACTION][ac_enabled] == true)
	{
		if (onFootData[PR_specialAction] == SPECIAL_ACTION_USEJETPACK)
		{
			if (onFootData[PR_specialAction] != PLAYER_DATA[playerid][p_special_action])
			{
				ot_ReturnCallback(playerid, CHEAT_SPECIAL_ACTION);
				return 0;
			}
		}
	}

	/* FlyHack */
	if (ANTICHEAT_PLAYER[playerid][CHEAT_FLY][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_FLY][ac_enabled] == true)
	{
		switch(onFootData[PR_animationId])
        {
			case 157, 159, 161:
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
	if (ANTICHEAT_PLAYER[playerid][CRASHER_BULLET][pac_immunity] == false && ANTICHEAT_INFO[CRASHER_BULLET][ac_enabled] == true && onFootData[PR_position][2] == -5.5)
	{
		ot_ReturnCallback(playerid, CRASHER_BULLET);
		return 0;
	}

	/* Speed Hack */
	if (ANTICHEAT_PLAYER[playerid][CHEAT_SPEED][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_SPEED][ac_enabled] == true && onFootData[PR_surfingVehicleId] > 0)
	{
		if (onFootData[PR_specialAction] == SPECIAL_ACTION_USEJETPACK || IsPlayerInAnyVehicle(playerid) || onFootData[PR_animationId] == 1130) return 1;

		new Float:speed = floatsqroot( floatpower(floatabs(onFootData[PR_velocity][0]), 2.0) + floatpower(floatabs(onFootData[PR_velocity][1]), 2.0) + floatpower(floatabs(onFootData[PR_velocity][2]), 2.0)) * 253.3;

		if (speed > 80.0)
		{
			ot_ReturnCallback(playerid, CHEAT_SPEED);
			return 0;
		}
	}

	/* Teleport Hack */
	if (ANTICHEAT_PLAYER[playerid][CHEAT_TELEPORT][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_TELEPORT][ac_enabled] == true)
	{
		new 
			Float:distance = GetPlayerDistanceFromPoint(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]),
			Float:limit_distance = 15.0,
			player_state = GetPlayerState(playerid)
		;

		switch(player_state)
		{
			case PLAYER_STATE_ONFOOT: limit_distance = 5.0;
			case PLAYER_STATE_DRIVER, PLAYER_STATE_PASSENGER: limit_distance = 20.0;
		}

		if (distance > limit_distance)
		{
			ot_ReturnCallback(playerid, CHEAT_TELEPORT);
			return 0;
		}

		if (GetPlayerInterior(playerid) != PLAYER_DATA[playerid][p_interior])
		{
			ot_ReturnCallback(playerid, CHEAT_TELEPORT);
			return 0;
		}

		if (GetPlayerVirtualWorld(playerid) != PLAYER_DATA[playerid][p_virtual_world])
		{
			ot_ReturnCallback(playerid, CHEAT_TELEPORT);
			return 0;
		}

		GetPlayerPos(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]);
	}

	/* UnFreeze Hack */
	if (ANTICHEAT_PLAYER[playerid][CHEAT_UNFREEZE][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_UNFREEZE][ac_enabled] == true)
	{
		if (PLAYER_DATA[playerid][p_freeze] == 1)
		{
			new Float:distance = GetPlayerDistanceFromPoint(playerid, PLAYER_DATA[playerid][p_position][0], PLAYER_DATA[playerid][p_position][1], PLAYER_DATA[playerid][p_position][2]);
		
			if (distance > 5.0)
			{
				ot_ReturnCallback(playerid, CHEAT_UNFREEZE);
				return 0;
			}
		}
	}

	/* Invisible vehicle surfing */
	if (ANTICHEAT_PLAYER[playerid][CHEAT_INVISIBLE_SURFING][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_INVISIBLE_SURFING][ac_enabled] == true)
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

	if (ANTICHEAT_PLAYER[playerid][CHEAT_SPEC][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_SPEC][ac_enabled] == true)
	{
		if (GetPlayerState(playerid) == PLAYER_STATE_SPECTATING && PLAYER_DATA[playerid][p_spectating] == 0)
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

    if (PLAYER_DATA[playerid][p_kicked] == true) return 0;

    /* Invalid Camera */
	if (ANTICHEAT_PLAYER[playerid][CRASHER_CAMERA][pac_immunity] == false && ANTICHEAT_INFO[CRASHER_CAMERA][ac_enabled] == true && aimData[PR_camMode] == 45 || aimData[PR_camMode] == 49)
	{
		ot_ReturnCallback(playerid, CRASHER_CAMERA);
		return 0;
	}

	/* Invisible Aim */
	if (ANTICHEAT_PLAYER[playerid][CHEAT_INVISIBLE_AIM][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_INVISIBLE_AIM][ac_enabled] == true)
	{
		if (aimData[PR_aimZ] != aimData[PR_aimZ])
    	{
			ot_ReturnCallback(playerid, CHEAT_INVISIBLE_AIM);
			return 0;
    	}
	}
    return 1;
}

IPacket:WEAPONS_UPDATE_SYNC(playerid, BitStream:bs)
{
    new weaponsUpdate[PR_WeaponsUpdate];
    
    BS_IgnoreBits(bs, 8);
    BS_ReadWeaponsUpdate(bs, weaponsUpdate);

    if (PLAYER_DATA[playerid][p_kicked] == true) return 0;

    for (new i = 0; i < sizeof(PLAYER_WEAPONS); i++)
	{
		if (ANTICHEAT_PLAYER[playerid][CHEAT_WEAPON][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_WEAPON][ac_enabled] == true)
		{
			if (weaponsUpdate[PR_slotWeaponId][i] > 0 && PLAYER_WEAPONS[playerid][i][w_valid] == false)
			{
				ot_ReturnCallback(playerid, CHEAT_WEAPON);
				return 0;
			}
		}

		if (ANTICHEAT_PLAYER[playerid][CHEAT_AMMO][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_AMMO][ac_enabled] == true)
		{
			if (PLAYER_WEAPONS[playerid][i][w_ammo] < weaponsUpdate[PR_slotWeaponAmmo][i])
			{
				ot_ReturnCallback(playerid, CHEAT_AMMO);
				return 0;
			}
			PLAYER_WEAPONS[playerid][i][w_ammo] = weaponsUpdate[PR_slotWeaponAmmo][i];
		}
	}
    return 1;
}


IPacket:BULLET_SYNC(playerid, BitStream:bs)
{
    new bulletData[PR_BulletSync];

    BS_IgnoreBits(bs, 8);
    BS_ReadBulletSync(bs, bulletData);

    if (PLAYER_DATA[playerid][p_kicked] == true) return 0;

    if (ANTICHEAT_PLAYER[playerid][CHEAT_FAKEBULLET][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_FAKEBULLET][ac_enabled] == true)
	{
		new slot = GetWeaponSlot(bulletData[PR_weaponId]);
		if (PLAYER_WEAPONS[playerid][slot][w_valid] == false)
		{
			ot_ReturnCallback(playerid, CHEAT_FAKEBULLET);
			return 0;
		}
	}

	if (ANTICHEAT_PLAYER[playerid][CRASHER_BULLET_POS][pac_immunity] == false && ANTICHEAT_INFO[CRASHER_BULLET_POS][ac_enabled] == true)
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
    new unoccupiedData[PR_UnoccupiedSync];
 
    BS_IgnoreBits(bs, 8);
    BS_ReadUnoccupiedSync(bs, unoccupiedData);
 
    if (PLAYER_DATA[playerid][p_kicked] == true) return 0;

    //thx https://vk.com/franzesco and https://vk.com/dataflex
    if (ANTICHEAT_PLAYER[playerid][CRASHER_UNOCCUPIED][pac_immunity] == false && ANTICHEAT_INFO[CRASHER_UNOCCUPIED][ac_enabled] == true)
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

	//thx f0Re3t and Nexius
	if (ANTICHEAT_PLAYER[playerid][CRASHER_ENTER][pac_immunity] == false && ANTICHEAT_INFO[CRASHER_ENTER][ac_enabled] == true)
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
    new inCarData[PR_InCarSync];

    BS_IgnoreBits(bs, 8);
    BS_ReadInCarSync(bs, inCarData);

    if (PLAYER_DATA[playerid][p_kicked] == true) return 0;

    PLAYER_DATA[playerid][p_position][0] = inCarData[PR_position][0];
    PLAYER_DATA[playerid][p_position][1] = inCarData[PR_position][1];
    PLAYER_DATA[playerid][p_position][2] = inCarData[PR_position][2];

    if (ANTICHEAT_PLAYER[playerid][CHEAT_CARJACK][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_CARJACK][ac_enabled] == true)
	{
	    if(IsPlayerInAnyVehicle(playerid) && PLAYER_DATA[playerid][p_vehicleid] != inCarData[PR_vehicleId])
	    {
			ot_ReturnCallback(playerid, CHEAT_CARJACK);
			return 0;
		}
	}
    return 1;
}

IPacket:SPECTATING_SYNC(playerid, BitStream:bs)
{
    new spectatingData[PR_SpectatingSync];
    
    BS_IgnoreBits(bs, 8);
    BS_ReadSpectatingSync(bs, spectatingData);

    if (PLAYER_DATA[playerid][p_kicked] == true) return 0;

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

public OnIncomingRPC(playerid, rpcid, BitStream:bs)
{
	if (PLAYER_DATA[playerid][p_kicked] == true) return 0;

	/* Chat Flood */
	if (rpcid == 101)
	{
		if (ANTICHEAT_PLAYER[playerid][FLOOD_CHAT][pac_immunity] == false && ANTICHEAT_INFO[FLOOD_CHAT][ac_enabled] == true)
		{
			if ((GetTickCount() - PLAYER_DATA[playerid][p_last_chat_message]) < 500)
			{
				PLAYER_DATA[playerid][p_chat_flood_attemps] ++;

				if (PLAYER_DATA[playerid][p_chat_flood_attemps] >= MAX_CHAT_FLOOD)
				{
					ot_ReturnCallback(playerid, FLOOD_CHAT);
					return 0;
				}
			}
			PLAYER_DATA[playerid][p_last_chat_message] = GetTickCount();
		}
	}

	/* Death Flood */
	if (rpcid == 53)
	{
		if (ANTICHEAT_PLAYER[playerid][FLOOD_DEATH][pac_immunity] == false && ANTICHEAT_INFO[FLOOD_DEATH][ac_enabled] == true)
		{
			if ((GetTickCount() - PLAYER_DATA[playerid][p_last_death]) < 500)
			{
				PLAYER_DATA[playerid][p_death_flood_attemps] ++;

				if (PLAYER_DATA[playerid][p_death_flood_attemps] >= MAX_DEATH_FLOOD)
				{
					ot_ReturnCallback(playerid, FLOOD_DEATH);
					return 0;
				}
			}
			PLAYER_DATA[playerid][p_last_death] = GetTickCount();
		}
	}

	/* Enter Vehicle Flood */
	if (rpcid == 26)
	{
		if (ANTICHEAT_PLAYER[playerid][FLOOD_ENTER_VEH][pac_immunity] == false && ANTICHEAT_INFO[FLOOD_ENTER_VEH][ac_enabled] == true)
		{
			if ((GetTickCount() - PLAYER_DATA[playerid][p_last_enter_veh]) < 500)
			{
				PLAYER_DATA[playerid][p_enter_veh_attemps] ++;

				if (PLAYER_DATA[playerid][p_enter_veh_attemps] >= MAX_ENTER_VEH_FLOOD)
				{
					ot_ReturnCallback(playerid, FLOOD_ENTER_VEH);
					return 0;
				}
			}
			PLAYER_DATA[playerid][p_last_enter_veh] = GetTickCount();
		}
	}

	/* Exit Vehicle Flood */
	if (rpcid == 154)
	{
		if (ANTICHEAT_PLAYER[playerid][FLOOD_EXIT_VEH][pac_immunity] == false && ANTICHEAT_INFO[FLOOD_EXIT_VEH][ac_enabled] == true)
		{
			if ((GetTickCount() - PLAYER_DATA[playerid][p_last_exit_veh]) < 500)
			{
				PLAYER_DATA[playerid][p_exit_veh_attemps] ++;

				if (PLAYER_DATA[playerid][p_exit_veh_attemps] >= MAX_EXIT_VEH_FLOOD)
				{
					ot_ReturnCallback(playerid, FLOOD_EXIT_VEH);
					return 0;
				}
			}
			PLAYER_DATA[playerid][p_last_exit_veh] = GetTickCount();
		}
	}

	/* Request Class Flood */
	if (rpcid == 128)
	{
		if (ANTICHEAT_PLAYER[playerid][FLOOD_CLASS][pac_immunity] == false && ANTICHEAT_INFO[FLOOD_CLASS][ac_enabled] == true)
		{
			if ((GetTickCount() - PLAYER_DATA[playerid][p_last_request_class]) < 500)
			{
				PLAYER_DATA[playerid][p_class_attemps] ++;

				if (PLAYER_DATA[playerid][p_class_attemps] >= MAX_CLASS_FLOOD)
				{
					ot_ReturnCallback(playerid, FLOOD_CLASS);
					return 0;
				}
			}
			PLAYER_DATA[playerid][p_last_request_class] = GetTickCount();
		}
	}

	/* Request Spawn Flood */
	if (rpcid == 129)
	{
		if (ANTICHEAT_PLAYER[playerid][FLOOD_SPAWN][pac_immunity] == false && ANTICHEAT_INFO[FLOOD_SPAWN][ac_enabled] == true)
		{
			if ((GetTickCount() - PLAYER_DATA[playerid][p_last_request_spawn]) < 500)
			{
				PLAYER_DATA[playerid][p_spawn_attemps] ++;

				if (PLAYER_DATA[playerid][p_spawn_attemps] >= MAX_SPAWN_FLOOD)
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

public OnIncomingConnection(playerid, ip_address[], port)
{
	if (!( 0 <= playerid < MAX_PLAYERS ))
	{
		#if defined DEBUG
			printf("[OCTA DETECTION] %s:%d > Invalid PlayerID", ip_address, port);
		#endif

		BlockIpAddress(ip_address, 120000);
	}

	if (ANTICHEAT_PLAYER[playerid][CHEAT_LIMIT_SESSION][pac_immunity] == false && ANTICHEAT_INFO[CHEAT_LIMIT_SESSION][ac_enabled] == true)
	{
		new 
			temp_ip[16],
			count = 0
		;

		for(new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
		{
			if(IsPlayerConnected(i))
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

    if (health >= 250.0)
    {
    	#if defined DEBUG
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
	switch(oldstate)
	{
		case PLAYER_STATE_PASSENGER: PLAYER_DATA[playerid][p_vehicleid] = -1;
		case PLAYER_STATE_DRIVER: PLAYER_DATA[playerid][p_vehicleid] = -1;
	}
	
	#if defined OT_OnPlayerStateChange
		return OT_OnPlayerStateChange();
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
	forward OT_OnPlayerStateChange();
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

stock ot_GivePlayerWeapon(playerid, weaponid, ammo)
{
	new slot = GetWeaponSlot(weaponid);
	PLAYER_WEAPONS[playerid][slot][w_valid] = true;
	PLAYER_WEAPONS[playerid][slot][w_id] = weaponid;
	PLAYER_WEAPONS[playerid][slot][w_ammo] = ammo;

	GivePlayerWeapon(playerid, weaponid, ammo);
	return 1;
}

stock ot_SetPlayerAmmo(playerid, weaponslot, ammo)
{
	PLAYER_WEAPONS[playerid][weaponslot][w_ammo] = ammo;
	SetPlayerAmmo(playerid, weaponslot, ammo);
	return 1;
}

stock ot_ResetPlayerWeapons(playerid)
{
	for (new i = 0; i < sizeof(PLAYER_WEAPONS); i++)
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


#if defined _ALS_SetPlayerVirtualWorld
	#undef SetPlayerVirtualWorld
#else
	#define _ALS_SetPlayerVirtualWorld
#endif
#define SetPlayerVirtualWorld ot_SetPlayerVirtualWorld


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
