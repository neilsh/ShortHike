/* Copyright (C) 1986-2001 by Digital Mars. $Revision: 1.1.1.1 $ */
#if __SC__ || __RCC__
#pragma once
#endif

#ifndef __NEW_H
#define __NEW_H 1

/* Define _CRTAPI1 (for compatibility with the NT SDK) */
#ifndef _CRTAPI1
#define _CRTAPI1 __cdecl
#endif

/* Define _CRTAPI2 (for compatibility with the NT SDK) */
#ifndef _CRTAPI2
#define _CRTAPI2 __cdecl
#endif

/* Define CRTIMP */
#ifndef _CRTIMP
#if defined(_WIN32) && defined(_DLL)
#define _CRTIMP  __declspec(dllimport)
#else
#define _CRTIMP
#endif
#endif

extern "C++" {

#if !defined(__NEW_DEFINED)  

#define __NEW_DEFINED 1
void *operator new(unsigned, void *p);
#if _ENABLE_ARRAYNEW
void *operator new[](unsigned, void *p);
#endif
#endif

typedef int (__cdecl * _PNH)( unsigned );	//for Microsoft compatibility
typedef void (* PFVV)();		//as defined in ARM


_PNH __cdecl _set_new_handler(_PNH);		//for Microsoft compatibility
PFVV set_new_handler(PFVV);			//as defined in ARM
}

#endif
