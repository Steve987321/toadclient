toadclient
==========

A Minecraft cheat/client for Lunar client 1.7.10 & 1.8.9. 

This project is primarily for my personal learning and experimentation purposes. It may contain unfinished or experimental code. Feel free to explore and use this project as you will for your own projects.

## How to setup in Visual Studio 
*It is recommended to use Visual Studio 2022 to build this project.*

Logging is enabled by default, go to the top of `ToadClient/Toad.h` and comment out: 
```c++
#define ENABLE_LOGGING
```
When logging is enabled it will enable the console for log messages and create a log file named `Toad.log` in the documents folder.

1.  Clone the repository or download the zip and extract it.

2.  Open the project folder and load the project into Visual Studio by opening the `ToadClient.sln` file.

    Set the solution configuration to **Release** and the solution platform to **x64**.

    Now right click on the solution and select **Build Solution** or press **CTRL+SHIFT+B**. 

3.  After compiling finishes, you can run the loader from Visual Studio by setting your startup project to **Loader** and starting a debugging session.

## How to use
1.  When in the loader, select the minecraft instance you want to inject to (Lunar client 1.7.10/1.8.9). 
2.  After you're done close the loader and it will uninject. (this client is not *screenshare* proof)

## Features
*Some features are unfinished/unstable as this project is still WIP. Most features will still work but are probably not fully polished.*

### Combat 

**Left Clicker**
-   weapons only
-   break blocks
-   block hit
-   smart cps (just increases cps when fighting)
-   trade assist (doesn't work/not tested)
-   click check (Only in game, Only in inventory, Always click)
-   extra options (the 3 dots on the top right, you can resize the two windows when you drag your mouse between them)
    -   edit boosts & drops (opens a window where you can edit clicking behavior)
    -   visualize randomization (opens a window to view clicking behavior not recommended, still very limited)
    -   break blocks 
        -   start delay (when looking at a block while clicking it will invoke a small delay before it pauses clicking)
        -   stop delay (when looking away from a block while clicking it will invoke a small delay before resuming clicking)
    -   block hit (right clicks mouse after hits)
        -   pause left click (stops the clicking while a block hit is being send)
        -   block hit delay (how long rmb will be held down in ms)

**Right Clicker**
-   blocks only
-   start delay (invokes a delay when starting to right click)
-   click check (same as left clicker)

**Aim Assist**
-   speed
-   fov check
-   distance
-   horizontal only
-   invisibles
-   always aim
-   target lock
-   aim in target
-   break blocks
-   target by (closest to player, lowest health, closest to crosshair)

**Velocity**
-   use jump reset
    -   press chance
    -   only when moving
    -   only when clicking
    -   kite
-   only when moving
-   only when clicking
-   kite
-   horizontal
-   vertical
-   chance
-   delay (waits before reducing the velocity)

### Misc

**Bridge Assist**
-   pitch check (only enables when player is looking down below a certain pitch)
-   block height check (only enables when player is on the edge of a certain height in blocks)
-   only initiate when sneaking (only enables when player was holding down the sneak button)

**ESP**
-   outline color
-   fill color
-   esp type (box 3d, static box 2d, dynamic box 2d)
-   open esp settings (preview of ur esp settings & extra settings)
    -   player properties
        -   show name
        -   show distance
        -   show health
        -   show sneaking
        -   health [0-20]hp (preview value slider)
        -   show background (background box for text)
        -   background color
    -   box properties
        -   Border
    -   text/font
        -   text color
        -   text shadow
        -   text size
        -   font 

**Block ESP**  
-   search (search bar for blocks, is very limited to types of blocks. This will write the correct value to block ID)
-   block id (value that is being read when press the **Add** button)
-   List of blocks
    -   **block name** | **block id** **color selector** **remove**

**Blink**
-   keycode (toggle button)
-   max limit in seconds (disables blink after having it on for ... seconds)
-   render trail (show trail, also known as bread crumbs)
-   pause incoming packets (also pauses incoming packets)

Array List (draws enabled features, no customization settings were added for this feature)

### Config 

*Toad auto loads configs when it finds one, if there multiple configs present, it loads the newest edited one.*

-   shows configs with `.toad` extension
-   refresh list
-   load (from file/clipboard)
-   save (to file/clipboard)

### Other

*Miscellaneous features that can be found by pressing the 3 stripes on the right top of the menu.*

-   tooltips (not implemented)
-   exclude from capture (only in external ui)
-   internal/external ui (switches to internal/external ui, can be buggy when switching to internal. Use **INSERT** to hide/show the internal ui)


## GUI 
![image](https://github.com/Steve987321/toadclient/assets/88980055/69bcf48c-4964-4bed-b8aa-cc6ec37d422a)
![image](https://github.com/Steve987321/toadclient/assets/88980055/659884ba-f57b-4eba-9248-a4de5ba39821)

## Disclaimer 

This project is provided as-is, without any warranty or support. I make no guarantees about its correctness or suitability for any purpose. Use at your own risk.
