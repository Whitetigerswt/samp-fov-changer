samp-fov-changer
================

Change FOV like in many FPS games, in SA-MP. I won't explain what FOV is, just google it. I'll be assuming you know what it is, and what it does.

Tested on:

Euro v1.0 (hoodlum crack) 

and

American v1.0 (not cracked)

How to change your FOV
-----------------
The mod will create fov.cfg in your GTA directory after first load. This file contains a few options, as of recently, if you were using a version prior to 1.2 you will have to delete this file and let it regenerate.

it will show like this when generated (defaults):

```
fov 70.0
aiming_fov 70.0
zoom 0.0 ; default 3.0, leave it as 0.0 unless you're playing lagcomp off, then put it as 3.0 or you will have trouble hitting people when you're using high FOV.
```

1. The fov field is the number you want your FOV to be, it can be any number, and can include decimals. example: 75.3 or 84.4444
2. The aiming_fov field is the number you want your FOV to be while aiming. if your playing lagcomp off, use 70.0 fov under all circumstances, or you will have trouble hitting people.
3. The zoom field is how quickly the game will zoom in your camera to 70.0 fov (default fov) while aiming. This is necessary with lagcomp off servers to have a number higher than 0.0, or else you will have trouble hitting players. It is also necessary for lagcomp off players to use 70.0 fov while aiming


Server-Side FOV detection
-----------------
It is somewhat possible for server scripters to detect the FOV of their clients using GetPlayerCameraZoom.

Using the following code:
```cpp
#include <a_samp>

stock GetPlayerFOV(playerid) {
	// 0.667 is just a temp number until I can check what SA-MP is actually doing to these numbers, it adds decent accuracy across most playable FOV's.
	return GetPlayerCameraZoom(playerid) + 0.667;
}
```

GetPlayerCameraZoom returns values between 35.0 and 70.0 ALWAYS, so when we pass over 70, it wraps around back to 35. Also you should only use this function while a player isn't aiming with a zoom weapon, sniper, camera etc.


1. A FOV of 70.0 outputs 70.6
2. A FOV of 71.0 outputs 35.6
3. A FOV of 100.0 outputs 65.1 (100 - 35) 
4. A FOV of 120.0 outputs 49.5555... (120 - 35 = 85, 85 - 70 = 15, 35 + 15 = 50, which is about what this returns).
5. A FOV of 35.0 outputs 35.6

Obviously, there is some collisions with low FOV's and high FOV's, which is why I say it's only somewhat possible for server scripters to detect FOV of clients.


Before and After
-----------------

Before installing mod (70 FOV):
![ScreenShot](http://www.sixtytiger.com/tiger/screenshots/sa-mp-018.png)

After installing mod (100 FOV)
![ScreenShot](http://www.sixtytiger.com/tiger/screenshots/sa-mp-017.png)
