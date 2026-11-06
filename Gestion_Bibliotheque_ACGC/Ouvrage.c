/*
 * ============================================================
 *  Ouvrage.c
 *  Implémentation complète des fonctions de gestion d'ouvrages
 *  et de la bibliothèque numérique
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Ouvrage.h"
#include <time.h>
#include<stdbool.h>

#ifdef _WIN32
    #define EFFACER "cls"
#else
    #define EFFACER "clear"
#endif

/* ============================================================
 * FONCTIONS UTILITAIRES
 * ============================================================ */



unsigned int annee_courante(void) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_year + 1900; // année réelle du système
}


/**
 * Écrit un ouvrage complet dans le fichier binaire.
 * On enregistre la structure Ouvrage (sans le tableau de pointeurs auteurs),
 * puis on écrit chaque auteur individuellement.
 */
void ecrire_ouvrage(FILE* fichier, Ouvrage* o) {
    fwrite(o, sizeof(Ouvrage) - sizeof(Personne**), 1, fichier);
    for (unsigned int i = 0; i < o->nbAuteurs; i++) {
        fwrite(o->auteurs[i], sizeof(Personne), 1, fichier);
    }
}

/**
 * Lit un ouvrage complet depuis le fichier binaire.
 * On lit la structure Ouvrage, puis on recharge les auteurs associés.
 * Retourne false si la lecture échoue (fin de fichier).
 */
bool lire_ouvrage(FILE* fichier, Ouvrage* o) {
    if (fread(o, sizeof(Ouvrage) - sizeof(Personne**), 1, fichier) != 1)
        return false;
    charger_auteurs(fichier, o);
    return true;
}

/**
 * Charge en mémoire dynamique les auteurs d’un ouvrage
 * à partir du fichier binaire.
 */
void charger_auteurs(FILE* fichier, Ouvrage* o) {
    o->auteurs = (Personne**)malloc(o->nbAuteurs * sizeof(Personne*));
    for (unsigned int i = 0; i < o->nbAuteurs; i++) {
        o->auteurs[i] = (Personne*)malloc(sizeof(Personne));
        fread(o->auteurs[i], sizeof(Personne), 1, fichier);
    }
}

/**
 * Libère la mémoire allouée pour les auteurs d’un ouvrage.
 * Évite les fuites mémoire.
 */
void liberer_auteurs(Ouvrage* o) {
    if (o->auteurs != NULL) {
        for (unsigned int i = 0; i < o->nbAuteurs; i++) {
            free(o->auteurs[i]);
        }
        free(o->auteurs);
        o->auteurs = NULL;
        o->nbAuteurs = 0;
    }
}

/**
 * Ouvre un fichier en mode binaire
 * @param nomFichier : nom du fichier à ouvrir
 * @return : pointeur FILE si succès, NULL sinon
 */
FILE* OuvertureFichier(const char *nomFichier) {
    FILE *fichier = fopen(nomFichier, "r+b");
    if (fichier == NULL) {
        fichier = fopen(nomFichier, "w+b");
    }
    return fichier;
}

/**
 * Vide le buffer d'entrée standard
 */
void vider_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void attendre_entree(void) {
    printf("\nAppuyez sur Entree...");
    getchar();
}

/**
 * Échange deux nombres entiers
 * @param a : premier nombre
 * @param b : deuxième nombre
 */
void Permutation(unsigned int* a, unsigned int* b) {
    unsigned int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Vérifie si une personne a déjà été comptée
 * @param p1 : première personne
 * @param p2 : deuxième personne
 * @return : true si les deux personnes sont identiques
 */
bool Deja_Compter(Personne p1, Personne p2) {
    return (strcmp(p1.identifiant, p2.identifiant) == 0);
}

/**
 * Affiche le menu principal
 * @return : choix de l'utilisateur (1-8)
 */
unsigned int menu_principal(void) {
    unsigned int choix;

    printf("\n+--------------------------------------------------------+\n");
    printf("|          GESTION DE BIBLIOTHEQUE NUMERIQUE             |\n");
    printf("+--------------------------------------------------------+\n\n");

    printf(" [1]  Enregistrement\n");
    printf(" [2]  Affichage\n");
    printf(" [3]  Rechercher\n");
    printf(" [4]  Modification\n");
    printf(" [5]  Suppression\n");
    printf(" [6]  Statistiques\n");
    printf(" [7]  Listes\n");
    printf(" [0]  Quitter\n\n");

    do {
        printf("Votre choix -> ");
        if (scanf("%u", &choix) != 1) {
            printf("Entree invalide. Veuillez entrer un nombre.\n");
            while (getchar() != '\n') // vider le tampon
            choix = 0; // valeur par défaut
        }
        vider_buffer();
    } while (choix > 7);

    return choix;
}

/**
 * Affiche le sous-menu des modifications
 * @return : choix de l'utilisateur (1-6)
 */
unsigned int sous_menu_modification(void) {
    unsigned int choix;

    printf("\n+-----------------------------------------+\n");
    printf("|        MODIFICATION D'OUVRAGES          |\n");
    printf("+-----------------------------------------+\n\n");

    printf(" [1]  Modifier le titre\n");
    printf(" [2]  Modifier l'editeur\n");
    printf(" [3]  Modifier l'annee d'edition\n");
    printf(" [4]  Modifier la quantite\n");
    printf(" [5]  Modifier la date complete d'edition\n");
    printf(" [6]  Modifier les auteurs\n");
    printf(" [0]  Retour au menu principal\n\n");

    do {
        printf("Votre choix -> ");
        if (scanf("%u", &choix) != 1) {
            printf("Entree invalide. Veuillez entrer un nombre.\n");
            while (getchar() != '\n')
            choix = 0;
        }
        vider_buffer();
    } while (choix > 6);

    return choix;
}


/**
 * Affiche le sous-menu des statistiques
 * @return : choix de l'utilisateur (1-10)
 */
unsigned int sous_menu_statistiques(void) {
    unsigned int choix;

    printf("\n+-----------------------------------------+\n");
    printf("|        STATISTIQUES OUVRAGES            |\n");
    printf("+-----------------------------------------+\n\n");

    printf(" [1]  Ouvrages avec au moins n auteurs\n");
    printf(" [2]  Ouvrages par editeur\n");
    printf(" [3]  Ouvrages par auteur\n");
    printf(" [4]  Ouvrages d'un editeur a une date\n");
    printf(" [5]  Ouvrages avec quantite = nombre d'auteurs\n");
    printf(" [6]  Ouvrages d'un editeur entre deux annees\n");
    printf(" [7]  Ouvrages d'Auteurs actifs entre deux annees\n");
    printf(" [8]  Ouvrages a une date avec quantite donnee\n");
    printf(" [9]  Ouvrages avec meme titre\n");
    printf(" [10] Ouvrages d'un auteur et d'un editeur precis\n");
    printf(" [0]  Retour au menu principal\n\n");

    do {
        printf("Votre choix -> ");
        if (scanf("%u", &choix) != 1) {
            printf("Entree invalide. Veuillez entrer un nombre.\n");
            while (getchar() != '\n');
            choix = 0;
        }
        vider_buffer();
    } while (choix > 10);

    return choix;
}


/**
 * Affiche le sous-menu des listes
 * @return : choix de l'utilisateur (1-10)
 */
unsigned int sous_menu_listes(void) {
    unsigned int choix;

    printf("\n+-----------------------------------------+\n");
    printf("|           LISTES D'OUVRAGES             |\n");
    printf("+-----------------------------------------+\n\n");

    printf(" [1]  Liste ouvrages avec au moins n auteurs\n");
    printf(" [2]  Liste ouvrages par editeur\n");
    printf(" [3]  Liste ouvrages par auteur\n");
    printf(" [4]  Liste ouvrages d'un editeur a une date\n");
    printf(" [5]  Liste ouvrages avec quantite = nombre d'auteurs\n");
    printf(" [6]  Liste ouvrages d'un editeur entre deux annees\n");
    printf(" [7]  Liste auteurs actifs entre deux annees\n");
    printf(" [8]  Liste ouvrages a une date avec quantite donnee\n");
    printf(" [9]  Liste ouvrages avec meme titre\n");
    printf(" [10] Liste ouvrages d'un auteur et d'un editeur precis\n");
    printf(" [0]  Retour au menu principal\n\n");

    do {
        printf("Votre choix -> ");
        if (scanf("%u", &choix) != 1) {
            printf("Entree invalide. Veuillez entrer un nombre.\n");
            while (getchar() != '\n');
            choix = 0;
        }
        vider_buffer();
    } while (choix > 10);

    return choix;
}


/**
 * Quitte le programme
 */
void Quitter(void) {
    printf("\n+-----------------------------------------+\n");
    printf("| Merci d'avoir utilise notre service     |\n");
    printf("+-----------------------------------------+\n\n");
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

/* ============================================================
 * FONCTIONS DE GESTION DES DATES
 * ============================================================ */

/**
 * Vérifie si une année est bissextile
 * @param annee : année à vérifier
 * @return : 1 si bissextile, 0 sinon
 */
unsigned int est_bissextile(unsigned int annee) {
    return (annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0);
}

/**
 * Vérifie si une date est valide
 * @param d : date à vérifier
 * @return : 1 si valide, 0 sinon
 */
unsigned int date_valide(Date d) {
    if (d.mois < JANVIER || d.mois > DECEMBRE) return 0;
    switch (d.mois) {
        case JANVIER: case MARS: case MAI: case JUILLET:
        case AOUT: case OCTOBRE: case DECEMBRE:
            return (d.jour >= 1 && d.jour <= 31);
        case AVRIL: case JUIN: case SEPTEMBRE: case NOVEMBRE:
            return (d.jour >= 1 && d.jour <= 30);
        case FEVRIER:
            if (est_bissextile(d.annee))
                return (d.jour >= 1 && d.jour <= 29);
            else
                return (d.jour >= 1 && d.jour <= 28);
        default:
            return 0;
    }
}

// Lecture du mois
unsigned int Lec_Mois(void)
{
    unsigned int m;
    do {
        printf("\n Donner le mois (1-12) : ");
        scanf("%u", &m);
        vider_buffer();

        if (m < 1 || m > 12) {
            printf(" Mois invalide (%hu). Veuillez entrer un mois entre 1 et 12.\n", m);
        }
    } while (m < 1 || m > 12);

    return m;
}

// Lecture du jour dans le mois
unsigned int Lec_JourMois(unsigned int annee, unsigned int mois){
    unsigned int j;
    do {
        printf("\n Donner le jour : ");
        scanf("%u", &j);
        vider_buffer();
    } while (j < 1 || j > 31 ||
             (mois == 2 && j > (est_bissextile(annee) ? 29 : 28)) ||
             ((mois == 4 || mois == 6 || mois == 9 || mois == 11) && j > 30));
    return j;
}

// Lecture d'une date complète
void lecture_date(Date *d) {
    int valide = 0;

    while (!valide) {
        // --- Saisie de l'année ---
        do {
            printf("\n Donner l'annee (%d-%u) : ", ANNEE_MIN, annee_courante());
            if (scanf("%u", &d->annee) != 1) {
                printf("Entree invalide. Veuillez entrer un nombre.\n");
                while (getchar() != '\n') // vider le tampon
                continue;
            }
            vider_buffer();

            if (d->annee < ANNEE_MIN || d->annee > annee_courante()) {
                printf(" Annee invalide (%u). Veuillez entrer une annee entre %d et %u.\n",
                       d->annee, ANNEE_MIN, annee_courante());
            } else break;
        } while (1);

        // --- Saisie du mois ---
        d->mois = Lec_Mois();

        // --- Saisie du jour ---
        d->jour = Lec_JourMois(d->annee, d->mois);

        // --- Vérification finale ---
        if (!date_valide(*d)) {
            printf(" Date invalide (ex: 30 fevrier ou 31 avril). Veuillez ressaisir la date.\n");
        } else {
            valide = 1; // sortie de la boucle
        }
    }
}




/**
 * Affiche une date formatée
 * @param d : date à afficher
 */
void affichage_date(Date d) {
    printf("%02u/%02u/%04u", d.jour, (unsigned int)d.mois, d.annee);
}


/**
 * Vérifie si deux dates sont égales
 * @param d1 : première date
 * @param d2 : deuxième date
 * @return : true si les dates sont identiques
 */
bool Date_Egale(Date d1, Date d2) {
    return (d1.jour == d2.jour && d1.mois == d2.mois && d1.annee == d2.annee);
}

/* ============================================================
 * FONCTIONS DE GESTION DES AUTEURS
 * ============================================================ */

/**
 * Lit les informations d'un auteur
 * @param ouvrage : pointeur vers l'ouvrage
 * @param auteur : pointeur vers l'auteur à lire
 */
void Lecture_D_Un_Auteur(Ouvrage* ouvrage, Personne* auteur) {
    printf("\n--- Informations de l'auteur ---\n");
    printf("Nom : ");
    fgets(auteur->nom, MAX_NOM, stdin);
    auteur->nom[strcspn(auteur->nom, "\n")] = '\0';

    printf("Prenom : ");
    fgets(auteur->prenom, MAX_PRENOM, stdin);
    auteur->prenom[strcspn(auteur->prenom, "\n")] = '\0';

    printf("Identifiant : ");
    fgets(auteur->identifiant, MAX_ID, stdin);
    auteur->identifiant[strcspn(auteur->identifiant, "\n")] = '\0';
}

/**
 * Affiche les informations de plusieurs auteurs
 * @param ouvrage : ouvrage contenant l'auteur
 */

void affichage_Auteurs(Ouvrage ouvrage ){
    if (ouvrage.nbAuteurs == 0) {
        printf("  Aucun auteur\n");
        return;
    }

    for (unsigned int i = 0; i < ouvrage.nbAuteurs; i++) {
        printf("  Auteur %u : %s %s (ID: %s)\n",
               i + 1,
               ouvrage.auteurs[i]->prenom,
               ouvrage.auteurs[i]->nom,
               ouvrage.auteurs[i]->identifiant);
    }
}

/**
 * Lit tous les auteurs d'un ouvrage
 * @param ouvrage : pointeur vers l'ouvrage
 * @param nbAuteurs : nombre d'auteurs à lire
 */
void Lecture_Auteurs(Ouvrage* ouvrage, unsigned int nbAuteurs) {
    ouvrage->nbAuteurs = nbAuteurs;
    ouvrage->auteurs = (Personne**)malloc(nbAuteurs * sizeof(Personne*));

    if (ouvrage->auteurs == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour les auteurs\n");
        return;
    }

    for (unsigned int i = 0; i < nbAuteurs; i++) {
        ouvrage->auteurs[i] = (Personne*)malloc(sizeof(Personne));
        if (ouvrage->auteurs[i] == NULL) {
            fprintf(stderr, "Erreur d'allocation mémoire pour l'auteur %u\n", i + 1);
            return;
        }
        Lecture_D_Un_Auteur(ouvrage, ouvrage->auteurs[i]);
    }
}


/**
 * Affiche un auteur spécifique d'un ouvrage en utilisant sa position
 * @param ouvrage : ouvrage contenant les auteurs
 * @param position : index de l'auteur (retourné par recherche_auteur)
 */
void affichage_d_un_auteur(Ouvrage* ouvrage, unsigned int position) {
        if (ouvrage->nbAuteurs == 0) {
            printf(" Aucun auteur enregistre\n");
            return;
        }
        if (position >= ouvrage->nbAuteurs) {
            printf(" Position invalide (max %u).\n", ouvrage->nbAuteurs - 1);
            return;
        }

        Personne* auteur = ouvrage->auteurs[position];
        printf("\n=== Auteur trouve===\n");
        printf("Nom : %s\n", auteur->nom);
        printf("Prenom :%s\n", auteur->prenom);
        printf("Identifiant : %s\n", auteur->identifiant);
    }



/**
 * Ajoute un auteur à un ouvrage existant
 * @param ouvrage : ouvrage auquel ajouter un auteur
 */
void Ajout_Auteur(Ouvrage* ouvrage) {
    // Incrémenter le nombre d'auteurs
    ouvrage->nbAuteurs += 1;

    // Utiliser un pointeur temporaire pour sécuriser la réallocation
    Personne** temp = realloc(ouvrage->auteurs, ouvrage->nbAuteurs * sizeof(Personne*));
    if (temp == NULL) {
        fprintf(stderr, "Erreur de réallocation mémoire pour les auteurs\n");
        ouvrage->nbAuteurs -= 1; // revenir à l'état précédent
        return;
    }
    ouvrage->auteurs = temp;

    // Allouer le nouvel auteur
    ouvrage->auteurs[ouvrage->nbAuteurs - 1] = (Personne*)malloc(sizeof(Personne));
    if (ouvrage->auteurs[ouvrage->nbAuteurs - 1] == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour le nouvel auteur\n");
        ouvrage->nbAuteurs -= 1; // revenir à l'état précédent
        return;
    }

    // Lecture des informations du nouvel auteur
    Lecture_D_Un_Auteur(ouvrage, ouvrage->auteurs[ouvrage->nbAuteurs - 1]);
    printf(" Auteur ajoute avec succes.\n");
}



/**
 * Recherche un auteur dans un ouvrage par son identifiant numérique
 * @param ouvrage : pointeur vers l'ouvrage
 * @param identifiant : identifiant numérique de l'auteur
 * @return : position de l'auteur trouvé, ou -1 si non trouvé
 */
int recherche_auteur(Ouvrage* o, const char* id) {
    for (unsigned i=0; i<o->nbAuteurs; i++)
        if (strcmp(o->auteurs[i]->identifiant, id)==0) return (int)i;
    return -1;
}

// Verifie si un auteur existe
bool auteur_existe(FILE* fichier, const char* id_auteur) {
    if (!fichier) return false;
    Ouvrage o;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o)) {
        for (unsigned int i = 0; i < o.nbAuteurs; i++) {
            if (strcmp(o.auteurs[i]->identifiant, id_auteur) == 0) {
                liberer_auteurs(&o);
                return true;
            }
        }
        liberer_auteurs(&o);
    }
    return false;
}

// Verifie si un éditeur existe
bool editeur_existe(FILE* fichier, const char* id_editeur) {
    if (!fichier) return false;
    Ouvrage o;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o)) {
        if (strcmp(o.editeur.identifiant, id_editeur) == 0) {
            liberer_auteurs(&o);
            return true;
        }
        liberer_auteurs(&o);
    }
    return false;
}


/* ============================================================
 * FONCTIONS DE GESTION DES OUVRAGES
 * ============================================================ */

/**
 * Affiche les informations d'un ouvrage
 * @param fichier : fichier contenant les ouvrages
 */
void afficher_bibliotheque(FILE* fichier) {
    if (!fichier) { fprintf(stderr, "Fichier invalide\n"); return; }

    Ouvrage o;
    rewind(fichier);
    int count = 0;

    printf("\n+--------------------------------------------------------+\n");
    printf("|                 TOUS LES OUVRAGES                      |\n");
    printf("+--------------------------------------------------------+\n\n");

    while (lire_ouvrage(fichier, &o)) {
        count++;
        printf("+--------------------------------------------------------+\n");
        printf("| Ouvrage #%u                                             |\n", count);
        printf("+--------------------------------------------------------+\n");

        affichage_ouvrage(&o);
        liberer_auteurs(&o);

        printf("+--------------------------------------------------------+\n\n");
    }
    if (count == 0) {
        printf("+--------------------------------------------------------+\n");
        printf("| Aucun ouvrage dans la bibliotheque.                    |\n");
        printf("+--------------------------------------------------------+\n");
    }
}



/**
 * Lit les informations d'un ouvrage depuis l'entrée utilisateur
 * @param ouvrage : pointeur vers l'ouvrage à lire
 */
void Lecture_Ouvrage(Ouvrage* ouvrage) {
    printf("\n--- Informations de l'ouvrage ---\n");

    // --- Numéro d'ouvrage ---
    do {
        printf("Numero d'ouvrage : ");
        if (scanf("%u", &ouvrage->numero) != 1) {
            printf("Entree invalide. Veuillez entrer un nombre.\n");
            while (getchar() != '\n'); // vider le tampon
            continue; // redemander
        }
        vider_buffer();
        break; // sortie si saisie correcte
    } while (1);

    // --- Titre ---
    printf("Titre : ");
    fgets(ouvrage->titre, MAX_TITRE, stdin);
    ouvrage->titre[strcspn(ouvrage->titre, "\n")] = '\0';

    // --- Editeur ---
    printf("\n--- Informations de l'éditeur ---\n");
    printf("Nom de l'editeur : ");
    fgets(ouvrage->editeur.nom, MAX_NOM, stdin);
    ouvrage->editeur.nom[strcspn(ouvrage->editeur.nom, "\n")] = '\0';

    printf("Prenom de l'editeur : ");
    fgets(ouvrage->editeur.prenom, MAX_PRENOM, stdin);
    ouvrage->editeur.prenom[strcspn(ouvrage->editeur.prenom, "\n")] = '\0';

    printf("Identifiant de l'editeur : ");
    fgets(ouvrage->editeur.identifiant, MAX_ID, stdin);
    ouvrage->editeur.identifiant[strcspn(ouvrage->editeur.identifiant, "\n")] = '\0';

    // --- Date d'édition ---
    printf("\n--- Date d'edition ---\n");
    lecture_date(&ouvrage->dateEdition);

    // --- Quantité ---
    do {
        printf("Quantite : ");
        if (scanf("%u", &ouvrage->quantite) != 1) {
            printf("Entree invalide. Veuillez entrer un nombre.\n");
            while (getchar() != '\n');
            continue;
        }
        vider_buffer();
        break;
    } while (1);

    // --- Nombre d'auteurs ---
    do {
        printf("Nombre d'auteurs : ");
        if (scanf("%u", &ouvrage->nbAuteurs) != 1) {
            printf("Entree invalide. Veuillez entrer un nombre.\n");
            while (getchar() != '\n');
            continue;
        }
        vider_buffer();
        break;
    } while (1);

    Lecture_Auteurs(ouvrage, ouvrage->nbAuteurs);
}


// Fonction d’accès par indice
void afficher_Ouvrage_par_pos(FILE* fichier, long pos) {
    if (fichier == NULL || pos < 1) return;

    Ouvrage o;
    rewind(fichier);
    int compteur = 0;

    while (fread(&o, sizeof(Ouvrage) - sizeof(Personne**), 1, fichier) == 1) {
        compteur++;
        if (compteur == pos) {
            charger_auteurs(fichier, &o);

            printf("\n+----------------------------------+\n");
            printf("|        OUVRAGE TROUVE            |\n");
            printf("+----------------------------------+\n\n");

            affichage_ouvrage(&o);

            liberer_auteurs(&o);
            return;
        } else {
            fseek(fichier, o.nbAuteurs * sizeof(Personne), SEEK_CUR);
        }
    }
}

/**
 * Affiche un ouvrage complet
 * @param o : pointeur vers l'ouvrage
 */

void affichage_ouvrage(Ouvrage* o) {
    printf("\n+----------------------------------------------+\n");
    printf("|              DETAILS OUVRAGE                 |\n");
    printf("+----------------------------------------------+\n\n");

    printf(" Numero         : %u\n", o->numero);
    printf(" Titre          : %s\n", o->titre);
    printf(" Editeur        : %s %s (ID: %s)\n",
           o->editeur.prenom, o->editeur.nom, o->editeur.identifiant);

    printf(" Date d'edition : ");
    affichage_date(o->dateEdition);

    printf("\n Quantite     : %u\n", o->quantite);

    printf("\n--- Liste des auteurs ---\n");
    affichage_Auteurs(*o);

    printf("\n+----------------------------------------------+\n");
}




/**
 * Ajoute un nouvel ouvrage à la bibliothèque
 * @param fichier : fichier contenant les ouvrages
 */

void ajouter_Ouvrage(FILE* fichier) {
    if (!fichier) { fprintf(stderr, "Fichier invalide\n"); return; }

    Ouvrage o;
    Lecture_Ouvrage(&o);

    if (!verifier_numero_unique(fichier, o.numero)) {
        printf("Erreur : un ouvrage avec le numéro %u existe déjà.\n", o.numero);
        liberer_auteurs(&o);
        return;
    }

    fseek(fichier, 0, SEEK_END);
    ecrire_ouvrage(fichier, &o);

    printf(" Ouvrage ajoute avec succes.\n");
    liberer_auteurs(&o);
}

/**
 * Enregistre plusieurs ouvrages successivement
 * Utilise ecrire_ouvrage pour uniformiser l’écriture.
 */

void Enregistrement(FILE* fichier) {
    if (fichier == NULL) {
        fprintf(stderr, "Fichier invalide\n");
        return;
    }

    unsigned int continuer = 1;
    while (continuer) {
        Ouvrage o;
        Lecture_Ouvrage(&o);

        if (!verifier_numero_unique(fichier, o.numero)) {
            printf("Numero %u deja existant. Enregistrement annule pour cet ouvrage.\n", o.numero);
            liberer_auteurs(&o);
        } else {
            fseek(fichier, 0, SEEK_END);
            ecrire_ouvrage(fichier, &o);
            printf(" Ouvrage ajoute.\n");
            liberer_auteurs(&o);
        }

        // --- Contrôle de saisie pour continuer ---
        do {
            printf("\nVoulez-vous ajouter un autre ouvrage ? (1 = Oui, 0 = Non) : ");
            if (scanf("%u", &continuer) != 1) {
                printf("Entrée invalide. Veuillez entrer 1 ou 0.\n");
                while (getchar() != '\n')
                continuer = 0;
                continue;
            }
            vider_buffer();
            if (continuer != 0 && continuer != 1) {
                printf("Valeur invalide. Tapez 1 pour Oui ou 0 pour Non.\n");
                continuer = 0;
            }
            break;
        } while (1);
    }
    printf("Enregistrement termine.\n");
}



/**
 * Recherche un ouvrage par son numéro
 * @param fichier : fichier contenant les ouvrages
 * @param numero : numéro de l'ouvrage à rechercher
 * @return : 1 si trouvé, -1 sinon
 */

 long rechercher_Ouvrage(FILE* fichier, unsigned int numero) {
    if (fichier == NULL) return -1;

    Ouvrage o;
    rewind(fichier);
    unsigned int index = 0; // compteur d’ouvrages

    while (fread(&o, sizeof(Ouvrage) - sizeof(Personne**), 1, fichier) == 1) {
        index++; // incrémente à chaque ouvrage lu

        if (o.numero == numero) {
            return index; //  renvoie l’index trouvé
        } else {
            fseek(fichier, o.nbAuteurs * sizeof(Personne), SEEK_CUR);
        }
    }

    return -1; // non trouvé
}




/**
 * Supprime un ouvrage de la bibliothèque
 * @param fichier : fichier contenant les ouvrages
 * @param numero : numéro de l'ouvrage à supprimer
 */

void supprimer_Ouvrage(FILE** fichier, unsigned int numero) {
    if (!*fichier) {
        fprintf(stderr, "Fichier invalide\n");
        return;
    }

    unsigned int continuer;
    do {
        printf("\nVoulez-vous vraiment supprimer l'ouvrage #%u ? (1 = Oui, 0 = Non) : ", numero);
        if (scanf("%u", &continuer) != 1) {
            printf("Entree invalide. Veuillez entrer 1 ou 0.\n");
            while (getchar() != '\n'); // vider le buffer
            continuer = 0;
            continue;
        }
        vider_buffer();
        if (continuer != 0 && continuer != 1) {
            printf("Valeur invalide. Tapez 1 pour Oui ou 0 pour Non.\n");
            continuer = 0;
        }
        break;
    } while (1);

    if (continuer == 0) {
        printf("Suppression annulee.\n");
        return;
    }

    Ouvrage o;
    FILE *temp = fopen("temp.dat", "wb");
    if (!temp) {
        fprintf(stderr, "Erreur création fichier temporaire\n");
        return;
    }

    rewind(*fichier);
    bool trouve = false;

    while (lire_ouvrage(*fichier, &o)) {
        if (o.numero == numero) {
            trouve = true;
            printf("Ouvrage #%u supprimé.\n", numero);
        } else {
            ecrire_ouvrage(temp, &o);
        }
        liberer_auteurs(&o);
    }

    if (!trouve) {
        printf("Ouvrage #%u non trouve.\n", numero);
    }

    fclose(*fichier);
    fclose(temp);
    remove(FICHIER_BIBLIO);
    rename("temp.dat", FICHIER_BIBLIO);
    *fichier = fopen(FICHIER_BIBLIO, "rb+");
}


/**
 * Compte le nombre total d’ouvrages dans le fichier.
 * Utilise lire_ouvrage pour uniformiser la lecture.
 */

unsigned int compter_ouvrages(FILE* fichier) {
    if (fichier == NULL) return 0;

    Ouvrage o;
    unsigned int count = 0;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o)) {
        count++;
        liberer_auteurs(&o);
    }

    return count;
}


/**
 * Vérifie si un numéro d’ouvrage est unique.
 */
bool verifier_numero_unique(FILE* fichier, unsigned int numero) {
    if (fichier == NULL) return true;  // Fichier inexistant -> considéré unique

    Ouvrage o;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o)) {
        if (o.numero == numero) {
            liberer_auteurs(&o);
            return false;  // Numéro déjà présent
        }
        liberer_auteurs(&o);
    }
    return true;  // Numéro non trouvé → unique
}

/* ============================================================
 * FONCTIONS DE MODIFICATION
 * ============================================================ */

void modifier_titre(FILE** fichier, unsigned int numero) {
    if (!*fichier) return;
    FILE* temp = fopen("temp.dat", "wb");
    if (!temp) { perror("Erreur création fichier temporaire"); return; }

    Ouvrage o;
    rewind(*fichier);
    unsigned int trouve = 0;

    while (lire_ouvrage(*fichier, &o)) {
        if (o.numero == numero) {
            printf("Nouveau titre : ");
            fgets(o.titre, MAX_TITRE, stdin);
            o.titre[strcspn(o.titre, "\n")] = '\0';
            trouve = 1;
        }
        ecrire_ouvrage(temp, &o);
        liberer_auteurs(&o);
    }

    fclose(*fichier);
    fclose(temp);
    remove(FICHIER_BIBLIO);
    rename("temp.dat", FICHIER_BIBLIO);
    *fichier = fopen(FICHIER_BIBLIO, "rb+");

    if (trouve) printf(" Titre modifie.\n");
    else printf(" Ouvrage #%u non trouve.\n", numero);
}

void modifier_editeur(FILE** fichier, unsigned int numero) {
    if (!*fichier) return;
    FILE* temp = fopen("temp.dat", "wb");
    if (!temp) { perror("Erreur création fichier temporaire"); return; }

    Ouvrage o;
    rewind(*fichier);
    unsigned int trouve = 0;

    while (lire_ouvrage(*fichier, &o)) {
        if (o.numero == numero) {
            printf("Nouvel editeur (nom, prenom, ID) :\n");
            fgets(o.editeur.nom, MAX_NOM, stdin);
            o.editeur.nom[strcspn(o.editeur.nom, "\n")] = '\0';
            fgets(o.editeur.prenom, MAX_PRENOM, stdin);
            o.editeur.prenom[strcspn(o.editeur.prenom, "\n")] = '\0';
            fgets(o.editeur.identifiant, MAX_ID, stdin);
            o.editeur.identifiant[strcspn(o.editeur.identifiant, "\n")] = '\0';
            trouve = 1;
        }
        ecrire_ouvrage(temp, &o);
        liberer_auteurs(&o);
    }

    fclose(*fichier);
    fclose(temp);
    remove(FICHIER_BIBLIO);
    rename("temp.dat", FICHIER_BIBLIO);
    *fichier = fopen(FICHIER_BIBLIO, "rb+");

    if (trouve) printf(" Editeur modifie.\n");
    else printf(" Ouvrage #%u non trouve.\n", numero);
}

void modifier_annee(FILE** fichier, unsigned int numero) {
    if (!*fichier) return;
    FILE* temp = fopen("temp.dat", "wb");
    if (!temp) { perror("Erreur création fichier temporaire"); return; }

    Ouvrage o;
    rewind(*fichier);
    unsigned int trouve = 0;

    while (lire_ouvrage(*fichier, &o)) {
        if (o.numero == numero) {
            unsigned int nouvelle_annee;
            do {
                printf("Nouvelle annee : ");
                if (scanf("%u", &nouvelle_annee) != 1) {
                    printf("Entree invalide. Veuillez entrer un nombre.\n");
                    while (getchar() != '\n');
                    continue;
                }
                vider_buffer();

                if (nouvelle_annee < ANNEE_MIN || nouvelle_annee > annee_courante()) {
                    printf("Annee invalide (%u). Veuillez entrer une annee entre %d et %u.\n",
                           nouvelle_annee, ANNEE_MIN, annee_courante());
                } else {
                    o.dateEdition.annee = nouvelle_annee;
                    trouve = 1;
                }
            } while (!trouve);
        }
        ecrire_ouvrage(temp, &o);
        liberer_auteurs(&o);
    }

    fclose(*fichier);
    fclose(temp);
    remove(FICHIER_BIBLIO);
    rename("temp.dat", FICHIER_BIBLIO);
    *fichier = fopen(FICHIER_BIBLIO, "rb+");

    if (trouve) printf("Annee modifiee.\n");
    else printf("Ouvrage #%u non trouve.\n", numero);
}


void modifier_quantite(FILE** fichier, unsigned int numero) {
    if (!*fichier) return;
    FILE* temp = fopen("temp.dat", "wb");
    if (!temp) { perror("Erreur création fichier temporaire"); return; }

    Ouvrage o;
    rewind(*fichier);
    unsigned int trouve = 0;

    while (lire_ouvrage(*fichier, &o)) {
        if (o.numero == numero) {
            do {
                printf("Nouvelle quantite : ");
                if (scanf("%u", &o.quantite) != 1) {
                    printf("Entree invalide. Veuillez entrer un nombre.\n");
                    while (getchar() != '\n');
                    continue;
                }
                vider_buffer();
                break;
            } while (1);
            trouve = 1;
        }
        ecrire_ouvrage(temp, &o);
        liberer_auteurs(&o);
    }

    fclose(*fichier);
    fclose(temp);
    remove(FICHIER_BIBLIO);
    rename("temp.dat", FICHIER_BIBLIO);
    *fichier = fopen(FICHIER_BIBLIO, "rb+");

    if (trouve) printf("Quantite modifiee.\n");
    else printf("Ouvrage #%u non trouve.\n", numero);
}


void modifier_date_edition(FILE** fichier, unsigned int numero) {
    if (!*fichier) return;
    FILE* temp = fopen("temp.dat", "wb");
    if (!temp) { perror("Erreur création fichier temporaire"); return; }

    Ouvrage o;
    rewind(*fichier);
    unsigned int trouve = 0;

    while (lire_ouvrage(*fichier, &o)) {
        if (o.numero == numero) {
            printf("\n--- Nouvelle date d'edition ---\n");
            lecture_date(&o.dateEdition);
            trouve = 1;
        }
        ecrire_ouvrage(temp, &o);
        liberer_auteurs(&o);
    }

    fclose(*fichier);
    fclose(temp);
    remove(FICHIER_BIBLIO);
    rename("temp.dat", FICHIER_BIBLIO);
    *fichier = fopen(FICHIER_BIBLIO, "rb+");

    if (trouve) printf("Date d'edition modifiee.\n");
    else printf("Ouvrage #%u non trouvé.\n", numero);
}



void modifier_auteur_par_id(FILE** fichier, unsigned int numero, const char* id_auteur) {
    if (!*fichier) return;
    FILE* temp = fopen("temp.dat", "wb");
    if (!temp) { perror("Erreur création fichier temporaire"); return; }

    Ouvrage o;
    rewind(*fichier);
    unsigned int trouve_ouvrage = 0, trouve_auteur = 0;

    while (lire_ouvrage(*fichier, &o)) {
        if (o.numero == numero) {
            trouve_ouvrage = 1;
            for (unsigned int i = 0; i < o.nbAuteurs; i++) {
                if (strcmp(o.auteurs[i]->identifiant, id_auteur) == 0) {
                    printf("Auteur trouve (ID=%s).\n", id_auteur);
                    printf("Nouveau nom : ");
                    fgets(o.auteurs[i]->nom, MAX_NOM, stdin);
                    o.auteurs[i]->nom[strcspn(o.auteurs[i]->nom, "\n")] = '\0';
                    printf("Nouveau prénom : ");
                    fgets(o.auteurs[i]->prenom, MAX_PRENOM, stdin);
                    o.auteurs[i]->prenom[strcspn(o.auteurs[i]->prenom, "\n")] = '\0';
                    trouve_auteur = 1;
                    break;
                }
            }
            if (!trouve_auteur) {
                printf("Auteur avec ID=%s non trouve dans cet ouvrage.\n", id_auteur);
            }
        }
        ecrire_ouvrage(temp, &o);
        liberer_auteurs(&o);
    }

    fclose(*fichier);
    fclose(temp);
    remove(FICHIER_BIBLIO);
    rename("temp.dat", FICHIER_BIBLIO);
    *fichier = fopen(FICHIER_BIBLIO, "rb+");

    if (!trouve_ouvrage) printf("Ouvrage #%u non trouve.\n", numero);
    else if (trouve_auteur) printf("Auteur modifie.\n");
}


/* ============================================================
 * FONCTIONS DE STATISTIQUES
 * ============================================================ */

unsigned int Nb_Ouvrages_au_moins_n_auteurs(FILE* fichier, unsigned int n) {
    if (fichier == NULL) return 0;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o)) {
        if (o.nbAuteurs >= n) count++;
        liberer_auteurs(&o);
    }
    return count;
}

unsigned int Nb_Ouvrages_par_editeur(FILE* fichier, Personne editeur) {
    if (fichier == NULL) return 0;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o)) {
        if (strcmp(o.editeur.identifiant, editeur.identifiant) == 0) count++;
        liberer_auteurs(&o);
    }
    return count;
}

unsigned int Nb_Ouvrages_par_auteur(FILE* fichier, Personne auteur) {
    if (fichier == NULL) return 0;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o)) {
        for (unsigned int i = 0; i < o.nbAuteurs; i++) {
            if (strcmp(o.auteurs[i]->identifiant, auteur.identifiant) == 0) {
                count++;
                break;
            }
        }
        liberer_auteurs(&o);
    }
    return count;
}

unsigned int Nb_Ouvrages_Editeur_Date(FILE* fichier, Personne editeur, Date d) {
    if (fichier == NULL) return 0;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o)) {
        if (strcmp(o.editeur.identifiant, editeur.identifiant) == 0 &&
            Date_Egale(o.dateEdition, d)) {
            count++;
        }
        liberer_auteurs(&o);
    }
    return count;
}

unsigned int Nb_Ouvrages_quantite_egale_auteurs(FILE* fichier) {
    if (fichier == NULL) return 0;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o)) {
        if (o.quantite == o.nbAuteurs) count++;
        liberer_auteurs(&o);
    }
    return count;
}

unsigned int Nb_Ouvrages_Editeur_Entre_Annees(FILE* fichier, Personne editeur, unsigned int anneeDebut, unsigned int anneeFin) {
    if (fichier == NULL) return 0;
    if (anneeDebut > anneeFin) Permutation(&anneeDebut, &anneeFin);

    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o)) {
        if (strcmp(o.editeur.identifiant, editeur.identifiant) == 0 &&
            o.dateEdition.annee >= anneeDebut && o.dateEdition.annee <= anneeFin) {
            count++;
        }
        liberer_auteurs(&o);
    }
    return count;
}

unsigned int Nb_auteurs_intervalle_annees(FILE* fichier, unsigned int anneeDebut, unsigned int anneeFin) {
    if (fichier == NULL) return 0;
    if (anneeDebut > anneeFin) Permutation(&anneeDebut, &anneeFin);

    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o)) {
        if (o.dateEdition.annee >= anneeDebut && o.dateEdition.annee <= anneeFin) {
            for (unsigned int i = 0; i < o.nbAuteurs; i++) {
                bool deja = false;
                for (unsigned int j = 0; j < i; j++) {
                    if (Deja_Compter(*(o.auteurs[i]), *(o.auteurs[j]))) {
                        deja = true;
                        break;
                    }
                }
                if (!deja) count++;
            }
        }
        liberer_auteurs(&o);
    }
    return count;
}

unsigned int Nb_Ouvrages_Date_Quantite(FILE* fichier, Date d, unsigned int Quantite) {
    if (fichier == NULL) return 0;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o)) {
        if (Date_Egale(o.dateEdition, d) && o.quantite == Quantite) count++;
        liberer_auteurs(&o);
    }
    return count;
}

unsigned int Nb_Ouvrages_meme_titre(FILE* fichier) {
    if (fichier == NULL) return 0;
    Ouvrage o1, o2; unsigned int count = 0;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o1)) {
        long pos1 = ftell(fichier);
        rewind(fichier);
        bool dejaCompte = false;

        while (lire_ouvrage(fichier, &o2)) {
            if (strcmp(o1.titre, o2.titre) == 0 && o1.numero != o2.numero) {
                dejaCompte = true;
                liberer_auteurs(&o2);
                break;
            }
            liberer_auteurs(&o2);
        }

        if (dejaCompte) count++;
        liberer_auteurs(&o1);
        fseek(fichier, pos1, SEEK_SET);
    }
    return count;
}

unsigned int Nb_Ouvrages_auteur_editeur(FILE* fichier, Personne auteur, Personne editeur) {
    if (fichier == NULL) return 0;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    while (lire_ouvrage(fichier, &o)) {
        bool auteurTrouve = false;
        for (unsigned int i = 0; i < o.nbAuteurs; i++) {
            if (strcmp(o.auteurs[i]->identifiant, auteur.identifiant) == 0) {
                auteurTrouve = true;
                break;
            }
        }
        if (auteurTrouve && strcmp(o.editeur.identifiant, editeur.identifiant) == 0) {
            count++;
        }
        liberer_auteurs(&o);
    }
    return count;
}

/* ============================================================
 * FONCTIONS DE LISTES
 * ============================================================ */

void Liste_Ouvrages_au_moins_n_auteurs(FILE* fichier, unsigned int n) {
    if (fichier == NULL) return;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    printf("\n=== Ouvrages avec au moins %u auteur(s) ===\n", n);

    while (lire_ouvrage(fichier, &o)) {
        if (o.nbAuteurs >= n) {
            count++;
            printf("\n--- Ouvrage #%u ---\n", count);
            affichage_ouvrage(&o);
        }
        liberer_auteurs(&o);
    }

    if (count == 0) printf(" Il n'en existe pas\n");
    else printf("\nTotal : %u ouvrage(s).\n", count);
}

void Liste_Ouvrages_par_editeur(FILE* fichier, Personne editeur) {
    if (fichier == NULL) return;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    printf("\n=== Ouvrages edites par (ID: %s) ===\n", editeur.identifiant);

    while (lire_ouvrage(fichier, &o)) {
        if (strcmp(o.editeur.identifiant, editeur.identifiant) == 0) {
            count++;
            printf("\n--- Ouvrage #%u ---\n", count);
            affichage_ouvrage(&o);
        }
        liberer_auteurs(&o);
    }

    if (count == 0) printf(" Il n'en existe pas\n");
    else printf("\nTotal : %u ouvrage(s).\n", count);
}

void Liste_Ouvrages_par_auteur(FILE* fichier, Personne auteur) {
    if (fichier == NULL) return;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    printf("\n=== Ouvrages de l'auteur (ID: %s) ===\n", auteur.identifiant);
    while (lire_ouvrage(fichier, &o)) {
        for (unsigned int i = 0; i < o.nbAuteurs; i++) {
            if (strcmp(o.auteurs[i]->identifiant, auteur.identifiant) == 0) {
                count++;
                printf("\n--- Ouvrage #%u ---\n", count);
                affichage_ouvrage(&o);
                break;
            }
        }
        liberer_auteurs(&o);
    }

    if (count == 0) printf("Il n'en existe pas\n");
    else printf("\nTotal : %u ouvrage(s).\n", count);
}

void Liste_Ouvrages_Editeur_Date(FILE* fichier, Personne editeur, Date d) {
    if (fichier == NULL) return;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    printf("\n=== Ouvrages edites par (ID: %s) le ", editeur.identifiant);
    affichage_date(d); printf(" ===\n");

    while (lire_ouvrage(fichier, &o)) {
        if (strcmp(o.editeur.identifiant, editeur.identifiant) == 0 &&
            Date_Egale(o.dateEdition, d)) {
            count++;
            printf("\n--- Ouvrage #%u ---\n", count);
            affichage_ouvrage(&o);
        }
        liberer_auteurs(&o);
    }

    if (count == 0) printf(" Il n'en existe pas\n");
    else printf("\nTotal : %u ouvrage(s).\n", count);
}

void Liste_Ouvrages_quantite_egale_auteurs(FILE* fichier) {
    if (fichier == NULL) return;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    printf("\n=== Ouvrages ou quantite = nombre d'auteurs ===\n");

    while (lire_ouvrage(fichier, &o)) {
        if (o.quantite == o.nbAuteurs) {
            count++;
            printf("\n--- Ouvrage #%u ---\n", count);
            affichage_ouvrage(&o);
        }
        liberer_auteurs(&o);
    }

    if (count == 0) printf(" Il n'en existe pas\n");
    else printf("\nTotal : %u ouvrage(s).\n", count);
}

void Liste_Ouvrages_Editeur_Entre_Annees(FILE* fichier, Personne editeur,unsigned int anneeDebut, unsigned int anneeFin) {
    if (fichier == NULL) return;
    if (anneeDebut > anneeFin) Permutation(&anneeDebut, &anneeFin);

    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    printf("\n=== Ouvrages edites par (ID: %s) entre %u et %u ===\n",
           editeur.identifiant, anneeDebut, anneeFin);

    while (lire_ouvrage(fichier, &o)) {
        if (strcmp(o.editeur.identifiant, editeur.identifiant) == 0 &&
            o.dateEdition.annee >= anneeDebut && o.dateEdition.annee <= anneeFin) {
            count++;
            printf("\n--- Ouvrage #%u ---\n", count);
            affichage_ouvrage(&o);
        }
        liberer_auteurs(&o);
    }

    if (count == 0) printf(" Il n'en existe pas\n");
    else printf("\nTotal : %u ouvrage(s).\n", count);
}

void Liste_auteurs_intervalle_annees(FILE* fichier, unsigned int anneeDebut, unsigned int anneeFin) {
    if (fichier == NULL) return;
    if (anneeDebut > anneeFin) Permutation(&anneeDebut, &anneeFin);

    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    printf("\n=== Auteurs actifs entre %u et %u ===\n", anneeDebut, anneeFin);

    while (lire_ouvrage(fichier, &o)) {
        if (o.dateEdition.annee >= anneeDebut && o.dateEdition.annee <= anneeFin) {
            for (unsigned int i = 0; i < o.nbAuteurs; i++) {
                Personne* auteur = o.auteurs[i];
                bool deja = false;
                for (unsigned int j = 0; j < i; j++) {
                    if (Deja_Compter(*auteur, *(o.auteurs[j]))) {
                        deja = true;
                        break;
                    }
                }
                if (!deja) {
                    count++;
                    affichage_d_un_auteur(&o, i);
                }
            }
        }
        liberer_auteurs(&o);
    }

    printf("\nTotal : %u auteur(s).\n", count);
}

void Liste_Ouvrages_Date_Quantite(FILE* fichier, Date d, unsigned int Quantite) {
    if (fichier == NULL) return;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    printf("\n=== Ouvrages edites le "); affichage_date(d);
    printf(" avec quantite %u ===\n", Quantite);

    while (lire_ouvrage(fichier, &o)) {
        if (Date_Egale(o.dateEdition, d) && o.quantite == Quantite) {
            count++;
            printf("\n--- Ouvrage #%u ---\n", count);
            affichage_ouvrage(&o);
        }
        liberer_auteurs(&o);
    }

    if (count == 0) printf(" Il n'en existe pas\n");
    else printf("\nTotal : %u ouvrage(s).\n", count);
}


void Liste_Ouvrages_meme_titre(FILE* fichier) {
    if (fichier == NULL) return;
    Ouvrage o1, o2; unsigned int count = 0;
    rewind(fichier);

    printf("\n=== Ouvrages avec titres identiques ===\n");

    while (lire_ouvrage(fichier, &o1)) {
        long pos = ftell(fichier);
        rewind(fichier);
        unsigned int doublons = 0;

        while (lire_ouvrage(fichier, &o2)) {
            if (strcmp(o1.titre, o2.titre) == 0 && o1.numero != o2.numero) {
                doublons++;
            }
            liberer_auteurs(&o2);
        }

        if (doublons > 0) {
            count++;
            printf("\n--- Ouvrage #%u ---\n", count);
            affichage_ouvrage(&o1);
        }

        liberer_auteurs(&o1);
        fseek(fichier, pos, SEEK_SET);
    }

    if (count == 0) printf(" Il n'en existe pas\n");
    else printf("\nTotal : %u ouvrage(s) avec titres identiques.\n", count);
}

void Liste_Ouvrages_auteur_editeur(FILE* fichier, Personne auteur, Personne editeur) {
    if (fichier == NULL) return;
    Ouvrage o; unsigned int count = 0;
    rewind(fichier);

    printf("\n=== Ouvrages de l'auteur (ID: %s) edites par (ID: %s) ===\n",
       auteur.identifiant, editeur.identifiant);
    while (lire_ouvrage(fichier, &o)) {
        bool auteurTrouve = false;
        for (unsigned int i = 0; i < o.nbAuteurs; i++) {
            if (strcmp(o.auteurs[i]->identifiant, auteur.identifiant) == 0) {
                auteurTrouve = true;
                break;
            }
        }
        if (auteurTrouve && strcmp(o.editeur.identifiant, editeur.identifiant) == 0) {
            count++;
            printf("\n--- Ouvrage #%u ---\n", count);
            affichage_ouvrage(&o);
        }
        liberer_auteurs(&o);
    }

    if (count == 0) printf(" Il n'en existe pas \n");
    else printf("\nTotal : %u ouvrage(s).\n", count);
}
