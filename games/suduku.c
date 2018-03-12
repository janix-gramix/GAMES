#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define	SUDU_SIZE	9
#define SUDU_MASK_ALLOWED	((1<<SUDU_SIZE)-1)
#define SUDU_VALUE unsigned char
#define SUDU_VAL_ALLOW	unsigned int
#define REM_ALLOWED(allow,val)	(allow) &= SUDU_MASK_ALLOWED - (1<<(val))
#define IS_ALLOWED(allow,val)	((allow) & (1<<val))
#define	xTrue 1
#define xFalse 0
typedef struct SuduAllowed {
	int	nb ;
	SUDU_VAL_ALLOW allow ;
} SuduAllowed ;

typedef struct SuduData {
	SuduAllowed	allowed[SUDU_SIZE][SUDU_SIZE] ; // pour chaque case
	SuduAllowed allowI[SUDU_SIZE] ;				// pour chaque ligne
	SuduAllowed allowJ[SUDU_SIZE] ;				// pour chaque colonne
	SuduAllowed allowSQ[SUDU_SIZE] ;			// pour chaque carre
	int histoNbAllowed[SUDU_SIZE+1] ;
	int errI ;
	int errJ ;
	int isPrintSet ;
}	SuduData ;

typedef struct SuduInit {
	int i ;
	int j ;
	char val ; 
} SuduInit ;

SuduData * Sudu_Free(SuduData *sudu) {
	if(sudu != NULL) {
		free(sudu);
	}
	return NULL ;
}
SuduData * Sudu_Alloc(void) {
	SuduData * sudu = calloc(1,sizeof(sudu[0])) ;
	if(sudu == NULL) return Sudu_Free(sudu);
	{
		int i,j ;
		for(i=0;i<SUDU_SIZE;i++) {
			for(j=0;j<SUDU_SIZE;j++) {
				sudu->allowed[i][j].nb = 9 ;
				sudu->allowed[i][j].allow =  SUDU_MASK_ALLOWED ;
				sudu->histoNbAllowed[sudu->allowed[i][j].nb]++ ;
			}
			sudu->allowSQ[i].nb = 9 ;
			sudu->allowSQ[i].allow = SUDU_MASK_ALLOWED  ;
			sudu->allowI[i].nb = 9 ;
			sudu->allowI[i].allow = SUDU_MASK_ALLOWED  ;
			sudu->allowJ[i].nb = 9 ;
			sudu->allowJ[i].allow = SUDU_MASK_ALLOWED  ;
		}
	}
	return sudu ;
}
int Sudu_SetValue(SuduData *sudu,int i0,int j0, int val,char *motif) {
	if(sudu->allowed[i0][j0].nb == 0 || ((sudu->allowed[i0][j0].allow & (1<<val)) == 0) ) {
		sudu->errI = i0; sudu->errJ = j0 ;
		return xFalse ;
	}
	// deja traite
	if(motif!=NULL) fprintf(stderr,"%s[%d,%d]=%d ",motif,i0+1,j0+1,val+1);
	sudu->histoNbAllowed[sudu->allowed[i0][j0].nb]-- ;
	sudu->allowed[i0][j0].nb = 0 ;
	sudu->histoNbAllowed[sudu->allowed[i0][j0].nb]++ ;
	sudu->allowed[i0][j0].allow = 1<<val ;
	sudu->allowI[i0].nb-- ;
	REM_ALLOWED(sudu->allowI[i0].allow,val);
	sudu->allowJ[j0].nb-- ;
	REM_ALLOWED(sudu->allowJ[j0].allow,val);
	
	// mise a juor du carre
	{
		int ik = 3 * (i0/3) + j0/3 ;
		SuduAllowed *suduA = &sudu->allowSQ[ik] ;
		if(IS_ALLOWED(suduA->allow,val)) {
			REM_ALLOWED(suduA->allow,val);
			suduA->nb-- ;
		}
	}
	// on met a jour les valeurs autorisees pour les autres
	{	// en ligne
		int i ;
		for(i=0;i<SUDU_SIZE;i++) {
			if(i != i0) {
				SuduAllowed *suduA = &sudu->allowed[i][j0] ;
				if(IS_ALLOWED(suduA->allow,val)) {
					if(suduA->nb  <= 1) {
						sudu->errI = i; sudu->errJ = j0 ;
						return xFalse ; 
					}
					REM_ALLOWED(suduA->allow,val);
					sudu->histoNbAllowed[suduA->nb]-- ;
					suduA->nb-- ;
					sudu->histoNbAllowed[suduA->nb]++ ;
				}
			}
		}
	}
	{	// en colonne
		int j ;
		for(j=0;j<SUDU_SIZE;j++) {
			if(j != j0) {
				SuduAllowed *suduA = &sudu->allowed[i0][j] ;
				if(IS_ALLOWED(suduA->allow,val)) {
					if(suduA->nb  <= 1) {
						sudu->errI = i0; sudu->errJ = j ;
						return xFalse ; 
					}
					REM_ALLOWED(suduA->allow,val);
					sudu->histoNbAllowed[suduA->nb]-- ;
					suduA->nb-- ;
					sudu->histoNbAllowed[suduA->nb]++ ;
				}
			}
		}
	}
	{ // le carre 
		int i,j ;
		for(i=3*(i0/3);i<3*(i0/3+1);i++) {
			for(j=3*(j0/3);j<3*(j0/3+1);j++) {
				if(i!= i0 && j != j0) {
					SuduAllowed *suduA = &sudu->allowed[i][j] ;
					if(IS_ALLOWED(suduA->allow,val)) {
						if(suduA->nb  <= 1) {
							sudu->errI = i; sudu->errJ = j ;
							return xFalse ; 
						}
						REM_ALLOWED(suduA->allow,val);
						sudu->histoNbAllowed[suduA->nb]-- ;
						suduA->nb-- ;
						sudu->histoNbAllowed[suduA->nb]++ ;
					}
				}
			}
		}
	}
	return xTrue ;
}

int Sudu_Check(SuduData *sudu) {
	int val ;
	for(val=0;val<SUDU_SIZE;val++) {
		int masqVal = 1 << val ;
		// on verifie les lignes
		int i0,j0,k0 ;
		for(i0=0;i0<SUDU_SIZE;i0++) {
			if(sudu->allowI[i0].allow & masqVal) {
				int j, nb = 0,j1 ;
				for(j=0;j<SUDU_SIZE;j++) {
					if(sudu->allowed[i0][j].allow & masqVal) {
						nb++ ;
						j1= j ;
					}
				}
				if(nb == 1) {
					if(Sudu_SetValue(sudu,i0,j1,val,"LIG") == xFalse) {
						return xFalse ;
					}
				}
			}
		}
		// on verifie les colonnes
		for(j0=0;j0<SUDU_SIZE;j0++) {
			if(sudu->allowJ[j0].allow & masqVal) {
				int i, nb = 0,i1 ;
				for(i=0;i<SUDU_SIZE;i++) {
					if(sudu->allowed[i][j0].allow & masqVal) {
						nb++ ;
						i1= i ;
					}
				}
				if(nb == 1) {
					if(Sudu_SetValue(sudu,i1,j0,val,"COL") == xFalse) {
						return xFalse ;
					}
				}
			}
		}
		// on verifie les carres
		for(i0=0;i0<SUDU_SIZE;i0+=3) {
			for(j0=0;j0<SUDU_SIZE;j0+=3) {
				k0 = 3*(i0/3)+j0/3;
				if(sudu->allowSQ[k0].allow & masqVal) {
					int i,j,i1,j1, nb = 0 ;
					for(i=i0;i<i0+3;i++) {
						for(j=j0;j<j0+3;j++) {
							if(sudu->allowed[i][j].allow & masqVal) {
								nb++ ;
								i1= i ;
								j1 = j ;
							}
						}
					}
					if(nb == 1) {
						if(Sudu_SetValue(sudu,i1,j1,val,"CAR") == xFalse) {
							return xFalse ;
						}
					}
				}
			}
		}
	}
	if(sudu->histoNbAllowed[1] > 0) { 
		int i0,j0 ;
		for(i0=0;i0<SUDU_SIZE;i0++) {
			for(j0=0;j0<SUDU_SIZE;j0++) {
				if(sudu->allowed[i0][j0].nb == 1) {
					int k ;
					for(k=0;k<SUDU_SIZE;k++) {
						if(sudu->allowed[i0][j0].allow == 1<<k) {
							if(Sudu_SetValue(sudu,i0,j0,k,"VAL") == xFalse) {
								return xFalse ;
							}
						}
					}
				}
			}
		}
	}
    return xTrue ;
}
void  Sudu_print(FILE * fout,SuduData *sudu) {
	int i,j ;
	unsigned char line[2*SUDU_SIZE+1] ;
	for(i=0;i<SUDU_SIZE;i++) { line[2*i] = ' ' ; }
	line[2*SUDU_SIZE] = '\0' ;
	fprintf(fout,"\n\n");
	for(i=0;i<SUDU_SIZE;i++) {
		for(j=0;j<SUDU_SIZE;j++) {
			if(sudu->allowed[i][j].nb == 0) {
				int k ;
				for(k=0;k<SUDU_SIZE;k++) {
					if(sudu->allowed[i][j].allow == 1 << k) { break ; }
				}

				line[2*j+1] = '1' + k ;
			} else {
				if(sudu->allowed[i][j].nb == 1) {
					line[2*j+1] = '*' ;
				} else if(sudu->allowed[i][j].nb == 2) {
					line[2*j+1] = 'x' ;
				} else  {
					line[2*j+1] = '.' ;
				}
			}
		}
		fprintf(fout,"%s\n",line) ;
	}
	fprintf(fout,"\n");
	for(i=0;i<SUDU_SIZE+1;i++) {fprintf(fout,"[%d]=%d%c ",i,sudu->histoNbAllowed[i],(i==1) ? '*' : ((i==2) ? 'x' : ' '));}
	fprintf(fout,"\n");
	fprintf(fout,"Line: ");for(i=0;i<SUDU_SIZE;i++) {fprintf(fout,"%d ",sudu->allowI[i].nb);}
	fprintf(fout,"Col : ");for(i=0;i<SUDU_SIZE;i++) {fprintf(fout,"%d ",sudu->allowJ[i].nb);}
	fprintf(fout,"Sqr : ");for(i=0;i<SUDU_SIZE;i++) {fprintf(fout,"%d ",sudu->allowSQ[i].nb);}
	fprintf(fout,"\n");
	return ;
}

void  Sudu_out(FILE * fout,SuduData *sudu) {
    int i,j ;
    for(i=0;i<SUDU_SIZE;i++) {
        for(j=0;j<SUDU_SIZE;j++) {
            if(sudu->allowed[i][j].nb == 0) {
                int k ;
                for(k=0;k<SUDU_SIZE;k++) {
                    if(sudu->allowed[i][j].allow == 1 << k) { break ; }
                }
                
                fprintf(fout,"%c",'1' + k );
            } else {
                if(sudu->allowed[i][j].nb == 1) {
                    fprintf(fout,"*");
                } else if(sudu->allowed[i][j].nb == 2) {
                    fprintf(fout,"x");
                } else  {
                    fprintf(fout,".");
                }
            }
        }
        fprintf(fout,"\n") ;
    }
    fprintf(fout,"\n");
    return ;
}


static SuduInit SuduI[] = {
	{1,2,'4'},{1,4,'7'},{1,9,'5'}, 
	{2,1,'1'},
	{3,1,'5'},{3,5,'8'},{3,6,'3'},{3,9,'7'},

	{4,1,'8'},{4,4,'4'},{4,6,'5'},{4,9,'1'},
	{5,3,'5'},{5,7,'2'},
	{6,1,'3'},{6,4,'6'},{6,6,'7'},{6,9,'9'},

	{7,1,'2'},{7,4,'5'},{7,5,'1'},{7,9,'8'},
	{8,9,'6'},
	{9,1,'7'},{9,6,'9'},{9,8,'3'}

} ;
/*
static SuduInit SuduI[] = {
	{1,2,'6'},{1,5,'7'},{1,7,'2'}, 
	{2,3,'9'},{2,6,'1'},
	{3,3,'5'},{3,6,'3'},

	{4,5,'2'},{4,8,'3'},{4,9,'6'},
	{5,1,'1'},{5,5,'4'},{5,9,'5'},
	{6,1,'3'},{6,2,'8'},{6,5,'9'},

	{7,4,'8'},{7,7,'7'},
	{8,4,'9'},{8,7,'1'},
	{9,3,'1'},{9,5,'6'},{9,8,'4'}

} ;
*/
/*
static SuduInit SuduI[] = {
	{1,1,'1'},{1,5,'9'},{1,6,'2'},
	{2,2,'2'},{2,3,'4'},{2,9,'6'},
	{3,3,'6'},{3,7,'8'},{3,8,'3'},

	{4,7,'3'},
	{5,3,'2'},{5,4,'9'},{5,7,'4'},
	{6,1,'6'},{6,2,'1'},{6,6,'4'},{6,8,'8'},

	{7,2,'8'},{7,8,'9'},
	{8,5,'2'},{8,9,'8'},
	{9,3,'5'},{9,5,'7'},{9,7,'2'}

} ;

*/
int Sudu_Level1(FILE *fout,SuduData *sudu) {
	int nbFind ;
	do {
		nbFind = sudu->histoNbAllowed[0] ;
		if(Sudu_Check(sudu) == xFalse) {
			if(fout != NULL) Sudu_print(fout,sudu);
			return xFalse ;
		} else {
			if(fout != NULL)  Sudu_print(stderr,sudu);
		}
	} while ( (sudu->histoNbAllowed[0]  < 81) && (sudu->histoNbAllowed[0] > nbFind) ) ;
	return xTrue ;
}

int main(int argc, char **argv) {
    FILE * grilleIn = stdin ;
	SuduData *sudu = Sudu_Alloc() ;
	int xOK= xTrue ;
    int isFile = 0 ;
	/*{
		int i ;
		for(i=0;i<sizeof(SuduI)/sizeof(SuduInit);i++) {
			if(Sudu_SetValue(sudu,SuduI[i].i-1,SuduI[i].j-1,SuduI[i].val-'1') == xFalse) {
				xOK = xFalse ;
				break ;
			}
		}
	}*/
    if(argc > 1) {
        grilleIn = fopen(argv[1],"rt");
        if(grilleIn == NULL) {
            fprintf(stderr,"Fail to open %s\n",argv[1]);
        }
        isFile = 1 ;
    }
    
	{
		int i;
		SuduData *stry = Sudu_Alloc() ;
		stry->isPrintSet = 0 ;
		for(i=0;i<9;i++) {
			char line[20] ;
			int xOK = 0 ;
			do {
				int j ;
				*stry = *sudu ;
				do {
					if(!isFile) fprintf(stderr,"\r ligne %d : ",i+1);
					fscanf(grilleIn,"%s",line);
					j = strlen(line);
				} while(j!=9) ;
				for(j=0;j<9;j++) {
					if(line[j] != '0') {
						if(Sudu_SetValue(stry,i,j,line[j]-'1',NULL) == xFalse) {
							xOK = xFalse ;
							break ;
						}
					}
				}
				if(j==9)xOK = xTrue ;
			} while( xOK == 0) ;
			*sudu = *stry ;
		}
	}

	Sudu_print(stderr,sudu);
	sudu->isPrintSet = 1 ;
	if(xOK == xFalse) {
		fprintf(stderr,"!!!! GRILLE INCORRECTE [%d,%d] ??\n",sudu->errI+1,sudu->errJ+1);
	}
	if(Sudu_Level1(stderr,sudu) == xFalse) {
		fprintf(stderr,"!!!! GRILLE INCORRECTE [%d,%d] ??\n",sudu->errI+1,sudu->errJ+1);
        Sudu_out(stdout,sudu) ;
		exit(0);
	}
	if(sudu->histoNbAllowed[0] == 81) {
        Sudu_out(stdout,sudu) ;
		fprintf(stderr,"!!! FINI !!! \n");
 	} else {
		if(sudu->histoNbAllowed[2] > 0) {
			int i , j ;
			SuduData *stry = Sudu_Alloc() ;
			for(i=0;i<SUDU_SIZE;i++) {
				for(j=0;j<SUDU_SIZE;j++) {
					if(sudu->allowed[i][j].nb == 2) {
						int k1 , k2 ;
						for(k1=0;k1<SUDU_SIZE;k1++) {
							if(sudu->allowed[i][j].allow & (1<<k1)){
								for(k2=k1+1;k2<SUDU_SIZE;k2++) {
									if(sudu->allowed[i][j].allow & (1<<k2)) break ;
								}
								break;
							}
						}
						*stry = *sudu ;
						// on essaie k1
						fprintf(stderr,"\n");
						Sudu_SetValue(stry,i,j,k1,"\nHYP1==>");
						fprintf(stderr," =>");
						if(Sudu_Level1(NULL,stry) == xTrue) {
							if(stry->histoNbAllowed[0] == 81) {
								fprintf(stderr,"\n HYP1-WIN[%d,%d]=%d +++++++++++++++++++++\n",i+1,j+1,k1+1);
								Sudu_SetValue(sudu,i,j,k1,"WIN");
								Sudu_Level1(stderr,sudu);
								fprintf(stderr,"!!! FINI  !!! \n");
                                Sudu_out(stdout,sudu) ;
								exit(0);
							} else {
								*stry = *sudu ;
								Sudu_SetValue(stry,i,j,k2,"\n +HYP2==>");
								if(Sudu_Level1(NULL,stry) == xFalse) {
									fprintf(stderr,"\n -HYP2-FAIL[%d,%d]=%d forbidden(+%d) => [%d,%d]=%d -------------------\n"
										,i+1,j+1,k2+1,stry->histoNbAllowed[0]-sudu->histoNbAllowed[0],i+1,j+1,k1+1);
									// k2 faux c'est k1
									Sudu_SetValue(sudu,i,j,k1,"HYP-OK");
									Sudu_Level1(stderr,sudu);
								} else {
									if(stry->histoNbAllowed[0] == 81) {
										fprintf(stderr,"\n  HYP2-WIN[%d,%d]=%d its wins ++++++++++++++++++++\n",i+1,j+1,k2+1);
										Sudu_SetValue(sudu,i,j,k2,"WIN");
										Sudu_Level1(stderr,sudu);
										fprintf(stderr,"!!! FINI  !!! \n");
                                        Sudu_out(stdout,sudu) ;
										exit(0);
									} else {
										fprintf(stderr,"\n HYP-AMBIG[%d,%d]=%d ou %d Ambigu ????????????????\n",i+1,j+1,k1=1,k2+1);
									}
								}
							}
						} else {
							fprintf(stderr,"\n -HYP1-FAIL[%d,%d]=%d forbidden(+%d) => [%d,%d]=%d ------------------\n"
								,i+1,j+1,k1+1,stry->histoNbAllowed[0]-sudu->histoNbAllowed[0],i+1,j+1,k2+1);
							// k1 faux c'est k2
							Sudu_SetValue(sudu,i,j,k2," HYP-OK");
							Sudu_Level1(stderr,sudu);
							if(sudu->histoNbAllowed[0] == 81) {
								fprintf(stderr,"!!! FINI !!! \n");
                                Sudu_out(stdout,sudu) ;
								exit(0);
							}
						}
					}
				}
			}
		}
	}
}
