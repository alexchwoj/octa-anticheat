/*
* Octa AntiCheat
* Developed by Atom, Vinter
* GitHub: github.com/RealAtom
* Powered by Hyaxe
*/

#if defined _OCTA_DMG_INCLUDED
	#endinput
#endif
#define _OCTA_DMG_INCLUDED

#include <a_samp>
#include <Pawn.RakNet>

enum _:E_DAMAGE_BODY_PART
{
	e_iBodyPartTorso = 3,
	e_iBodyPartGroin,
	e_iBodyPartLeftArm,
	e_iBodyPartRightArm,
	e_iBodyPartLeftLeg,
	e_iBodyPartRightLeg,
	e_iBodyPartHead
};

enum (<<= 1)
{
	e_bmPlayerDead = 1
};

new
	g_iPlayerHealth[MAX_PLAYERS],
	g_iPlayerArmour[MAX_PLAYERS],
	g_iLastDamageTick[(MAX_PLAYERS + 1)],
	g_rgbitsPlayerFlags[MAX_PLAYERS],
	g_iDamagerWarns[MAX_PLAYERS],
	g_iDamagerLastWarn[MAX_PLAYERS],
	g_iLastShotTick[MAX_PLAYERS],
	g_iDamageDelay = 100;

new g_rgiWeaponsDamage[][E_DAMAGE_BODY_PART] = {
    { 1, 1, 1, 1, 1, 1, 2 }, // Puño
	{ 1, 1, 1, 1, 1, 1, 2 }, // Manopla
	{ 1, 1, 1, 1, 1, 1, 2 }, // Palo de golf
	{ 7, 5, 5, 5, 5, 5, 10 }, // Porra
	{ 5, 2, 2, 2, 2, 2, 10 }, // Cuchillo
	{ 3, 3, 3, 3, 3, 3, 10 }, // Bate
	{ 4, 3, 3, 3, 3, 3, 10 }, // Pala
	{ 3, 1, 1, 1, 1, 1, 10 }, // Palo de pool
	{ 8, 8, 8, 8, 8, 8, 15 }, // Katana
	{ 10, 5, 5, 5, 5, 5, 10 }, // Motosierra
	{ 1, 1, 1, 1, 1, 1, 2 }, // Dildo morado
	{ 1, 1, 1, 1, 1, 1, 2 }, // Dildo
	{ 1, 1, 1, 1, 1, 1, 2 }, // Vibrador 
	{ 1, 1, 1, 1, 1, 1, 2 }, // Vibrador gris
	{ 1, 1, 1, 1, 1, 1, 2 }, // Flores
	{ 4, 1, 1, 1, 1, 1, 2 }, // Baston
	{ 0, 0, 0, 0, 0, 0, 0 }, // Granada
	{ 0, 0, 0, 0, 0, 0, 0 }, // Granada de gas
	{ 0, 0, 0, 0, 0, 0, 0 }, // Molotov
	{ 0, 0, 0, 0, 0, 0, 0 }, // Invalid slot
	{ 0, 0, 0, 0, 0, 0, 0 }, // Invalid slot
	{ 0, 0, 0, 0, 0, 0, 0 }, // Invalid slot
    { 8, 8, 5, 5, 5, 5, 20 }, // Colt45
	{ 8, 8, 5, 5, 5, 5, 20 }, // Silenced
	{ 20, 20, 20, 20, 20, 20, 30 }, // Deagle
	{ 15, 12, 12, 12, 12, 12, 15 }, // Escopeta
	{ 15, 10, 10, 10, 10, 10, 15 }, // Recortada
	{ 8, 8, 8, 8, 8, 8, 10 }, // Combate
	{ 6, 5, 5, 5, 5, 5, 10 }, // Uzi
	{ 5, 4, 4, 4, 4, 4, 10 }, // MP5
	{ 12, 10, 10, 10, 10, 10, 20 }, // AK-47
	{ 20, 15, 15, 15, 15, 15, 30 }, // M4
	{ 11, 10, 10, 10, 10, 10, 13 }, // Tec-9
	{ 25, 25, 25, 25, 25, 25, 35 }, // Rifle
	{ 100, 70, 70, 70, 70, 70, 100 }, // Sniper
	{ 0, 0, 0, 0, 0, 0, 0 }, // RPG
	{ 0, 0, 0, 0, 0, 0, 0 }, // Bazooka
	{ 0, 0, 0, 0, 0, 0, 0 }, // Lanzallamas
	{ 0, 0, 0, 0, 0, 0, 0 }, // Minigun
	{ 0, 0, 0, 0, 0, 0, 0 }, // C4
	{ 0, 0, 0, 0, 0, 0, 0 }, // Detonador
	{ 0, 0, 0, 0, 0, 0, 0 }, // RPG
	{ 0, 0, 0, 0, 0, 0, 0 }, // Spray
	{ 0, 0, 0, 0, 0, 0, 0 }, // Extinguidor
	{ 1, 1, 1, 1, 1, 1, 2 }, // Nigh vision
	{ 1, 1, 1, 1, 1, 1, 2 }, // Thermal 
	{ 1, 1, 1, 1, 1, 1, 2 }, // Paracaidas
	{ 1, 1, 1, 1, 1, 1, 2 }, // Cellphone
	{ 1, 1, 1, 1, 1, 1, 2 }, // Jetpack
	{ 1, 1, 1, 1, 1, 1, 2 }, // Skate
	{ 1, 1, 1, 1, 1, 1, 2 }, // Vehicle
	{ 1, 1, 1, 1, 1, 1, 2 }, // Helicopter
	{ 1, 1, 1, 1, 1, 1, 2 }, // BOOM
	{ 1, 1, 1, 1, 1, 1, 2 }, // Drowned
	{ 1, 1, 1, 1, 1, 1, 2 } // Splat
};

new Float:gDefaultWeaponDamage[] = {
    7.000000, // 0 (Puño)
    7.000000, // 1 (Nudillos de cobre)
    7.000000, // 2 (Palo de golf)
    7.000000, // 3 (Porra)
    7.000000, // 4 (Cuchillo)
    7.000000, // 5 (Bate)
    7.000000, // 6 (Pala)
    7.000000, // 7 (Palo de pool)
    7.000000, // 8 (Katana)
    28.000000, // 9 (Motosierra)
    7.000000, // 10 (Dildo)
    3.000000, // 11 (Dildo)
    7.000000, // 12 (Dildo)
    3.000000, // 13 (Dildo)
    7.000000, // 14 (Ramo de flores)
    7.000000, // 15 (Bastón)
    0.0, // 16 (Granada)
    0.0, // 17 (Gases lacrimógenos)
    0.0, // 18 (Molotov)
    0.0, // 19 (Arma inválida)
    0.0, // 20 (Arma inválida)
    0.0, // 21 (Arma inválida)
    9.000000, // 22 (Colt45)
    14.000000, // 23 (Colt45 con silenciador)
    47.000000, // 24 (Desert Eagle)
    50.000000, // 25 (Escopeta)
    50.000000, // 26 (Escopetas recortadas)
    40.000000, // 27 (Escopeta de combate)
    7.000000, // 28 (Uzi)
    9.000000, // 29 (MP5)
    10.000000, // 30 (AK-47)
    10.000000, // 31 (M4)
    7.000000, // 32 (Tec-9)
    25.000000, // 33 (Rifle)
    42.000000, // 34 (Sniper)
    0.0 // Arma inválida
};

// On player get damage
forward OnPlayerDamage(playerid, issuerid, amount, weaponid, bodypart);

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

// Damage delay
stock Damage_SetDelay(delay)
{
	if (delay < 0) return 0;

	g_iDamageDelay = delay;
	return 1;
}

// Edit weapon damage
stock Weapon_SetDamage(weapon_id, body, damage)
{
	#if defined OCTA_DEBUG
		printf("New weapon damage: weapon_id = %d, body = %d, body_id = %d, damage = %d", weapon_id, body, body - 3, damage);
	#endif
	
	if (body == -1)
	{
		for (new i = 0; i < 7; i++)
		{
			g_rgiWeaponsDamage[weapon_id][i] = damage;
		}
		return 1;
	}

	g_rgiWeaponsDamage[weapon_id][body - 3] = damage;
	return 1;
}

// Death
stock Player_IsDead(playerid)
{
	return (g_rgbitsPlayerFlags[playerid] & e_bmPlayerDead);
}

// Edit health
stock Player_SetHealth(playerid, amount)
{
	if (!IsPlayerConnected(playerid))
		return 0;

	g_iPlayerHealth[playerid] = amount;
	SetPlayerHealth(playerid, amount);
	return 1;
}

// Edit armour
stock Player_SetArmour(playerid, amount)
{
	if (!IsPlayerConnected(playerid))
		return 0;
		
	g_iPlayerArmour[playerid] = amount;
	SetPlayerArmour(playerid, amount);
	return 1;
}

// Get health
stock Player_HetHealth(playerid)
{
	return g_iPlayerHealth[playerid]
}

// Get armour
stock Player_GetArmour(playerid)
{
	return g_iPlayerArmour[playerid]
}

// Internal damage
stock Damage_Send(to, from, Float:amount, weaponid)
{
	if (g_rgbitsPlayerFlags[to] & e_bmPlayerDead || (IsPlayerConnected(from) && g_rgbitsPlayerFlags[from] & e_bmPlayerDead))
		return 0;
	
	if (!(0 <= weaponid <= 54))
		return 0;

	if (GetTickCount() < g_iLastDamageTick[to])
		return 0;

	if (!IsPlayerStreamedIn(to, from) || !IsPlayerStreamedIn(from, to))
		return 0;

	if (16 <= weaponid <= 34)
    {
		new diff = GetTickDiff(GetTickCount(), g_iLastShotTick[from]);

		if (diff > 2000)
        {
			printf("(DAMAGE: %d - %d) Daño inválido. (tick: %d)", to, from, diff);
			Kick(from);
			return 0;
		}
	}

	new armour, damage = floatround(amount);

	if ((armour = g_iPlayerArmour[to]) >= 1)
	{
		if ((g_iPlayerArmour[to] - damage) <= 0)
		{
			Player_SetArmour(to, 0);
			Damage_Send(to, from, -(armour - amount), weaponid);
		}
		else Player_SetArmour(to, g_iPlayerArmour[to] - damage);
	}
	else
	{
		if ((g_iPlayerHealth[to] - damage) <= 0)
		{
			OnPlayerDeath(to, from, weaponid);
			g_rgbitsPlayerFlags[to] |= e_bmPlayerDead;

			Player_SetHealth(to, 0);
		}
		else Player_SetHealth(to, g_iPlayerHealth[to] - damage);
	}

	// Damage delay
	g_iLastDamageTick[to] = GetTickCount() + g_iDamageDelay;
	return 1;
}

/* Player Callbacks */
public OnPlayerConnect(playerid)
{
	g_rgbitsPlayerFlags[playerid] = 0b00000000;
	g_iLastDamageTick[playerid] = g_iLastDamageTick[MAX_PLAYERS];
	
	#if defined Damage_OnPlayerConnect
		return Damage_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	g_iLastShotTick[playerid] = GetTickCount();

	#if defined Damage_OnPlayerWeaponShot
		return Damage_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
	#else
		return 1;
	#endif
}

public OnPlayerSpawn(playerid)
{
	g_rgbitsPlayerFlags[playerid] &= ~e_bmPlayerDead;
	
	Player_SetHealth(playerid, 100);
	Player_SetArmour(playerid, 0);

	#if defined Damage_OnPlayerSpawn
		return Damage_OnPlayerSpawn(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerDeath(playerid, killerid, reason)
{
	if (g_rgbitsPlayerFlags[playerid] & e_bmPlayerDead)
		return 0;
	
	g_rgbitsPlayerFlags[playerid] |= e_bmPlayerDead;

	g_iPlayerHealth[playerid] = 0;
	g_iPlayerArmour[playerid] = 0;

	#if defined Damage_OnPlayerDeath
		return Damage_OnPlayerDeath(playerid, killerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerGiveDamage(playerid, damagedid, Float:amount, weaponid, bodypart)
{
	if (!IsPlayerConnected(playerid) || !IsPlayerConnected(damagedid))
		return 0;

	if (!(3 <= bodypart <= 9))
	{
		Kick(playerid);
        return ~0;
	}

	if (weaponid < (sizeof(gDefaultWeaponDamage) - 1))
	{
		if (amount > gDefaultWeaponDamage[weaponid])
		{
			if (gettime() > g_iDamagerLastWarn[playerid])
			{
				g_iDamagerWarns[playerid] = 0;
			}

			if (++g_iDamagerWarns[playerid] == 5)
			{
				g_iDamagerWarns[playerid] = 0;
				Kick(playerid);
			}

			g_iDamagerLastWarn[playerid] = (gettime() + 2);
			return 0;
		}
	}

	new
		Float:distance,
		Float:x1, Float:y1, Float:z1,
		Float:x2, Float:y2, Float:z2
	;

	GetPlayerPos(playerid, x1, y1, z1);
	GetPlayerPos(damagedid, x2, y2, z2);

	distance = VectorSize(x1 - x2, y1 - y2, z1 - z2);
	if (distance <= 1.0) weaponid = 0;

	if (weaponid <= WEAPON_SNIPER)
		amount = g_rgiWeaponsDamage[weaponid][bodypart - 3];
	else
		if (amount <= 0.9) amount = 1.0;

	if (OnPlayerDamage(damagedid, playerid, floatround(amount), weaponid, bodypart))
		Damage_Send(damagedid, playerid, amount, weaponid);

	return 0;
}

public OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart)
{
	if (issuerid != INVALID_PLAYER_ID && weaponid < 50 && weaponid != WEAPON_FLAMETHROWER)
		return 0;

	if (amount <= 0.9) amount = 1.0;

	OnPlayerDamage(playerid, issuerid, floatround(amount), weaponid, bodypart);
	Damage_Send(playerid, issuerid, amount, weaponid);
	return 0;
}

IPacket:207(playerid, BitStream:bs)
{
	new onfoot_data[PR_OnFootSync];
	BS_IgnoreBits(bs, 8);
	BS_ReadOnFootSync(bs, onfoot_data);

	// Rewrite OnFoot data
	onfoot_data[PR_health] = g_iPlayerHealth[playerid];
	onfoot_data[PR_armour] = g_iPlayerArmour[playerid];

	BS_SetWriteOffset(bs, 8);
	BS_WriteOnFootSync(bs, onfoot_data);
	return 1;
}

#if defined Damage_OnPlayerDeath
	forward Damage_OnPlayerDeath(playerid, killerid, reason);
#endif

#if defined _ALS_OnPlayerDeath
	#undef OnPlayerDeath
#else
	#define _ALS_OnPlayerDeath
#endif

#define OnPlayerDeath Damage_OnPlayerDeath

#if defined Damage_OnPlayerSpawn
	forward Damage_OnPlayerSpawn(playerid);
#endif

#if defined _ALS_OnPlayerSpawn
	#undef OnPlayerSpawn
#else
	#define _ALS_OnPlayerSpawn
#endif

#define OnPlayerSpawn Damage_OnPlayerSpawn

#if defined Damage_OnPlayerConnect
	forward Damage_OnPlayerConnect(playerid);
#endif

#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect Damage_OnPlayerConnect

#if defined _ALS_OnPlayerWeaponShot
	#undef OnPlayerWeaponShot
#else
	#define _ALS_OnPlayerWeaponShot
#endif

#define OnPlayerWeaponShot Damage_OnPlayerWeaponShot
#if defined Damage_OnPlayerWeaponShot
	forward Damage_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
#endif