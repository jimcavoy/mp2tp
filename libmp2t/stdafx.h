// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef WIN32
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define _CRTDBG_MAP_ALLOC
#include <WinSock2.h>
#include <crtdbg.h>

#ifdef _DEBUG_NEW
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif // _DEBUG_NEW
#endif
