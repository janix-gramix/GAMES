//
//  main.c
//  tst_reorgbuddy
//  Created by Jeannot on 16/04/13.
//  Copyright (c) 2013 Jeannot. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "xos.h"


#define BUDDY_EXP   4


// version pour le rangement suivant
/*
             1
      2             3
  4      5      10      11
6  7   8  9   12  13  14   15
*/
static  uint32_t msb_2(uint32_t x) {
    if (x ==0) {
        return 0 ;
    } else {
        int expP = 0 ;
        if(x & 0xffff0000 ) {
            expP += 16 ;
        }
        if((x >> expP)  & 0xff00 ) {
            expP += 8 ;
        }
        if((x >> expP) & 0xf0 ) {
            expP += 4 ;
        }
        if((x >> expP) & 0xc ) {
            expP += 2 ;
        }
        if ( (x >> expP) & 0x2) {
            return ++ expP ;
        } else {
            return expP ;
        }
    }
}


static int nbLoop =0 ;





static uint32_t Buddy(uint32_t y ) {
    return y ^ ( (y ^ (y-1)) +1 ) ;
}

static uint32_t BuddyFindFather(uint32_t y) {
    return (y + Buddy(y)) >> 1;
}

static uint32_t BuddyDown(uint32_t y) {
    if (y & 1) return -1 ;
    else return (y + (y & (y-1)) ) >> 1;
}

static uint32_t Size(uint32_t y) {
    return ((y ^ (y-1)) +1 ) >> 1;
}



#   define OFFSET_TO_INDEX(addr)           ((addr != 0) ?  ( addr + (addr | (addr -1))+1) : (1 << BUDDY_EXP)) 

uint32_t IndexToAddr(uint32_t y) {
    return ((y & (y-1)) +1 ) >> 1 ;
}


int main(int argc, const char * argv[]) {
    int i ;
 
    printf("index->father\n");
    for(i=2;i<(1<< (BUDDY_EXP+1)); i++) {
        printf("%d->%d%c",i,BuddyFindFather(i),(i % 16) == 0 ? '\n' : ' ');
    }
    printf("\nindex->down\n");
    for(i=1;i<(1<< (BUDDY_EXP+1)); i++) {
        printf("%d->%d%c",i,BuddyDown(i),(i % 16) == 0 ? '\n' : ' ');
    }
    printf("\naddr->Hindex\n");
    for(i=0;i<(1<< (BUDDY_EXP)); i++) {
        printf("%d->%d%c",i,OFFSET_TO_INDEX(i),(i % 16) == 15 ? '\n' : ' ');
    }
    printf("\nindex->addr\n");
    for(i=1;i<(1<< (BUDDY_EXP+1)); i++) {
        printf("%d->%d:%d%c",i,IndexToAddr(i) , Size(i),(i % 16) == 0 ? '\n' : ' ');
    }

    printf("\n nbLoop=%d\n",nbLoop);
}

