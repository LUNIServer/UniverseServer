# LUNI 

## Setup Instructions
You should be familiar with the setup of LUNI before working with this repository.

1. Setup xampp with mysql and import luni.sql
2. Open the project in Visual Studio
	- Please use the Debug mode for now to see where crashes come from
3. Some mysql servers seem to be very picky about unset values in INSERT statements. If you experience problems creating accounts/characters/sessions, please add the necessary values to the *DB.cpp files.
	- I'll do that myself when i have time, but for me it works, and only have been told about this
4. When Login, Character Creation and World loading succeded, wou will notice to be in Venture Explorer without being able to move or see your character. This is actually the correct behaviour.
	- The game has a mission at the beginning to press shift to "wake up", which is not implemented yet.
	- So for the time being please use /tp <zoneid> to get to another world or try adding mission 1727 to the missions table to have that mission already done