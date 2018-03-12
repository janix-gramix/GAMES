#ifndef _X_DEFAUT_H_
#define _X_DEFAUT_H_

/*
 * Definitions dependantes des compilateurs et des machines.
 *
 * (C) COPYRIGHT FRANCE TELECOM 1996, 1997.
 *
 *
 * Ce fichier contient les dependences envers les compilateurs, car helas, malgre
 * le C ANSI il en reste.
 * Normalement (i.e. excepte pour des bugs de compilateur et cas incontournables)
 * la dependence des compilateurs doit passer par ce fichier.
 *
 * Points traites :
 *
 *  - definition d'un nom symbolique pour chaque compilateur, du type
 *      X_BORL_16 , X_ZTC_32, X_UNIX_32 .Il est conseille d'utiliser une
 *      definition de ce genre pour faciliter le changement de compilateur
 *     et de faire dependre toutes les definitions suivantes de celles-ci.
 *     Pour les compilateurs sous UNIX on suppose que la macro "unix" est
 *     definie a la compilation
 *
 * - gestion du "systeme". On definit les symboles suivants :
 *      X_PCDOS et X_SYSTEM_STRING = "PCDOS" pour l'environnement DOS.
 *      X_UNIX  et X_SYSTEM_STRING = "UNIX"  pour l'environnement UNIX.
 *     La dependance a l'environnement est surtout utilisee pour l'acces
 *     aux fichiers (noms des fichiers, balayage d'un repertoire...)
 *
 * - gestion de l'alignement (1, 2, 4 ). Pour les compilateurs qui alignent
 *   les donnees sur des frontieres de 4 on definit la macro X_PACKING_DATA
 *
 * - gestion du cdecl . On definit un type X_USER qui permet pour certains
 *   compilateur de qualifier les routines vues par les bibliotheques
 *   systemes (main, routines avec nb arg variable, argument de qsort...)
 *
 * - types de donnees par defaut:
 *     On definit pour tout compilateur les types X_INT16 et X_INT32 corres
 *     pondant aux entiers 16 et 32 bits. Pour les compilateurs 16 bits on
 *     definit X_DEF_INT_16,pour les 32 bits X_DEF_INT_32.
 *
 * Historique
 * 03-Nov-2003	L.D.P.		Ajout des limites
 * 11-Dec-2002   J. Monne   Rajout X_MSC_WCE (pour pocket PC and co)
 * 10-Juin-1999  J. Monne   Forcer X_USER pour les DLL
 * 13-Avr-1999  J. Monne    Synchronisation avec DJ pour API modeles
 * 02-Oct-1998   J. Monne        #define en premiere ligne
 * 15-Mai-1998  J. Monne    Prise en compte X_UNIX_64 pour DEC
 * 11-Avr-1997  J. Monne    Rajout de X_DLLEXPORT
 * 08-Avr-1997  J. Monne    Rajout des defaut pour X_TMS_C30
 * 11-Mar-1997  J. Monne    Rajout des defaut pour X_MSC_16
 * 13-Jan-1997	D. Jouvet	Changement de nom pour eviter conflits.
 * 20-Dec-1996  D. Jouvet   Dernieres retouches pour version 1.0.
 * 19-Dec-1996  D. Jouvet   Creation a partir d'une grande partie de x_compil.h
 * 10-Oct-1996  J. Monne    Modif X_USER pour X_MSC_32
 * 28-Mai-1996  J. Monne    Rajout pour Microsoft 32 bits (X_MSC_32)
 * 15-Nov-1995  D. Jouvet   Corrige warning Solaris (elif sans rien -> else).
 * 02-Jui_1995  J. Monne    Rajout d'une definition pour TEXAS TMSC30
 * 08-Fev-1995  J. Monne    Definition pour BORL_16 (avec DPMI)
 * 31-Jan-1995  J. Monne    Definition x_user pour Borland BORL_32
 * 25-nov-1994  J. Monne    Definition compilateur dos-extender sous borlandc
 * 05-avr-1994  J. Monne    Essai de definition compilateur NT sous borlandc
 * 08-mar-1994  J. MONNE    Derniers commentaires
 */

/**************** DEFINITION DE SYMBOLE PAR COMPILATEUR ***************************/
#include <limits.h>
#include <float.h>

#if defined(__BORLANDC__)
#define X_BORL
#if defined(__WIN32__)
#define X_BORL_32
#elif defined(__DPMI32__)
#define X_BORL_DPMI32
#elif defined(__DPMI16__)
#define X_BORL_DPMI16
#else
#define X_BORL_16
#endif
#endif      /* Borland */

#if defined(_MSC_VER)
#	if defined(MSDOS)
#		define X_MSC_16
#	elif defined(WIN32) || defined(_WIN32)
#		define X_MSC_32
#		if defined(_WIN32_WCE)
#			define X_MSC_WCE
#		endif
#	endif
#endif      /* Microsoft */

#if defined(__ZTC__)
#define X_ZTC_32
#endif      /* Zortech */

#if defined(__WATCOMC__)
#define X_WATC_32
#endif      /* Watcom */

#if defined(unix)
#if defined(__osf__)
#define X_UNIX_64
#else
#define X_UNIX_32
#endif
#endif      /* Unix */

#if defined(_TMS320C30)
#define X_TMS_C30
#endif      /* TMS C30 */
#if defined(__HOS_OS2__)
#define X_OS2_32
#endif


/************************* GESTION DU SYSTEME *************************************/

#if defined(X_BORL) || defined(X_MSC_16) || defined(X_MSC_32) \
|| defined(X_ZTC_32) || defined(X_WATC_32)
#define X_PCDOS
#define X_SYSTEM_STRING     "PCDOS"
/* miscellanous pour Zortech */
#if defined(X_ZTC_32)
#define _dos_getdrive       dos_getdrive
#define _dos_setdrive       dos_setdrive
#define _dos_setfileattr    dos_setfileattr
#endif
#elif defined(X_UNIX_32) || defined(X_UNIX_64)
#define X_UNIX
#define X_SYSTEM_STRING     "UNIX"
#elif defined(X_TMS_C30)
#define X_TMSC30
#define X_SYSTEM_STRING     "TMSC30"
#elif defined(X_OS2_32)
#define X_OS2
#define X_SYSTEM_STRING      "OS/2"
#else
#error Macro de nom du systeme non definies X_SYSTEM
#endif      /* Gestion systeme */


/************** PACKING DES DATA POUR ECRITURE DANS LES FICHIERS ******************/
#if defined(X_UNIX_32) || defined(X_UNIX_64)
#define X_PACKING_DATA  4
#endif      /* Gestion Packing */


/******************************* GESTION cdecl ************************************/

#if defined(X_MSC_16)
#define X_USER  cdecl
#elif defined(X_MSC_32)
#define X_USER  __cdecl
#elif defined(X_BORL_16)
#define X_USER  _USERENTRY
#elif defined(X_BORL_32) || defined(X_BORL_DPMI32) || defined(X_BORL_DPMI16)
#define X_USER  __cdecl
#else
#define X_USER
#endif   /* Gestion cdecl */

/******************************* GESTION X_DLLEXPORT ************************************/

#if defined(X_MSC_32)
#define X_DLLEXPORT	 extern __declspec(dllexport)
#define X_DLLEXPORT_C(type)   extern __declspec(dllexport) type X_USER
#define X_DLLEXPORT_H(type)   extern __declspec(dllexport) type X_USER
#elif defined(X_OS2)
#define X_DLLEXPORT_C(type) extern type _Export
#define X_DLLEXPORT_H(type) extern type
#else
#define X_DLLEXPORT X_USER
#define X_DLLEXPORT_C(type) type X_USER
#define X_DLLEXPORT_H(type) type X_USER
#endif   /* Gestion dllexport */



/**************************** DEFIITION DES TYPES *********************************/
/* Definis pour l'instant uniquement en mode standard 32 bits. */

#if defined(X_BORL_32) || defined(X_MSC_32) \
||  defined(X_ZTC_32) || defined(X_WATC_32) \
||  defined(X_UNIX_32) || defined(X_UNIX_64) || defined(X_OS2_32)
#define X_CHAR      char
#define X_INT16     short int
#define X_INT32     int
#define X_FLOAT32   float
#define X_FLOAT64   double
/* limites */
#define X_SCHAR_MIN SCHAR_MIN
#define X_SCHAR_MAX SCHAR_MAX
#define X_UCHAR_MAX UCHAR_MAX

#define X_INT16_MIN SHRT_MIN
#define X_INT16_MAX SHRT_MAX
#define X_UINT16_MAX USHRT_MAX

#define X_INT32_MIN INT_MIN
#define X_INT32_MAX INT_MAX
#define X_UINT32_MAX UINT_MAX

#define X_FLOAT32_MIN FLT_MIN
#define X_FLOAT32_MAX FLT_MAX

#define X_FLOAT64_MIN DBL_MIN
#define X_FLOAT64_MAX DBL_MAX

#elif defined(X_MSC_16)
#define X_CHAR      char
#define X_INT16     short int
#define X_INT32     long
#define X_FLOAT32   float
#define X_FLOAT64   double

/* limites */
#define X_SCHAR_MIN SCHAR_MIN
#define X_SCHAR_MAX SCHAR_MAX
#define X_UCHAR_MAX UCHAR_MAX

#define X_INT16_MIN SHRT_MIN
#define X_INT16_MAX SHRT_MAX
#define X_UINT16_MAX USHRT_MAX

#define X_INT32_MIN LONG_MIN
#define X_INT32_MAX LONG_MAX
#define X_UINT32_MAX ULONG_MAX

#define X_FLOAT32_MIN FLT_MIN
#define X_FLOAT32_MAX FLT_MAX

#define X_FLOAT64_MIN DBL_MIN
#define X_FLOAT64_MAX DBL_MAX

#elif defined(X_TMS_C30)
#define X_CHAR      char
#define X_INT16     short int
#define X_INT32     long
#define X_FLOAT32   float
/* limites */
#define X_SCHAR_MIN SCHAR_MIN
#define X_SCHAR_MAX SCHAR_MAX
#define X_UCHAR_MAX UCHAR_MAX

#define X_INT16_MIN SHRT_MIN
#define X_INT16_MAX SHRT_MAX
#define X_UINT16_MAX USHRT_MAX

#define X_INT32_MIN LONG_MIN
#define X_INT32_MAX LONG_MAX
#define X_UINT32_MAX ULONG_MAX

#define X_FLOAT32_MIN FLT_MIN
#define X_FLOAT32_MAX FLT_MAX

#else
#error "Types generaux non definis ou environnement inconnu."
#endif  /* Definition des types. */


/*************************** Types logiques. **************************************/
typedef enum {xFalse = 0,  xTrue = 1} xBoolean;

typedef enum {xError = -1,  xOK = 1}  xStatus;

#endif /* _X_DEFAUT_H_ */
