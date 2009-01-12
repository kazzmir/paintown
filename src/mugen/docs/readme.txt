
                           M. U. G. E. N

                  beta release version 2002.04.14
                           2002 Elecbyte
                             ---------

      Please read this whole document before proceeding.

      This is a public beta test version. You may use it to create
      your own game, but please take note: we may change
      specifications at any time during the beta phase, so anything
      you make now may not necessarily work with later versions of
      M.U.G.E.N.

      Not all docs are complete yet. Please check mugenfaq.txt for
      answers to common problems and questions.

      This distribution includes a sample character Kung Fu Man. You
      can check his movelist in the readme file in his directory:
        chars/kfm/

      --------------------------------------------------------------
      UPDATE NOTICE: 
           - MUGEN has moved from DOS to Linux! Please see the Known
	     Issues / Notes section below for other important
	     information.

           - Some configuration files, such as data/mugen.cfg and 
             data/select.def, may have had new parameters added to them.
             Please overwrite your existing files if you have an older
             version of MUGEN.
             We recommend you do a clean install as there may be
             files and directories in previous version that are now
             no longer used.
      --------------------------------------------------------------



Updates
-------
Please check updates.txt for updates.
Please check incompt2.txt for incompatibility notices regarding
characters made for MUGEN version 2000.01.01 and earlier.



Contents
--------
I.      MUGEN Info
II.     Key Configuration
III.    Running the Game
IV.     Game HotKeys
V.      Speed Issues
VI.     Memory Issues
VII.    Known Issues / Notes
A.      License Agreement
B.      Disclaimer
C.      Thanks


=====================================================================
I. MUGEN Info
=====================================================================

Please carefully read the license agreement in Appendix A before
using MUGEN.

If you have any questions, please refer to mugenfaq.txt.


System requirements
-------------------
Minimum: Pentium 90
Recommended: Pentium 200

M.U.G.E.N is not set up for maximum performance by default. See
"Speed Issues" (section IV) for information on how to optimize
this program for a slower computer.
It is hard to state exact memory requirements, as it varies
according to character and stage complexities. 32 MB should suffice,
although 64 MB is probably much safer. Simultaneous team battle
modes will require more memory, as it needs to load up more than
two characters. For larger characters, more memory will be required.


What MUGEN?
-----------
MUGEN is a 2D fighting game engine, but you probably knew that already.
MUGEN is also an acroymn for something, but we forgot what it is. :)
We created MUGEN not only to make a fighting game, but also to let 
others create their own fighting games.  You can control almost every
aspect of the engine to suit your tastes.  That's the primary goal of
MUGEN.  Since this is a work in progress, there are many things that
it can and cannot do, but here's a short list of some of the things
you can do...

- Customizable title screen, character select screen, life and power
  bars, game sound effects, fonts and more
- Characters can have any number of sounds and sprites of any size
  limited by your computer's memory
- Up to 12 different color schemes for each character
- Background music can be mp3, mod, midi or CD audio
- Supports keyboard, joystick and Playstation joypads (through the use
  of a converter).

Game Engine
- Use up to 6 buttons for a character (7 if you count the Start button)
- Regular moves/Special moves/Supers
- Projectiles
- Multiple air jumps
- Move cancel/2-in-1 or whatever you want to call it
- Multi-part moves/throws
- What your character can do is defined by how you build his
  state machine


How MUGEN?
----------
MUGEN is being developed in C using Allegro.  It is being compiled
with gcc 2.95.2 and linked with Allegro 4.0.0.


Where MUGEN?
------------
http://mugen.elecbyte.com


Why MUGEN?
----------
To tell you the truth, we were making a shooting game and all of a
sudden it turned into this.  Although we were making a shooting game
in the beginning, we noticed that there weren't any good commercial
fighting games on the PC.


Who MUGEN?
----------
If you want to contact us about something, please visit
http://www.elecbyte.com/contact.html


When MUGEN?
-----------
I don't quite remember when we started this project.
Our first public release version was 9X.06.27.



=====================================================================
II. Key Configuration
=====================================================================

This is the default key configuration. You can change it from the
options menu when you run the game.

Button      Player 1      Player 2
------      --------      --------
  Up        Up arrow      Numpad Up
 Down       Down arrow    Numpad Down
 Left       Left arrow    Numpad Left
Right       Right arrow   Numpad Right
  X         L             Numlock 
  Y         semicolon     Numpad slash
  Z         double-quote  Numpad asterisk
  A         comma         Numpad -
  B         period        Numpad +
  C         slash         Numpad Enter
Start       Enter         Numpad PgDn

If you have a joystick, you can enable it through the options screen.
Press F1 to access Input Config and set up your joystick from there.
Scroll left/right on the joystick type option to pick your joystick
type. If MUGEN is unable to detect your joystick, its name will be
shown in grey. You can also set the joystick type from the [Input]
group in data/mugen.cfg.



=====================================================================
III. Running the Game
=====================================================================

At a Linux console, type "./mugen" to begin.

M.U.G.E.N can run under console and X-Windows.

Main menu:
----------
Arcade         - Go 1 on 1 against the computer
Versus         - Go 1 on 1 against your friend
Team Arcade    - Play various team-up modes against the computer
Team Versus    - Play various team-up modes against your friend
Team Co-op     - Gang up against the computer with your friend
Survival       - See how long you can last in an endless battle!
Survival Co-op - Play survival mode with your friend as a partner
Training       - Try out moves and combos
Watch          - Watch AI-controlled characters fight
Options        - Set up basic game options
Exit           - Returns you to the DOS prompt

For "Arcade" mode, the key you hit will determine which side you play
on. If you chose with one of Player One's keys, your character will
play on the left side. If you chose with Player Two's keys, you will
start on the right side.

For the "team" modes, you first choose what team mode you would like
to play in. Press up/down to choose a mode. Some modes, such as Turns
mode, allow you to set the number of players on a team by pressing
left/right. Press a key to choose an option. In "Team Arcade", after
selecting your players, you have a choice of your opponents' team
mode. These are the different kinds of team modes:
  Single - Just you alone. Your character has 2 rounds.
  Simul  - You and a partner at the same time. Your team has 2 rounds.
  Turns  - You and up to 3 partners. When one character is KOed, the
           next will join in. Each character has 1 round.
The characters' starting life will be adjusted according the number
of players on each side.

Team Co-op is slightly different. The only team mode allowed is
Simul, which is automatically selected. Player One first gets to
select his character, followed by Player Two selecting the partner
character. When Player Two is done, Player One chooses the opponent's
team mode.

In Survival mode, there is an endless stream of opponents. The
objective is to beat as many opponents as possible. The game is over
when your team gets KOed. You can choose to play alone or in a team.
Single player mode gives you highest hit points and healing (when you
win a round). The more players you have on your team, the less damage
each player can take, and the less each healing you get after each
round.

In "Watch" mode, first choose the team mode and characters to be on
Player One's side, then do the same for Player Two.


Switching order in Turns mode
-----------------------------
When you are playing Turns team mode, you can change your team order
during one of these situations:

1. right before the match start loading
2. after you lose a round, and right before the next round is loaded

The order switching is achieved by holding a directional button down.
Hold forward to rotate your team order by one member forwards. Hold
back to rotate your team order by one member backwards. If you have
4 available members, you can rotate it by 2 members by holding up.
Note that you can only rotate with members that have not yet been
KO'ed.

Here is a chart to clarify the way it works.

	         starting team
            1 2 3 4  |  1 2 3  |  1 2
           ----------+---------+------
 hold fwd:  2 3 4 1  |  2 3 1  |  2 1
 hold back: 4 1 2 3  |  3 1 2  |  2 1
 hold up:   3 4 1 2  |  ^      |  ^
            ^           |         |
            |           |         |
     this is the character that will come in


Command-line arguments
----------------------

M.U.G.E.N accepts optional command-line parameters for Quick-VS mode.

The format is (optional parameters are enclosed in square brackets):
  ./mugen [player1 player2 [-s stage]]

For example, to start quick versus between players named KFM and
Suave, you can type:
  ./mugen kfm suave
To play the same two characters in a stage named TEMPLE, type:
  ./mugen kfm suave -s temple
You can specify alternate .def files for your character:
  ./mugen kfm/newkfm.def suave -s temple

Here are other useful command-line options:
 -h            Displays help
 -r <sysfile>  Loads a motif (more information below)
 -p3 <pname>   Loads a character named pname as player3
 -p4 <pname>   Loads a character named pname as player4
 -s <sname>    Loads a stage named sname.def in stages/

For a full list of command-line options, type:
  ./mugen -h


Using motifs
------------

A motif is a custom configuration of the graphics and sounds
used in the game's interface, as well as other things such
as the character roster. The default motif is found in the
data/ directory. Custom motifs are placed as subdirectories
under data/. For example, the "kfm" motif is placed in
data/kfm/, and to use it you would type:
  ./mugen -r kfm

Here's what a motif covers (and which files each is in):
- character roster and stage list (select.def)
- title screen graphics, sounds and music (system.def)
- character screen graphics, sounds and music (system.def)
- versus screen graphics and music (system.def)
- option screen graphics, sound and music (system.def)
- filenames of storyboards (logo, intro, credits, etc) (system.def)
- storyboard files (filenames specified in system.def)
- fight demo options (system.def)
- continue/game over options (system.def)
- fight screen graphics and sounds (fight.def)
  The fight screen includes the life and power bars, "round X"
  images and sounds, common hit sparks and sound effects, etc.

To set the default motif, edit data/mugen.cfg with
a text editor, and change "motif" under [Options].

To make your own motif, create a subdirectory under
data/ with the name of your motif, and copy system.def
into the new directory. If you'd like to edit any other
files besides system.def, make copies of them in your
motif directory and change them there. Any data file
that doesn't exist in your motif directory will default
to the one in the data/ directory, so if you did not
copy select.def over, data/select.def will be used
when you run with your motif selected.

Two sample motifs are included: 
"big" - gives you many spaces to put characters
"kfm" - the Kung Fu Man game motif


Training Menu
-------------

A training menu has been added to Training mode. You can choose menu
items with your directional keys and exit Pause Mode by pressing
any attack key or Pause. The following options are available:

Dummy control: Cooperative, AI, or Manual
  When in cooperative mode, the dummy will perform the actions you 
  specify elsewhere in the training menu. When in AI mode, the dummy
  will act like a normal computer opponent. When in manual mode, the
  dummy can be controlled with the opponent's keys.
 
Guard mode: None, Auto
  If guard mode is set to Auto, the dummy will attempt to block most
  attacks. If guard mode is set to None, the dummy will not block any
  attacks.

Dummy mode: Stand, Crouch, Jump, W Jump
  Depending on your selection, the dummy will stand, crouch, or 
  repeatedly jump. If you choose W Jump, the dummy will keep air 
  jumping as long as it can.

Distance: Any, Close, Medium, Far
  If you choose Close, Medium, or Far, the dummy will try to keep the 
  appropriate distance from you by walking forward/backward as 
  necessary. If you choose Any, the dummy will not move forward or 
  backward.

Button jam: None, A, B, C, X, Y, Z, Start
  The dummy will repeatedly mash the selected button. Good for testing
  a character's blocking.

You can minimize the training menu by pressing M while it is active.
Press M again to reenable it.



=====================================================================
IV. Game HotKeys
=====================================================================

These are the hotkeys recognized at the fight screen:

Key       Function
---       --------
Pause     Toggle pause
ScrollLck Frame-step within pause
Esc       Quit

The following hotkeys are for debugging purposes.

Key       Function
---       --------
F1        Sets Player 2's life to zero
Ctrl-F1   Sets Player 1's life to zero
F2        Sets both players' life to 1
Ctrl-F2   Sets Player 1's life to 1
Shift-F2  Sets Player 2's life to 1
F3        Gives both players full power
F4        Reset the round
Shift-F4  Reloads stage, characters and fight data
F5        Time Over
F12       Take a screenshot (saved to mugen?.pcx)
Ctrl-C    Toggles display of collision boxes, target data (including
          remaining juggle points) and NotHitBy attributes
Ctrl-D    Toggles debug information display
Ctrl-I    Forces both players into stand state
Ctrl-L    Toggles display of the life and power bars
Ctrl-S    Run the game as fast as possible
Ctrl-V    Enable V-sync (stops "shearing")
Ctrl-#    (where # is from 1-4) Toggles AI for the #th player
Ctrl-Alt-# (where # is from 1-4) Enables/Disables the player
Space     Restores full life and power to all players



=====================================================================
V. Speed Issues
=====================================================================

If you find this runs slowly on your machine, there are several
things you can do to improve its performance. You can change these
options in data/mugen.cfg.

i. Use the most efficient video mode
   Running in a window can be slow. Try running in fullscreen. 
   Look under [Video Linux] in data/mugen.cfg. Also, running MUGEN as
   root may improve video performance.

ii. Select the correct resolution
   M.U.G.E.N is meant to run under 320x240 in 16-bit colour.
   Anything larger or in a different bit depth may decrease the
   speed.

iii. Turn off stretching
   If you find you cannot run in 320x240, you can choose 640x480
   and keep the "stretch" option off.

iv. Turn off shadows
   You can turn shadows off to speed up drawing. Look in 
   data/mugen.cfg and set the "DrawShadows" option under [Config]
   to 0.

v. Turn off sound or music
   Playing music during the game takes up CPU power. This is true
   especially for MP3s. To turn off the background music, open
   data/mugen.cfg and set "PlayMIDI" and "PlayMP3" to 0.
   To turn off all sounds and music, set "Sound" to 0. You can
   also use the -nosound command line option to disable sounds,
   or the -nomusic option to disable music only.

vi. Disable your joystick
   Some joysticks may cause slowdown in M.U.G.E.N. You can disable
   your joystick in the Options menu from within the game, or run
   with the -nojoy option.

vii. Use frameskip
   Auto-frameskip is enabled by default. The game will not draw
   some frames in case the computer is not fast enough, and this
   helps maintain a constant game speed.
   If you'd like to run at a constant framerate, you can hit Ctrl-F
   repeatedly to adjust the frameskip. It will switch from "auto"
   to "skip none" to "skip 1" to "skip 2" and then back to "auto".
   This works only when you are in the fight screen.

viii. Free up memory
   Memory may be swapped to the hard disk when you run low, and
   this will severely impact performance. See the next section
   on how to reduce memory usage.

ix. Disable precaching
   If you find the game slows down too much when loading in the
   background, you can disable precaching. This will increase
   load times, however. Under [Misc], set precache to 0.



=====================================================================
VI. Memory Issues
=====================================================================

If you find the program exiting or running slowly because you don't
have enough memory, here are some solutions:

i. Reduce the player cache
   M.U.G.E.N will try to keep players in memory in order to reduce
   loading times. You might want to reduce the number of players
   that are kept in memory at any one time.
   Open up data/mugen.cfg and look under the [Misc] section.
   Change PlayerCache to a smaller number. 0 will save you the
   most memory.

ii. Reduce the number of characters
   Having a large number of characters can consume a substantial
   amount of memory. You can split your character roster up between
   multiple motifs. See "Using Motifs" in this readme file.

iii. Turn off sound or music
   You can save some memory by disabling sound and music. See part
   v. under "Speed Issues".

iv. Disable buffered read
   You can save memory while loading characters by turning this
   option off. It is found under the [Misc] section, and called
   "BufferedRead". Loading times will increase as a result.

v. Reduce effects and limits
   Set the options under [Config] to a smaller numbers (see mugen.cfg
   for descriptions). Reducing HelperMax gives the most savings.

vi. Enable system file unload
   Set UnloadSystem under [Misc] to 1.

vii. Optimize your sprite files
   When you generate a sprite file, run sprmaker with the -c -f -p
   command line options. This generates the smallest possible sprite
   file.



=====================================================================
VII. Known Issues / Notes
=====================================================================

General
-------
- Linux version is new. There are several known problems at this time:
  - depth in mugen.cfg must match depth when in X
  - console must use same res and depth as screen. Not allowed to switch
    res/depth in console (unless root)
  - be careful of filename case sensitivity and \ and / in files.
  - multi-joystick and sound do work, but linux must be set up correctly
    first
  - linux sound uses esd or /dev/dsp, but 2.4.X kernels have changed to
    /dev/sound/dsp
  - cannot disable key repeat in X. You can make a script to run mugen:
      xset r off    (disable)
      ./mugen
      xset r on     (enable)

- Storyboards lack text and sound options


Players
-------
- No support for multiple palettes in players

- Helpers used as projectile replacements do not have all the desired
  properties of a projectile

- Collision boxes in helpers inherit parent's scale, regardless of own
  scale

- HitOverride controller always prevents player from being hit by
  attacks with a HitDef p2stateno parameter

- Stick-around helpers and explods get removed during intro skip



=====================================================================
A. License Agreement
=====================================================================

By using MUGEN beta 2002.04.14, you agree to the following terms and
conditions of use:

1. MUGEN version 2002.04.14 is beta software. You agree to indemnify
Elecbyte from liability for any damage incurred to any computer
hardware, software, or other property, as well as from any injury
incurred to your person or others, through use of this software.
Elecbyte shall not be held responsible for any failure of MUGEN
2002.04.14 to operate properly, whether through deficiencies of the
software or through user error. Elecbyte disclaims all express and
implied warranties, including but not limited to warranties of
merchantability and fitness for a particular purpose.

2. The MUGEN environment, defined as any or all of the 2002.04.14
executable, documentation, sample character and stage files,
configuration files, and other associated data files provided by
Elecbyte and necessary for proper operation of the executable, is
copyrighted by Elecbyte and may not be redistributed in whole or in
part, altered or unaltered, without Elecbyte's express written
permission. Only the following files are exempt from this
restriction, and may be freely distributed and/or altered: all files
in the data/ directory, all files in the chars/kfm/ directory, 
stages/kfm.def and stages/kfm.sff.

3. Elecbyte places no restrictions on distribution of character files,
stage files, add-on packs, or similar items which operate under the
MUGEN 2002.04.14 environment, as long as said distribution is pursuant
to the conditions of paragraph 2. All applicable laws governing such
distribution remain in force.  You agree to indemnify Elecbyte from any
legal liability for your use, or distribution, of such files.

4. Agreement to this license grants you the right to use the MUGEN
environment, version 2002.04.14, until and not beyond 12:01 AM
(Greenwich Mean Time) on June 1, 2002. After this time, Elecbyte may,
at its sole discretion, choose to offer, or not to offer, a new public
license. If a new license is offered, its terms shall govern any
continued use of the MUGEN 2002.04.14 beta. If no new license is
offered, all use of the 2002.04.14 beta must cease.

5. Failure by Elecbyte to enforce any of the terms of this agreement
shall not constitute forfeiture of Elecbyte's right to enforce said
terms.

6. In the event of an inconsistency between this license agreement and
other Elecbyte documents, the terms of this license agreement shall
prevail, subject only to possible supersession by subsequent license
agreements. If any of the terms of this license agreement conflict with
the laws in your locale, the conflicting terms will be rendered null and
void. The remainder of this agreement shall still obtain.



=====================================================================
B. Disclaimer
=====================================================================

Elecbyte will not be held liable for any damages to either the user
or the system it is run on, that may occur as a direct or indirect
result of the use of MUGEN and its associated tools. Elecbyte holds
no responsibility for the actions of the user, including but not
limited to the user's infringement on the intellectual rights of
others.



=====================================================================
C. Thanks
=====================================================================

DJ Delorie - For DJGPP
Shawn Hargreaves and many others - For Allegro
Tomislav Uzelac and Ove Kaaven - For LibAmp, and its Allegro port
Guan Foo Wah - For JGMOD
Peter Wang, Brennan Underwood and friends - For libcda
Earle F. Philhower, III - For DirectPad Pro converter schematics
David Cornish - For SFX Maker, to make the self-extractable archive
All our beta testers
Everyone who contributed to us in any way
All of you who gave feedback to us

If we forgot to mention someone, please let us know!
