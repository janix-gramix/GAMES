#ifndef	_X_ALLOC_H
#define _X_ALLOC_H

#include <stdlib.h>
#include "x_defaut.h"
/*
 * Specification des fonctions d'allocation memoire par defaut
 * Si X_ALLOC_SPECIAL n'est pas defini on remplace par malloc
 * classique
 *
 * (C) COPYRIGHT FRANCE TELECOM 1996, 1997.
 *
 *
 * Historique :
 * 03-Sep-1999  J. Monne    Rajout X_USER (__cdecl) pour les fonctions allocation 
 * 02-Mar-1999  J. Monne    Rajout du calcul de la taille memoire
 * 02-Oct-1998   J. Monne        #define en premiere ligne
 * 05-Mar-1997  J. Monne    Creation du fichier.
 */

#if defined(X_ALLOC_SPECIAL)
	void * X_USER XMalloc( size_t size );
	void * X_USER XCalloc( size_t num, size_t size );
	void * X_USER XRealloc( void *memblock, size_t size );
	void X_USER XFree( void *memblock );
	unsigned X_INT32 XGetCurSizeAlloc(void) ;
	unsigned X_INT32 XGetMaxSizeAlloc(void) ;
#else
#define	XMalloc				malloc
#define	XCalloc				calloc
#define	XRealloc			realloc
#define	XFree				free
#endif

#endif
