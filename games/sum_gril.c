//
//  main.c
//  sum_gril
//
//  Created by Jeannot on 12/04/13.
//  Copyright (c) 2013 Jeannot. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>


// 0  1  2
// 3  4  5
// 6  7  8



typedef struct Permut {
    int size ;
    int * permut ;
    int * indPerm ;
    int is ;
} Permut ;

Permut * PermutFree(Permut * prm) {
    if(prm) {
        free(prm->indPerm);
        free(prm->permut);
        free(prm);
    }
    return NULL ;
}

void PermutRewind(Permut * prm) {
    int i ;
    for(i=0;i<prm->size-1;i++) {
        prm->permut[i] = i ;
        prm->indPerm[i] = i ;
    }
    prm->permut[prm->size-1] = prm->size-1 ;
    prm->is=0 ;
    return ;
}

Permut * PermutAlloc(int size) {
    if(size < 2) {
        return NULL ;
    } else {
        Permut * prm = calloc (sizeof(Permut),1) ;
        if(prm == NULL) return NULL ;
        prm->size = size ;
        prm->permut = malloc(size * sizeof(prm->permut[0])) ;
        prm->indPerm = malloc((size-1) * sizeof(prm->indPerm[0])) ;
        if(prm->permut == NULL || prm->indPerm == NULL) {
            return PermutFree(prm) ;
        } else {
            PermutRewind(prm) ;
            return prm ;
        }
    }
}

const int * PermutNext ( Permut * prm) {
    int is = prm->is ;
    int * permut = prm->permut ;
    int * indPerm = prm->indPerm ;
    if(is == prm->size - 1  ) { // if faut redescendre
        while(is-- >= 0){
            if(indPerm[is] != is) {
                int ip = permut[is] ;
                permut[is] = permut[indPerm[is]] ;
                permut[indPerm[is]] = ip ;
            }
            if( ++indPerm[is]  < prm->size ) {
                break ;
            } else {
                indPerm[is] = is ;
            }
        }       
    }
    while(is >= 0) {
        if(indPerm[is] != is) {
            int ip = permut[is] ;
            permut[is] = permut[indPerm[is]] ;
            permut[indPerm[is]] = ip ;
        }
        if(++is == prm->size-1){
            // on est au bout, on sauve is et on retouren la nouvelle permut
            prm->is = is ;
            return prm->permut ;
        }
    }
    prm->is = is ;
    return NULL ;
}



static void val(int * x , int i) {
    switch (i) {
        case 0 : x[0] =  x[1] + x[3] + x[4]  ; break ;
        case 1 : x[1] = x[0] + x[2] + x[3] + x[4] + x[5] ; break ;
        case 2 : x[2] = x[1] + x[4] + x[5] ; break ;
        case 3 : x[3] =  x[0] + x[1] + x[4] + x[6] + x[7]  ; break ;
        case 4 : x[4] =  x[0] + x[1] + x[2] + x[3] + x[5] + x[6] + x[7] + x[8] ; break ;
        case 5 : x[5] =  x[1] + x[2] + x[4] + x[7] + x[8] ; break ;
        case 6 : x[6] =  x[3] + x[4] + x[7] ; break ;
        case 7 : x[7] =  x[3] + x[4] + x[5] + x[6] + x[8] ; break ;
        case 8 : x[8] =  x[7] + x[4] + x[5] ; break ;
        default: break ;
    }
}


#define SIZE_PERMUT 9

#define CHK_DIFF

int main(int argc, const char * argv[]) {
#if defined(CHK_DIFF)
    static int isValFound[57] ;
    int isNewVal ;
#else
    int sumMax= 0 ;
#endif
    Permut * prm = PermutAlloc(SIZE_PERMUT) ;
    const int * permut ;
    while((permut = PermutNext(prm)) != NULL) {
        int x[SIZE_PERMUT] ;
        int i ;
        x[permut[0]] = 1;
        x[permut[1]] = 1 ;
        for (i=2 ; i<9 ; i++) { x[permut[i]] = 0 ; }
        for (i=2 ; i<9 ; i++) { val(x,permut[i]) ; }
#if defined(CHK_DIFF)
        isNewVal =0 ;
        for(i=0;i<9;i++) {
            if(isValFound[x[i]] == 0 ) {
                printf("V=%d ",x[i]) ;
                isNewVal = 1;
            }
            isValFound[x[i]]++ ;
        }
        if( isNewVal ) {
            printf(" => ");
#else
        if ( x[permut[8]] >= sumMax ) {
            sumMax= x[permut[8]] ;
#endif
            for (i=0;i<9; i++) { printf ("%d ", permut[i]) ; }
            printf("\n");
            for (i=0;i<9; i++) {
                printf("%2d%c", x[i], ((i % 3 ) == 2) ? '\n' : ' ' );
            }
            printf("\n");
        }       
    }
    prm = PermutFree(prm);
#if defined(CHK_DIFF)
        {
            int i ;
            for(i=0;i<=57;i++) { printf("%d x %d%c",i,isValFound[i],(i%10)==0 ? '\n' : '\t') ; }
            printf ("\n");
            
        }
#endif
    return 0;
}

