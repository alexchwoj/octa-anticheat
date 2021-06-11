![logo](https://i.imgur.com/RwQNuf4.png)

  

Octa is a powerful anti-cheat service that counteracts cheating in San Andreas Multiplayer, using **Pawn.RakNet**, which makes it **faster** and more **effective** than any classic anti-cheat.

  
  

## Implementation

  

1. Download the latest version in [releases](https://github.com/RealAtom/octa-anticheat/releases)

2. Put the `octa.pwn` file in `pawno/include`

3. Enter this line in your initial script (remember that it must go before any other and after a_samp)

```pawn

#include <octa>

```

4. Install [Pawn.RakNet](https://github.com/urShadow/Pawn.RakNet/)

  

## Usage

Octa calls a function when a cheating attempt is detected, this function contains the identifier number of the cheat and that of the player. In the `test.pwn` file you have a usage example.

  

```pawn

public OnCheatDetected(playerid, cheat_id, const cheat_name[], cheat_detections)
{
	static const fmt_str[] = "[{939393}%i{FFFFFF}] Cheat Detected: {EE5454}%s{FFFFFF} - Detections: {EFD755}%d";
	new string[sizeof(fmt_str) + (-2 + 64) + (-2 + 8)];

	strunpack(string, ANTICHEAT_INFO[cheat_id][ac_name], sizeof(string));
	format(string, sizeof(string), fmt_str, playerid, string, cheat_detections);

	SendClientMessageToAll(-1, string);
	ot_CheatKick(playerid, cheat_id);
	return 1;
}

```

  

## Fuctions

```pawn

ToggleCheatDetection(cheat_id, bool:enable);

SetCheatImmunity(playerid, cheat_id, bool:immunity);

GetCheatImmunity(playerid, cheat_id);

GetPlayerDetections(playerid, cheat_id);

GetPlayerLastDetection(playerid, cheat_id);

```

  

## Features

* Anti WeaponHack

* Anti AmmoHack

* Anti SpecialAction Hack

* Anti fly

* Anti UnFreeze

* Anti Teleport Hack

- Position

- Interior

- Virtual World

* Anti Crasher

- Invalid camera

- Bullet crasher

- Unoccupied crasher

- Enter crasher

- Bullet fake position

* Anti Flood

- Chat

- Deaths

- Enter vehicle

- Exit vehicle

- Request class

- Request spawn

* Anti invisible vehicle surfing

* Anti invisible aim

* Anti spec

* Anti MoneyHack

* Anti FakeBullets

* Anti CarJack

* Anti TrollCheat

* Anti DialogHack

* IP Sessions limiter

  

## Credits

*  [Atom](https://github.com/RealAtom) - Programming

*  [leHeix](https://github.com/joseepe) - Provide help and programming

*  [katursis](https://github.com/katursis) - [Pawn.RakNet](https://github.com/katursis/Pawn.RakNet/)

### Were you about to copy and paste all my work? Quite there ruffian, you have to know something!
This is an old and discontinued version of Octa Anti-Cheat, it is more than 1 year out of date compared to the current version used in Hyaxe, but who knows, maybe it will be useful for someone!

### Discord

[![Hyaxe](https://discordapp.com/api/guilds/586980198910656521/embed.png?style=banner2)](https://www.hyaxe.com/discord)

[![PawnoZone](https://discordapp.com/api/guilds/670111078700810278/embed.png?style=banner2)](https://discord.gg/QZ6wyyM)