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
	
## Reporting Bugs

Please report any bugs that you may encounter here in the issues tracker. Please check any existing issues first if it has already been reported.
When you do that, please provide
- the client log at *\AppData\Local\LEGO Software\LEGO Universe\Log Files*
- the server log (please copy the console output for the time being, will work on a log file soon(-ish)
- a detailed description of what you did, ideally a way to reproduce the error
- A stack trace if it's a server crash an you're running in debug mode

### Please note
There are different kinds of "crashes" that can happen, so please pay attention to what happens.

#### client
- The client get's stuck and doesn't respond anymore
- The client get's stuck while logging in, showing a message, should abort login after a while with an error
- The client returns to the login screen without error
- The client returns to the login screen with an error message in a blue box
- The client just shows a white window (had that once, don't remember when)
- The client crashes/exits, with an error file created in the Log Files directory

#### server
- The server crashes (in debugging, the crash appears in VS)
- The server displays an error in the log
- The server does not recieve anything/shows nothing in the log

> The LEGO® Group has not endorsed or authorized the operation of this game and is not liable for any safety issues in relation to the operation of this game. All files are under the General Public License, version 3, and are only for use on a non-commercial basis.
