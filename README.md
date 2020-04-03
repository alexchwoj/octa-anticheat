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
	new str_text[128];
	format(str_text, sizeof(str_text), "[{939393}%i{FFFFFF}] Cheat Detected: {EE5454}%s{FFFFFF} - Detections: {EFD755}%d", playerid, cheat_name, cheat_detections);
	SendClientMessageToAll(-1, str_text);
	return 1;
}
```

## Fuctions
```pawn
CheatEnableDisable(cheat_id, bool:enable);
SetImmunityCheat(playerid, cheat_id, bool:immunity);
GetImmunityCheat(playerid, cheat_id);
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
* IP Sessions limiter

## Credits
* [Atom](https://github.com/RealAtom) - Programming
* [leHeix](https://github.com/leHeix) - Provide help and programming
* [urShadow](https://github.com/urShadow) - [Pawn.RakNet](https://github.com/urShadow/Pawn.RakNet/)
