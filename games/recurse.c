//
//  recurse.c
//  recurse
//
//  Created by Jeannot on 18/01/2018.
//  Copyright © 2018 Jeannot. All rights reserved.
// Avec optimisation (a partir du niveau -O1), la fonction SumN_termrecur (terminale récursive) est optimisee et transformee en boucle
// La fonction SumN_recur ne l'est pas et provoque un stackoverflow pour N= 1000000
// On peut le verifier en debug en mettant des breakpoints sur les instructions compteur++
#include <stdio.h>
#include <stdlib.h>

int compteur = 0 ;


int64_t SumN_recur(int n) {
    if ((n % 100000) == 0) {
        compteur++ ;
    }
    if (n <= 1) return n ;
    else {
        return SumN_recur(n-1)+n;
    }
}

int64_t SumN_termrecur(int n, int64_t aux) {
    if ((n % 100000) == 0) {
        compteur++ ;
    }
    if(n  <= 1) {
        return aux ;
    } else {
        return SumN_termrecur(n-1,aux+n ) ;
    }
}


int main(int argc, const char * argv[]) {
    int isTermRecur = 0 ;
    int N = 1 ;
    int64_t Sum ;
    // insert code here...
    if(argc < 2) {
        fprintf(stdout,"IsTerminalRecursive(0/1) N(size)1 ?\n");
        scanf("%d %d",&isTermRecur,&N) ;
    } else {
        isTermRecur = atoi(argv[1]) ;
        N = atoi(argv[2]) ;
    }
    if(isTermRecur) {
        Sum = SumN_termrecur(N,1);
    } else {
        Sum = SumN_recur(N);
    }
    fprintf(stdout,"Somme de 1 a %d = %lld, compteur = %d\n",N,Sum,compteur) ;
    return 0;
}

