server: const.h connector.h kbhit.h security.h
	gcc -Wall -o server server.c
	gcc -Wall -o startserver startserver.c

startserver: netrate.h kbhit.h
	gcc -Wall -o startserver startserver.c

clean:
	rm server
	rm startserver
