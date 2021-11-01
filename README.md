![logo](https://i.imgur.com/RwQNuf4.png)
Octa is a powerful anti-cheat service that counteracts cheating in San Andreas Multiplayer, using **Pawn.RakNet**, which makes it **faster** and more **effective** than any classic anti-cheat.

## Implementation
1. Download the latest version in [releases](https://github.com/Hyaxe/octa-anticheat/releases)
2. Put the Octa's Files file in `pawno/include`
3. Enter this line in your initial script
```pawn
#include <octa-anticheat>
#include <octa-damage>
```
4. Install [Pawn.RakNet](https://github.com/katursis/Pawn.RakNet)

## Usage
Octa calls a function when a cheating attempt is detected, this function contains the identifier number of the cheat and ID of the player. In the `test.pwn` file you have a usage example.

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

## Credits
* [Atom](https://github.com/RealAtom) - Development leader
* [leHeix](https://github.com/joseepe) - Provide help and programming
* [katursis](https://github.com/katursis) - [Pawn.RakNet](https://github.com/katursis/Pawn.RakNet/)

### Discord
[![Hyaxe](https://discordapp.com/api/guilds/586980198910656521/embed.png?style=banner2)](https://www.hyaxe.com/discord)