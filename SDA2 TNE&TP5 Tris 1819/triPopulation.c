/* ------------------------------------------------------------------------ */
/**
 *  \file       triPopulation.c
 *  \brief      Programme réalisant des recherches et des tris d'une
 *				table de villes chargée à partir d'un fichier.
 *
 *  \author     <indiquer ici votre nom et votre groupe>
 *
 *  \date       <indiquer ici la date du TP>
 *
 *	\version    1.0
 * *
 *	\remark		Le fichier utilisé doit être rangé dans le dossier
 *				contenant ce fichier
 *
 */

/* ------------------------------------------------------------------------ */
/*		I N C L U D Es  S T A N D A R Ds									*/
/* ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <sys/types.h>
#include <sys/timeb.h>

#if defined (_WIN32)
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#elif defined (__unix__)
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#if defined (_MSC_VER)
/*
 *	Sélection du niveau le plus élevé d'affichage des messages
 *	d'avertissement
 */
#pragma warning( push, 4 )

/*
 *	Désactivation de l'affichage des messages d'avertissement liés à
 *	l'utilisation de fonctions dépréciées par Visual C++ car remplacées
 *	par des fonctions spécifiques plus s˚res mais non standards
 */
#pragma warning( disable : 4996 )
#endif

/* ------------------------------------------------------------------------ */
/*		C O N S T A N T E S   S Y M B O L I Q U E S							*/
/* ------------------------------------------------------------------------ */

#define	FILENAME		"Population59.txt"

#define MAXLEN          256
#define MAXLIGNE        255
#define MAXFILENAME     64
#define NBMAXVILLES     1000
#define MAXNOM          40
#define VILLES_PAR_PAGE 20


#if defined(_WIN32)
#define SYMBOLE_DEGRE   '\xf8'
#else
#define SYMBOLE_DEGRE   '∞'
#endif

/* ------------------------------------------------------------------------ */
/*		D E F I N I T I O N S    D E     T Y P E S							*/
/* ------------------------------------------------------------------------ */
typedef struct
{
    char	nomVille[MAXNOM + 1];
    int		codeCommune;
    int		populationTotale;
    int		populationMasculine;
    int		populationFeminine;
} villePop_t;

/* ------------------------------------------------------------------------ */
/*		P R O T O T Y P E S   D E   F O N C T I O N S						*/
/* ------------------------------------------------------------------------ */
unsigned int chargerVillesPop(const char *Filename, villePop_t VP[]);
void	afficherVillesPop(const villePop_t VP[], int n);

int		rechercheSeqParCommune(const villePop_t VP[], int n, int codeCommune);
int		rechercheRecParCommune(const villePop_t VP[], int debut, int fin, int codeCommune);
void	MinMaxPopMas(const villePop_t VP[], int n, int *pMin, int *pMax);

void	triSelectionParPopTotale(villePop_t VP[], int n);
void	triInsertionParNom(villePop_t VP[], int n);
void	triFusionParPopFem(villePop_t VP[], int n);

void	triFusion_PopFem(villePop_t VP[], int debut, int fin);
void    fusionner_PopFem(villePop_t VP[], int debut, int milieu, int fin);
void    triInsertionParPopFem(villePop_t VP[], int n);

void    afficherVillesPop_test(const villePop_t VP[], int n);
void	rechercheSeqParCommune_test(const villePop_t VP[], int n);
void    rechercheRecParCommune_test(const villePop_t VP[], int n);
void    MinMaxPopMas_test(const villePop_t VP[], int n);

void    triSelectionParPopTotale_test(villePop_t VP[], int n);
void    triInsertionParNom_test(villePop_t VP[], int n);
void    triFusionParPopFem_test(villePop_t VP[], int n);

void echangerVilles(villePop_t *v1, villePop_t *v2);


void    epilogue(void);

/* ------------------------------------------------------------------------ */
/*		V A R I A B L E S   G L O B A L E S     							*/
/* ------------------------------------------------------------------------ */
struct
{
    unsigned long nbComparaisons;
    unsigned long nbAffectations;
} mesures = { 0L, 0L };

/* ------------------------------------------------------------------------ */
/*		F O N C T I O N     P R I N C I P A L E 							*/
/* ------------------------------------------------------------------------ */
int main(void)
{
    villePop_t villesPop[NBMAXVILLES] = { { "" } };	// Table des villes
    unsigned int     nbVilles;			// Nombre de villes
    char    fileName[] = FILENAME;		// Nom du fichier des villes
    
    atexit(epilogue);
    // Initialisation du générateur de nombres aléatoires
    srand((unsigned int)time(NULL));
    
    printf("Chargement du fichier \"%s\" ... ", fileName);
    nbVilles = chargerVillesPop(fileName, villesPop);
    if (nbVilles == 0) return EXIT_FAILURE;
    printf("effectue.\n%d ville(s) chargee(s) dans la table des villes\n\n",
           nbVilles);

    triFusionParPopFem_test(villesPop, nbVilles);
    
#if (0)
    afficherVillesPop_test(villesPop, nbVilles);
    rechercheSeqParCommune_test(villesPop, nbVilles);
    rechercheRecParCommune_test(villesPop,nbVilles);
    MinMaxPopMas_test(villesPop, nbVilles);
    triSelectionParPopTotale_test(villesPop, nbVilles);
    triInsertionParNom_test(villesPop, nbVilles);
    triFusionParPopFem_test(villesPop, nbVilles);
    
#endif
    
    return 0;
}


/* ------------------------------------------------------------------------ */
/*				F O N C T I O N S   A   C O M P L E T E R					*/
/* ------------------------------------------------------------------------ */
void affecterVilles(villePop_t *v1, villePop_t *v2)
{
    strcpy( v1->nomVille, v2->nomVille );
    v1->populationTotale = v2->populationTotale;
    v1->codeCommune = v2->codeCommune;
    v1->populationFeminine = v2->populationFeminine;
    v1->populationMasculine = v2->populationMasculine;
}


int	rechercheSeqParCommune(const villePop_t VP[], int n, int codeCommune)
{
    for(int i = 0; i < n;i++)
    {
        mesures.nbComparaisons++;
        if( VP[i].codeCommune == codeCommune)
            return i;
    }
    return -1;
}

/* ------------------------------------------------------------------------ */
int	rechercheRecParCommune(const villePop_t VP[], int debut, int fin, int codeCommune)
{
    int milieu = (fin+debut)/2;

    if( debut > fin)    // on ne compte pas cette comparaison.
        return -1;

    mesures.nbComparaisons+=2;

    if( VP[milieu].codeCommune == codeCommune )
        return milieu;

    if( VP[milieu].codeCommune < codeCommune )
        return rechercheRecParCommune(VP,milieu+1,fin,codeCommune);

    return rechercheRecParCommune(VP,debut,milieu-1,codeCommune);
}


/* ------------------------------------------------------------------------ */
void MinMaxPopMas(const villePop_t VP[], int n, int *pMin, int *pMax)
{
    int i, iMin = 0, iMax = 0;
    // pMin et pMax représentent respectivement l'indice de la ville la moins et la peuplée
    for(i = 1; i < n-1; i+=2 )
    {
        mesures.nbComparaisons+=3;
        if( VP[i].populationTotale < VP[i+1].populationTotale)
        {
            if( VP[i].populationTotale < VP[iMin].populationTotale )
                iMin = i;
            if( VP[i+1].populationTotale > VP[iMax].populationTotale )
                iMax = i+1;
        }
        else
        {
            if( VP[i+1].populationTotale < VP[iMin].populationTotale )
                iMin = i+1;
            if( VP[i].populationTotale > VP[iMax].populationTotale )
                iMax = i;
        }
    }


    if( iMin != iMax)
    {
        *pMin = iMin;
        *pMax = iMax;
    }
}

/* ------------------------------------------------------------------------ */
void triSelectionParPopTotale(villePop_t VP[], int n)
{
    int iMin;
    for(int i = 0; i < n-1;i++)
    {
        iMin = i;
        for(int j = i+1; j < n; j++)
        {
            mesures.nbComparaisons++;
            if( VP[j].populationTotale < VP[iMin].populationTotale)
            {
                iMin = j;
            }
        }
        if( iMin != i)
        {
            // echanger VP[i] et VP[iMin]
            mesures.nbAffectations += 3;
            villePop_t villeAnnexe;
            affecterVilles(&villeAnnexe,&VP[iMin]);
            affecterVilles(&VP[iMin],&VP[i]);
            affecterVilles(&VP[i],&villeAnnexe);
        }
    }
}


/* ------------------------------------------------------------------------ */
void triInsertionParNom(villePop_t VP[], int n)
{
    for(int i =1; i <n;i++)
    {
        villePop_t eltAnnexe;
        int j = i;
        mesures.nbAffectations++;
        affecterVilles(&eltAnnexe,&VP[i]);
        mesures.nbComparaisons++;
        while(j>0 && ( strcmp(VP[j-1].nomVille,eltAnnexe.nomVille) > 0 ) )
        {
            mesures.nbAffectations++;
            affecterVilles(&VP[j],&VP[j-1]);
            j--;
            mesures.nbComparaisons++;
        }

        if( j<=0) {
            mesures.nbComparaisons--;
            mesures.nbAffectations--;
        }

        mesures.nbAffectations++;
        affecterVilles(&VP[j], &eltAnnexe);
    }
}
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
void fusionner_PopFem(villePop_t VP[], int d, int m, int f)
{
    // Le tableau [d...m] est trié, tout comme le tableau [m+1...f]
    // Ces deux tableaux sont "adjacents" dans la mémoire.
    int inc1=d;
    int inc2=m+1;

    // tableAnnexe contient la sauvegarde des éléments du 1er tableau
    villePop_t *tableAnnexe =malloc((m-d+1)*sizeof(villePop_t));

    //on recopie les éléments du début du tableau
    for(int i=d;i<=m;i++)
    {
        mesures.nbAffectations++;
        affecterVilles(&tableAnnexe[i-d],&VP[i]);
    }

    // On s'arrête lorsqu'on a utilisé tous les éléments du 1er tableau
    // ou lorsqu'on a parcouru tout le tableau (et posé le dernier élément du tableau 1 à la fin du tableau initial).
    for(int i=d;i<=f && inc1 < m+1;i++)
    {
        mesures.nbAffectations++;
        if (inc2==(f+1)) //Si tous les éléments du second tableau ont été utilisés
        {
            affecterVilles(&VP[i],&tableAnnexe[inc1-d]); //on ajoute les éléments restants du premier tableau
            inc1++;
        }
        else
        {
            if (tableAnnexe[inc1 - d].populationFeminine < VP[inc2].populationFeminine) {
                // si l'élément du 1er tableau est plus petit que l'élément du 2e tableau
                affecterVilles(&VP[i], &tableAnnexe[inc1 - d]); //on ajoute un élément du premier tableau
                inc1++;
            }
            else {
                affecterVilles(&VP[i], &VP[inc2]); //on ajoute un élément du second tableau
                inc2++;
            }
            mesures.nbComparaisons++;
        }
    }


    free(tableAnnexe);

}

/* ------------------------------------------------------------------------ */
/*		F O N C T I O N S    D E   T E S T   A   C O M P L E T E R			*/
/* ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */
void triSelectionParPopTotale_test(villePop_t VP[], int n)
{
    /*							à compléter									*/
    /*
     *	1) RàZ de la variable de comptage des comparaisons et affectation
     *	2) Lancement du tri
     *	3) Affichage de la table triée
     *	4) Affichage des nombres de comparaisons et d'affectatins réalisées
     */
    mesures.nbAffectations = 0;
    mesures.nbComparaisons = 0;
    // Questions 12 -> 13

    printf("Table avant le tri : \n");
    afficherVillesPop(VP,n);
    triSelectionParPopTotale(VP,n);
    printf("Table après le tri :\n");
    afficherVillesPop(VP,n);
    printf("Nombre de comparaison(s) = %lu && Nombre d'affectation(s) = %lu\n",
           mesures.nbComparaisons,mesures.nbAffectations);

    /*
    mesures.nbAffectations = 0;
    mesures.nbComparaisons = 0;
    triSelectionParPopTotale(VP,8);
    printf("On trie un tableau déjà trié : \n");
    afficherVillesPop(VP,8);
    printf("Nombre de comparaison(s) = %lu && Nombre d'affectation(s) = %lu\n",
           mesures.nbComparaisons,mesures.nbAffectations);
    */


    // Question 14
    /*
    triSelectionParPopTotale(VP,n);
    afficherVillesPop(VP,n);
    printf("Nombre de comparaison(s) = %lu && Nombre d'affectation(s) = %lu\n",
           mesures.nbComparaisons,mesures.nbAffectations);
    */

}


/* ------------------------------------------------------------------------ */
void triInsertionParNom_test(villePop_t VP[], int n)
{
    // Questions 17 -> 18
    /*
    const size_t nbElts = 8;
    mesures.nbAffectations = 0;
    mesures.nbComparaisons = 0;
    printf("Table avant le tri : \n");
    afficherVillesPop(&VP[n-nbElts],nbElts);
    triInsertionParNom(&VP[n-nbElts],nbElts);
    printf("Table après le tri :\n");
    afficherVillesPop(&VP[n-nbElts],nbElts);
    printf("Nombre de comparaison(s) = %lu && Nombre d'affectation(s) = %lu\n",
           mesures.nbComparaisons,mesures.nbAffectations);
    */

    // Question 19

    mesures.nbAffectations = 0;
    mesures.nbComparaisons = 0;
    triInsertionParNom(VP,n);
    printf("Tri par insertion par nom :\n");
    afficherVillesPop(VP,n);
    printf("Nombre de comparaison(s) = %lu && Nombre d'affectation(s) = %lu\n",
           mesures.nbComparaisons,mesures.nbAffectations);

}

/* ------------------------------------------------------------------------ */
void triFusionParPopFem_test(villePop_t VP[], int n){
    mesures.nbAffectations = 0;
    mesures.nbComparaisons = 0;
    printf("Table avant le tri : \n");
    afficherVillesPop(VP,n);
    triFusionParPopFem(VP,n);
    printf("Table après le tri :\n");
    afficherVillesPop(VP,n);
    printf("Nombre de comparaison(s) = %lu && Nombre d'affectation(s) = %lu\n",
           mesures.nbComparaisons,mesures.nbAffectations);

}


/* ------------------------------------------------------------------------ */
/*		F O N C T I O N S    M I S E S   à   D I S P O S I T I O N			*/
/* ------------------------------------------------------------------------ */
void afficherVillesPop_test(const villePop_t VP[], int n)
{
    int     indice;
    
    /*  Quelques exemples d'utilisation de la fonction d'affichage			*/
    printf("\nAffichage des 5 premieres villes\n");
    afficherVillesPop(VP, 5);
    
    printf("\nAffichage des 3 dernieres villes\n");
    afficherVillesPop(&VP[n - 3], 3);
    
    indice = rand() % (n - 3);
    printf("\nAffichage de 3 communes a partir de la %deme commune\n",
           indice + 1);
    afficherVillesPop(&VP[indice], 3);
}

/* ------------------------------------------------------------------------ */
void rechercheSeqParCommune_test(const villePop_t VP[], int n)
{
    int indice, commune;
    
    printf("Entrez le(s) code(s) commune(s) de(s) ville(s) recherchees (. pour arreter)\n");
    while (1)
    {
        printf("Quel code commune ? ");
        if (scanf("%d", &commune) != 1 || commune == 0) return;
        
        memset(&mesures, 0, sizeof mesures);
        indice = rechercheSeqParCommune(VP, n, commune);
        if (indice == -1)
            printf("La commune n%c %05d est inconnue\n", SYMBOLE_DEGRE, commune);
        else
            afficherVillesPop(&VP[indice], 1);
        printf("Nombre de comparaison(s) = %lu\n",
               mesures.nbComparaisons);
    }
}

/* ------------------------------------------------------------------------ */
void rechercheRecParCommune_test(const villePop_t VP[], int n)
{
    int indice, commune;
    
    printf("\nRecherche dichotomique par code postal de la commune (. pour arreter)\n");
    while (1)
    {
        printf("Quel est le code postal ? ");
        if (scanf("%d", &commune) != 1)
        {
            while (getc(stdin) != '\n');
            return;
        }
        if (commune == 0) return;
        
        memset(&mesures, 0, sizeof mesures);
        indice = rechercheRecParCommune(VP, 0, n - 1, commune);
        if (indice == -1)
            printf("La commune n%c %05d est inconnue\n", SYMBOLE_DEGRE, commune);
        else
            afficherVillesPop(&VP[indice], 1);
        printf("Nombre de comparaison(s) = %lu\n",
               mesures.nbComparaisons);
    }
}


/* ------------------------------------------------------------------------ */
void MinMaxPopMas_test(const  villePop_t VP[], int n)
{
    int min = -1, max = -1;
    
    memset(&mesures, 0, sizeof mesures);
    MinMaxPopMas(VP, n, &min, &max);
    if (max != -1)
    {
        printf("\nVille ayant la plus grande population masculine\n");
        afficherVillesPop(&VP[max], 1);
    }
    if (min != -1)
    {
        printf("\nVille ayant la plus petite population masculine\n");
        afficherVillesPop(&VP[min], 1);
    }
    printf("Nombre de comparaison(s) = %lu\n",
           mesures.nbComparaisons);
}

/* ------------------------------------------------------------------------ */
void triFusion_PopFem(villePop_t VP[], int debut, int fin)
{
    int milieu;
    
    if (debut < fin)
    {
        milieu = (debut + fin) / 2;
        triFusion_PopFem(VP, debut, milieu);
        triFusion_PopFem(VP, milieu + 1, fin);
        fusionner_PopFem(VP, debut, milieu, fin);
    }
}
void triFusionParPopFem(villePop_t VP[], int n)
{
    triFusion_PopFem(VP, 0, n - 1);
}

/* ------------------------------------------------------------------------ */
/**
 * \brief       Cette fonction charge le contenu d'un fichier texte contenant
 *              des informations sur des villes dans une table de villes
 *
 * \param       Filename    nom du fichier contenant les informations
 * \param       VP          la table recevant les informations sur les villes.
 *
 * \return      le nombre de villes rangées dans la table. Zéro si l'ouverture
 *              du fichier échoue, ou s'il est vide.
 *
 */
unsigned int chargerVillesPop(const char *Filename, villePop_t VP[])
{
    FILE *fp_in;
    char Ligne[MAXLIGNE + 1];
    char *p;
    int NoChamp;
    unsigned int NoVille;
    villePop_t villePop;
    
    fp_in = fopen(Filename, "rt");
    if (fp_in == NULL)
    {
        fprintf(stderr, "Ouverture du fichier %s impossible\n", Filename);
        return 0;
    }
    
    /* Ignorer la 1ère ligne qui est la ligne de titre */
    fgets(Ligne, sizeof Ligne, fp_in);
    
    for (NoVille = 0; fgets(Ligne, sizeof Ligne, fp_in) != NULL; NoVille++)
    {
        p = strtok(Ligne, "\n\t");
        memset(&villePop, 0, sizeof villePop);
        for (NoChamp = 0; p != NULL; NoChamp++)
        {
            switch (NoChamp)
            {
                case 0: /* Nom de la ville */
                    strcpy(villePop.nomVille, p);
                    break;
                    
                case 1: /* Code de la commune */
                    sscanf(p, "%d", &villePop.codeCommune);
                    break;
                    
                case 2: /* population totale de la ville */
                    sscanf(p, "%d", &villePop.populationTotale);
                    break;
                    
                case 3: /* population masculine de la ville */
                    sscanf(p, "%d", &villePop.populationMasculine);
                    break;
                    
                case 4: /* population féminine de la ville */
                    sscanf(p, "%d", &villePop.populationFeminine);
                    break;
                    
                default: /* On ne devrait jamais atteindre ce cas */
                    printf("\aErreur\n");
                    break;
            }  /* Fin du switch */
            p = strtok(NULL, "\n\t");
        } /* Fin du for (No ... */
        VP[NoVille] = villePop;
    }
    fclose(fp_in);
    return NoVille;
}

/* ------------------------------------------------------------------------ */
/**
 * \brief       Affiche sur l'écran les informations des villes
 *              rangées dans une table de villes
 *
 * \param       VP  tables de villes (Nom, code commune, PopFem, PopMas, PopTot)
 * \param       n   nombre de villes à afficher
 *
 * \return      néant
 *
 * \remark      une pause est effectu»e après l'affichage d'un certain
 *              nombre de villes
 *
 */
void afficher1VillePop(const villePop_t villePop)
{
    printf("\t| %-24s | %05d  | %7d | %7d | %7d |\n",
           villePop.nomVille,
           villePop.codeCommune,
           villePop.populationFeminine,
           villePop.populationMasculine,
           villePop.populationTotale
           );
    
}
void afficherVillesPop(const villePop_t VP[], int n)
{
    unsigned int i = 0;
    
    printf("\t+--------------------------+--------+---------+---------+---------+\n");
    printf("\t|       NOM                |CODE COM| POP_FEM | POP_MAS | POP_TOT |\n");
    printf("\t+--------------------------+--------+---------+---------+---------+\n");
    for (i = 0; i < n; i++)
    {
        afficher1VillePop(VP[i]);
        
        if ((i + 1) % VILLES_PAR_PAGE == 0)
            system("PAUSE");
    }
    printf("\t+--------------------------+--------+---------+---------+---------+\n");
}
/* ------------------------------------------------------------------------ */
void epilogue(void)
{
    static const char * jour[] = {
        "Dimanche", "Lundi", "Mardi", "Mercredi",
        "Jeudi", "Vendredi", "Samedi" };
    
    static const char * mois[] = {
        "Janvier", "Fevrier", "Mars", "Avril", "Mai", "Juin",
        "Juillet", "Aout", "Septembre", "Octobre", "Novembre", "Decembre",
    };
    
    time_t ltime;
    struct tm * today;
    char userName[MAXLEN + 1] = "";
    char hostName[MAXLEN + 1] = "";
    unsigned long name_len = MAXLEN + 1;
    
#if defined (_WIN32)
    WSADATA wsaData;
#endif
    
    struct hostent * pHostEntry;
    struct in_addr *pinAddr;
    
#if defined (_WIN32)
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    
    /*	Nom de l'utilisateur   					                			*/
#if defined (_WIN32)
    GetUserName(userName, &name_len);
#elif defined (__unix__)
    getlogin_r(userName, name_len);
#endif
    printf("\n%-9.9s ", userName);
    
    /*	Nom de l'ordinateur   								                */
    gethostname(hostName, MAXLEN);
    
    /* Nom réseau et adresse IP de l'ordinateur								*/
    pHostEntry = gethostbyname(hostName);
    if (pHostEntry != NULL)
    {
        printf("%-13.10s ", pHostEntry->h_name);
        pinAddr = ((struct in_addr *)pHostEntry->h_addr_list[0]);
        if (pinAddr != NULL)
            printf("%s ", inet_ntoa(*pinAddr));
    }
    
    /*	Date et heure														*/
    tzset();
    time(&ltime);
    today = localtime(&ltime);
    printf("%9s %d %s %4d a %dh %dmn %ds\n",
           jour[today->tm_wday], today->tm_mday,
           mois[today->tm_mon], today->tm_year + 1900,
           today->tm_hour, today->tm_min, today->tm_sec);
    
#if defined (_WIN32)
    WSACleanup();
#endif
}
/* ------------------------------------------------------------- THE END -- */


