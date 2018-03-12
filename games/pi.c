
/*

a[52514],b,c=52514,d,e,f=1e4,g,h;
main(){
    for(;b=c-=14;h=printf("%04d",e+d/f))
        for(e=d%=f; g=--b*2;d/=g) d=d*b+f*(h?a[b]:f/5),a[b]=d%--g;}
*/
 



#include <stdio.h>

int a[52514];
int main(void){
    int i,c ;
    int e= 0, d = 0 ;
    for(i = 52514  ; --i ; ) {
        d= d * i + 10000 * 2000 ;
        a[i]= d % (2 * i -1);
        d /= (2 * i - 1) ;
    }
    printf("%04d",e+d/10000) ;
    
    for( c=3750 ;--c ; ) {
        d = d % 10000 ;
        e = d ;
        for(i = c * 14 ; --i ; ) {
            d= d * i + 10000 * a[i]  ;
            a[i]= d % (2 * i - 1);
            d /= (2 * i -1) ;
        }
        printf("%04d",e+d/10000) ;
    }
}




/*
#include <stdio.h>

#define NB  3
#define NBD 11

int nbD = NBD ;
int a[NBD+1 ];
int main(void){
    int i,c ;
    int e= 0, d = 0 ;
    for(i = nbD  ; --i ; ) {
        d= d * i + 10 * 2 ;
        printf("\ni=%d d=%3d",i,d) ;
        a[i]= d % (2 * i -1);
        d /= (2 * i - 1) ;
        printf(",%2d ",d) ;
        { int k ;for(k=1;k<nbD;k++) { printf("%2d%c ",a[k], (k==i) ? '*' : ' ');}}
    }
    printf("%01d",e+d/10) ;
    
    for( c=NB ;--c ; ) {
        d = d % 10 ;
        e = d ;
        for(i = NBD ; --i ; ) {
            d= d * i + 10 * a[i]  ;
            printf("\ni=%d d=%3d",i,d) ;
            a[i]= d % (2 * i - 1);
            d /= (2 * i - 1) ;
            printf(",%2d ",d) ;
            { int k ;for(k=1;k<nbD;k++) { printf("%2d%c ",a[k], (k==i) ? '*' : ' ');}}
        }
        printf("%01d",e+d/10) ;
    }
}
*/