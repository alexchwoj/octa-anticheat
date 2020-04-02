#include <a_samp>
#include <core>
#include <float>
#include "../../octa.inc"

main()
{
	print("Running: Octa Test Server");
}

public OnPlayerConnect(playerid)
{
	GameTextForPlayer(playerid,"~w~Octa~r~ AntiCheat", 5000, 5);
	return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
	SendDeathMessage(killerid, playerid, reason);
 	return 1;
}

/* Callback Example */
forward OnCheatDetected(playerid, cheat_id, const cheat_name[], cheat_detections);
public OnCheatDetected(playerid, cheat_id, const cheat_name[], cheat_detections)
{
	new str_text[128];
	format(str_text, sizeof(str_text), "[{939393}%i{FFFFFF}] Cheat Detected: {EE5454}%s{FFFFFF} - Detections: {EFD755}%d", playerid, cheat_name, cheat_detections);
	SendClientMessageToAll(-1, str_text);
	return 1;
}
/* End */

public OnPlayerRequestClass(playerid, classid)
{
    ot_SetSpawnInfo(playerid, 0, 167, 1589.339843, -1054.775756, 23.633340, 90.0, -1, -1, -1, -1, -1, -1);
	return 0;
}

public OnGameModeInit()
{
	SetGameModeText("Octa Anti-Cheat");
	UsePlayerPedAnims();
	return 1;
}