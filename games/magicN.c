

#include <stdio.h>
#include <stdlib.h>

#define TORDU
// nb d'essai max de test
#define NB_MAX_TRIAL 20

// retourne la parite (0 ou 1) des bits de k
int Parity (u_int64_t k) {
    while ( k > 3) {
        k = (k >> 2) ^ (k & 3) ;
    }
    return (int) (k>>1) ^ ( k & 1) ;
}

// on suppose 2 << N cases.
// retourne le numero de case associee a un choix des valeurs des cases v
// Si F(N,v) designe la fonction
// On la calcule par recurrence sur l'exposant N
//  F(N+1,v) = 2 * F(N,vH ^ vL) + Parity(vH) où vL sont les 2<<N bits de poids faible de v ,et vH les 2<<N bits de poids fort.
//
// si l'on suppose que F(N,.) verifie que pour tout v les 2<<N voisins de v (en changeant un bit de v) prennent toutes les valeurs [0,...2<<N[
// alors pour un v donné, et un numéro de case 2*k+p ( k appartient a [0  (1<<N) [  ; p=0 ou 1).
// d'apres il existe un bit a changer dans pour vH ^ vL que F(N, (vH ^ vL)*) == k. Comme ce bit peut etre changé dans vL ou vH on choisit pour obtenir p. CQFD


int NumCase(u_int64_t v,int N){

#if defined(TORDU)
    static int numCase[16] = {0,1,3,1,2,2,3,0,0,3,2,2,1,3,1,0} ;
#endif
    {
        int i ;
        int n = 0 ;
#if defined(TORDU)
        for(i=0;i< N-2;i ++) { //boucle sur l'exposant du nombre de case
#else
        for(i=0;i< N;i ++) { //boucle sur l'exposant du nombre de case
#endif
            int ie = 1 << (N-1-i) ;
            u_int64_t iee = ((u_int64_t)1) << ie ; // pour extraire les bits de poids faibles
            u_int64_t vLow = v & ( iee - 1) ;
            u_int64_t vHigh = v >> ie ;
            v = vLow ^ vHigh ; // ordre inferieur
            n += Parity(vLow) << i ; // parite decalee en suivant la recurence
        }
#if defined(TORDU)
        n += numCase[v] << (N-2) ;
#endif
    return n ;
    }
}

// verifie pour l'exposant N
// que les 2<<N voisins de v prennentpour numéro de case toutes les valeurs [0, ... 2 <<N[
//
void Check(u_int64_t v,int N) {
    int nbCase = 1 << N ;
    int *voisin=calloc(sizeof(voisin[0]),nbCase); // callob iniialise a zero
    char *strBin = malloc(nbCase+1)  ; // pour decomposition en binaire de v
    int isOK= 1;
    int case_B = 0 ;
    int k ;
    int num_v = NumCase(v,N) ;
    strBin[nbCase] = 0 ;
    for(k=0;k<nbCase;k++) {
        int numVoisin = NumCase(v ^ (((u_int64_t)1)  << k),N) ; // on inverse le bit de rang k et l'on calcule le numero du voisin
        if(numVoisin == num_v) { case_B = k ; }
        strBin[k] = ( v & (((u_int64_t)1)  << k) ) ? '1' : '0' ;
        if(voisin[numVoisin] == 0) { // numero deja trouve ?
            voisin[numVoisin]++ ; // non, OK on le marque trouve.
        } else {
            isOK = 0 ; // erreur un unmero trouve 2 fois ou plus.
            break ;
        }
    }
    // Comme 2<<N voisins et 2<<N numéros,si aucun numero n'a ete trouvé 2 fois, chacun a ete trouve une et une seule fois
    printf(" %s -> %2d %s  %d",strBin,NumCase(v,N),isOK ? "OK" : "ERROR",nbCase-1-case_B) ;
    strBin[case_B] = '1'+'0' - strBin[case_B] ;
    printf("  B=%s\n",strBin) ;
    free(voisin);
    free(strBin );
    
}
//
int main(int argc, char **argv){
    int N = 6 ;
    int nbCase ;
    if(argc > 1) {
        N = atoi(argv[1]) ;
        if(N > 6) N= 6 ;
    } else {
        printf("Execution par defaut avec l'exposant 6 => 64 cases\n"
               "Pour changer l'exposant le donner en argument au programme\n");
    }
    nbCase = 1 << N ;
    // verification
    if(nbCase <= 8 ) { // verif systematique
        int i ;
        for(i=0;i<(1<<nbCase);i++) {
            Check(i,N) ;
        }
    } else {
        int i ;
        printf("NbCase=%d\n",nbCase);
        for(i=0 ;i < NB_MAX_TRIAL ; i++ ){ // verif sur qulques cas
            u_int64_t numc = 0 ;
            int j ;
            for(j=0;j<nbCase/4;j++) { numc = (numc << 4) +  ( rand() & 15 ) ; }
            printf("%16llx ",numc);
            Check(numc,N);
        }
    }
    
}

