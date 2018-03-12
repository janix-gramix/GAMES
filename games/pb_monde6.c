//
//  pb_monde6.c
//  Matrix
//
//  Created by Jeannot on 03/05/13.
//  Copyright (c) 2013 Jeannot. All rights reserved.
//
#define MAX_NBDIGIT 20
#include <stdio.h>
#include <stdlib.h>
int main (int argc, char **argv) {
    int nbDigit = 0 ;
    int isRule[10] ;
    int Digits[MAX_NBDIGIT] ;
    char * rules = "1234567890" ;
    if (argc > 1) {
        rules = argv[1] ;
    }
    {
        int i; char * c ;
        for(i=0;i<10;i++) isRule[i] =0 ;
        for(c=rules;*c != 0 ; c++) {
            int j = *c - '0' ;
            if(j < 10) isRule[j] = 1 ;
        }
        printf("Rules : %s => ",rules);
    }
    nbDigit = 0 ;
    while (1) {
        int nd ;
        for(nd=0;nd < nbDigit; nd++) {
            if ( Digits[nd] != 9 ) {
                Digits[nd]++ ; break ;
            } else {
                Digits[nd] = 0 ;
            }
        }
        if(nd == nbDigit ) {
            if(nbDigit == MAX_NBDIGIT-1) { break ; }
            Digits[nbDigit++] = 1 ;
        }
        // test regle 1
        if(isRule[1]){
            int i ;
            int isOne = 0 ;
            int isDecrease= 1 ;
            for(i=0;i<nbDigit;i++) {  if(Digits[i] == 1) {  isOne = 1;   break ;  }        }
            for(i=0;i<nbDigit-1;i++) {  if(Digits[i] > Digits[i+1]) {  isDecrease = 0 ;   break ;     }
            }
            if(isOne ^ isDecrease) { continue ;   }
        }
        // test regle 2
        if(isRule[2]){
            int i ;
            int isTwo = 0 ;
            int nbOdd = 0 ;
            int is2odd = 0 ;
            for(i=0;i<nbDigit;i++) {  if(Digits[i] == 2) {  isTwo = 1 ;   break ;  }        }
            for(i=0;i<nbDigit;i++) {  if ( Digits[i] & 1 ) { nbOdd++ ;   } }
            if(nbOdd >= 2 ) { is2odd = 1 ; }
            if(isTwo ^ is2odd) { continue ;   }
        }
        // test regle 3
        if(isRule[3]){
            int i ;
            int isThree = 0 ;
            int hist[10] ;
            int isDifferent = 1 ;
            for(i=0;i<10;i++) { hist[i] = 0 ; }
            for(i=0;i<nbDigit;i++) {  if(Digits[i] == 3) {  isThree = 1 ;   break ;  }        }
            for(i=0;i<nbDigit;i++) {  if( hist[Digits[i]] ) { isDifferent =0; break ; } else { hist[Digits[i]] = 1 ; } }
            if(isThree ^ isDifferent) { continue ;   }
        }
        // test regle 4
        if(isRule[4]){
            int i ;
            int isFour = 0 ;
            int is4even = ( (nbDigit >= 4)  && ((Digits[nbDigit-4] & 1)==0 ) ) ? 1 : 0 ;
            for(i=0;i<nbDigit;i++) {  if(Digits[i] == 4) {  isFour = 1 ;   break ;  }        }
            if(isFour ^ is4even) { continue ;   }
        }
        // test regle 5
        if(isRule[5]){
            int i ;
            int isFive = 0 ;
            int isNotMult5 = 1 ;
            for(i=0;i<nbDigit;i++) {  if(Digits[i] == 5) {  isFive = 1 ;   break ;  }        }
            for(i=0;i<nbDigit;i++) {  if(Digits[i] == 0 || Digits[i] == 5 ) {  isNotMult5 = 0 ;   break ;     } }
            if(isFive ^ isNotMult5) { continue ;   }
        }
        // test regle 6
        if(isRule[6]){
            int i ;
            int isSix = 0 ;
            int is3odd = 0 ;
            for(i=0;i<nbDigit;i++) {  if(Digits[i] == 6) {  isSix = 1 ;   break ;  }        }
            for(i=0;i<nbDigit-2;i++) {  if((Digits[i] & 1 )  &&  (Digits[i+1] & 1 ) && (Digits[i+2] & 1 ) ) {  is3odd = 1 ;   break ;     } }
            if(isSix ^ is3odd) { continue ;   }
        }
        // test regle 8
        if(isRule[8]){
            int i ;
            int isEight = 0 ;
            int isNot3even = 1 ;
            for(i=0;i<nbDigit;i++) {  if(Digits[i] == 8) {  isEight = 1 ;   break ;  }        }
            for(i=0;i<nbDigit-2;i++) {  if(((Digits[i] & 1)  == 0 )  &&  ((Digits[i+1] & 1) == 0 ) && ((Digits[i+2] & 1) == 0 ) ) {  isNot3even = 0 ;   break ;     } }
            if(isEight ^ isNot3even) { continue ;   }
        }
        // test regle 9
        if(isRule[9]){
            int i ;
            int isNine = 0 ;
            int nb3 = 0  ;
            int nb5 = 0 ;
            int nb7 = 0 ;
            int isProductSquare = 1 ;
            for(i=0;i<nbDigit;i++) {  if(Digits[i] == 9) {  isNine = 1 ;   break ;  }        }
            for(i=0;i<nbDigit;i++) {  if(Digits[i] == 3) nb3++ ; else if(Digits[i] == 5) nb5 ++ ; else if(Digits[i] == 7) nb7 ++ ;  }
            if ( (nb3 & 1) || (nb5 & 1) || (nb7 & 1) ) { isProductSquare = 0 ; }
            if(isNine ^ isProductSquare) { continue ;   }
        }
        // test regle 0
        if(isRule[9]){
            int i ;
            int isZero = 0 ;
            int sum   ;
            int isSum = 0 ;
            for(i=0;i<nbDigit;i++) {  if(Digits[i] == 0 ) {  isZero = 1 ;   break ;  }        }
            sum = 0 ; for(i=0;i<nbDigit;i++) {  sum += Digits[i] ;  }
            for(i=0;i<nbDigit;i++) {  if( sum ==  Digits[i] * 2) { isSum++ ;  } }
            if(isSum > 1) isSum = 0 ;
            if(isZero ^ isSum ) { continue ;   }
        }

        
        { int i ; for(i=nbDigit-1;i>=0;i--) { printf("%d",Digits[i]); } }
        printf("\n");
        break ;
    }
}
