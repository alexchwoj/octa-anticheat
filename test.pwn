#include <a_samp>
#include <core>
#include <float>
#include <octa>

main()
{
	print("\nRunning: Octa Test Server");
}

public OnPlayerConnect(playerid)
{
	GameTextForPlayer(playerid,"~w~Octa~r~ AntiCheat", 5000, 5);
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
	//new str_text[128];
	//format(str_text, sizeof(str_text), "[{939393}%i{FFFFFF}] Cheat Detected: {EE5454}%s{FFFFFF} - Detections: {EFD755}%d", playerid, cheat_name, cheat_detections);
	//SendClientMessageToAll(-1, str_text);
	ot_CheatKick(playerid, cheat_id);
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	if (strcmp("/mp5", cmdtext, true, 10) == 0)
	{
		GivePlayerWeapon(playerid, 29, 100);
		return 1;
	}

	if (strcmp("/testveh", cmdtext, true, 10) == 0)
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
	return 1;
}
