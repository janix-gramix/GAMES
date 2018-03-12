#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "heap.h"

#define SORT_PIECE

#define NB_PIECES	256
#define NB_CASES	NB_PIECES
#define NB_BORDER	NB_CASES

#define	NB_COTES	4
#define NB_ROTATIONS	NB_COTES

#define	SIZE_ECHIQ	16

#define NBDIF_COTE	24

// type pour les differents motifs de bord
typedef u_int8_t T_COTE   ; // avec les cas particuliers : 0 = NULL =joker ; 1 = bord ; >1 motif different de bord
// 4 cote regroupe
typedef u_int32_t T_4COTE   ;

// numero de case, signe pour autoriser le -1 (pas de case)
#define T_CASE  int16_t
// numero de piece, signe pour autoriser le -1 (pas de piece)
#define T_PIECE int16_t
// numero de piece avec rotation (code rot*
#define T_PIECE_ROTATE int16_t
// rang pour classement case ou piee
#define T_RANG  int16_t
// nombre de candidat (piece pour case)
#define T_NB_CAND   int16_t
// MAX (aucune pcase vide ne peut avoir ce nombre de candidat)
#define MAX_CAND    NB_PIECES
// nombre de couples (Rotation,Piece)
#define NB_PIECES_ROTATE         (NB_ROTATIONS*NB_PIECES)
// indice joker pour marquer NULL piece ec rotation
#define IND_NULL_PIECE_ROTATE    NB_PIECES_ROTATE


// contrainte sur 4 cotes : pour chaque cote
// 0 = no contrainte
// 1 = bord
// >1 = cote non bord
typedef union T_COTE_CONSTRAINT {
	T_4COTE cote4 ;
	T_COTE  cote[NB_COTES];
} T_COTE_CONSTRAINT ;

// decrit un tableau partiellement rempli
// contient les contraintes sur chaque case
// contient le contenu de chaque case
typedef struct GLB_POSITION	GLB_POSITION ;
struct GLB_POSITION {
	T_COTE_CONSTRAINT caseConstraint[NB_CASES] ;
	T_PIECE_ROTATE echiqPieceM1 ;
	T_PIECE_ROTATE echiqPiece[NB_CASES]  ;
} ;


typedef struct FREEPIECE FREEPIECE ;
struct FREEPIECE {
	int nbFree ;
	int freePiece[NB_PIECES] ;
	int piece2Free[NB_PIECES] ;
} ;



// permet de classer chaque case libre par son nombre de pieces possibles
typedef struct CAND_CASE CAND_CASE ;
struct CAND_CASE {
	T_CASE iCase ;          // le numero de la case
	T_NB_CAND nbCand ;      // la taille de la liste ds candidats (MAX_CAND == case prise)
} ;

// contient les infos associees aux cases libres (liste des pieces candidates)
// classee par ordre croissant du nombre de candidats
typedef struct FREE_CASES FREE_CASES ;
struct FREE_CASES {
	int nbFreeCase ;                        // nombre de case libres
	CAND_CASE	sortCandCase[NB_BORDER] ;   // case libres triees par croissance du nbre de candidat
	T_RANG rangForFreeCase[NB_CASES] ;         // en gros la transformation inverse
	T_NB_CAND saveNbCand[NB_CASES] ;
    
	T_PIECE listCandForCase[NB_CASES*NB_PIECES] ;
#if defined(SORT_PIECE)
    int nbFreeCaseByPiece[NB_PIECES] ;
#endif
    
} ;

// Voisinage
//   2
// 3   1
//   0
typedef struct Description Description ;
// element constant decrivant les relations entre les elements
struct Description {
    T_COTE_CONSTRAINT coteConstraintsM1 ;
    T_COTE_CONSTRAINT coteConstraints[IND_NULL_PIECE_ROTATE+1] ;	// valeur de chaque cote pour chaque piece avec rotation
    // on rajoute au bout un joker piece non definie
	T_CASE voisinCase[NB_CASES*NB_ROTATIONS] ;			// nuemro des 4 cases voisines d'une case, -1 si hors echiquier
	int hist1DifCote[NBDIF_COTE] ;                      // histogrammes des valeurs des cotes (inusite)
	int hist2DifCote[NBDIF_COTE*NBDIF_COTE] ;           // histogrammes des couple de valeurs des cotes avec une rotation (inusite)
} ;



#define ligCase(j)	((j)/16)
#define colCase(j)	((j)&15)
#define caseLigCol(i,j)		((i)*16+(j))


#define numPiece139	138
#define rotPiece139	2
#define lig139		8
#define	col139		7
#define ir139		0

typedef struct piece {
	int num ;
	char name[4] ;
	int	x4 ;
} piece ;


/*
   2
 3   1
   0
 */
static piece tbPiece[NB_PIECES] = {
{1,"AVWA",0}, {2,"AUWA",0}, {3,"AVSA",0}, {4,"ASVA",0}, {5,"AWOW",0}, {6,"ASLW",0}, {7,"AWBW",0}, {8,"ATBW",0}, {9,"AVJW",0}, {10,"AURW",0},
{11,"ASEW",0}, {12,"AUNW",0}, {13,"ATNW",0}, {14,"AUPW",0}, {15,"AWLS",0}, {16,"AVGS",0}, {17,"ATMS",0}, {18,"ATHS",0}, {19,"ASRS",0}, {20,"ASES",0},
{21,"AUKS",0}, {22,"AWQS",0}, {23,"ATQS",0}, {24,"AWIS",0}, {25,"AWPS",0}, {26,"ASOV",0}, {27,"AVOV",0}, {28,"AVLV",0}, {29,"AVGV",0}, {30,"ATRV",0},
{31,"ASEV",0}, {32,"AVIV",0}, {33,"ASNV",0}, {34,"AUNV",0}, {35,"ATFV",0}, {36,"AUCV",0}, {37,"AWGU",0}, {38,"ATDU",0}, {39,"ATJU",0}, {40,"ASHU",0},
{41,"AVHU",0}, {42,"AWEU",0}, {43,"ASEU",0}, {44,"AVEU",0}, {45,"AWKU",0}, {46,"AUIU",0}, {47,"AUNU",0}, {48,"AUFU",0}, {49,"ATOT",0}, {50,"AWLT",0},
{51,"ASLT",0}, {52,"AWBT",0}, {53,"AURT",0}, {54,"AUKT",0}, {55,"ATKT",0}, {56,"AVNT",0}, {57,"AWFT",0}, {58,"ATFT",0}, {59,"ASPT",0}, {60,"AVCT",0},
{61,"OGBO",0}, {62,"ORMO",0}, {63,"ODLL",0}, {64,"ONOG",0}, {65,"OCGG",0}, {66,"OMMG",0}, {67,"OLJG",0}, {68,"OBIG",0}, {69,"ONCG",0}, {70,"ORDD",0},
{71,"OQRD",0}, {72,"OGMJ",0}, {73,"OKEJ",0}, {74,"OEIJ",0}, {75,"ODNJ",0}, {76,"OEMH",0}, {77,"OEHH",0}, {78,"OFDR",0}, {79,"ODIR",0}, {80,"OPFR",0},
{81,"OGHE",0}, {82,"OGGK",0}, {83,"OKJK",0}, {84,"OHGQ",0}, {85,"OMBQ",0}, {86,"OQNQ",0}, {87,"OIFQ",0}, {88,"OPOI",0}, {89,"OCBI",0}, {90,"OFKI",0},
{91,"OQJN",0}, {92,"OEHN",0}, {93,"OKHN",0}, {94,"OPKN",0}, {95,"OMQN",0}, {96,"ODPP",0}, {97,"OHKC",0}, {98,"ONIC",0}, {99,"OBPC",0}, {100,"OPCC",0},
{101,"LEQL",0}, {102,"LFQL",0}, {103,"LHFL",0}, {104,"LBCL",0}, {105,"LEKG",0}, {106,"LNDB",0}, {107,"LNHB",0}, {108,"LEKB",0}, {109,"LJFB",0}, {110,"LQEM",0},
{111,"LEQM",0}, {112,"LHID",0}, {113,"LFID",0}, {114,"LKMJ",0}, {115,"LQRJ",0}, {116,"LJQJ",0}, {117,"LFCJ",0}, {118,"LPDH",0}, {119,"LQFR",0}, {120,"LCNE",0},
{121,"LCMK",0}, {122,"LFBI",0}, {123,"LHMI",0}, {124,"LEII",0}, {125,"LCQN",0}, {126,"LEPN",0}, {127,"LBMF",0}, {128,"LPHF",0}, {129,"LDKF",0}, {130,"LNIF",0},
{131,"LIBP",0}, {132,"LMQP",0}, {133,"LFMC",0}, {134,"LKJC",0}, {135,"LDKC",0}, {136,"LIFC",0}, {137,"GRIG",0}, {138,"GDBB",0}, {139,"GJBB",0}, {140,"GQBB",0},
{141,"GPHB",0}, {142,"GBEB",0}, {143,"GQFB",0}, {144,"GPPB",0}, {145,"GKDM",0}, {146,"GQGD",0}, {147,"GCGD",0}, {148,"GMDD",0}, {149,"GQED",0}, {150,"GJBH",0},
{151,"GCKH",0}, {152,"GJJR",0}, {153,"GHJR",0}, {154,"GFCR",0}, {155,"GRRK",0}, {156,"GQRK",0}, {157,"GRCK",0}, {158,"GFRI",0}, {159,"GBNI",0}, {160,"GPPN",0},
{161,"GIBF",0}, {162,"GIMF",0}, {163,"GJEC",0}, {164,"GFKC",0}, {165,"BKDM",0}, {166,"BNKM",0}, {167,"BDDJ",0}, {168,"BEJH",0}, {169,"BPHH",0}, {170,"BNKR",0},
{171,"BFIR",0}, {172,"BJPR",0}, {173,"BEEE",0}, {174,"BIQE",0}, {175,"BPRK",0}, {176,"BHRQ",0}, {177,"BKMI",0}, {178,"BFQN",0}, {179,"BENN",0}, {180,"BFRF",0},
{181,"BFJP",0}, {182,"BJRP",0}, {183,"MNHM",0}, {184,"MRCD",0}, {185,"MQHJ",0}, {186,"MNNJ",0}, {187,"MRPH",0}, {188,"MPMR",0}, {189,"MHDR",0}, {190,"MHFR",0},
{191,"MDDE",0}, {192,"MIEE",0}, {193,"MRJK",0}, {194,"MJPQ",0}, {195,"MCPQ",0}, {196,"MECQ",0}, {197,"MCJI",0}, {198,"MNHI",0}, {199,"MIII",0}, {200,"MDHN",0},
{201,"MNPF",0}, {202,"MRCF",0}, {203,"MHQP",0}, {204,"MNQP",0}, {205,"MDFP",0}, {206,"MPCP",0}, {207,"DRCD",0}, {208,"DCHJ",0}, {209,"DENJ",0}, {210,"DHJR",0},
{211,"DEFR",0}, {212,"DFDE",0}, {213,"DNNK",0}, {214,"DIDQ",0}, {215,"DCKQ",0}, {216,"DPCQ",0}, {217,"DEHI",0}, {218,"DQKF",0}, {219,"DKJP",0}, {220,"DFJC",0},
{221,"DPPC",0}, {222,"DCPC",0}, {223,"JCQJ",0}, {224,"JRIJ",0}, {225,"JEFJ",0}, {226,"JEHR",0}, {227,"JQQR",0}, {228,"JNHK",0}, {229,"JCRI",0}, {230,"JNFI",0},
{231,"JIQN",0}, {232,"JPQN",0}, {233,"HIHH",0}, {234,"HKFR",0}, {235,"HKRK",0}, {236,"HIRK",0}, {237,"HEQK",0}, {238,"HFKQ",0}, {239,"HCEI",0}, {240,"HEPI",0},
{241,"HPRN",0}, {242,"HPKN",0}, {243,"RQEE",0}, {244,"RNIE",0}, {245,"RICK",0}, {246,"RCFP",0}, {247,"ECPE",0}, {248,"EKQK",0}, {249,"EPKQ",0}, {250,"EPFI",0},
{251,"KNCK",0}, {252,"KQNQ",0}, {253,"QIFN",0}, {254,"IFPF",0}, {255,"ICFC",0}, {256,"NCPC",0}
} ;


FILE *fhist ;

typedef struct START START ;
struct START {
	int nbCaseOnEchiq ;				// nombre de case placees (avec rotation)
	int ordMax;						// ordre max impose
	T_PIECE_ROTATE echiq[NB_CASES] ;
	T_CASE ord_rg2Case[NB_PIECES] ;
} ;


typedef struct CONTEXT CONTEXT ;

typedef struct RANGHISTO	RANGHISTO ;

struct RANGHISTO {
	int rangMax ;
	int rangMax1 ;
	int rangMin;
	int64_t exp ;
	int64_t expCum ;
	int nbFindOrdMax ;

} ;
typedef struct RANG	RANG ;
struct RANG {
	int rang ;
	int lastDeltaRang ;
	int rg2case[NB_PIECES] ;	// la case pour le rg concerne
	int rg2cand[NB_PIECES] ;	// le candidat pour le rang : cand= ind_ds_listePiece + 256 * rotation

	// variable lie a l'historique du parcours
	RANGHISTO rhist ;
} ;

#define	INDfromCAND(cand)		((cand) & 255 )
#define	ROTfromCAND(cand)		((cand) / 256 )
#define CANDfromIND_ROT(ind,rot)	( (rot) * 256 + (ind) )

#define PIECEfromPIEROT(iPieceRot)		((iPieceRot) & 255)
#define ROTfromPIEROT(iPieceRot)		((iPieceRot) /256 )
#define PIEROTfromPIECE_ROT(piece,rot)	( (rot) * 256 + (piece) )



struct CONTEXT {
	RANG            rg ;
	GLB_POSITION    cstr ;
	FREEPIECE       frP ;
	FREE_CASES      frC ;
} ;

typedef struct CONF CONF ;
struct CONF {
	char nameStart[300] ;
	char nameHist[300] ;
	int  ordMax ;
	int  rgOk ;
	int  findOrd ;
#if defined(SORT_PIECE)
	int  isSort ;
#endif
	int  isDebug ;
    int  isno139 ;
	int  reverse ;
	int64_t	exp0 ;
	int choixP ;
    int deepFirst ;
    int initP ;
} ;

typedef struct opt opt ;
struct opt {
	char *name ;
	char *format ;
	char *defaultValue ;
	void *var ;
} ;
static CONF conf  ;


static void ReadStart(START *st,const char *startName) ;
static void Precomp (Description *c,int isDebug) ;
static int  InitFromStart(const START *st,CONTEXT *ctx,int isDebug) ;


static START st ;
static Description gDesc ;
static CONTEXT ctx ;


void Precomp_ELfromPieces(void) ;
void Precomp_Histo(void) ;
void Init_CasexPiece(CONTEXT *ctx,int isDebug);

int32_t	cmpNbPiece (const void *el1,const void *el2) {
	return *((int32_t *)el2) - *((int32_t *)el1) ;
}



typedef char * (FORMAT)(void *ctx,int i,const char * format);

char * Format_Ident(void *ctx,int i,const char * format) {
	static char tmp[20] ;
	sprintf(tmp,format,i,' ') ;
	return tmp ;
}
char * Format_Piece(void *ctx,int i,const char * format) {
	static char tmp[20] ;
	int ip = ( i == -1) ? 0 : ((i&255)+1) ;
	char ic = 'a' + ( ( i == -1) ? 0 : (i/256 ) ) ;
	sprintf(tmp,format,ip,ic);
	return tmp;
}

char * Format_StatusBorder(void *ctx,int i, const char * format) {
		static char tmp[20] ;
		FREE_CASES *frc = (FREE_CASES *) ctx ;
		int ip ;

        if(i <= -1) {
			ip = (i==-1) ? -1 : 256 ;
		} else {
			ip = frc->sortCandCase[i].nbCand ;
		}
	sprintf(tmp,format,ip);
	return tmp;

}

static void Print_EchiqPiece(int16_t *val,const char *comment) {
	int i,j;
    char ligne_up[4*SIZE_ECHIQ+2] ;
    char ligne_bot[4*SIZE_ECHIQ+2] ;
    char ligne[4*SIZE_ECHIQ+2] ;
	fprintf(stdout,"\n%s\n",comment);
	fprintf(fhist,"\n%s\n",comment);
    memset(ligne_up,' ',4*SIZE_ECHIQ+2);
    ligne_up[4*SIZE_ECHIQ+1] = 0 ;
    memset(ligne_bot,' ',4*SIZE_ECHIQ+2);
    ligne_bot[4*SIZE_ECHIQ+1] = 0 ;
    ligne_bot[4*SIZE_ECHIQ] = '+' ;
 
    fprintf(stdout,"   ");
    fprintf(fhist,"   ");
    
    for(j=0;j<SIZE_ECHIQ;j++) {
        ligne_bot[4*j+2] = 'A' ;
        ligne_bot[4*j] = '+' ;
        fprintf(stdout,"  %x ",j);
        fprintf(fhist,"  %x ",j);
    }
    fprintf(stdout,"\n\n");
    fprintf(fhist,"\n\n");

    memset(ligne,' ',4*SIZE_ECHIQ+2);
    ligne[4*SIZE_ECHIQ+1] = 0 ;
    for(i=0;i<SIZE_ECHIQ;i++) {
        memcpy(ligne_up,ligne_bot,4*SIZE_ECHIQ+1) ;
        ligne[0] ='A' ;
        for(j=0;j<SIZE_ECHIQ;j++) {
            if(val[i*SIZE_ECHIQ+j] == -1) {
                ligne_bot[4*j+2] = '-' ;
                ligne[4*j+2] = '@' ;
                ligne[4*j+4]= '|' ;
            } else {
                piece pc = tbPiece[val[i*SIZE_ECHIQ+j] & 255] ;
                int ir = val[i*SIZE_ECHIQ+j] / 256 ;
                ligne_up[4*j+2] = pc.name[(2+ir) & 3 ] ;
                ligne[4*j] = pc.name[(3+ir) & 3 ] ;
                ligne[4*j+4] = pc.name[(1+ir) & 3 ] ;
                ligne[4*j+2] = ' ' ;
                ligne_bot[4*j+2] = pc.name[(+ir) & 3 ] ;
            }
            ligne[4*SIZE_ECHIQ]= 'A' ;
        }
        fprintf(stdout,"   %s\n",ligne_up);
        fprintf(fhist,"   %s\n",ligne_up);
        fprintf(stdout,"%x  %s\n",i,ligne);
        fprintf(fhist,"%x  %s\n",i,ligne);
    }
    for(j=0;j<SIZE_ECHIQ;j++) { ligne_bot[4*j+2] = 'A' ; }
    fprintf(stdout,"   %s\n",ligne_bot);
    fprintf(fhist,"   %s\n",ligne_bot);
	fflush(fhist);
}


static void Print_Echiq16(int16_t *val,void *ctx,FORMAT *trans,const char *format,const char *comment) {
	int i;
	fprintf(stdout,"\n%s\n",comment);
	fprintf(fhist,"\n%s\n",comment);
	for(i=0;i<NB_CASES;i++) {
		fprintf(stdout,"%s",trans(ctx,val[i],format));
		fprintf(fhist,"%s",trans(ctx,val[i],format));
		fprintf(stdout,"%c",(colCase(i)==15)?'\n' : ' ');
		fprintf(fhist,"%c",(colCase(i)==15)?'\n' : ' ');
	}
	fflush(fhist);
}

__inline static void RangIncrease(RANG *rg,int indCand, int rotCand) {
	int nxtCand = (rotCand  < NB_ROTATIONS-1 ) ? CANDfromIND_ROT(indCand,rotCand+1) : CANDfromIND_ROT(indCand+1,0)  ;
	rg->rg2cand[rg->rang] = nxtCand ; // mise a jour new cand pour la prochaine exploration (rotation++, ou piece suivante)
	rg->lastDeltaRang = 1 ;
	rg->rang++ ;
}

__inline static void RangNewCase(RANG *rg,int iCase) {
	rg->rg2case[rg->rang] = iCase ;
	rg->rg2cand[rg->rang] = 0 ;
}

__inline static void RangDecrease(RANG *rg) {
	rg->rg2case[rg->rang] = -1 ;
	rg->lastDeltaRang = -1 ;
	rg->rang-- ;
}

__inline static int RangGetCase(RANG *rg) {
	return rg->rg2case[rg->rang]  ;
}

// calcule et inscrit pour la case iCase les contraintes dues aux voisins deja places (dans glbP)
// retourne la contrainte
 __inline static  T_COTE_CONSTRAINT  SetContraintesForCase(GLB_POSITION *glbP, int iCase) {
	T_COTE_CONSTRAINT cst ;
	if(iCase == caseLigCol(lig139,col139) && (conf.isno139 == 0) ) {
		cst.cote4 =  gDesc.coteConstraints[numPiece139 + NB_PIECES * rotPiece139].cote4 ;
	} else {
		int i;
		for(i=0;i<4;i++) {
			cst.cote[i] = gDesc.coteConstraints[glbP->echiqPiece[gDesc.voisinCase[4*iCase+i]]].cote[i^2]  ;
		}
	}
	glbP->caseConstraint[iCase] = cst ;
	return cst ;
}
// retourne la contrainte existant pour une case
__inline  static T_COTE_CONSTRAINT GetContraintesForCase(GLB_POSITION *cstr,int iCase) {
	return cstr->caseConstraint[iCase] ;
}



static int FrpFindPiece(FREEPIECE *frP,int iPiece) {
	int i ;
	for(i=0;i<frP->nbFree;i++) {
		if(frP->freePiece[i] == iPiece) {
			return i ;
		}
	}
	return -1 ;
}
static void FrpRemovePiece(FREEPIECE *frP,int iPiece) {
/*	int i ;
	for(i=frP->piece2Free[iPiece]+1;i<frP->nbFree;i++) {
		frP->freePiece[i-1] = frP->freePiece[i] ;
		frP->piece2Free[frP->freePiece[i]] = i-1 ;	
	}
	frP->nbFree-- ;
*/
	int indF = frP->piece2Free[iPiece] ;
	frP->nbFree-- ;
	if(indF < frP->nbFree) {
		int ip = frP->freePiece[frP->nbFree] ;
		frP->freePiece[indF] = ip ;
		frP->piece2Free[ip] = indF ;
	}

}

#if defined(SORT_PIECE)
/*
int CmpBorderX(const void *el1,const void *el2) {
	int i =  ((BORD_CASE *)el1)->sizeList - ((BORD_CASE *)el2)->sizeList ;
	if(i!= 0) return i ;
	return ((BORD_CASE *)el1)->cumPieceBorder - ((BORD_CASE *)el2)->cumPieceBorder ;
}
*/
#endif

int CmpCandCase(const void *el1,const void *el2) {
    int diff = ((CAND_CASE *)el1)->nbCand - ((CAND_CASE *)el2)->nbCand  ;
    if(diff !=0) return diff ;
    else return ((CAND_CASE *)el1)->iCase - ((CAND_CASE *)el2)->iCase ;
}

// cherche si iPieceRotate convient
static int ChkContraintesPieceRotate(T_COTE_CONSTRAINT cst,int iPieceRotate) {
    int ir =  iPieceRotate >> 8 ;
    int iPiece = iPieceRotate & 255 ;
		{
			T_4COTE cv = gDesc.coteConstraints[ir*NB_PIECES+iPiece].cote4 ;
			T_4COTE xov = cst.cote4 ^ cv ; //egalite car cv n'est jamis nul
			T_4COTE yov = (((0x21212121 -cv ) & 0x20202020) >> 5) | cst.cote4  ; // cst.v == 0 && cv != 1 (pour chaque octet)
			T_4COTE tst = ((0x1f1f1f1f + xov) & (0x1f1f1f1f + yov)) & 0x20202020; // xov == 0 ou yov == 0 pour chaque octet
			if(tst==0) return ir ;
		}
	return -1 ;
}

// cherche une rotation superieure a ir, qui satisfait les contraintes pout une case cst, pour une piece iPiece
static int ChkContraintesIr(T_COTE_CONSTRAINT cst,int iPiece,int ir) {
	for(;ir<NB_ROTATIONS;ir++) {
		{
			T_4COTE cv = gDesc.coteConstraints[ir*NB_PIECES+iPiece].cote4 ;
			T_4COTE xov = cst.cote4 ^ cv ; //egalite car cv n'est jamis nul
			T_4COTE yov = (((0x21212121 -cv ) & 0x20202020) >> 5) | cst.cote4  ; // cst.v == 0 && cv != 1 (pour chaque octet)
			T_4COTE tst = ((0x1f1f1f1f + xov) & (0x1f1f1f1f + yov)) & 0x20202020; // xov == 0 ou yov == 0 pour chaque octet
			if(tst==0) return ir ;
		}

	}
	return -1 ;
}
static int ChkContraintes(T_COTE_CONSTRAINT cst,int iPiece) {
	return ChkContraintesIr(cst,iPiece,0) ;
}


void Init_CasexPiece(CONTEXT *ctx,int isDebug){
	int i,j ;
    ctx->cstr.echiqPieceM1 = IND_NULL_PIECE_ROTATE ;
	memset(ctx->cstr.echiqPiece,-1,sizeof(ctx->cstr.echiqPiece[0])*NB_CASES);
	memset(ctx->rg.rg2case,-1,sizeof(ctx->rg.rg2case[0])*NB_CASES);
#if defined(SORT_PIECE)
	memset(ctx->frC.nbFreeCaseByPiece,0,sizeof(ctx->frC.nbFreeCaseByPiece[0])*NB_PIECES);
#endif
	for(i=0;i<NB_PIECES;i++) {
		if((i == numPiece139) && (conf.isno139 == 0)) {

		} else {
			int nbBord ;
			for(nbBord=0,j=0;j<NB_COTES;j++) {
				if(gDesc.coteConstraints[i].cote[j]==1)nbBord++ ;
			}
		}
	}
	ctx->frC.nbFreeCase = 0 ;
	for(i=0;i<NB_CASES;i++) {
		T_COTE_CONSTRAINT cst = SetContraintesForCase(&ctx->cstr,i);
		if(cst.cote4 != 0 ) { // il y a une contrainte, donc un bord
			CAND_CASE * candCase = ctx->frC.sortCandCase + ctx->frC.nbFreeCase++ ;
			candCase->iCase = i ;
			candCase->nbCand = 0 ;
			{
				int j ;
				for(j=0;j<NB_PIECES;j++) {
					if(ChkContraintes(cst,j) >= 0) {
						ctx->frC.listCandForCase[i*NB_PIECES+candCase->nbCand++] = j ;
#if defined(SORT_PIECE)
						ctx->frC.nbFreeCaseByPiece[j]++ ;
#endif
					}
				}
			}
		}
	}
	for(i=0;i<NB_CASES;i++) {
		ctx->frC.rangForFreeCase[i] = -1; // libre
	}
	qsort(ctx->frC.sortCandCase,ctx->frC.nbFreeCase,sizeof(ctx->frC.sortCandCase[0]),CmpCandCase);
	for(i=0;i<ctx->frC.nbFreeCase;i++) {
		ctx->frC.rangForFreeCase[ctx->frC.sortCandCase[i].iCase] = i ;
	}
	if(isDebug) Print_Echiq16(ctx->frC.rangForFreeCase,&ctx->frC,Format_StatusBorder,"%3d","Status Border");

	ctx->frP.nbFree = 0 ;
	for(i=0;i<NB_PIECES;i++) {
		ctx->frP.piece2Free[i] = ctx->frP.nbFree ;
		ctx->frP.freePiece[ctx->frP.nbFree++] = i ;
	}
}


START * ReverseOrder(int nbCaseOnEchiq,int pivot0, const int rg2case[NB_PIECES],const T_PIECE echiq[NB_CASES],START *str) {
	int i ;
	GLB_POSITION glbP ;
	// on initialise une struct ctr pour calculer les bordure
	glbP.echiqPieceM1 = IND_NULL_PIECE_ROTATE ;
	memset(glbP.echiqPiece,-1,sizeof(glbP.echiqPiece[0])*NB_CASES);
	for(i=0;i<NB_CASES;i++) {
		SetContraintesForCase(&glbP,i);
	}
	memset(str,0,sizeof(START)) ;
	{
		int pivot ;
		for(pivot=0;str->nbCaseOnEchiq < nbCaseOnEchiq; ) {
			int j ;
			for(j=pivot;j<nbCaseOnEchiq;j++) {
				T_CASE iCase = rg2case[j] ;
				if(glbP.echiqPiece[iCase] == -1 && GetContraintesForCase(&glbP,iCase).cote4 != 0) {
					// on a trouve une bordure et une case  vide
					glbP.echiqPiece[iCase] = echiq[iCase] ;
					SetContraintesForCase(&glbP,iCase);
					str->ord_rg2Case[str->nbCaseOnEchiq++] = iCase ;
					for(i=0;i<4;i++) {
						if(gDesc.voisinCase[4*iCase+i] >= 0) {
							SetContraintesForCase(&glbP,gDesc.voisinCase[4*iCase+i]);
						}
					}
//					if(str->nbCaseOnEchiq==1)              {
						pivot = pivot0 ;
//					} else
                    {
						while(glbP.echiqPiece[pivot] != -1) {
							pivot++ ; 
						}
					}
					break ;
				}
			}
			if(j==nbCaseOnEchiq) {
				pivot = 1 ;
			}
		}
	}
	memcpy(str->echiq,glbP.echiqPiece,NB_CASES * sizeof(str->echiq[0])) ;
	str->ordMax = str->nbCaseOnEchiq -pivot0 ;
	return str ;
}

//   2
// 3   1
//   0

static void AddCaseToFree(FREE_CASES *frC,int iCase) {

	// on rajoute a la liste des bordures
	frC->rangForFreeCase[iCase] = frC->nbFreeCase;
	{
		CAND_CASE * candCase = frC->sortCandCase + frC->nbFreeCase++ ;
		candCase->iCase = iCase ;
		candCase->nbCand = frC->saveNbCand[iCase] ;
	// on ne recree pas la liste qui existait deja (a faire si on autorise les remplacement en desordre)

#if defined(SORT_PIECE)
		{
			int i ;
			T_PIECE * listePiece = frC->listCandForCase + iCase * NB_PIECES ;
			for(i=0;i<candCase->nbCand;i++) {
				frC->nbFreeCaseByPiece[listePiece[i]]++ ;
			}
		}

#endif
	}
}

static void RemovePieceInCase(CONTEXT *ctx,int iCase/*,int rang*/) {
	int iPiece = ctx->cstr.echiqPiece[iCase] & 255 ;
//	int il = ligCase(iCase) ;
//	int ic = colCase(iCase) ;
	ctx->cstr.echiqPiece[iCase] = -1 ;
//	nbPieceByCase[iCase] -= 256 ;
	

	AddCaseToFree(&ctx->frC,iCase) ;
	{
		int i;
		for(i=0;i<4;i++) {
			if(gDesc.voisinCase[4*iCase+i] >= 0) {
				T_CASE icas1 = gDesc.voisinCase[4*iCase+i] ;
				T_COTE_CONSTRAINT cst =  SetContraintesForCase(&ctx->cstr,icas1);
				int rg = ctx->frC.rangForFreeCase[icas1]  ;
				if(rg >= 0) {
					CAND_CASE * candCase = ctx->frC.sortCandCase + rg ;
					T_PIECE * listePiece = ctx->frC.listCandForCase + icas1 * NB_PIECES ;
#if defined(SORT_PIECE)
					int k ;
					for(k=0;k<candCase->nbCand;k++) {
						ctx->frC.nbFreeCaseByPiece[listePiece[k]]-- ;
					}
#endif
					if(cst.cote4 != 0 ) { // on reconstruit la liste des possible
						int k ;
						int nbP = 0 ;			
						for(k=0;k<ctx->frP.nbFree;k++) {
							int ip = ctx->frP.freePiece[k] ;
							if(ChkContraintes(cst,ip) >= 0) {
								listePiece[nbP++] = ip ;
#if defined(SORT_PIECE)
								ctx->frC.nbFreeCaseByPiece[ip]++ ;
#endif
							}
						}
						candCase->nbCand = nbP ;
					} else {
						// on l'enleve de la liste des contraints
						ctx->frC.rangForFreeCase[icas1] = -1 ;
						candCase->nbCand = MAX_CAND ;
					}
				}
			}
		}
	}
	{
		int j ;
		for(j=0;j<ctx->frC.nbFreeCase;j++) {
			CAND_CASE * candCase = ctx->frC.sortCandCase + j ;
			int ic = candCase->iCase ;
			if(ic != iCase && ChkContraintes(GetContraintesForCase(&ctx->cstr,ic),iPiece) >= 0) {
				if(candCase->nbCand < MAX_CAND)
				{
					ctx->frC.listCandForCase [ic * NB_PIECES + candCase->nbCand++] = iPiece ;
#if defined(SORT_PIECE)
					ctx->frC.nbFreeCaseByPiece[iPiece]++ ;
#endif
				}
			}
		}
	}
	{
		ctx->frP.piece2Free[iPiece] = ctx->frP.nbFree ;
		ctx->frP.freePiece[ctx->frP.nbFree++] = iPiece ;
	}
	qsort(ctx->frC.sortCandCase,ctx->frC.nbFreeCase,sizeof(ctx->frC.sortCandCase[0]),CmpCandCase);
	{
		int i ;
		for(i=0;i<ctx->frC.nbFreeCase;i++) {
			if(ctx->frC.rangForFreeCase[ctx->frC.sortCandCase[i].iCase] == -1) {
				break ;
			}
			ctx->frC.rangForFreeCase[ctx->frC.sortCandCase[i].iCase] = i ;
		}
		ctx->frC.nbFreeCase = i ;
	}
}
static void PlacePieceInCase(CONTEXT *ctx,T_PIECE_ROTATE iPieceRot,int iCase/*,int rang*/) {
	// rajoute la piece
//	int il = ligCase(iCase) ;
//	int ic = colCase(iCase) ;
	ctx->cstr.echiqPiece[iCase] = iPieceRot ;
	FrpRemovePiece(&ctx->frP,iPieceRot & 255);
	{	// on l'enleve de la liste des bordures
		int rg = ctx->frC.rangForFreeCase[iCase] ;
		if(rg >= 0)  {
			int i ;
#if defined(SORT_PIECE)
			{
				int i ;
				T_PIECE * listePiece = ctx->frC.listCandForCase + iCase * NB_PIECES ;
				for(i=0;i<ctx->frC.sortCandCase[rg].nbCand;i++) {
					ctx->frC.nbFreeCaseByPiece[listePiece[i]]-- ;
				}
			}
#endif	
			ctx->frC.saveNbCand[iCase] = ctx->frC.sortCandCase[rg].nbCand	 ;
			for(i=rg+1;i<ctx->frC.nbFreeCase;i++) {
				ctx->frC.rangForFreeCase[ctx->frC.sortCandCase[i].iCase] = (i - 1) ;
				ctx->frC.sortCandCase[i-1] = ctx->frC.sortCandCase[i] ;
			}
			ctx->frC.nbFreeCase-- ;
		} else {
				Print_Echiq16(ctx->frC.rangForFreeCase,&ctx->frC,Format_StatusBorder,"%4d","Status Border");
				Print_Echiq16(ctx->cstr.echiqPiece,NULL,Format_Piece,"%3d%c","Echiq");

			fprintf(stdout," ERROR placement d'une piece non bordure %d un %d \n",iPieceRot,iCase) ;
			exit(0);
		}
		ctx->frC.rangForFreeCase[iCase] = -2 - iPieceRot ;

	}
	{
		int j ;
		for(j=0;j<ctx->frC.nbFreeCase;j++) {
			CAND_CASE *candCase = ctx->frC.sortCandCase+ j  ;
			if(ChkContraintes(GetContraintesForCase(&ctx->cstr,candCase->iCase),iPieceRot & 255) >= 0) {
				int k ;
				T_PIECE * listePiece = ctx->frC.listCandForCase + candCase->iCase * NB_PIECES ;
				for(k=0;k<candCase->nbCand ;k++) {
					if(listePiece[k] == (iPieceRot & 255)) {
						if(k+1 < candCase->nbCand) {
							memmove(listePiece+k,listePiece+k+1,(candCase->nbCand - k - 1)*sizeof(listePiece[0]));
						}
						candCase->nbCand-- ;
#if defined(SORT_PIECE)
						ctx->frC.nbFreeCaseByPiece[iPieceRot & 255]-- ;
#endif
						break ;
					}
				}
			}
		}
	}
	{
		int i ;
		for(i=0;i<4;i++) {
			if(gDesc.voisinCase[4*iCase+i] >= 0) {
				T_CASE icas1 = gDesc.voisinCase[4*iCase+i] ;
				T_COTE_CONSTRAINT cst = SetContraintesForCase(&ctx->cstr,icas1);
				int rg = ctx->frC.rangForFreeCase[icas1] ;
				if(rg >= 0) {
					// on filtre la liste des possible
					CAND_CASE * candCase = ctx->frC.sortCandCase + rg ;
					int k ;
					int nbP = 0 ;
					T_PIECE * listePiece = ctx->frC.listCandForCase + icas1 * NB_PIECES ;
					for(k=0;k<candCase->nbCand;k++) {
						if(ChkContraintes(cst,listePiece[k]) >= 0) {
							listePiece[nbP++] = listePiece[k] ;
						} 
#if defined(SORT_PIECE)
						else {
							ctx->frC.nbFreeCaseByPiece[listePiece[k]]-- ;
						}
#endif
					}
					candCase->nbCand = nbP ;
				} else if (rg == -1) {
					int k ;
					int nbP = 0 ;
					T_PIECE * listePiece = ctx->frC.listCandForCase + icas1 * NB_PIECES ;
					CAND_CASE * candCase =ctx->frC.sortCandCase + ctx->frC.nbFreeCase ;
					ctx->frC.rangForFreeCase[icas1] = ctx->frC.nbFreeCase++  ;
					// on rajoute la case dans les contraints
					candCase->iCase = icas1 ;
					for(k=0;k<ctx->frP.nbFree;k++) {
						int ip = ctx->frP.freePiece[k] ;
						if(ChkContraintes(cst,ip) >= 0) {
							listePiece[nbP++] = ip ;
#if defined(SORT_PIECE)
							ctx->frC.nbFreeCaseByPiece[ip]++ ;
#endif
						}
					}
					candCase->nbCand = nbP ;
				}
			}
		}
	}
	qsort(ctx->frC.sortCandCase,ctx->frC.nbFreeCase,sizeof(ctx->frC.sortCandCase[0]),CmpCandCase);
	{
		int i ;
		for(i=0;i<ctx->frC.nbFreeCase;i++) {
			ctx->frC.rangForFreeCase[ctx->frC.sortCandCase[i].iCase] = i ;
		}
	}
//Print_Echiq(nbFreeCaseByPiece,Format_Ident,"%3d%c","Border after place");

}
int verifHypothese(CONTEXT *ctx,int level) {
	int	VnbPieceINCase[NB_CASES] ;
	int VnbCasebyPiece[NB_PIECES] ;
	int VtbCxP[NB_PIECES*NB_CASES] ;
#if defined(SORT_PIECE)
	int VnbFreeCaseByPiece[NB_PIECES] ;
#endif
	int error =0 ;
	int nbPiece = 0 ;
	int i,j ;
	memset(VtbCxP,0,NB_PIECES*NB_CASES*sizeof(VtbCxP[0])) ;
#if defined(SORT_PIECE)
	memset(VnbFreeCaseByPiece,0,NB_PIECES*sizeof(VnbFreeCaseByPiece[0])) ;
#endif
	for(i=0;i<NB_PIECES;i++) { VnbPieceINCase[i] = 0 ; VnbCasebyPiece[i] = 0 ; }
	for(i=0;i<NB_CASES;i++) {
		if(ctx->cstr.echiqPiece[i] >= 0) {
			VnbPieceINCase[ctx->cstr.echiqPiece[i] & 255] += 256 ;
		}
	}
	for(i=0;i<NB_PIECES;i++) {
		int kFree = FrpFindPiece(&ctx->frP,i) ;
		if(VnbPieceINCase[i] > 0) {
			nbPiece++ ;
			if(VnbPieceINCase[i] > 256) {
				error++ ; fprintf(stdout,"#### ERROR Piece %d placee %d fois \n",i,VnbPieceINCase[i]/256) ;
			}
			if(kFree >= 0) {
				error++ ;	fprintf(stdout,"#### ERROR Piece %d placee mais presente dans la liste free  rg=%d)\n",i,kFree) ;
			}

		} else if(kFree < 0 ) {
			error++ ;	fprintf(stdout,"#### ERROR Piece %d non placee mais absente de la liste free \n",i) ;
		}
	}
	// verif liste free
	if(ctx->frP.nbFree+nbPiece != NB_PIECES) {
		error++ ; fprintf(stdout,"#### taille liste Free %d != %d pieces non placee  \n",ctx->frP.nbFree,NB_PIECES-nbPiece) ;
	} 
	{
		int k ;
		for(k=0;k<ctx->frP.nbFree;k++) {
			if(ctx->frP.piece2Free[ctx->frP.freePiece[k]] != k) {
				error++ ;
				fprintf(stdout,"#### indirection piece2Free incorrecte pour la piece %d -> %d != %d  \n",ctx->frP.freePiece[k],ctx->frP.piece2Free[ctx->frP.freePiece[k]],k) ;
			}
		}
	}
	// verif liste triee
	{
		int k ;
		for(k=0;k<ctx->frC.nbFreeCase;k++) {
			if(ctx->frC.rangForFreeCase[ctx->frC.sortCandCase[k].iCase] != k ) {
				error++ ;
				fprintf(stdout,"#### indirection case->border incorrecte pour la case %d -> %d != %d  \n",ctx->frC.sortCandCase[k].iCase,ctx->frC.rangForFreeCase[ctx->frC.sortCandCase[k].iCase],k) ;
			}

#if defined(SORT_PIECE)
			if( ( k> 0) && 	(CmpCandCase(ctx->frC.sortCandCase+k-1,ctx->frC.sortCandCase+k) > 0) ) {
				error++ ;
				fprintf(stdout,"#### indirection border mal trie rg=%d size[%d]=%d > size[%d]=%d  \n",k-1
					,ctx->frC.sortCandCase[k-1].iCase,ctx->frC.sortCandCase[k-1].nbCand
					,ctx->frC.sortCandCase[k].iCase,ctx->frC.sortCandCase[k].nbCand  ) ;
			}
#else
			if(k> 0 && ctx->frC.sortCandCase[k-1].nbCand > ctx->frC.sortCandCase[k].nbCand) {
				error++ ;
				fprintf(stdout,"#### indirection border mal trie rg=%d size[%d]=%d > size[%d]=%d  \n",k-1
				,ctx->frC.sortCandCase[k-1].iCase,ctx->frC.sortCandCase[k-1].nbCand
				,ctx->frC.sortCandCase[k].iCase,ctx->frC.sortCandCase[k].nbCand  ) ;
			}
#endif

		}

	}


	for(i=0;i<NB_CASES;i++) {
		int nbPos = 0 ;
		T_COTE_CONSTRAINT cst = GetContraintesForCase(&ctx->cstr,i) ;
		if(SetContraintesForCase(&ctx->cstr,i).cote4 != cst.cote4) {
			error++ ;
			fprintf(stdout,"#### ERROR Case %d contrainte %d != %d \n",i,ctx->cstr.caseConstraint[i].cote4,cst.cote4) ;
		}
		{
			for(j=0;j<NB_PIECES;j++) {
				if(VnbPieceINCase[j] == 0 && ChkContraintes(cst,j) >= 0  ) {
					VtbCxP[i*NB_PIECES+j] = 1 ;
					nbPos ++ ;
				}
			}
		}
		if(ctx->cstr.echiqPiece[i] >= 0) {  // piece prise
			if(- ctx->frC.rangForFreeCase[i] - 2  != ctx->cstr.echiqPiece[i]) {
				error++ ;
				fprintf(stdout,"#### occupation de la case %d differente %d != %d  \n",i,- ctx->frC.rangForFreeCase[i]-2,ctx->cstr.echiqPiece[i]) ;
			}
		} else {
			if(cst.cote4 == 0) {	// case libre sans contrainte
				if(ctx->frC.rangForFreeCase[i] != -1) {
					error++ ;
					fprintf(stdout,"#### case %d libre mais status border incorrect %d  \n",i,ctx->frC.rangForFreeCase[i]) ;
				}
			} else {
				int rg = ctx->frC.rangForFreeCase[i] ;
				if(rg < 0 ) {
					error++ ;
					fprintf(stdout,"#### case %d bordure mais status border incorrect %d  \n",i,rg) ;
				}
				if(nbPos != ctx->frC.sortCandCase[rg].nbCand) {
					error++ ;
					fprintf(stdout,"#### case %d bordure mais nb Possibililtes %d != exp %d  \n",i,ctx->frC.sortCandCase[rg].nbCand,nbPos) ;
				} else { // on verifie la liste
					int k ;
					for(k=0;k<nbPos;k++) {
						int ip = ctx->frC.listCandForCase[i*NB_PIECES+k] ;
						if(VtbCxP[i*NB_PIECES+ip] != 1) {
							error++ ;
							fprintf(stdout,"#### case %d bordure piece libre %d  rg(%d)en fait non possible  \n",i,ip,k) ;
						}
#if defined(SORT_PIECE)
						VnbFreeCaseByPiece[ip]++ ;
#endif
					}
				}
			}
		}
	}
#if defined(SORT_PIECE)
	for(i=0;i<NB_PIECES;i++) {
		if(VnbPieceINCase[i] == 0 && VnbFreeCaseByPiece[i] != ctx->frC.nbFreeCaseByPiece[i]) {
			error++ ;
			fprintf(stdout,"#### piece %d appartenant a %d bordures au lieu de %d  \n",i,ctx->frC.nbFreeCaseByPiece[i],VnbFreeCaseByPiece[i]) ;
		}
	}
#endif
	return error ;
}


static opt tbOpt[] = {
	{ "-omax" , "%d", "0", &conf.ordMax } ,
	{ "-rgok" , "%d", "210", &conf.rgOk } ,
	{ "-findord" , "%d", "0", &conf.findOrd } ,
#if defined(SORT_PIECE)
	{ "-sort" , "%d", "1", &conf.isSort } ,
#endif
    { "-no139" , "%d", "0", &conf.isno139 } ,
	{ "-debug" , "%d", "0", &conf.isDebug } ,
	{ "-rev" , "%d", "0", &conf.reverse } ,
	{ "-exp" , "%llx", "0x200000000", &conf.exp0 } ,
    { "-initP" , "%d" , "0" , &conf.initP } ,
	{ "-choixP" , "%d", "0", &conf.choixP } ,
    { "-deepfirst","%d", "0", &conf.deepFirst} ,
	{ "startfile" , NULL, "???", &conf.nameStart } ,
	{ NULL , NULL, NULL, NULL }
} ;
static void PrintUsage(void) ;
static void ParseOption(int argc,char **argv) ;
static void PrintOption(FILE *fhist,int argc,char **argv,CONF *conf);
static FILE * OpenHist(CONF *conf) ;

__inline static int FindCandidateNewRang(void) {
    if(ctx.rg.rang < conf.ordMax) { // ordMax rang en dessous duquel l'ordre de parcours des cases est impose (0 par defaut)
        if(ctx.frC.sortCandCase[0].nbCand > 0) { // Toutes les cases ont un candidat
            T_CASE iCase = st.ord_rg2Case[ctx.rg.rang] ;  // ordre de parcours impose
            CAND_CASE * candCase = ctx.frC.sortCandCase + ctx.frC.rangForFreeCase[iCase]  ;
            if(candCase->nbCand > 0) { // la case a un candidat
                RangNewCase(&ctx.rg,iCase) ; // on avance
            } else {
                return -1 ;
            }
        } else {
            return -1 ;
        }
    } else { //hors parcours impose
        int sizeListeMin ;
        int ib ;
        if(ctx.rg.rang == conf.ordMax) {
            ctx.rg.rhist.nbFindOrdMax++ ;
        }
        if((sizeListeMin = ctx.frC.sortCandCase[0].nbCand) == 0 && ctx.rg.rang < conf.rgOk ) {
            // on ne peut aller au bout (une case au moins a zero, donc on revient)
            return -1 ;
        } else {
            // soit il n'y a pas de cases sans candidats, soit on est au dela de rgOk et met le plus de piece possible qd meme
            int reverseChoix = (ctx.rg.rang <= conf.choixP) ? 1 : 0 ; // choix inverse : la case avec le plus de candidats
            if(reverseChoix) {
                int newCase  = ctx.frC.sortCandCase[ctx.frC.nbFreeCase-1].iCase ;
                RangNewCase(&ctx.rg,newCase);
                if(conf.findOrd) {
                    conf.ordMax = ctx.rg.rang+1 ;
                    st.ord_rg2Case[ctx.rg.rang] = newCase ;
                }
                
            } else {
                int ibMin ;
                if(sizeListeMin > 0) {
                    ibMin = 0;
                } else {
                    for(ibMin=0; ibMin < ctx.frC.nbFreeCase;ibMin++) {
                        sizeListeMin = ctx.frC.sortCandCase[ibMin].nbCand ;
                        if(sizeListeMin > 0) break ;
                    }
                    if(sizeListeMin == 0) { // tout est nul gasp, on ne peut plus placer de piece (impasse)
                        return -1 ;
                   }
                }
                {
                    T_CASE newCase ;
                    if(ctx.rg.rang < conf.deepFirst) {
                        // on va chercher la case pouvant aller le plus loin
                        int i ;
                        int deepLgBest = 255 ;
                        int iCaseBest = -1 ;
                        for(i=0;i<ctx.frC.nbFreeCase;i++) {
                            int deepLg = 0 ;
                            T_CASE caseHeap [NB_CASES ] ;
                            int iCase0 = ctx.frC.sortCandCase[i].iCase ;
                            int iCase = iCase0 ;
                            do {
                                T_COTE_CONSTRAINT cst = GetContraintesForCase(&ctx.cstr,iCase);
                                T_PIECE * pieceCand = ctx.frC.listCandForCase + iCase * NB_PIECES ;
                                int curCand ;
                                int candFind = 0 ;
                                for(curCand=0;INDfromCAND(curCand) < ctx.frC.sortCandCase[0].nbCand;)  {
                                    int ir = ROTfromCAND(curCand) ;
                                    int indCand = INDfromCAND(curCand) ;
                                    ir = ChkContraintesIr(cst,pieceCand[indCand],ir);
                                    if( ir  >= 0) {
                                        T_PIECE_ROTATE iPieceRot = ir * NB_PIECES + pieceCand[indCand];
                                        candFind = 1 ;
                                        PlacePieceInCase(&ctx,iPieceRot ,iCase) ;
                                        break ;
                                    } else {
                                        curCand =  CANDfromIND_ROT(indCand+1,0);
                                    }
                                }
                                if(candFind != 0) {
                                    caseHeap[deepLg++] = iCase ;
                                } else {
                                    break ;
                                }
                                iCase = ctx.frC.sortCandCase[0].iCase ;
                            } while (ctx.frC.sortCandCase[0].nbCand > 0 ) ;
                            if(deepLg < deepLgBest ){
                                deepLgBest = deepLg ;
                                iCaseBest = iCase0 ;
                            }
                            while (--deepLg >= 0) {
                                RemovePieceInCase(&ctx,caseHeap[deepLg]) ;
                            }
                        }
                        if(deepLgBest >0 ) {
                            RangNewCase(&ctx.rg,iCaseBest );
                            return 1 ;
                        } else {
                            return 0 ;
                        }
                    }
                    
#if defined(SORT_PIECE)
                    // pour toutes les case avec le meme nombre de candidats on cherche la piece candidate ayant le minimum de case
                    int minCumPiece = 65536 ;
                    for(ib=ibMin; (ib < ctx.frC.nbFreeCase) && (ctx.frC.sortCandCase[ib].nbCand == sizeListeMin) ;ib++) {
                        int k ;
                        int iCase = ctx.frC.sortCandCase[ib].iCase ;
                        int cumPiece = 65536 ;
                        for(k=0;k<sizeListeMin;k++) {
                            if( ctx.frC.nbFreeCaseByPiece[ctx.frC.listCandForCase[iCase*256+k]] < cumPiece) {
                                cumPiece = 	ctx.frC.nbFreeCaseByPiece[ctx.frC.listCandForCase[iCase*256+k]] ;
                            }
                        }
                        if(cumPiece < minCumPiece) {
                            newCase = iCase ;
                            minCumPiece = cumPiece ;
                        }
                    }
#else
                    // pour toutes les case avec le meme nombre de candidats on cherche la case ayant le minimum des cases voisines maxi.
                    int maxPieceVoisin = -1 ;
                    for(ib=ibMin; (ib < ctx.frC.nbFreeCase) && (ctx.frC.sortCandCase[ib].nbCand == sizeListeMin) ;ib++) {
                        int k ;
                        int iCase = ctx.frC.sortCandCase[ib].iCase ;
                        int minPieceVoisin = 256 ;
                        for(k=0;k<4;k++) {
                            int rg ;
                            if( (c.voisinCase[4*iCase+k] >= 0) && ( (rg = ctx.frC.rangForFreeCase[c.voisinCase[4*iCase+k]]) >= 0) ) {
                                if(ctx.frC.sortCandCase[rg].nbCand  < minPieceVoisin) {
                                    minPieceVoisin = ctx.frC.sortCandCase[rg].nbCand ;
                                }
                            }
                        }
                        if(minPieceVoisin > maxPieceVoisin) {
                            newCase = iCase ; 
                            maxPieceVoisin = minPieceVoisin ;
                        }
                    }
#endif
                    RangNewCase(&ctx.rg,newCase);
                    if(conf.findOrd) {
                        conf.ordMax = ctx.rg.rang+1 ;
                        st.ord_rg2Case[ctx.rg.rang] = newCase ;
                    }
                }
            }
        }
    }
    return 1 ;
}

int main(int argc,char **argv) {
	ParseOption(argc,argv);
	fhist = OpenHist(&conf);
	PrintOption(fhist,argc,argv,&conf);
	PrintOption(stderr,argc,argv,&conf);
	Precomp(&gDesc,conf.isDebug);

	ReadStart(&st,conf.nameStart) ;
	if(conf.ordMax == 0) { // non specifie
		conf.ordMax = st.nbCaseOnEchiq ;
	} else if ( conf.ordMax > st.nbCaseOnEchiq ){
//        conf.ordMax = st.nbCaseOnEchiq ;
    }

    Init_CasexPiece(&ctx,conf.isDebug) ;
    InitFromStart(&st,&ctx,conf.isDebug) ;
    Print_Echiq16(ctx.cstr.echiqPiece,NULL,Format_Piece,"%3d%c","Echiq");
    fprintf(fhist,"Conf rgOrder=%d \n",conf.ordMax) ;
    fprintf(fhist,"Conf rg0=%d , %d Exp0=%llxx\n",conf.rgOk,
#if defined(SORT_PIECE)
            conf.isSort,
#else
            0,
#endif
            conf.exp0) ;


	if(conf.reverse) {
		START str0 ;
		{
			T_RANG rgO[257] ;
			int i ;
			memset(rgO,-1,257*sizeof(rgO[0])) ;
			for(i=0;i<256;i++) {
				rgO[ctx.rg.rg2case[i]+1]= i ;
			}
			Print_Echiq16(rgO+1,NULL,Format_Ident,"%4d","Rang placement");
		}
		ReverseOrder(st.nbCaseOnEchiq,conf.reverse,ctx.rg.rg2case,ctx.cstr.echiqPiece,&str0) ;
		Init_CasexPiece(&ctx,conf.isDebug) ;
		InitFromStart(&str0,&ctx,conf.isDebug) ;
		Print_Echiq16(ctx.frC.rangForFreeCase,&ctx.frC,Format_StatusBorder,"%4d","Status Border");
		Print_Echiq16(ctx.cstr.echiqPiece,NULL,Format_Piece,"%3d%c","Echiq");
		{
			T_RANG rgO[257] ;
			int i ;
			memset(rgO,-1,257*sizeof(rgO[0])) ;
			for(i=0;i<256;i++) {
				rgO[ctx.rg.rg2case[i]+1]= i ;
			}
			Print_Echiq16(rgO+1,NULL,Format_Ident,"%4d","Rang placement");
		}
		st = str0 ;
		conf.ordMax = st.ordMax ;
	}


	{ 


		memset(&ctx.rg.rhist,0,sizeof(ctx.rg.rhist)) ;

        //Ajout pour demarrer l'impression de l'initialisation
//        ctx.rg.rhist.rangMax = st.ordMax ;
        
        
//		int rgRemove ;
		while(ctx.rg.rang<256) {
			ctx.rg.rhist.exp++;
			if (ctx.rg.lastDeltaRang == 1) { //lastDeltaRang = 1 <=> RangIncrease
                if(FindCandidateNewRang() == -1) {
                    RangDecrease(&ctx.rg) ;
                    continue ;
                }
			} else { //lastDeltaRang = -1 <=> RangDecrease
				int iCase = RangGetCase(&ctx.rg) ;
				RemovePieceInCase(&ctx,iCase) ; // on enleve la piece
				if(ctx.frC.sortCandCase[0].nbCand == 0) { // au cas ou il y aurait encore un impossibilite
					RangDecrease(&ctx.rg) ;
					if(ctx.rg.rang<=1) break ;
					continue ; //
				}
			}
//			if(ctx.rg.rhist.higExp >= conf.hExp0 && ctx.rg.rhist.lowExp >= conf.lExp0) {
			if(ctx.rg.rhist.exp >= conf.exp0 ) {
				RangDecrease(&ctx.rg) ;
				if(ctx.rg.rang<=1) break ;
			} else {	
				int iCase = RangGetCase(&ctx.rg) ;
				T_COTE_CONSTRAINT cst = GetContraintesForCase(&ctx.cstr,iCase);
				T_PIECE * pieceCand = ctx.frC.listCandForCase + iCase * NB_PIECES ;
				int curCand ;
				int candFind = 0 ;
				int rg = ctx.frC.rangForFreeCase[iCase] ;
				for(curCand=ctx.rg.rg2cand[ctx.rg.rang];INDfromCAND(curCand) < ctx.frC.sortCandCase[rg].nbCand;)  {
					int ir = ROTfromCAND(curCand) ;
					int indCand = INDfromCAND(curCand) ;
					ir = ChkContraintesIr(cst,pieceCand[indCand],ir);
					if( ir  >= 0) {
						T_PIECE_ROTATE iPieceRot = ir * NB_PIECES + pieceCand[indCand];
						candFind = 1 ;
						PlacePieceInCase(&ctx,iPieceRot ,iCase) ;
						RangIncrease(&ctx.rg,indCand,ir ) ;
						break ;
					} else {
						curCand =  CANDfromIND_ROT(indCand+1,0);
					}
				}
				if(candFind == 0) {
					RangDecrease(&ctx.rg);
                    if(ctx.rg.rang<=1) break ;
				}
			}

printOK:
			if((ctx.rg.rang > ctx.rg.rhist.rangMax  && ctx.rg.lastDeltaRang==1)  ) { 
					static clock_t lastClock ;
					static clock_t firstClock = 0 ;
					clock_t curClock = clock();
					if(firstClock == 0) firstClock = curClock ;
					printf("\n time=%.1f %.1f: ================>",
						(float) (curClock - firstClock) / CLOCKS_PER_SEC
						,(float) (curClock - lastClock) / CLOCKS_PER_SEC );
					fprintf(fhist,"\n time=%.1f %.1f: ================>",
						(float) (curClock - firstClock) / CLOCKS_PER_SEC
						,(float) (curClock - lastClock) / CLOCKS_PER_SEC );
					lastClock = curClock ;

					ctx.rg.rhist.expCum += ctx.rg.rhist.exp;
					ctx.rg.rhist.exp = 0 ;
					
					fprintf(fhist,"ir=%d min=%d nb=%llxx",ctx.rg.rang,ctx.rg.rhist.rangMin,ctx.rg.rhist.expCum);
					fprintf(stdout,"ir=%d min=%d nb=%llxx",ctx.rg.rang,ctx.rg.rhist.rangMin,ctx.rg.rhist.expCum);
					Print_Echiq16(ctx.frC.rangForFreeCase,&ctx.frC,Format_StatusBorder,"%4d","STATUS");

                    Print_EchiqPiece(ctx.cstr.echiqPiece,"PIECES") ;
					Print_Echiq16(ctx.cstr.echiqPiece,NULL,Format_Piece,"%3d%c","ECHIQ");
					{
						T_RANG rgO[257] ;
						int i ;
						memset(rgO,-1,257*sizeof(rgO[0])) ;
						for(i=0;i<256;i++) {
							rgO[ctx.rg.rg2case[i]+1]= i ;
						}
						Print_Echiq16(rgO+1,NULL,Format_Ident,"%4d","ORDER");
					}
					{
						int nbErr = verifHypothese(&ctx,0) ;
						fprintf(stdout,"ERR=%d\n",nbErr);
						if(nbErr > 0) exit(0) ;
					} 
					ctx.rg.rhist.rangMax = ctx.rg.rang ;
					ctx.rg.rhist.rangMin = ctx.rg.rhist.rangMax ;
					ctx.rg.rhist.expCum =0 ;
    
			} else if(ctx.rg.rang < ctx.rg.rhist.rangMin ) {
					ctx.rg.rhist.rangMin =ctx.rg.rang ;
					ctx.rg.rhist.expCum += ctx.rg.rhist.exp;
					ctx.rg.rhist.exp= 0 ;
					printf("(%d=>%-d nbOr=%x nb=%llxx)",ctx.rg.rhist.rangMax1,ctx.rg.rang,ctx.rg.rhist.nbFindOrdMax,
						ctx.rg.rhist.expCum);fflush(stdout);
					ctx.rg.rhist.rangMax1 = ctx.rg.rhist.rangMin ;
			} else if(ctx.rg.rang > ctx.rg.rhist.rangMax1) {
//					printf("(%d=>%-d %d)",ctx.rg.rhist.rangMin,rang,ctx.rg.rhist.lowExp);
					ctx.rg.rhist.rangMax1 = ctx.rg.rang ;
			} 

		}
	}

	return 0 ;
}




/********************* lecture d'une initialisation ****************************/
// remplit le structure start contenant les cases remplies, et l'ordre 
static void ReadStart(START *st,const char *startName) {
	FILE *fStart = fopen(startName,"rt");
	if((fStart == NULL ) && (strcmp(startName,"INIT") != 0 )) {
		fprintf(stderr,"Error open '%s'\n",startName) ;
		exit(0);
	}
	// on place la piece 139
    if(conf.isno139) {
        st->echiq[16*0+0] = 0  ;
        st->nbCaseOnEchiq = 1;
        memset(st->ord_rg2Case,-1,NB_CASES*sizeof(st->ord_rg2Case[0])) ;
        st->ord_rg2Case[0] = 16*0+0 ;
        st->ordMax = 1 ;
        
    } else {
        st->echiq[16*lig139+col139] = numPiece139 + ir139 * 256 ;
        st->nbCaseOnEchiq = 1;
        memset(st->ord_rg2Case,-1,NB_CASES*sizeof(st->ord_rg2Case[0])) ;
        st->ord_rg2Case[0] = 16*lig139+col139 ;
        st->ordMax = 1 ;
    }
    if ( fStart != NULL) {
        for(;;) {
            char tmp[500] ;
            fgets(tmp,500,fStart);
            if(tmp[0] == '!') {
                continue ;
            }
            if(strncmp("ECHIQ",tmp,5) == 0) {
                int il,ic ;
                for(il=0;il<16;il++) {
                    char *ptc = tmp ;
                    fgets(tmp,500,fStart) ;
                    for(ic=0;ic<16;ic++) {
                        int ipiece, nbcar;
                        char ir ;
                        sscanf(ptc,"%d%c%n",&ipiece,&ir,&nbcar);
                        ir -= 'a' ; ptc += nbcar ; 	ipiece-- ;
                        st->echiq[16*il+ic] = ipiece + ir * 256 ;
                        if(ipiece >= 0) {
                            if(ipiece != numPiece139) {
                                st->nbCaseOnEchiq++ ;
                            } else {
                                if(16*il+ic != caseLigCol(lig139,col139)) {
                                    fprintf(stderr,"Piece 139 en %d au lieu de  %d \n ",16*il+ic,caseLigCol(lig139,col139));
                                    exit(0) ;
                                }
                            }
                        }
                    }
                }
            } else if(strncmp("ORDER",tmp,5) == 0) {
                int case2rg[NB_CASES] ;
                int il, ic ,ia;
                int maxRg = 0 ;
                for(il=0;il<16;il++) { // on lit le tableau case2rg
                    char *ptc = tmp ;
                    fgets(tmp,500,fStart) ;
                    for(ic=0;ic<16;ic++) {
                        int nbcar;
                        sscanf(ptc,"%d%n",case2rg+16*il+ic,&nbcar);
                        ptc += nbcar ;
                    }
                }
                for(ia=0;ia<256;ia++) { // construction table inverse case2rg -> rg2case
                    if(ia==caseLigCol(lig139,col139) && (conf.isno139 == 0) ) {
                        if(case2rg[ia] != 0) { // verif case 139 deja rangee
                            fprintf(stderr,"ERR order pour 139 %d != 0 \n",case2rg[ia]);
                            exit(0);
                        } else {
                            continue ;
                        }
                    } else if(case2rg[ia] >= 0) {
                        if(st->ord_rg2Case[case2rg[ia]] != -1) {
                            fprintf(stderr," ERR order rang %d en double %d et %d \n",case2rg[ia],st->ord_rg2Case[case2rg[ia]],ia);
                            exit(0) ;
                        } else {
                            st->ord_rg2Case[case2rg[ia]] = ia ;
                            st->ordMax++ ;
                            if(case2rg[ia] > maxRg) {
                                maxRg = case2rg[ia] ;
                            }
                        }
                    }
                }
                if(st->ordMax != maxRg + 1) {
                    int ir;
                    fprintf(stderr," ERR order rang %d cases != rang max %d+1 \n Missing :",st->ordMax,maxRg);
                    for(ir=0;ir<maxRg;ir++) {
                        if(st->ord_rg2Case[ir] == -1) { fprintf(stderr,"%d ",ir); }
                    }
                    exit(0) ;
                }
            } else if(strncmp("EXIT",tmp,4) == 0) {
                break ;
            }
        }
    }
	if(st->ordMax < st->nbCaseOnEchiq) {
		fprintf(stderr,"Error ordre trop court %d < %d, reverse\n",st->ordMax,st->nbCaseOnEchiq);
        // on force la decouverte de l'ordre au dela
        if(conf.reverse < st->ordMax) {
            conf.reverse = st->ordMax ;
        }
        
//		exit(0);
	}
}


/***************** precalcul des liens,histos, .....  ****************************/
//   2
// 3   1
//   0

static void Precomp (Description *descr,int isDebug) {
// precalcul des voisins
	int ia ;
	for(ia=0;ia<NB_CASES;ia++) {
		int il = ligCase(ia)	;
		int ic = colCase(ia) ;
		descr->voisinCase[ia*4] =(il<15) ?  caseLigCol(il+1,ic) : -1 ;
		descr->voisinCase[ia*4+1] =(ic<15) ?  caseLigCol(il,ic+1) : -1 ;
		descr->voisinCase[ia*4+2] =(il>0) ?  caseLigCol(il-1,ic) : -1 ;
		descr->voisinCase[ia*4+3] =(ic>0) ?  caseLigCol(il,ic-1) : -1 ;
	}
// rotations 
	{	int ip,io,ir ;
//		c->elM1.v = 0 ;
//		c->elM2.v = 0x01010101 ;
        descr->coteConstraintsM1.cote4 = 0 ;
        descr->coteConstraints[IND_NULL_PIECE_ROTATE].cote4 = 0x01010101 ;
		for(ip=0;ip<NB_PIECES;ip++) {
			for(io=0;io<NB_COTES;io++) {
				for(ir=0;ir<NB_ROTATIONS;ir++) {
					descr->coteConstraints[ip+ir*NB_PIECES].cote[io] = tbPiece[ip].name[(io+ir) % NB_COTES]-'@'  ; // attention commence a 1
				}
			}
		}
	}
// histogrammes
	{
		int ip,io,ih,jh ;
		memset(descr->hist1DifCote,0,NBDIF_COTE*sizeof(descr->hist1DifCote[0])) ;
		memset(descr->hist2DifCote,0,NBDIF_COTE*NBDIF_COTE*sizeof(descr->hist2DifCote[0])) ;
		for(ip=0;ip<NB_PIECES;ip++) {
			for(io=0;io<NB_COTES;io++) {
				descr->hist1DifCote[descr->coteConstraints[ip].cote[io]]++ ;
				descr->hist2DifCote[descr->coteConstraints[ip].cote[io]*NBDIF_COTE+descr->coteConstraints[ip].cote[(io+1) % NB_COTES]]++ ;
			}
		}
		if(isDebug) {
			for(ih=1;ih<NBDIF_COTE;ih++) {
				fprintf(stdout,"%c=%-3d\n",ih+'@',descr->hist1DifCote[ih]);
			}
			for(ih=1;ih<NBDIF_COTE;ih++) {
				for(jh=1;jh<NBDIF_COTE;jh++) {
					fprintf(stdout,"%c%c=%-2d ",ih+'@',jh+'@',descr->hist2DifCote[ih*NBDIF_COTE+jh]);
				}
				fprintf(stdout,"\n");
			}
		}
	}
}

static int InitFromStart(const START *st,CONTEXT *ctx,int isDebug) {
	int nbStartPiece = st->nbCaseOnEchiq ;
	ctx->rg.rang = 0 ;
	while(nbStartPiece > 0) {
		// on va rechercher une case bordure non initialiser
		int k1;
		int ifind = 0 ;
		for(k1=0;k1<ctx->frC.nbFreeCase;k1++) {
            int k = (ctx->rg.rang > conf.initP) ? k1 : (ctx->frC.nbFreeCase -k1 -1) ;
            T_CASE iCase = ctx->frC.sortCandCase[k].iCase ;
			if(ctx->rg.rang < st->ordMax && st->ord_rg2Case[ctx->rg.rang] != iCase ) continue ;
			if(st->echiq[iCase] >= 0 && ctx->cstr.echiqPiece[iCase] < 0) {
				int j ;
				int iPiece = st->echiq[iCase] & 255 ;
				int ir = st->echiq[iCase] / 256 ;
				T_PIECE * candP = ctx->frC.listCandForCase + iCase * NB_PIECES ;
				// on verifie que le candidat est bien ds la liste des possibles
				for(j=0;j<ctx->frC.sortCandCase[k].nbCand;j++) {
					if(candP[j] == iPiece) {
						T_COTE_CONSTRAINT cst = GetContraintesForCase(&ctx->cstr,iCase) ;
						if(j > 0) {	// on permute pour mettre en tete
							candP[j] = candP[0] ;
							candP[0] = iPiece ;
						}
						ir = ChkContraintesIr(cst,iPiece,ir);
						if( ir  >= 0) {
							T_PIECE_ROTATE iPieceRot = ir * NB_PIECES + iPiece;
							PlacePieceInCase(ctx,iPieceRot ,iCase) ;
							RangNewCase(&ctx->rg,iCase);							
							RangIncrease(&ctx->rg,0,ir);
							nbStartPiece-- ;
							ifind = 1 ;
							break ;
						} else {
							printf("Piece %d en %d non possible \n ",st->echiq[iCase],iCase);
							exit(0) ;

						}
					}
				}
				if(ifind == 0) {
					printf("Piece %d en %d non possible \n ",st->echiq[iCase],iCase);
					exit(0) ;
				} else {
					break ;
				}
			}
		}
		if(ifind == 0) {
			Print_Echiq16(ctx->cstr.echiqPiece,NULL,Format_Piece,"%3d%c","Echiq");
			printf("Plus de case en bordure a placer \n");
			exit(0) ;
		}
		if(isDebug) {
			Print_Echiq16(ctx->frC.rangForFreeCase,&ctx->frC,Format_StatusBorder,"%4d","Status Border");
			Print_Echiq16(ctx->cstr.echiqPiece,NULL,Format_Piece,"%3d%c","Echiq");
		}
		{
			int nbErr = verifHypothese(ctx,0) ;
			if(nbErr > 0) {
				fprintf(stderr,"ERR=%d\n",nbErr);
				exit(0) ;
			}
		} 
	}
	return ctx->rg.rang ;
}

/******************* traitment des options *****************************************/
static void PrintUsage(void) {
	opt *op = tbOpt ;
	fprintf(stderr,"EII") ;
	while(op->name != NULL) {
		if(op->name[0] == '-') {
			fprintf(stderr," [%s",op->name);
			if(op->format != NULL) {
				fprintf(stderr," %s",op->defaultValue);
			}
			fprintf(stderr,"]");
		} else {
			fprintf(stderr," %s",op->name);
		}
		op++ ;
	}
	exit(0); 
}
static void ParseOption(int argc,char **argv) {
	opt *oparg =  tbOpt ;
	opt *op ;
	argc-- ; argv++ ;
	// on met les valeurs par defaut
	for(op=tbOpt;op->name != NULL; op++) {
		if(op->format != NULL) {
			sscanf(op->defaultValue,op->format,op->var);
		} else {
			strcpy(op->var,op->defaultValue);
		}

	}
	// on decode la ligne de commande
	while(argc > 0) {
		opt *op ;
		for(op= (argv[0][0] == '-') ? tbOpt : oparg ;op->name != NULL; op++) {
			if(argv[0][0] == '-') {
				if(strcmp(op->name,argv[0]) == 0) {
					argv++ ; 
					if(--argc == 0)  { PrintUsage() ; }
				} else {
					continue ;
				}
			} else if ( op->name[0] == '-' ) continue ;
			if(op->format != NULL) {
				sscanf(argv[0],op->format,op->var);
			} else {
				strcpy(op->var,argv[0]);
			}
			if(op->name[0] != '-' ) oparg = op+1 ;
			break ;
		}
		if(op->name == NULL) {	PrintUsage() ;	}
		argv++ ; argc-- ;
	}
	/* on verifie que tous les arguments sont donnes */
	for(;oparg->name != NULL; oparg++) {
		if ( oparg->name[0] != '-' ) {	PrintUsage() ;	}
	}
}

static void PrintOption(FILE *fhist,int argc,char **argv,CONF *conf) {
	fprintf(fhist,"!CMD :");
	while(argc-- > 0) {
		fprintf(fhist," %s",argv[0]) ;
		argv++ ; 
	}
	fprintf(fhist,"\n!CONF %s -> %s OrderMax=%d rangOk=%d Explo=%0llxx SortFree=%d\n ",
		conf->nameStart,conf->nameHist,conf->ordMax,conf->rgOk,conf->exp0,0 /*conf->isSort*/);
	return ;
}
FILE * OpenHist(CONF *conf) {
	FILE *fh = NULL  ;
	time_t tm  ;
	struct tm *stTime;
	char *ptNameStart = strrchr(conf->nameStart,'\\');
	if(ptNameStart == NULL)  {
		ptNameStart= conf->nameStart ;
	} else {
		ptNameStart++ ;
	}
	time(&tm);
	stTime=localtime(&tm);
	sprintf(conf->nameHist,"Hist_%04d%02d%02d@%02d%02d%02d.%s.hst",
		stTime->tm_year+1900,stTime->tm_mon+1,stTime->tm_mday,stTime->tm_hour,stTime->tm_min,stTime->tm_sec,ptNameStart);
	fh = fopen(conf->nameHist,"wt");
	if(fh == NULL) {
		fprintf(stderr,"Error open '%s'\n",conf->nameHist); exit(0);
	}
	return fh ;

}

