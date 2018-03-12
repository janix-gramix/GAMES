#ifndef _ERR_UTI_H_
#define _ERR_UTI_H_

 /**************************************************************************
 *                                                                         *
 *        Copyright (C) FRANCE TELECOM 1992 - TOUS DROITS RESERVES.        *
 *                                                                         *
 *                                                                         *
 *    Le present logiciel est inscrit au repertoire de l'AGENCE POUR LA    *
 *    PROTECTION DES PROGRAMMES, 119, rue de Flandre - 75019 PARIS.        *
 *                                                                         *
 *    Il est protege  en France  par les lois  du 11 mars 1957  et du 3    *
 *    juillet 1985, et a l'etranger par les conventions internationales    *
 *    sur le droit d'auteur.                                               *
 *                                                                         *
 *    Il est interdit  de reproduire,  adapter,  louer  ou desassembler    *
 *    ce logiciel ainsi que la documentation qui y est associee.           *
 *                                                                         *
 *    Il est rappele que  toute contrefacon  est sanctionnee  en France    *
 *    par les articles 425  et suivants  du code penal  et  punie d'une    *
 *    peine d'emprisonnement de 3 mois a 2 ans  et d'une amende de 6000    *
 *    a 120000 F ou de l'une de ces deux peines seulement.                 *
 *                                                                         *
 *    Les conditions d'utilisation sont precisees dans la licence.         *
 *                                                                         *
 *    L'APP  est mandatee  par l'auteur  pour faire  sanctionner toutes    *
 *    copie et/ou utilisation non autorisees.                              *
 *                                                                         *
 **************************************************************************/

/* -----------------------------------------------------------------------------
 * Definition des codes d'erreur des fonctions de la librairie libuti
 *
 *
 * Historique:
 * 28-Dec-2001	D. Jouvet	Ajout codes erreur module CFG.
 * 21-Fev-2000	D. Jouvet	Ajout erreur ERR_UNDEFINED.
 * 14-Oct-1999	D. Jouvet	Ajout code erreur ERR_CR_STOP...
 * 04-Oct-1999	D. Jouvet	Ajout code erreur ERR_TXT_SYNTAX.
 * Annee  1998	marche CRE2A-DI	- Mise en forme API FMD.
 * -----------------------------------------------------------------------------
 */

/* ----------------------
 * Directives d'inclusion
 * ----------------------
 */
#include "x_defaut.h"			/* Fcts utilitaires */

/*
 * Ajout pour fonctionnement possible en DLL. Dans ce cas definir USE_DLL_UTI
 */
#if defined ( USE_DLL_UTI )
#define DLL_FCT(type)   X_DLLEXPORT(type)
#define DLL_FCT_H(type) X_DLLEXPORT_H(type)

#else
#define DLL_FCT(type)   type
#define DLL_FCT_H(type) type

#endif

/* ------------------------------------
 * Declaration des Constantes exportees
 * ------------------------------------
 */
typedef enum {	
				CR_OK					= 0,
				ERR_CR_OK				= 0,
				ERR_CR_STOP				= 0x0001,
				ERR_UNDEFINED,

				/* Erreurs communes */
				ERR_ALLOC_MEMORY		= 0x0011,
				ERR_PTR_NULL,

				/* Module CVT */
				ERR_CVT_VAL_INT			= 0x0101,
   				ERR_CVT_VAL_FLOAT,
				ERR_CVT_OVF_INT,
				ERR_CVT_OVF_FLOAT,

				/* Module SMB */
				ERR_SMB_ORDRE_SYMBOLE	= 0x0121,

				/* Module STR */
				ERR_STR_STRING_TOO_LONG = 0x0141,

				/* Module FIC */
				ERR_FIC_PARSING			= 0x0161,
				ERR_FIC_BUFF_SIZE,
				ERR_FIC_BUFFER,
				ERR_FIC_OUVERTURE,
				ERR_FIC_FERMETURE,
				ERR_FIC_OPENDIR,
				ERR_FIC_READDIR,
				ERR_FIC_NO_MORE_FILES,

				/* Module TXT */
				ERR_TXT_LECTURE			= 0x0181,
				ERR_TXT_BUFFER,
				ERR_TXT_INCLUDE_NO_FILE_NAME,
				ERR_TXT_INCLUDE_BAD_FILE_NAME,
				ERR_TXT_SEQUENCE,
				ERR_TXT_STRING,
				ERR_TXT_DEFINE,
				ERR_TXT_VAL_END,
				ERR_TXT_VAL_FMT,
				ERR_TXT_TYPE_DIFFERENT,
				ERR_TXT_INT_NOVALUE,
				ERR_TXT_FLOAT_NOVALUE,

				ERR_TXT_EXTRA_DATA		= 0x01A1,
			 	ERR_TXT_MISSING_END,
				ERR_TXT_BAD_KEYWORD,
				ERR_TXT_BAD_DELIMITER,
				ERR_TXT_KEYWORD,
				ERR_TXT_SYNTAX,

				/* Module SET */
				ERR_SET_REDEFINITION	= 0x01C1,

				/* Module PLD */
				ERR_PLD_FLAG			= 0x01E1,
				ERR_PLD_FLAG_VALUE,
				ERR_PDL_NOM_DEFAUT,

				/* Module NET */
				ERR_NET_TRAN_INVALIDE_BAD_ETATS = 0x0201,
				ERR_NET_TRAN_INVALIDE_BAD_UNITE,
				ERR_NET_REDEFINITION,

				/* Module CMD */
				ERR_CMD_BUFF_SIZE		= 0x0221,
				ERR_CMD_GETS,
				ERR_CMD_OPT_UNDEF,
				ERR_CMD_OPT_BAD_NAME,
				ERR_CMD_OPT_DUPLIC,
				ERR_CMD_ARG_UNDEF,
				ERR_CMD_ARG_BAD_NAME,
				ERR_CMD_ARG_DUPLIC,
				ERR_CMD_ARG_OBLIG,

				/* Module CFG */
				ERR_CFG_FILE_OPEN	= 0x0241,
				ERR_CFG_LINE_FORMAT,
				ERR_CFG_READ_FILE,
				ERR_CFG_BLOC_NOT_FOUND,
				ERR_CFG_KEY_NOT_FOUND

}  eERR_UTI;

/*
 * Macro
 */
#define ERR_UTI_RET(cr)	if( cr != ERR_CR_OK ) { return( cr ); }

/* ---------------------------------------------
 * Declaration des Types et Structures exportees
 * ---------------------------------------------
 */

/* ----------------------------------
 * Prototypes des Fonctions exportees
 * ----------------------------------
 */

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------------
 * Fonction	: ERRUTI_ObtenirMsgErreur
 *
 * Role		:
 *	Retourne le message d'erreur standard lie au code d'erreur fourni
 *
 * Retour	:
 *	- le message d'erreur
 * -----------------------------------------------------------------------------
 */
DLL_FCT_H( const X_CHAR * ) ERRUTI_ObtenirMsgErreur(
	const eERR_UTI	unCodeErreur);	/* Code d'erreur */

#ifdef __cplusplus
}
#endif

#endif	/* _ERR_UTI_H_ */
