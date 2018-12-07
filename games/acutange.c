//
//  acutange.c
//
//  Created by Jeannot on 28/11/2018.
//
//

#include <stdio.h>
#include <stdlib.h>

#define NB_POINTS  35
#define NB2 (NB_POINTS*NB_POINTS)
typedef struct ACP {
    int16_t x ;
    int16_t y ;
} ACP ;

typedef struct ACUT {
    ACP A ;
    ACP B ;
    ACP C ;
    
} ACUT ;
#define INT32(A) (((A).y * NB_POINTS) + (A).x)


#define SCALAR(A,B,C)   (((B).x-(A).x)*((C).x-(A).x)+((B).y-(A).y)*((C).y-(A).y))

int main(int argc, char **argv) {
    int nbAC = 0 ;
    int nbP = NB2 ;
    ACUT T ;
    int *HistoP = calloc(NB2,sizeof(HistoP[0])) ;
    int *savHistoP = malloc(NB2*sizeof(savHistoP[0])) ;
    int *bestHistoP = malloc(NB2*sizeof(bestHistoP[0])) ;
    ACUT * act = malloc(NB2*NB2*NB2/6*sizeof(act[0])) ;
    int * P2T = malloc(NB2*NB2*NB2/4*sizeof(P2T[0]));
    for(T.A.x=0;T.A.x<NB_POINTS;T.A.x++){
        for(T.A.y=0;T.A.y<NB_POINTS;T.A.y++){
            int iA = INT32(T.A) ;
            for(T.B.x=T.A.x;T.B.x<NB_POINTS;T.B.x++){
                for(T.B.y= (T.B.x==T.A.x) ? T.A.y : 0;T.B.y<NB_POINTS;T.B.y++){
                    int iB = INT32(T.B) ;
                    for(T.C.x=T.B.x;T.C.x<NB_POINTS;T.C.x++){
                        for(T.C.y = (T.C.x==T.B.x) ? T.B.y : 0;T.C.y<NB_POINTS;T.C.y++){
                            if(SCALAR(T.A,T.B,T.C) > 0 && SCALAR(T.B,T.C,T.A) > 0 && SCALAR(T.C,T.A,T.B) > 0) {
                                int iC = INT32(T.C) ;
                                P2T[iA*NB2*NB2/4+HistoP[iA]++]= nbAC ;
                                P2T[iB*NB2*NB2/4+HistoP[iB]++]= nbAC ;
                                P2T[iC*NB2*NB2/4+HistoP[iC]++]= nbAC ;
//                                HistoP[iA]++ ; HistoP[iB]++ ; HistoP[iC]++ ;
                                act[nbAC++] = T ;
                            }
                        }
                    }
                }
            }
        }
    }
    int i ;
    for(i=0;i<NB2;i++) {
        if(HistoP[i] == 0)nbP-- ;
    }
    double bestPn = (nbAC*6.0)/((int64_t)nbP*nbP*nbP) ;
    printf("NB acutangle=%d / %d %.3f \n",nbAC,nbP,bestPn );
    for(i=0;i<NB2;i++) {
        printf("%4d%c",HistoP[i], (i % NB_POINTS) == NB_POINTS-1 ? '\n' : ' ') ;
    }
    memcpy(savHistoP,HistoP,NB2*sizeof(HistoP[0])) ;
    while(nbP > 3) {
        int iMin=0 ;
        int nbMin = NB2*NB2 ;
        for(i=0;i<NB2;i++) {
            if(HistoP[i] && HistoP[i]<nbMin) {
                iMin = i;
                nbMin = HistoP[i] ;
            }
        }
        // on va supprimer iMin
        int k ;
        for(k=0;k<savHistoP[iMin];k++) {
            int iT = P2T[iMin*NB2*NB2/4+k] ;
             ACUT T = act[iT] ;
            if(HistoP[INT32(T.A)]>0 && HistoP[INT32(T.B)]>0 && HistoP[INT32(T.C)]>0) {
 //               printf("[(%d,%d)(%d,%d)(%d,%d)]",T.A.x,T.A.y,T.B.x,T.B.y,T.C.x,T.C.y);
                HistoP[INT32(T.A)]-- ; if(HistoP[INT32(T.A)]==0) nbP-- ;
                HistoP[INT32(T.B)]-- ; if(HistoP[INT32(T.B)]==0) nbP-- ;
                HistoP[INT32(T.C)]-- ; if(HistoP[INT32(T.C)]==0) nbP-- ;
                nbAC-- ;
            }
            
        }
        double Pn = (nbAC*6.0)/(nbP*(int64_t)nbP*nbP) ;
        if(Pn > bestPn) {
            bestPn = Pn ;
            memcpy(bestHistoP,HistoP,NB2*sizeof(HistoP[0])) ;
            printf("\nRem(%d,%d) acutangle=%d  nbP=%d %.3f \n",iMin % NB_POINTS,iMin/NB_POINTS,nbAC,nbP,Pn );
            for(i=0;i<NB2;i++) {
                printf("%4d%c",HistoP[i], (i % NB_POINTS) == NB_POINTS-1 ? '\n' : ' ') ;
            }
        }
    }
}
