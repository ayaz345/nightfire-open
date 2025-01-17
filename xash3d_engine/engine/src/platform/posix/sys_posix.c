/*
sys_win.c - posix system utils
Copyright (C) 2019 a1batross

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include <unistd.h>  // fork
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "platform/platform.h"
#include "EngineInternalAPI/menu_int.h"
#include "PlatformLib/File.h"
#include "PlatformLib/System.h"
#include "PlatformLib/String.h"

static qboolean Sys_FindExecutable(const char* baseName, char* buf, size_t size)
{
	const char* envPath;
	char* part;
	size_t length;
	size_t baseNameLength;
	size_t needTrailingSlash;

	if ( !baseName || !baseName[0] )
	{
		return false;
	}

	envPath = PlatformLib_GetEnv("PATH");

	if ( !COM_CheckString(envPath) )
	{
		return false;
	}

	baseNameLength = Q_strlen(baseName);

	while ( *envPath )
	{
		part = Q_strchr(envPath, ':');

		if ( part )
		{
			length = part - envPath;
		}
		else
		{
			length = Q_strlen(envPath);
		}

		if ( length > 0 )
		{
			needTrailingSlash = (envPath[length - 1] == '/') ? 0 : 1;

			if ( length + baseNameLength + needTrailingSlash < size )
			{
				Q_strncpy(buf, envPath, length + 1);

				if ( needTrailingSlash )
				{
					Q_strcpy(buf + length, size - length, "/");
				}

				Q_strcpy(buf + length + needTrailingSlash, size - length - needTrailingSlash, baseName);
				buf[length + needTrailingSlash + baseNameLength] = '\0';

				if ( access(buf, X_OK) == 0 )
				{
					return true;
				}
			}
		}

		envPath += length;

		if ( *envPath == ':' )
		{
			envPath++;
		}
	}

	return false;
}

void Platform_ShellExecute(const char* path, const char* parms)
{
	char xdgOpen[128];

	(void)parms;

	if ( !Q_strcmp(path, GENERIC_UPDATE_PAGE) || !Q_strcmp(path, PLATFORM_UPDATE_PAGE) )
		path = DEFAULT_UPDATE_PAGE;

	if ( Sys_FindExecutable("xdg-open", xdgOpen, sizeof(xdgOpen)) )
	{
		const char* argv[] = {xdgOpen, path, NULL};
		pid_t id = fork();
		if ( id == 0 )
		{
			execv(xdgOpen, (char**)argv);
			fprintf(stderr, "error opening %s %s", xdgOpen, path);
			_exit(1);
		}
	}
	else
	{
		Con_Reportf(S_WARN "Could not find xdg-open utility\n");
	}
}

void Posix_Daemonize(void)
{
	// to be accessed later
	if ( (host.daemonized = Sys_CheckParm("-daemonize")) )
	{
#if XASH_POSIX() && defined(_POSIX_VERSION)
		pid_t daemon;

		daemon = fork();

		if ( daemon < 0 )
		{
			Host_Error("fork() failed: %s\n", PlatformLib_StrError(errno));
		}

		if ( daemon > 0 )
		{
			// parent
			Con_Reportf("Child pid: %i\n", daemon);
			exit(0);
		}
		else
		{
			// don't be closed by parent
			if ( setsid() < 0 )
			{
				Host_Error("setsid() failed: %s\n", PlatformLib_StrError(errno));
			}

			// set permissions
			umask(0);

			// engine will still use stdin/stdout,
			// so just redirect them to /dev/null
			PlatformLib_Close(STDIN_FILENO);
			PlatformLib_Close(STDOUT_FILENO);
			PlatformLib_Close(STDERR_FILENO);
			PlatformLib_Open("/dev/null", O_RDONLY);  // becomes stdin
			PlatformLib_Open("/dev/null", O_RDWR);  // stdout
			PlatformLib_Open("/dev/null", O_RDWR);  // stderr

			// fallthrough
		}
#else
		Sys_Error("Daemonize not supported on this platform!");
#endif
	}
}

#if !XASH_SDL
void Platform_Init(void)
{
	Posix_Daemonize();
}
void Platform_Shutdown(void)
{
}
#endif

#if XASH_TIMER == TIMER_POSIX
double Platform_DoubleTime(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
}

void Platform_Sleep(int msec)
{
	usleep(msec * 1000);
}
#endif  // XASH_TIMER == TIMER_POSIX
