# LUNI 

## Setup Instructions
You should be familiar with the setup of LUNI before working with this repository.

1. Setup xampp with mysql and import luni.sql
2. To configure the server to run correctly on your machine, you need to adjust the redirect_ip in the config.ini file.
	- The redirect IP there is used to tell the client where to find the char/world server i.e. it is the IP the server sends whenever it informs the client about a necessary redirect.
	- If you care about multiplayer, this IP needs to be one that is accessible for all PCs that want to connect, so for a LAN server this would be your local address (e.g. 192.168.0.9 for me)
	- You could set it up to use your global IP but then you have to deal with port forwarding and the like.
	- Finally if you only want to test it locally, you can simply put in 127.0.0.1
3. Open the project in Visual Studio
	- Please use the Debug mode for now to see where crashes come from
4. Some mysql servers seem to be very picky about unset values in INSERT statements.
	- If you experience problems creating accounts/characters/sessions, please add the necessary values to the *DB.cpp files.
	- I'll do that myself when i have time, but for me it works, and only have been told about this
5. When Login, Character Creation and World loading succeded, wou will notice to be in Venture Explorer without being able to move or see your character. This is actually the correct behaviour.
	- The game has a mission at the beginning to press shift to "wake up", which is not implemented yet.
	- So for the time being please use /tp <zoneid> to get to another world or try adding mission 1727 to the missions table to have that mission already done