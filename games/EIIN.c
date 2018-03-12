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

#define NB_RANGS     NB_CASES
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
#define MAX_PIECE_CAND    64
// nombre de couples (Rotation,Piece)
#define NB_PIECES_ROTATE         (NB_ROTATIONS*NB_PIECES)
// indice joker pour marquer NULL piece ec rotation
#define IND_NULL_PIECE_ROTATE    NB_PIECES_ROTATE


#define T_COST  int32_t
#define MAX_COST    0x7fffffff

#define ligCase(j)	((j)/16)
#define colCase(j)	((j)&15)
#define caseLigCol(i,j)		((i)*16+(j))


#define numPiece139	138
#define rotPiece139	2
#define lig139		8
#define	col139		7
#define case139     (lig139*16+col139)
#define ir139		rotPiece139

#define PIECEfromPIECEROT(iPieceRot)		((iPieceRot) & 255)
#define ROTfromPIECEROT(iPieceRot)		((iPieceRot) /256 )
#define PIEROTfromPIECE_ROT(piece,rot)	( (rot) * 256 + (piece) )


// contrainte sur 4 cotes : pour chaque cote
// 0 = no contrainte
// 1 = bord
// >1 = cote non bord
// Voisinage
//   2
// 3   1
//   0
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

typedef struct RANGHISTO	RANGHISTO ;

struct RANGHISTO {
	int rangMax ;
	int rangMax1 ;
	int rangMin;
	int64_t exp ;
	int64_t expCum ;
	int nbFindOrdMax ;
    
} ;

typedef struct CAND_BORDER_CASE CAND_BORDER_CASE ;
struct CAND_BORDER_CASE {
    T_COST  cost ;
    T_CASE  numCase ;
    T_NB_CAND   nbPieces ;
};
//
#define MAX_NB_CAND 4096

typedef struct DEVELOPPMENT DEVELOPPMENT ;
struct DEVELOPPMENT {
    GLB_POSITION  glb ;

    int nbFreeCase ;                        // nombre de case en bordures
    int nbFreePiece ;                       // nombre de pieces non placees
    int nbCandidate ;
    T_NB_CAND indCandForCase[NB_CASES] ;     // indice de la case ds sortCandCase (negatif si pas border)
    T_PIECE listFreePiece[NB_PIECES] ;      // liste des pieces libres
    T_NB_CAND nbCandForPiece[NB_PIECES] ;   // nombre de candidat (case) pour les pieces non placees
    CAND_BORDER_CASE  sortCandCase[NB_CASES] ;
    T_PIECE_ROTATE  listPieceRotateForCase[NB_CASES][MAX_PIECE_CAND] ; // liste des piecerotate candidat pour chaque case.
};

typedef struct TREE	TREE ;
struct TREE {
	int rang ;
	int lastDeltaRang ;
    DEVELOPPMENT dev[NB_RANGS] ;
    int rg2Case[NB_RANGS] ;
    int rg2indPieceCand[NB_RANGS] ; // l'indice de la piece dans dev->listPieceForCase
    int rg2indPieceCandMax[NB_RANGS] ; // le max des pieces a traiter
    
	// variable lie a l'historique du parcours
	RANGHISTO rhist ;
} ;


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



typedef struct START START ;
struct START {
	int nbCaseOnEchiq ;				// nombre de case placees (avec rotation)
	int ordMax;						// ordre max impose
	T_PIECE_ROTATE echiq[NB_CASES] ;
	T_CASE ord_rg2Case[NB_PIECES] ;
} ;




typedef struct CONF CONF ;
struct CONF {
	char nameStart[300] ;
	char nameHist[300] ;
	int  ordMax ;
	int  rgOk ;
	int  findOrd ;
    int  truncStart ;
#if defined(SORT_PIECE)
	int  isSort ;
#endif
	int  isDebug ;
    int  isno139 ;
    int  rand ;
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
FILE *fhist ;
static START st ;
static Description gDesc ;
static TREE tree ;


static void ReadStart(START *st,const char *startName) ;
START * ReverseOrder(int nbCaseOnEchiq,const T_PIECE rg2case[NB_PIECES],const T_PIECE echiq[NB_CASES],START *str) ;

static void Precomp (Description *c,int isDebug) ;
void Precomp_ELfromPieces(void) ;
void Precomp_Histo(void) ;
typedef char * (FORMAT)(void *ctx,int i,const char * format);

char * Format_Ident(void *ctx,int i,const char * format) ;
char * Format_Piece(void *ctx,int i,const char * format) ;
char * FRM_nbCand(void *dev,int i, const char * format) ;

static void Print_EchiqPiece(int16_t *val,const char *comment) ;
static void Print_Echiq16(int16_t *val,void *ctx,FORMAT *trans,const char *format,const char *comment) ;


int32_t	cmpNbPiece (const void *el1,const void *el2) {
	return *((int32_t *)el2) - *((int32_t *)el1) ;
}

__inline static void TreeIncreaseCand(TREE *tree) ;
__inline static void TreeSetCaseForRang(TREE *tree) ;
__inline static void TreeIncreaseRang(TREE *tree) ;
__inline static void TreeDecreaseRang(TREE *tree);

__inline static  T_COTE_CONSTRAINT  SetContraintesForCase(GLB_POSITION *glbP, int iCase) ;
__inline  static T_COTE_CONSTRAINT GetContraintesForCase(GLB_POSITION *cstr,int iCase) ;
static int ChkContraintesIr(T_COTE_CONSTRAINT cst,int iPiece,int ir) ;
static int ChkContraintesPieceRotate(T_COTE_CONSTRAINT cst,int iPieceRotate) ;
static int ChkContraintes(T_COTE_CONSTRAINT cst,int iPiece) ;

static T_COST ComputeMinCostForDev(DEVELOPPMENT *dev) ;

static int InitFromStart_Developpemnt(const START *st,TREE *tree,int isDebug) ;
void Init0_Developpement(DEVELOPPMENT *dev,int isDebug) ;
int VerifDeveloppment(DEVELOPPMENT *dev,int level) ;
static int PlacePiece(DEVELOPPMENT *newDev,DEVELOPPMENT *antDev,T_CASE candCase,int indP,T_CASE nxtCandCase) ;

static T_COST CmpCand(const void *el1,const void *el2) ;



//   2
// 3   1
//   0




static opt tbOpt[] = {
	{ "-omax" , "%d", "0", &conf.ordMax } ,
	{ "-rgok" , "%d", "210", &conf.rgOk } ,
	{ "-findord" , "%d", "0", &conf.findOrd } ,
    { "-trunc" , "%d", "0", &conf.truncStart } ,
#if defined(SORT_PIECE)
	{ "-sort" , "%d", "1", &conf.isSort } ,
#endif
    { "-no139" , "%d", "0", &conf.isno139 } ,
	{ "-debug" , "%d", "0", &conf.isDebug } ,
    { "-rand" , "%d", "0", &conf.rand } ,
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
  
    if(conf.reverse) {
		START str0 ;
		{
			T_RANG rgO[257] ;
			int i ;
			memset(rgO,-1,257*sizeof(rgO[0])) ;
			for(i=0;i<256;i++) {
				rgO[st.ord_rg2Case[i]+1]= i ;
			}
			Print_Echiq16(rgO+1,NULL,Format_Ident,"%4d","Rang placement");
		}
		ReverseOrder(st.nbCaseOnEchiq,st.ord_rg2Case,st.echiq,&str0) ;
        InitFromStart_Developpemnt(&str0,&tree,conf.isDebug);
	} else {
        InitFromStart_Developpemnt(&st,&tree,conf.isDebug);
        
    }

    

//    InitFromStart(&st,&ctx,conf.isDebug) ;
    Print_Echiq16(tree.dev[tree.rang].glb.echiqPiece,NULL,Format_Piece,"%3d%c","Echiq");
    fprintf(fhist,"Conf rgOrder=%d \n",conf.ordMax) ;
    fprintf(fhist,"Conf rg0=%d , %d Exp0=%llxx\n",conf.rgOk,
#if defined(SORT_PIECE)
            conf.isSort,
#else
            0,
#endif
            conf.exp0) ;
    
    
	{
        
        
		memset(&tree.rhist,0,sizeof(tree.rhist)) ;
        //Ajout pour demarrer l'impression de l'initialisation
        //        ctx.rg.rhist.rangMax = st.ordMax ;
        
        
        //		int rgRemove ;
		while(tree.rang<256) {
            int minCand ;
			tree.rhist.exp++;
            if(tree.rg2indPieceCand[tree.rang] >= tree.rg2indPieceCandMax[tree.rang]) {
//                printf("[-%d/%d]",tree.rang,tree.rg2numCand[tree.rang]);
                TreeDecreaseRang(&tree) ;
                if(tree.rang <= 1) break ;
                continue ;
            }
            //			if(ctx.rg.rhist.higExp >= conf.hExp0 && ctx.rg.rhist.lowExp >= conf.lExp0) {
			if(tree.rhist.exp >= conf.exp0 ) {
                if(conf.rand) {
                    int irand = rand() ;
                    if((irand % conf.rand) != 0) {
                        TreeIncreaseRang(&tree) ;
                        tree.rhist.expCum += tree.rhist.exp;
                        tree.rhist.exp= 0 ;
                        printf("(%d=>%-dR nbOr=%x nb=%llxx)",tree.rhist.rangMax1,tree.rang,tree.rhist.nbFindOrdMax,
                               tree.rhist.expCum);fflush(stdout);
                        tree.rhist.rangMax1 = tree.rang ;
                    } else {
                        TreeDecreaseRang(&tree) ;
                        if(tree.rang<=1) break ;
                    }
                }
			} else {
                if((minCand=PlacePiece(tree.dev+tree.rang+1,tree.dev+tree.rang,tree.rg2Case[tree.rang], tree.rg2indPieceCand[tree.rang],tree.rg2Case[tree.rang+1])) > 0 ) {
                      TreeIncreaseRang(&tree);
                      TreeSetCaseForRang(&tree);
                } else {
                    TreeIncreaseCand(&tree);
                 }

			}
            
        printOK:
			if((tree.rang > tree.rhist.rangMax  && tree.lastDeltaRang==1)  ) {
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
                
                tree.rhist.expCum += tree.rhist.exp;
                tree.rhist.exp = 0 ;
                
                fprintf(fhist,"ir=%d min=%d nb=%llxx",tree.rang,tree.rhist.rangMin,tree.rhist.expCum);
                fprintf(stdout,"ir=%d min=%d nb=%llxx",tree.rang,tree.rhist.rangMin,tree.rhist.expCum);
                Print_Echiq16(tree.dev[tree.rang].indCandForCase,tree.dev+tree.rang,FRM_nbCand,"%4d","NBCAND");

 //               Print_Echiq16(ctx.frC.rangForFreeCase,&ctx.frC,Format_StatusBorder,"%4d","STATUS");
                
                Print_EchiqPiece(tree.dev[tree.rang].glb.echiqPiece,"PIECES") ;
                Print_Echiq16(tree.dev[tree.rang].glb.echiqPiece,NULL,Format_Piece,"%3d%c","ECHIQ");
                {
                    T_RANG rgO[257] ;
                    int i ;
                    memset(rgO,-1,257*sizeof(rgO[0])) ;
                    // on n'utilise les valeurs que jusqu'a tree->rang, donc les valeurs au-dessus peuvent servir a imposer l'ordre
                    for(i=0;i<tree.rang;i++) {
                        rgO[tree.rg2Case[i]+1]= i ;
                    }
                    Print_Echiq16(rgO+1,NULL,Format_Ident,"%4d","ORDER");
                }
                {
                    int nbErr = VerifDeveloppment(&tree.dev[tree.rang],0) ;
                    fprintf(stdout,"ERR=%d\n",nbErr);
                    if(nbErr > 0) exit(0) ;
                }
                tree.rhist.rangMax = tree.rang ;
                tree.rhist.rangMin = tree.rhist.rangMax ;
                tree.rhist.expCum =0 ;
                
			} else if(tree.rang < tree.rhist.rangMin ) {
                tree.rhist.rangMin =tree.rang ;
                tree.rhist.expCum += tree.rhist.exp;
                tree.rhist.exp= 0 ;
                printf("(%d=>%-d nbOr=%x nb=%llxx)",tree.rhist.rangMax1,tree.rang,tree.rhist.nbFindOrdMax,
                       tree.rhist.expCum);fflush(stdout);
                tree.rhist.rangMax1 = tree.rhist.rangMin ;
			} else if(tree.rang > tree.rhist.rangMax1) {
                //					printf("(%d=>%-d %d)",ctx.rg.rhist.rangMin,rang,ctx.rg.rhist.lowExp);
                tree.rhist.rangMax1 = tree.rang ;
//                if(conf.isDebug){
//                    Print_Echiq16(tree.dev[tree.rang].glb.echiqPiece,NULL,Format_Piece,"%3d%c","ECHIQ");
//                }
			} 
            
		}
	}
    
	return 0 ;
}


//***************** manipulation de l'arbre d'exploration
__inline static void TreeIncreaseCand(TREE *tree) {
	tree->rg2indPieceCand[tree->rang]++ ; // mise a jour new cand pour la prochaine exploration (rotation++, ou piece suivante)
}

__inline static void TreeSetCaseForRang(TREE *tree) {
    int numCase  = tree->dev[tree->rang].sortCandCase[0].numCase ;
    tree->rg2Case[tree->rang] = numCase ;
    tree->rg2indPieceCand[tree->rang] = 0 ;
    tree->rg2indPieceCandMax[tree->rang] = tree->dev[tree->rang].sortCandCase[0].nbPieces  ;
}
__inline static void TreeIncreaseRang(TREE *tree) {
    TreeIncreaseCand(tree) ;
	tree->lastDeltaRang = 1 ;
	tree->rang++ ;
}
__inline static void TreeDecreaseRang(TREE *tree) {
    if(tree->rang >= conf.ordMax ) {
        tree->rg2Case[tree->rang] = -1 ;
    }
	tree->lastDeltaRang = -1 ;
	tree->rang-- ;
}

//********************** placement d'une piece et calcul du developpement suivant ************
int PlacePiece(DEVELOPPMENT *newDev,DEVELOPPMENT *antDev,T_CASE candCase,int indP,T_CASE nxtCandCase) {
    T_PIECE_ROTATE candPieceRotate = antDev->listPieceRotateForCase[candCase][indP] ;
    memcpy(&newDev->glb,&antDev->glb,sizeof(antDev->glb));
    memcpy(&newDev->indCandForCase,&antDev->indCandForCase,NB_CASES*sizeof(antDev->indCandForCase[0]));
    memcpy(&newDev->nbCandForPiece,&antDev->nbCandForPiece,NB_PIECES*sizeof(antDev->nbCandForPiece[0]));
    newDev->glb.echiqPiece[candCase] =  candPieceRotate ;
    
    {
        int i;
        newDev->nbFreePiece= 0 ;
        for(i=0;i<antDev->nbFreePiece;i++) {
            if( antDev->listFreePiece[i] == PIECEfromPIECEROT(candPieceRotate) ) {
                continue ;
            } else {
                newDev->listFreePiece[newDev->nbFreePiece++] = antDev->listFreePiece[i] ;
            }
        }
    }
    {
        int na ;
        int nbaCand = antDev->nbCandidate ;
        newDev->nbCandidate = 0 ;
        for(na=0;na<nbaCand;na++){
            CAND_BORDER_CASE aCandB = antDev->sortCandCase[na] ;
            CAND_BORDER_CASE * nCandB = newDev->sortCandCase + newDev->nbCandidate ;
            if(aCandB.numCase == candCase)  { //
                int indp ;
                for(indp=0; indp < aCandB.nbPieces;indp++) {
                    T_PIECE_ROTATE pieceR = antDev->listPieceRotateForCase[aCandB.numCase][indp] ;
                    newDev->nbCandForPiece[PIECEfromPIECEROT(pieceR)]-- ;
                }
                newDev->indCandForCase[candCase] = -2 - candPieceRotate ;
                continue ;
            } else {
                nCandB->numCase = aCandB.numCase ;
                nCandB->nbPieces = 0 ;
                newDev->nbCandidate++ ;
            }
            if(abs(aCandB.numCase - candCase) == 1 || abs(aCandB.numCase - candCase) == 16 ){
                int indp ;
                T_COTE_CONSTRAINT cst = SetContraintesForCase(&newDev->glb,aCandB.numCase);
                for(indp=0; indp < aCandB.nbPieces;indp++) {
                    // case voisine
                    T_PIECE_ROTATE pieceR = antDev->listPieceRotateForCase[aCandB.numCase][indp] ;
                    if(PIECEfromPIECEROT(pieceR  ^ candPieceRotate)  == 0) { //
                        continue ;
                    }
                    if(ChkContraintesPieceRotate(cst,pieceR) < 0) {
                        newDev->nbCandForPiece[PIECEfromPIECEROT(pieceR)]-- ;
                        continue ;
                    } else {
                        newDev->listPieceRotateForCase[aCandB.numCase][nCandB->nbPieces ++] = pieceR ;
                    }
                }
            } else {
                int indp ;
                for(indp=0; indp < aCandB.nbPieces;indp++) {
                    T_PIECE_ROTATE pieceR = antDev->listPieceRotateForCase[aCandB.numCase][indp] ;
                    if(PIECEfromPIECEROT(pieceR  ^ candPieceRotate) == 0) { //
                        continue ;
                    } else {
                        newDev->listPieceRotateForCase[aCandB.numCase][nCandB->nbPieces++] = pieceR ;
                    }
                }
            }
        }
    }
    
    
    {
		int i ;
 		for(i=0;i<4;i++) {
            T_CASE icas1 = gDesc.voisinCase[4*candCase+i] ;
			if(icas1 >= 0) {
            	T_COTE_CONSTRAINT cst = SetContraintesForCase(&newDev->glb,icas1);
                if(newDev->indCandForCase[icas1] == -1) { // pas ds les bordures
                    CAND_BORDER_CASE * candB = newDev->sortCandCase + newDev->nbCandidate ;
                    newDev->indCandForCase[icas1] = newDev->nbCandidate++ ;
                    candB->nbPieces = 0 ;
                    candB->numCase = icas1 ;
                    // on parcours la liste des pieces pour rajouter
                    {   int j ;
                        for(j=0;j<newDev->nbFreePiece;j++) {
                            T_PIECE ip = newDev->listFreePiece[j] ;
                            int ir = ChkContraintesIr(cst,ip,0) ;
                            while(ir != -1) {
                                newDev->listPieceRotateForCase[icas1][candB->nbPieces] = (ir << 8) + ip  ;
                                candB->nbPieces++ ;
                                newDev->nbCandForPiece[ip]++ ;
                                ir = ChkContraintesIr(cst,ip,ir+1) ;
                            }
                        }
                        
                    }
                }
            }
        }
	}
    if(nxtCandCase == -1) {
        int minP = MAX_PIECE_CAND ;
        int i ;
        // pas de contraites d'ordre
        ComputeMinCostForDev(newDev) ;
        for(i=0;i<newDev->nbCandidate;i++) {
            if(newDev->sortCandCase[i].nbPieces < minP) {
                minP = newDev->sortCandCase[i].nbPieces ;
            }
        }
        return minP ;
    } else {
        int i ;
        int indAsk = newDev->nbCandidate ;
        for(i=0;i<newDev->nbCandidate;i++) {
            newDev->indCandForCase[newDev->sortCandCase[i].numCase] = i ;
            if(newDev->sortCandCase[i].numCase == nxtCandCase ) {
                indAsk = i ;
            }
        }
        if ( indAsk == newDev->nbCandidate ) {
            return 0 ;
        } else {
            CAND_BORDER_CASE tmp = newDev->sortCandCase[0] ;
            newDev->sortCandCase[0] = newDev->sortCandCase[indAsk] ;
            newDev->sortCandCase[indAsk] = tmp ;
            newDev->indCandForCase[newDev->sortCandCase[0].numCase] = 0 ;
            newDev->indCandForCase[newDev->sortCandCase[indAsk].numCase] = indAsk ;
            return newDev->sortCandCase[0].nbPieces ;
        }
    }
        
}

static T_COST ComputeMinCostForDev(DEVELOPPMENT *dev) {
    T_COST minCost = MAX_COST ;
    {
        int i ;
        int indMinC = 0 ;
        for(i=0;i<dev->nbCandidate;i++) {
            CAND_BORDER_CASE * ptCand = dev->sortCandCase + i ;
            {
                int indP ;
                int minP = MAX_PIECE_CAND ;
                int indMinP = 0 ;
                for(indP=0;indP<ptCand->nbPieces;indP++) {
                    if(dev->nbCandForPiece[PIECEfromPIECEROT(dev->listPieceRotateForCase[ptCand->numCase][indP])] < minP) {
                        minP =  dev->nbCandForPiece[PIECEfromPIECEROT( dev->listPieceRotateForCase[ptCand->numCase][indP]) ] ;
                        indMinP = indP ;
                    }
                    
                }
                if(indMinP != 0) {
                    T_PIECE_ROTATE tmp = dev->listPieceRotateForCase[ptCand->numCase][0]  ;
                    dev->listPieceRotateForCase[ptCand->numCase][0] = dev->listPieceRotateForCase[ptCand->numCase][indMinP ] ;
                    dev->listPieceRotateForCase[ptCand->numCase][indMinP] = tmp ;
                }
                
                  ptCand->cost = (ptCand->nbPieces << 16) + (minP << 8) + ptCand->numCase   ;
//                ptCand->cost = (ptCand->nbPieces << 16) + ((MAX_PIECE_CAND - minP) << 8) + ptCand->numCase   ;
//                ptCand->cost = MAX_PIECE_CAND - ptCand->nbPieces ;
//            ptCand->cost = ( ptCand->nbPieces << 8 ) + ptCand->numCase ;
//                ptCand->cost = ptCand->nbPieces ;
            }
            dev->indCandForCase[dev->sortCandCase[i].numCase]= i ;
            if(ptCand->cost < minCost) {
                minCost = ptCand->cost ;
                indMinC = i ;
            }
        }
        if(indMinC != 0) {
            // on permute
            CAND_BORDER_CASE tmp = dev->sortCandCase[0] ;
            dev->sortCandCase[0] = dev->sortCandCase[indMinC] ;
            dev->sortCandCase[indMinC] = tmp ;
            // on retablit les 2 index
            dev->indCandForCase[dev->sortCandCase[indMinC].numCase]= indMinC ;
            dev->indCandForCase[dev->sortCandCase[0].numCase]= 0 ;
        }

        
        
    }
    return minCost ;
}

static T_COST CmpCand(const void *el1,const void *el2){
    return ((CAND_BORDER_CASE *)el1)->cost - ((CAND_BORDER_CASE *)el2)->cost ;
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
                if(conf.truncStart > 0) {
                    int ir ;
                    for(ir=conf.truncStart;ir<st->ordMax;ir++) {
                        st->echiq[st->ord_rg2Case[ir]] = -1 ;
                        st->nbCaseOnEchiq-- ;
                    }
                        
                }

            } else if(strncmp("EXIT",tmp,4) == 0) {
                break ;
            }
        }
    }
	if(st->ordMax < st->nbCaseOnEchiq) {
		fprintf(stderr,"Warning ordre trop court %d < %d, reverse\n",st->ordMax,st->nbCaseOnEchiq);
        // on force la decouverte de l'ordre au dela
        if(conf.reverse < st->ordMax) {
            conf.reverse = st->ordMax ;
        }
        
//		exit(0);
	}
}


void Init0_Developpement(DEVELOPPMENT *dev,int isDebug){
    {
        int ip ;
        dev->nbFreePiece = 0 ;
        for(ip=0;ip<NB_PIECES;ip++) {
            dev->listFreePiece[dev->nbFreePiece++] = ip ;
            dev->nbCandForPiece[ip] = 0 ;
        }
    }
    {
        int ic ;
        dev->nbFreeCase = 0 ;
        dev->nbCandidate = 0 ;
        dev->glb.echiqPieceM1 = IND_NULL_PIECE_ROTATE ;
        for(ic=0;ic<NB_CASES;ic++) {
            dev->glb.echiqPiece[ic] = -1 ;
            dev->indCandForCase[ic] = -1 ;
            dev->nbFreeCase++ ;
        }
        for(ic=0;ic<NB_CASES;ic++) {
            T_COTE_CONSTRAINT cst = SetContraintesForCase(&dev->glb,ic);
            if(cst.cote4 != 0 ) { // il y a une contrainte, donc un bord
                int ip ;
                for(ip=0;ip<NB_PIECES;ip++) {
                    int ir = ChkContraintesIr(cst,ip,0) ;
                    while(ir != -1) {
                        if(dev->indCandForCase[ic] < 0) {
                            // la case n'existe pas dans les free
                            dev->sortCandCase[dev->nbCandidate].numCase = ic ;
                            dev->sortCandCase[dev->nbCandidate].nbPieces = 0 ;
                            dev->indCandForCase[ic] = dev->nbCandidate++ ;
                            
                        }
                        dev->listPieceRotateForCase[ic][dev->sortCandCase[dev->indCandForCase[ic]].nbPieces] = (ir << 8) + ip  ;
                        dev->sortCandCase[dev->indCandForCase[ic]].nbPieces++ ;
                        dev->nbCandForPiece[ip]++ ;
                        ir=ChkContraintesIr(cst,ip,ir+1) ;
                    }
                }
            }
        }
    }
    ComputeMinCostForDev(dev) ;
}

static int InitFromStart_Developpemnt(const START *st,TREE *tree,int isDebug) {
	int nbStartPiece = st->nbCaseOnEchiq ;
    DEVELOPPMENT * curDev = &tree->dev[0] ;
    {
        int i ;
        for(i=0;i<NB_PIECES;i++) {
            tree->rg2Case[i] = -1 ;
        }
    }
	tree->rang = 0 ;
    Init0_Developpement(curDev,isDebug);
    if(tree->rang < st->ordMax) {
        int i ;
        // on ne prend que l'ordre specifie inferieur a conf.ordMax (barriere intangible pour l'ordre)
        for(i=tree->rang; (i < st->ordMax ) && (i < conf.ordMax ) ;  i++) {
            tree->rg2Case[i] = st->ord_rg2Case[i] ;
        }
        if(conf.findOrd == 0) { // pas de decouverte de l'ordre specifiee, donc si pas specifie en init on s'arrete a ce qui etait precise.
            if(st->ordMax < conf.ordMax) {
                conf.ordMax = st->ordMax ;
            }
        }
    }

	while(nbStartPiece > 0) {
		// on va rechercher une case bordure non initialiser
		int k1;
		int ifind = 0 ;
		for(k1=0;k1<curDev->nbCandidate;k1++) {
            int numCand = (tree->rang >= conf.initP) ? k1 : (curDev->nbCandidate -k1 -1) ;
            T_CASE iCase = curDev->sortCandCase[numCand].numCase ;
			if(tree->rang < st->ordMax && st->ord_rg2Case[tree->rang] != iCase ) continue ;
			if(st->echiq[iCase] >= 0 && curDev->glb.echiqPiece[iCase] < 0) {
                int indP ;
                for(indP=0;indP<curDev->sortCandCase[numCand].nbPieces;indP++) {
                    if(st->echiq[iCase] == curDev->listPieceRotateForCase[iCase][indP]) {
                        if(numCand != 0) {
                            CAND_BORDER_CASE tmp = curDev->sortCandCase[0] ;
                            curDev->sortCandCase[0] = curDev->sortCandCase[numCand] ;
                            curDev->sortCandCase[numCand] = tmp ;
                            curDev->indCandForCase[curDev->sortCandCase[0].numCase] = 0 ;
                            curDev->indCandForCase[curDev->sortCandCase[numCand].numCase] = numCand ;
                            
                            
                        }
                        if(indP > 0) {
                            // on met la piece en tete
                            T_PIECE_ROTATE tmp = curDev->listPieceRotateForCase[iCase][indP] ;
                            curDev->listPieceRotateForCase[iCase][indP] = curDev->listPieceRotateForCase[iCase][0] ;
                            curDev->listPieceRotateForCase[iCase][0] = tmp ;
                        }
                        PlacePiece(&tree->dev[tree->rang+1],curDev,iCase,0,tree->rg2Case[tree->rang+1] ) ;
                        TreeSetCaseForRang(tree);
                        TreeIncreaseRang(tree);
                        curDev = tree->dev + tree->rang ;
                        nbStartPiece-- ;
                        ifind = 1 ;
                        break ;
                    } else {
                        continue ;
                    }
                }
                if(ifind) break ;
            }
		}
		if(ifind == 0) {
			Print_Echiq16(tree->dev[tree->rang].glb.echiqPiece,NULL,Format_Piece,"%3d%c","Echiq");
			printf("Plus de case en bordure a placer \n");
			exit(0) ;
		}
		if(isDebug) {
			Print_Echiq16(tree->dev[tree->rang].indCandForCase,tree->dev+tree->rang,FRM_nbCand,"%4d","NBCAND");
			Print_Echiq16(tree->dev[tree->rang].glb.echiqPiece,NULL,Format_Piece,"%3d%c","Echiq");
		}
		{
			int nbErr = VerifDeveloppment(curDev,0) ;
			if(nbErr > 0) {
				fprintf(stderr,"ERR=%d\n",nbErr);
				exit(0) ;
			}
		}
	}
    TreeSetCaseForRang(tree);
	return tree->rang ;
}

START * ReverseOrder(int nbCaseOnEchiq,const T_PIECE rg2case[NB_PIECES],const T_PIECE echiq[NB_CASES],START *str) {
	int i ;
	GLB_POSITION glbP ;
	// on initialise une struct ctr pour calculer les bordure
	glbP.echiqPieceM1 = IND_NULL_PIECE_ROTATE ;
	memset(glbP.echiqPiece,-1,sizeof(glbP.echiqPiece[0])*NB_CASES);
	for(i=0;i<NB_CASES;i++) {
		SetContraintesForCase(&glbP,i);
	}
	memset(str,0,sizeof(START)) ;
    
    for(i=0;i<nbCaseOnEchiq;i++) {
        if(rg2case[i] == case139) {
            int j ;
            glbP.echiqPiece[case139] = echiq[case139] ;
            SetContraintesForCase(&glbP,case139);
            str->ord_rg2Case[str->nbCaseOnEchiq++] = case139 ;
            for(j=0;j<4;j++) {
                if(gDesc.voisinCase[4*case139+j] >= 0) {
                    SetContraintesForCase(&glbP,gDesc.voisinCase[4*case139+j]);
                }
            }
            break ;
        }
    }
	{
		int pivot = nbCaseOnEchiq - 1  ;
		while(str->nbCaseOnEchiq < nbCaseOnEchiq) {
			int j ;
			for(j=pivot;j>=0;j--) {
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
					while(glbP.echiqPiece[pivot] != -1) {
						pivot-- ;
					}
					break ;
				}
			}
		}
	}
	memcpy(str->echiq,glbP.echiqPiece,NB_CASES * sizeof(str->echiq[0])) ;
    str->ordMax = str->nbCaseOnEchiq ;
	return str ;
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

//********************** gestion des contraintes liees a une position globale
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

// cherche si iPieceRotate convient
static int ChkContraintesPieceRotate(T_COTE_CONSTRAINT cst,int iPieceRotate) {
    int ir =  ROTfromPIECEROT( iPieceRotate) ;
    {
        T_4COTE cv = gDesc.coteConstraints[iPieceRotate].cote4 ;
        T_4COTE xov = cst.cote4 ^ cv ; //egalite car cv n'est jamis nul
        T_4COTE yov = (((0x21212121 -cv ) & 0x20202020) >> 5) | cst.cote4  ; // cst.v == 0 && cv != 1 (pour chaque octet)
        T_4COTE tst = ((0x1f1f1f1f + xov) & (0x1f1f1f1f + yov)) & 0x20202020; // xov == 0 ou yov == 0 pour chaque octet
        if(tst==0) return ir ;
    }
    return -1 ;
}

static int ChkContraintes(T_COTE_CONSTRAINT cst,int iPiece) {
	return ChkContraintesIr(cst,iPiece,0) ;
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
	sprintf(conf->nameHist,"NHist_%04d%02d%02d@%02d%02d%02d.%s.hst",
		stTime->tm_year+1900,stTime->tm_mon+1,stTime->tm_mday,stTime->tm_hour,stTime->tm_min,stTime->tm_sec,ptNameStart);
	fh = fopen(conf->nameHist,"wt");
	if(fh == NULL) {
		fprintf(stderr,"Error open '%s'\n",conf->nameHist); exit(0);
	}
	return fh ;

}
//******************* Impressions diverses *************************
int VerifDeveloppment(DEVELOPPMENT *dev,int level) {
	int	VnbCaseForPiece[NB_PIECES] ;
	int VnbPieceForCase[NB_CASES] ;
	int VtbCxP[NB_PIECES*NB_CASES] ;
    int VtbCxPcand[NB_PIECES*NB_CASES] ;
	int VnbFreeCaseByPiece[NB_PIECES] ;
	int error =0 ;
	int nbPiece = 0 ;
	memset(VtbCxP,0,NB_PIECES*NB_CASES*sizeof(VtbCxP[0])) ;
	memset(VtbCxPcand,0,NB_PIECES*NB_CASES*sizeof(VtbCxPcand[0])) ;
    
	memset(VnbFreeCaseByPiece,0,NB_PIECES*sizeof(VnbFreeCaseByPiece[0])) ;
    
	{
        int i;
        for(i=0;i<NB_PIECES;i++) {
            VnbCaseForPiece[i] = 0 ;
            VnbPieceForCase[i] = 0 ;
        }
        for(i=0;i<NB_CASES;i++) {
            if(dev->glb.echiqPiece[i] >= 0) {
                VnbCaseForPiece[PIECEfromPIECEROT(dev->glb.echiqPiece[i])] += 256 ;
            }
        }
    }
    {
        int ip ;
        for(ip=0;ip<NB_PIECES;ip++) {
            int indFree = -1 ;
            {
                int ifr ;
                for(ifr=0;ifr<dev->nbFreePiece;ifr++) {
                    if(dev->listFreePiece[ifr] == ip) {
                        indFree = ifr ;
                        break ;
                    }
                }
            }
            if(VnbCaseForPiece[ip] > 0) {
                nbPiece++ ;
                if(VnbCaseForPiece[ip] > 256) {
                    error++ ; fprintf(stdout,"#### ERROR Piece %d placee %d fois \n",ip+1,VnbCaseForPiece[ip]/256) ;
                }
                if(indFree >= 0) {
                    error++ ;	fprintf(stdout,"#### ERROR Piece %d placee mais presente dans la liste free  rg=%d)\n",ip+1,indFree) ;
                }
            } else if(indFree < 0 ) {
                error++ ;	fprintf(stdout,"#### ERROR Piece %d non placee mais absente de la liste free \n",ip+1) ;
            }
        }
    }
	// verif liste free
	if(dev->nbFreePiece+nbPiece != NB_PIECES) {
		error++ ; fprintf(stdout,"#### taille liste Free %d != %d pieces non placee  \n",dev->nbFreePiece,NB_PIECES-nbPiece) ;
	}
	// verif liste triee
	{
		int k ;
        for(k=0;k<dev->nbCandidate;k++){
            int indp ;
            for(indp=0;indp<dev->sortCandCase[k].nbPieces;indp++) {
                int ifr ;
                int ip = PIECEfromPIECEROT( dev->listPieceRotateForCase[dev->sortCandCase[k].numCase][indp] ) ;
                int indFree = -1 ;
                VtbCxPcand[dev->sortCandCase[k].numCase*NB_PIECES+ip]++ ;
                for(ifr=0;ifr<dev->nbFreePiece;ifr++) {
                    if(dev->listFreePiece[ifr] == ip) {
                        indFree = ifr ;
                        break ;
                    }
                }
                if(indFree == -1) {
                    error++ ;
                    fprintf(stdout,"#### candidat rg=%d [c=%d,p=%x%c] utilisant une piece absente de la liste free \n",k
                            ,dev->sortCandCase[k].numCase,ip+1,'a' + ROTfromPIECEROT( dev->listPieceRotateForCase[dev->sortCandCase[k].numCase][indp] ) ) ;
                }
            }
            if(dev->glb.echiqPiece[dev->sortCandCase[k].numCase] != -1 ) {
                error++ ;
                fprintf(stdout,"#### case candidat rg=%d [c=%d,nbp=%d] pour une case deja occupe par la piece %d%c\n",k
                        ,dev->sortCandCase[k].numCase,dev->sortCandCase[k].nbPieces
                        ,PIECEfromPIECEROT(dev->glb.echiqPiece[dev->sortCandCase[k].numCase])+1 , 'a'+ ROTfromPIECEROT(dev->glb.echiqPiece[dev->sortCandCase[k].numCase])) ;
                
            }
            /*            if(k>0 && (dev->sortCandidate[k-1].cost > dev->sortCandidate[k].cost)) {
             error++ ;
             fprintf(stdout,"#### liste candidat mal trie rg=%d cost[c=%d,p=%d%c]=%d > cost[c=%d,p=%d%c]=%d  \n",k-1
             ,dev->sortCandidate[k-1].numCase,(dev->sortCandidate[k-1].pieceRotate & 255)+1,'a'+dev->sortCandidate[k-1].pieceRotate/256,dev->sortCandidate[k-1].cost
             ,dev->sortCandidate[k].numCase,(dev->sortCandidate[k].pieceRotate & 255)+1,'a'+dev->sortCandidate[k].pieceRotate/256,dev->sortCandidate[k].cost  ) ;
             }
             */        }
        
	}
    
    { int i;
        for(i=0;i<NB_CASES;i++) {
            int nbPos = 0 ;
            T_COTE_CONSTRAINT cst = GetContraintesForCase(&dev->glb,i) ;
            if(SetContraintesForCase(&dev->glb,i).cote4 != cst.cote4) {
                error++ ;
                fprintf(stdout,"#### ERROR Case %d contrainte %d != %d \n",i,dev->glb.caseConstraint[i].cote4,cst.cote4) ;
            }
            if(dev->glb.echiqPiece[i] >= 0) {  // piece prise
                if(- dev->indCandForCase[i] - 2  != dev->glb.echiqPiece[i]) {
                    error++ ;
                    fprintf(stdout,"#### occupation de la case %d differente %d != %d  \n",i,- dev->indCandForCase[i]-2,dev->glb.echiqPiece[i]) ;
                }
            } else {
                if(cst.cote4 == 0) {	// case libre sans contrainte
                    if(dev->indCandForCase[i] != -1) {
                        error++ ;
                        fprintf(stdout,"#### case %d libre mais status border incorrect %d  \n",i,dev->indCandForCase[i]) ;
                    }
                } else {
                    int nb = dev->sortCandCase[dev->indCandForCase[i]].nbPieces ;
                    {
                        int j ;
                        for(j=0;j<NB_PIECES;j++) {
                            if(VnbCaseForPiece[j] == 0) {
                                int ir = ChkContraintesIr( cst, j, 0);
                                while (ir >= 0) {
                                    VtbCxP[i*NB_PIECES+j]++ ;
                                    VnbFreeCaseByPiece[j]++ ;
                                    nbPos ++ ;
                                    ir = ChkContraintesIr(cst, j,ir+1);
                                }
                            }
                            if(VtbCxP[i*NB_PIECES+j] != VtbCxPcand[i*NB_PIECES+j]) {
                                error++ ;
                                fprintf(stdout,"#### la case %d bordure admet la piece libre %d en %d rotations mais %d candidats \n",i,j+1,VtbCxP[i*NB_PIECES+j],VtbCxPcand[i*NB_PIECES+j]) ;
                            }
                        }
                        
                    }
                    if(nb < 0 ) {
                        error++ ;
                        fprintf(stdout,"#### case %d non occupe avec contraintes mais status nombre de candidats negatif %d  \n",i,nb) ;
                    }
                    if(nbPos != nb) {
                        error++ ;
                        fprintf(stdout,"#### case %d bordure mais nb Possibililtes %d != exp %d  \n",i,nb,nbPos) ;
                    }
                    
                }
            }
        }
        
        for(i=0;i<NB_PIECES;i++) {
            if(VnbCaseForPiece[i] == 0 && VnbFreeCaseByPiece[i] != dev->nbCandForPiece[i]) {
                error++ ;
                fprintf(stdout,"#### piece %d appartenant a %d bordures au lieu de %d  \n",i+1,VnbFreeCaseByPiece[i],dev->nbCandForPiece[i]) ;
            }
        }
    }
	return error ;
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
                piece pc = tbPiece[PIECEfromPIECEROT( val[i*SIZE_ECHIQ+j] )] ;
                int ir = ROTfromPIECEROT( val[i*SIZE_ECHIQ+j] );
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

char * Format_Ident(void *ctx,int i,const char * format) {
	static char tmp[20] ;
	sprintf(tmp,format,i,' ') ;
	return tmp ;
}
char * Format_Piece(void *ctx,int i,const char * format) {
	static char tmp[20] ;
	int ip = ( i == -1) ? 0 : (PIECEfromPIECEROT(i)+1) ;
	char ic = 'a' + ( ( i == -1) ? 0 : ( ROTfromPIECEROT(i) ) ) ;
	sprintf(tmp,format,ip,ic);
	return tmp;
}

char * FRM_nbCand(void *dev,int i, const char * format) {
    static char tmp[20] ;
    int ip ;
    if(i <= -1) {
        ip = (i==-1) ? -1 : 256 ;
    } else {
        ip = ((DEVELOPPMENT *)dev)->sortCandCase[i].nbPieces ;
    }
	sprintf(tmp,format,ip);
	return tmp;
}



