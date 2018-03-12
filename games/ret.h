#ifndef _RET_H_
#define _RET_H_

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
 * Codes de retour vers le systeme d'exploitation.
 *
 * $History: RET.H $
 * 
 * *****************  Version 1  *****************
 * User: Fr001        Date: 12/08/98   Time: 11:29
 * Created in $/include
 * 
 * *****************  Version 1  *****************
 * User: Fr001        Date: 12/08/98   Time: 10:43
 * Created in $/FabricationModeles/include
 * Mise sous gestion de conf pour la livraison finale
 * 
 * *****************  Version 5  *****************
 * User: Fr001        Date: 11/08/98   Time: 16:49
 * Updated in $/FabricationModeles/libuti/source
 * Archivage pour livraison finale
 *
 * *****************  Version 4  *****************
 * User: Fr001        Date: 19/06/98   Time: 10:54
 * Updated in $/FabricationModeles/libuti/source
 * Archivage pour la livraison intermediaire du 19/06/98
 *
 * *****************  Version 2  *****************
 * User: Fr001        Date: 7/04/98    Time: 11:07
 * Updated in $/FabricationModeles/libuti/source
 *
 * Historique:
 * 07-Mar-1997  J. Monne    x_compil.h -> x_defaut.h
 * 02-Jun-1995  J. Monne    Rajout pour l'option X_TMSC30
 * 06-Mar-1994  J. Monne    Compatibilite x_compil.h
 * 27-Jan-1993  D. Jouvet   Ajout de la mention de copyright.
 * 20-Mar-1992  J. Monne    Rajout de la deifinition de False et True
 * 20-Fev-1992  D. Jouvet   Adaptation pour Unix.
 * 24-Jan-1992  D. Jouvet   Adaptation pour le compilateur ZORTECH.
 * 18-Sep-1989  D. Jouvet   Creation du fichier, d'apres nombreux exemples.
 * -----------------------------------------------------------------------------
 */

/* ----------------------
 * Directives d'inclusion
 * ----------------------
 */

#include "x_defaut.h"			/* Fcts utilitaires */

/* ------------------------------------
 * Declaration des Constantes exportees
 * ------------------------------------
 */

/*
 * Valeurs de retour vers le systeme d'exploitation.
 */
#if defined(X_PCDOS) || defined(X_UNIX) || defined(X_TMSC30) || \
    defined(X_OS2)
#define NORMAL  0
#define ERROR   4
#define X_ERROR   4
#else
#error	Environnement (PCDOS ou UNIX) non defini
#endif

/* ---------------------------------------------
 * Declaration des Types et Structures exportees
 * ---------------------------------------------
 */

typedef enum    { False , True }  Boolean;


#endif	/* _RET_H_ */
