

#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "defs.h"

#ifdef WIN32
#include <Windows.h>
#include <io.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

int GetTimeMs(void) {//Function for windows as well as for linux to get the time in milliseconds
#ifdef WIN32
	return GetTickCount();
#else
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec * 1000 + t.tv_usec / 1000;
#endif
}

// http://home.arcor.de/dreamlike/chess/
int InputWaiting()
{
#ifndef WIN32
	struct timeval tv;
	fd_set readfds;

	FD_ZERO (&readfds);
	FD_SET (fileno(stdin), &readfds);
	tv.tv_sec=0; tv.tv_usec=0;
	select(16, &readfds, 0, 0, &tv);

	return (FD_ISSET(fileno(stdin), &readfds));
#else
	static int init = 0, pipe;
	static HANDLE inh;
	DWORD dw;

	if (!init) {
		init = 1;
		inh = GetStdHandle(STD_INPUT_HANDLE);
		pipe = !GetConsoleMode(inh, &dw);
		if (!pipe) {
			SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
			FlushConsoleInputBuffer(inh);
		}
	}
	if (pipe) {
		if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
			return 1;
		return dw;
	} else {
		GetNumberOfConsoleInputEvents(inh, &dw);
		return dw <= 1 ? 0 : dw;
	}
#endif
}

void ReadInput(S_SEARCHINFO *info) {//To read the input from the file
	int bytes;	            //Hold the bytes 
	char input[256] = "", *endc;

	if(InputWaiting()) {//If input is not taken i.e, it's waiting
		info->stopped = TRUE;//stopped is set to true
		do {
			bytes=read(fileno(stdin), input, 256);//To read the bytes from the standard input file 256 characters
		} while (bytes<0);//while it's not 0
		endc = strchr(input, '\n');//It will return a pointer to the first occurence of the character in the string
		if (endc)
			*endc = 0;

		if (strlen(input) > 0) {
			if (!strncmp(input, "quit", 4))	{
				info->quit = TRUE;
			}
		}
		return;
	}
}
