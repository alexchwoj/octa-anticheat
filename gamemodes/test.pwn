#pragma option -(+
#pragma option -;+
//#pragma option -O1
#pragma option -d3

#include <a_samp>
#include <crashdetect>
#include "../../octa-damage.inc"
#include "../../octa-anticheat.inc"

main()
{
	print("\nRunning: Octa Test Server");
}

public OnPlayerConnect(playerid)
{
	GameTextForPlayer(playerid,"~b~Octa~w~ AntiCheat", 5000, 5);
	ResetPlayerWeapons(playerid);
	return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
	SendDeathMessage(killerid, playerid, reason);
 	return 1;
}

public OnCheatDetected(playerid, cheat_id, const cheat_name[], cheat_detections)
{
	static const fmt_str[] = "{939393}%s (ID: %i){FFFFFF}: Cheat Detected: {EE5454}%s{FFFFFF} - Detections: {EFD755}%d";
	new
		string[144],
		name[MAX_PLAYER_NAME]
	;

	GetPlayerName(playerid, name, MAX_PLAYER_NAME);

	strunpack(string, ANTICHEAT_INFO[cheat_id][ac_name], sizeof(string));
	format(string, sizeof(string), fmt_str, name, playerid, string, cheat_detections);

	SendClientMessageToAll(-1, string);
	ot_CheatKick(playerid, cheat_id);
	return 1;
}

public OnPlayerDamage(playerid, issuerid, amount, weaponid, bodypart)
{
	printf("OnPlayerDamage(playerid: %d, issuerid: %d, amount: %d, weaponid: %d, bodypart: %d)", playerid, issuerid, amount, weaponid, bodypart);
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	if (strcmp("/mp5", cmdtext, true, 10) == 0)
	{
		GivePlayerWeapon(playerid, 29, 100);
		return 1;
	}

	if (strcmp("/m4", cmdtext, true, 10) == 0)
	{
		GivePlayerWeapon(playerid, 31, 100);
		return 1;
	}

	if (strcmp("/9mm", cmdtext, true, 10) == 0)
	{
		GivePlayerWeapon(playerid, 22, 100);
		return 1;
	}

	if (strcmp("/infernus", cmdtext, true, 10) == 0)
	{
		CreateVehicle(411, 1589.339843, -1054.775756, 23.633340, 90.0, -1, -1, 0, 0);
		return 1;
	}
	return 0;
}

public OnPlayerRequestClass(playerid, classid)
{
    SetSpawnInfo(playerid, 0, 167, 1589.339843, -1054.775756, 23.633340, 90.0, -1, -1, -1, -1, -1, -1);
	return 0;
}

public OnGameModeInit()
{
	SetGameModeText("Octa Anti-Cheat");
	UsePlayerPedAnims();

	Weapon_SetDamage(22, -1, 1); // 9mm Damage (all body): 1
	Weapon_SetDamage(29, -1, 50); // MP5 Damage (all body): 50
	Weapon_SetDamage(31, 9, 100); // M4 Damage (head): 100
	return 1;
}
