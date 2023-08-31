/*
port.h -- Portability Layer for Windows types
Copyright (C) 2015 Alibek Omarov

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#pragma once
#ifndef PORT_H
#define PORT_H

#include "BuildDefs/build.h"

#if !XASH_WIN32

#define OS_LIB_EXT "so"
#define OPEN_COMMAND "xdg-open"

#define OS_LIB_PREFIX "lib"
#define VGUI_SUPPORT_DLL "libvgui_support." OS_LIB_EXT

#if XASH_POSIX
#include <unistd.h>
#include <dlfcn.h>
#define HAVE_DUP
#define O_BINARY 0
#define O_TEXT 0
#define _mkdir(x) mkdir(x, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#endif

typedef void* HANDLE;
typedef void* HINSTANCE;

typedef struct tagPOINT
{
	int x, y;
} POINT;
#else  // WIN32

#define HSPRITE WINAPI_HSPRITE
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#undef HSPRITE

#define OS_LIB_PREFIX ""
#define OS_LIB_EXT "dll"
#define VGUI_SUPPORT_DLL "../vgui_support." OS_LIB_EXT
#define HAVE_DUP
#endif  // WIN32

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if defined XASH_SDL && !defined REF_DLL
#include <SDL.h>
#endif

#endif  // PORT_H
