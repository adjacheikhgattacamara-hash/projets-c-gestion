#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Maquette.h"

// ============================================================
// FONCTIONS UTILITAIRES
// ============================================================

/*
 * But: Ouvre le fichier en lecture/écriture, le crée s'il n'existe pas.
 * Paramètres: nomFichier : nom du fichier à ouvrir
 * Retour: Pointeur FILE* ouvert, ou NULL en cas d'échec
 */
FILE* OuvertureFichier(const char *nomFichier) {
    FILE *fichier = fopen(nomFichier, "r+");
    if (fichier == NULL) {
        fichier = fopen(nomFichier, "w+");
        if (fichier != NULL) {
            // Écrire un en-tête explicatif (lignes commençant par #)
            fprintf(fichier, "# Fichier maquette pedagogique\n");
            fprintf(fichier, "# Format : UE -> code;credits;coeff;libelle;nb_EC\n");
            fprintf(fichier, "# Puis pour chaque EC -> code;libelle;coeff;volume_horaire\n");
            fprintf(fichier, "# Les lignes commençant par # sont des commentaires.\n");
            fflush(fichier);
        }
    }
    return fichier;
}

/*
 * But: Vide le buffer d'entrée après un scanf.
 * Paramètres: aucun
 * Retour: aucun
 */
void vider_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    if (c == EOF) clearerr(stdin);
}

/*
 * But: Attend que l'utilisateur appuie sur Entrée (gère EOF proprement).
 * Paramètres: aucun
 * Retour: aucun
 */
void attendre_entree(void) {
    printf("\nAppuyez sur Entree pour continuer...");
    fflush(stdout);
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
    if (c == EOF) clearerr(stdin);
}

/*
 * But: Échange deux entiers.
 * Paramètres: a, b : pointeurs vers les entiers à échanger
 * Retour: aucun
 */
void permutation_entier(unsigned int *a, unsigned int *b) {
    unsigned int temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * But: Échange deux réels.
 * Paramètres: a, b : pointeurs vers les réels à échanger
 * Retour: aucun
 */
void permutation_reel(float *a, float *b) {
    float temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * But: Vérifie l'unicité d'un code UE dans le fichier (restaure la position).
 * Paramètres: fichier : fichier de la maquette, code : code à vérifier, codeExclu : code à exclure (ou NULL)
 * Retour: true si unique, false sinon
 */
bool verifier_code_UE_unique(FILE *fichier, const char* code, const char* codeExclu) {
    if (fichier == NULL) return true;
    long pos = ftell(fichier);
    rewind(fichier);
    clearerr(fichier);
    Unite_Enseignement ue;
    bool unique = true;
    while (lire_UE(fichier, &ue)) {
        if (strcmp(ue.Code_UE, code) == 0) {
            if (codeExclu == NULL || strcmp(ue.Code_UE, codeExclu) != 0) {
                unique = false;
                liberer_EC(&ue);
                break;
            }
        }
        liberer_EC(&ue);
    }
    fseek(fichier, pos, SEEK_SET);
    return unique;
}

/*
 * But: Vérifie si un code EC est unique dans une UE (parmi les EC déjà saisis).
 * Paramètres: ue : pointeur vers l'UE, codeEC : code à vérifier, indexCourant : indice de l'EC en cours
 * Retour: true si unique, false sinon
 */
bool verifier_code_EC_unique(Unite_Enseignement *ue, const char *codeEC, unsigned int indexCourant) {
    for (unsigned int i = 0; i < indexCourant; i++) {
        if (strcmp(ue->ECs[i].Code_EC, codeEC) == 0)
            return false;
    }
    return true;
}

/*
 * But: Vérifie si un code EC est unique dans tout le fichier (restaure position).
 * Paramètres: fichier : fichier de la maquette, codeEC : code à vérifier, codeECExclu : code à exclure (ou NULL)
 * Retour: true si unique, false sinon
 */
bool verifier_code_EC_unique_fichier(FILE *fichier, const char *codeEC, const char *codeECExclu) {
    if (!fichier) return true;
    long pos = ftell(fichier);
    rewind(fichier);
    clearerr(fichier);
    Unite_Enseignement ue;
    bool unique = true;
    while (lire_UE(fichier, &ue)) {
        for (unsigned int i = 0; i < ue.Nb_EC; i++) {
            if (strcmp(ue.ECs[i].Code_EC, codeEC) == 0) {
                if (codeECExclu == NULL || strcmp(ue.ECs[i].Code_EC, codeECExclu) != 0) {
                    unique = false;
                    break;
                }
            }
        }
        liberer_EC(&ue);
        if (!unique) break;
    }
    fseek(fichier, pos, SEEK_SET);
    return unique;
}

/*
 * But: Vérifie si une UE est déjà présente dans un fichier de suivi.
 * Paramètres: fichierSuivi : fichier de suivi temporaire, ue : pointeur vers l'UE
 * Retour: true si présente, false sinon
 */
bool ue_est_dans_fichier(FILE *fichierSuivi, Unite_Enseignement *ue) {
    if (!fichierSuivi) return false;
    Unite_Enseignement temp;
    rewind(fichierSuivi);
    clearerr(fichierSuivi);
    while (lire_UE(fichierSuivi, &temp)) {
        if (strcmp(temp.Code_UE, ue->Code_UE) == 0) {
            liberer_EC(&temp);
            return true;
        }
        liberer_EC(&temp);
    }
    return false;
}

/*
 * But: Compte le nombre d'UE ayant le même crédit que l'UE située à une position donnée.
 * Paramètres: fichier : fichier principal, position : position de l'UE cible, fichierSuivi : fichier de suivi temporaire
 * Retour: nombre d'UE (y compris la cible) ayant le même crédit
 */
unsigned int compter_UE_meme_credit_position(FILE *fichier, long position, FILE *fichierSuivi) {
    if (!fichier) return 0;

    // Sauvegarde de la position actuelle du fichier principal
    long pos_originale = ftell(fichier);

    // Se positionner sur l'UE cible
    fseek(fichier, position, SEEK_SET);
    if (ferror(fichier)) {
        fseek(fichier, pos_originale, SEEK_SET);
        return 0;
    }

    Unite_Enseignement ueCourant;
    if (!lire_UE(fichier, &ueCourant)) {
        fseek(fichier, pos_originale, SEEK_SET);
        return 0;
    }

    if (ue_est_dans_fichier(fichierSuivi, &ueCourant)) {
        liberer_EC(&ueCourant);
        fseek(fichier, pos_originale, SEEK_SET);
        return 0;
    }

    unsigned int creditCible = ueCourant.Credit_UE;
    unsigned int compte = 1;

    // Marquer cette UE dans le fichier de suivi
    fseek(fichierSuivi, 0, SEEK_END);
    ecrire_UE(fichierSuivi, &ueCourant);
    liberer_EC(&ueCourant);

    // Parcourir tout le fichier pour compter les UE ayant le même crédit
    rewind(fichier);
    clearerr(fichier);
    Unite_Enseignement ue;
    while (lire_UE(fichier, &ue)) {
        if (!ue_est_dans_fichier(fichierSuivi, &ue) && ue.Credit_UE == creditCible) {
            compte++;
            fseek(fichierSuivi, 0, SEEK_END);
            ecrire_UE(fichierSuivi, &ue);
        }
        liberer_EC(&ue);
    }

    // Restaurer la position du fichier principal
    fseek(fichier, pos_originale, SEEK_SET);
    return compte;
}

// ============================================================
// SAISIE SÉCURISÉE FACTORISÉE
// ============================================================

/*
 * But: Saisie d'un entier > 0 avec message.
 * Paramètres: message : texte à afficher
 * Retour: entier saisi (strictement positif)
 */
unsigned int saisir_entier(const char *message) {
    unsigned int valeur;
    int ret;
    do {
        printf("%s", message);
        ret = scanf("%u", &valeur);
        if (ret == EOF) {
            clearerr(stdin);
            printf("Entree invalide. Reessayez.\n");
            continue;
        }
        if (ret != 1 || valeur == 0) {
            printf("Entree invalide. Veuillez entrer un entier positif.\n");
            vider_buffer();
            continue;
        }
        vider_buffer();
        break;
    } while (1);
    return valeur;
}

/*
 * But: Saisie d'un flottant > 0 avec message.
 * Paramètres: message : texte à afficher
 * Retour: flottant saisi (strictement positif)
 */
float saisir_flottant(const char *message) {
    float valeur;
    int ret;
    do {
        printf("%s", message);
        ret = scanf("%f", &valeur);
        if (ret == EOF) {
            printf("\nFin d'entree inattendue. Arret.\n");
            exit(0);
        }
        if (ret != 1 || valeur <= 0) {
            printf("Entree invalide. Veuillez entrer un nombre reel positif.\n");
            while (getchar() != '\n');
            continue;
        }
        vider_buffer();
        break;
    } while (1);
    return valeur;
}

/*
 * But: Saisie d'une chaîne non vide avec message.
 * Paramètres: message : texte à afficher, buffer : tableau de réception, taille : taille du buffer
 * Retour: aucun (remplit buffer)
 */
void saisir_chaine(const char *message, char *buffer, size_t taille) {
    if (taille == 0) return;
    do {
        printf("%s", message);
        fgets(buffer, taille, stdin);
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        } else {
            buffer[taille-1] = '\0';
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    } while (strlen(buffer) == 0);
}

/*
 * But: Saisie d'un choix binaire (1 ou 0) avec message.
 * Paramètres: message : texte à afficher
 * Retour: 1 pour Oui, 0 pour Non
 */
unsigned int saisir_choix_binaire(const char *message) {
    unsigned int choix;
    int ret;
    do {
        printf("%s (1=Oui, 0=Non) : ", message);
        ret = scanf("%u", &choix);
        if (ret == EOF) {
            printf("\nFin d'entree inattendue. Arret.\n");
            exit(0);
        }
        if (ret != 1 || (choix != 0 && choix != 1)) {
            printf("Entree invalide. Tapez 1 ou 0.\n");
            while (getchar() != '\n');
            choix = 0;
            continue;
        }
        vider_buffer();
        break;
    } while (1);
    return choix;
}

/*
 * But: Saisie d'un choix entre 0 et max.
 * Paramètres: max : valeur maximale autorisée
 * Retour: entier saisi dans l'intervalle [0, max]
 */
unsigned int saisir_choix(unsigned int max) {
    unsigned int choix;
    int ret;
    do {
        ret = scanf("%u", &choix);
        if (ret == EOF) {
            printf("\nFin d'entree inattendue. Arret.\n");
            exit(0);
        }
        if (ret != 1 || choix > max) {
            printf("Entree invalide. Veuillez entrer un nombre entre 0 et %u.\n", max);
            while (getchar() != '\n');
            choix = 0;
            continue;
        }
        vider_buffer();
        break;
    } while (1);
    return choix;
}

// ============================================================
// GESTION MÉMOIRE EC
// ============================================================

/*
 * But: Libère la mémoire allouée pour les EC d'une UE.
 * Paramètres: ue : pointeur vers l'UE
 * Retour: aucun
 */
void liberer_EC(Unite_Enseignement *ue) {
    if (ue->ECs != NULL) {
        free(ue->ECs);
        ue->ECs = NULL;
        ue->Nb_EC = 0;
    }
}

/*
 * But: Lit depuis le fichier tous les EC d'une UE déjà lue.
 * Paramètres: fichier : fichier de la maquette, ue : pointeur vers l'UE à compléter
 * Retour: true en cas de succès, false sinon
 */
bool charger_EC(FILE *fichier, Unite_Enseignement *ue) {
    ue->ECs = (Element_Constitutif*)malloc(ue->Nb_EC * sizeof(Element_Constitutif));
    if (ue->ECs == NULL) {
        fprintf(stderr, "Erreur d'allocation memoire pour les EC\n");
        exit(EXIT_FAILURE);
    }
    for (unsigned int i = 0; i < ue->Nb_EC; i++) {
        if (fscanf(fichier, "%19[^;];%99[^;];%u;%f\n",
                   ue->ECs[i].Code_EC,
                   ue->ECs[i].Libelle_EC,
                   &ue->ECs[i].Coefficient_EC,
                   &ue->ECs[i].Volume_Horaire_EC) != 4) {
            liberer_EC(ue);
            return false;
        }
    }
    return true;
}

// ============================================================
// GESTION DES ECs
// ============================================================

/*
 * But: Saisie d'un EC avec vérifications d'unicité.
 * Paramètres: ec : pointeur vers l'EC, ue : pointeur vers l'UE parente, fichier : fichier de la maquette, indexCourant : indice de l'EC
 * Retour: aucun
 */
void lecture_EC(Element_Constitutif *ec, Unite_Enseignement *ue, FILE *fichier, unsigned int indexCourant) {
    do {
        saisir_chaine("Code EC : ", ec->Code_EC, MAX_CODE);
        if (!verifier_code_EC_unique(ue, ec->Code_EC, indexCourant)) {
            printf("Erreur : ce code EC existe deja dans cette UE.\n");
            continue;
        }
        if (!verifier_code_EC_unique_fichier(fichier, ec->Code_EC, NULL)) {
            printf("Erreur : ce code EC existe deja dans une autre UE.\n");
            continue;
        }
        break;
    } while (1);

    saisir_chaine("Libelle EC : ", ec->Libelle_EC, MAX_LIBELLE);
    ec->Coefficient_EC = saisir_entier("Coefficient EC : ");
    ec->Volume_Horaire_EC = saisir_flottant("Volume horaire EC : ");
}

/*
 * But: Affiche un EC.
 * Paramètres: ec : pointeur vers l'EC
 * Retour: aucun
 */
void affichage_EC(Element_Constitutif *ec) {
    printf("+----------------------+----------------------------------+\n");
    printf("| Code EC              | %-32s |\n", ec->Code_EC);
    printf("+----------------------+----------------------------------+\n");
    printf("| Libelle EC           | %-32s |\n", ec->Libelle_EC);
    printf("+----------------------+----------------------------------+\n");
    printf("| Coefficient EC       | %-32u |\n", ec->Coefficient_EC);
    printf("+----------------------+----------------------------------+\n");
    printf("| Volume horaire EC    | %-32.2f |\n", ec->Volume_Horaire_EC);
    printf("+----------------------+----------------------------------+\n");
}

/*
 * But: Saisie d'un tableau d'EC pour une UE.
 * Paramètres: ue : pointeur vers l'UE à remplir, fichier : fichier de la maquette
 * Retour: aucun
 */
void lecture_tableau_EC(Unite_Enseignement *ue, FILE *fichier) {
    ue->Nb_EC = saisir_entier("Nombre d EC : ");
    ue->ECs = (Element_Constitutif*)calloc(ue->Nb_EC, sizeof(Element_Constitutif));
    if (!ue->ECs) {
        fprintf(stderr, "Erreur d'allocation memoire pour les EC\n");
        exit(EXIT_FAILURE);
    }
    for (unsigned int i = 0; i < ue->Nb_EC; i++) {
        printf("\n--- EC %u ---\n", i + 1);
        lecture_EC(&ue->ECs[i], ue, fichier, i);
    }
}

// ============================================================
// GESTION DES UE
// ============================================================

/*
 * But: Saisie complète d'une UE (code, crédits, coefficient, libellé, EC).
 * Paramètres: ue : pointeur vers l'UE à remplir, fichier : fichier de la maquette
 * Retour: aucun
 */
void lecture_UE(Unite_Enseignement *ue, FILE *fichier) {
    do {
        saisir_chaine("Code UE : ", ue->Code_UE, MAX_CODE);
        if (!verifier_code_UE_unique(fichier, ue->Code_UE, NULL)) {
            printf("Erreur : ce code UE existe deja.\n");
            continue;
        }
        break;
    } while (1);

    ue->Credit_UE = saisir_entier("Credits UE : ");
    ue->Coefficient_UE = saisir_entier("Coefficient UE : ");
    saisir_chaine("Libelle UE : ", ue->Libelle_UE, MAX_LIBELLE);
    lecture_tableau_EC(ue, fichier);
}

/*
 * But: Affiche une UE avec tous ses EC.
 * Paramètres: ue : pointeur vers l'UE
 * Retour: aucun
 */
void affichage_UE(Unite_Enseignement *ue) {
    printf("\n+------------------------------------------------------------+\n");
    printf("|                     DETAILS DE L'UE                        |\n");
    printf("+------------------------------------------------------------+\n");
    printf("| Code UE       : %-42s |\n", ue->Code_UE);
    printf("+------------------------------------------------------------+\n");
    printf("| Credits       : %-42u |\n", ue->Credit_UE);
    printf("+------------------------------------------------------------+\n");
    printf("| Coefficient   : %-42u |\n", ue->Coefficient_UE);
    printf("+------------------------------------------------------------+\n");
    printf("| Libelle       : %-42s |\n", ue->Libelle_UE);
    printf("+------------------------------------------------------------+\n");
    printf("| Nombre d EC   : %-42u |\n", ue->Nb_EC);
    printf("+------------------------------------------------------------+\n");
    for (unsigned int i = 0; i < ue->Nb_EC; i++) {
        printf("\nEC %u :\n", i + 1);
        affichage_EC(&ue->ECs[i]);
    }
}

/*
 * But: Affiche l'UE située à une position donnée dans le fichier.
 * Paramètres: fichier : fichier de la maquette, pos : position dans le fichier
 * Retour: aucun
 */
void afficher_UE_par_pos(FILE *fichier, long pos) {
    if (fichier == NULL || pos < 0) {
        printf("Position invalide.\n");
        return;
    }
    Unite_Enseignement ue;
    fseek(fichier, pos, SEEK_SET);
    if (ferror(fichier)) {
        printf("Erreur de positionnement.\n");
        return;
    }
    if (!lire_UE(fichier, &ue)) {
        printf("Impossible de lire l'UE a la position %ld.\n", pos);
        return;
    }
    affichage_UE(&ue);
    liberer_EC(&ue);
}

// ============================================================
// FICHIER TEXTE
// ============================================================

/*
 * But: Écrit une UE dans le fichier (format texte).
 * Paramètres: fichier : fichier de destination, ue : pointeur vers l'UE
 * Retour: aucun
 */
void ecrire_UE(FILE *fichier, Unite_Enseignement *ue) {
    fprintf(fichier, "%s;%u;%u;%s;%u\n",
            ue->Code_UE,
            ue->Credit_UE,
            ue->Coefficient_UE,
            ue->Libelle_UE,
            ue->Nb_EC);
    for (unsigned int i = 0; i < ue->Nb_EC; i++) {
        fprintf(fichier, "%s;%s;%u;%.2f\n",
                ue->ECs[i].Code_EC,
                ue->ECs[i].Libelle_EC,
                ue->ECs[i].Coefficient_EC,
                ue->ECs[i].Volume_Horaire_EC);
    }
    fflush(fichier);
}

/*
 * But: Lit une UE depuis le fichier (format texte).
 * Paramètres: fichier : fichier source, ue : pointeur vers l'UE à remplir
 * Retour: true en cas de succès, false sinon (fin de fichier ou erreur)
 */
bool lire_UE(FILE *fichier, Unite_Enseignement *ue) {
    if (fichier == NULL) return false;

    char buffer[256];
    int c;

    // Sauter les lignes vides et les commentaires
    while (1) {
        // Lire le premier caractère non blanc
        do {
            c = fgetc(fichier);
            if (c == EOF) return false;
        } while (c == '\n' || c == '\r' || c == ' ' || c == '\t');

        if (c == '#') {
            // Ligne de commentaire : on lit jusqu'à la fin de ligne
            while (c != '\n' && c != EOF) c = fgetc(fichier);
            continue; // passer à la ligne suivante
        } else {
            // Ce n'est pas un commentaire, on remet le caractère lu
            ungetc(c, fichier);
            break;
        }
    }

    // Maintenant on lit la ligne de l'UE
    if (fgets(buffer, sizeof(buffer), fichier) == NULL) return false;

    // Extraire les champs de l'UE (même format qu'avant)
    if (sscanf(buffer, "%19[^;];%u;%u;%99[^;];%u",
               ue->Code_UE,
               &ue->Credit_UE,
               &ue->Coefficient_UE,
               ue->Libelle_UE,
               &ue->Nb_EC) != 5) {
        return false;
               }

    return charger_EC(fichier, ue);
}

// ============================================================
// GESTION DE LA MAQUETTE
// ============================================================

/*
 * But: Ajoute une UE à la fin du fichier après vérifications.
 * Paramètres: fichier : fichier de la maquette
 * Retour: aucun
 */
void ajouter_UE(FILE *fichier) {
    if (!fichier) {
        fprintf(stderr, "Fichier invalide\n");
        return;
    }
    Unite_Enseignement ue;
    lecture_UE(&ue, fichier);
    if (!verifier_code_UE_unique(fichier, ue.Code_UE, NULL)) {
        printf("Erreur : une UE avec le code %s existe deja.\n", ue.Code_UE);
        liberer_EC(&ue);
        return;
    }
    for (unsigned int i = 0; i < ue.Nb_EC; i++) {
        if (!verifier_code_EC_unique_fichier(fichier, ue.ECs[i].Code_EC, NULL)) {
            printf("Erreur : code EC %s existe deja dans une autre UE.\n", ue.ECs[i].Code_EC);
            liberer_EC(&ue);
            return;
        }
    }
    fseek(fichier, 0, SEEK_END);
    ecrire_UE(fichier, &ue);
    printf("UE ajoutee avec succes.\n");
    liberer_EC(&ue);
}

/*
 * But: Affiche toutes les UE de la maquette.
 * Paramètres: fichier : fichier de la maquette
 * Retour: aucun
 */
void afficher_maquette(FILE *fichier) {
    if (!fichier) {
        fprintf(stderr, "Fichier invalide\n");
        return;
    }

    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    int count = 0;

    printf("\n============================================================\n");
    printf("|                 MAQUETTE PEDAGOGIQUE                     |\n");
    printf("============================================================\n\n");

    while (lire_UE(fichier, &ue)) {
        count++;
        printf("------------------------------------------------------------\n");
        printf("|                     UE #%d                                |\n", count);
        printf("------------------------------------------------------------\n");
        affichage_UE(&ue);
        liberer_EC(&ue);
        printf("\n");
    }

    if (count == 0)
        printf("Aucune UE enregistree.\n");
}


/*
 * But: Supprime une UE du fichier (avec fichier temporaire).
 * Paramètres: fichier : pointeur vers le pointeur du fichier principal, codeUE : code de l'UE à supprimer
 * Retour: aucun
 */
void supprimer_UE(FILE **fichier, const char *codeUE) {
    if (!*fichier) return;

    unsigned int continuer = saisir_choix_binaire("Voulez-vous vraiment supprimer l'UE ?");
    if (continuer == 0) {
        printf("Suppression annulee.\n");
        return;
    }

    const char *tempName = "temp_suppr.txt";
    FILE *temp = fopen(tempName, "w");
    if (!temp) {
        fprintf(stderr, "Erreur creation fichier temporaire\n");
        return;
    }

    Unite_Enseignement ue;
    rewind(*fichier);
    clearerr(*fichier);
    bool trouve = false;
    bool erreur = false;

    while (lire_UE(*fichier, &ue)) {
        if (strcmp(ue.Code_UE, codeUE) == 0) {
            trouve = true;
            printf("UE %s supprimee.\n", codeUE);
        } else {
            ecrire_UE(temp, &ue);
        }
        if (ferror(temp)) {
            fprintf(stderr, "Erreur d'ecriture dans le fichier temporaire\n");
            erreur = true;
            liberer_EC(&ue);
            break;
        }
        liberer_EC(&ue);
    }

    if (!trouve) printf("UE %s non trouvee.\n", codeUE);

    fclose(*fichier);
    fclose(temp);

    if (!erreur) {
        remove(FICHIER_MAQUETTE);
        rename(tempName, FICHIER_MAQUETTE);
    } else {
        remove(tempName);
    }

    *fichier = fopen(FICHIER_MAQUETTE, "r+");
    if (!*fichier) {
        fprintf(stderr, "Erreur reouverture du fichier principal\n");
        exit(1);
    }
}

/*
 * But: Recherche la position d'une UE par son code.
 * Paramètres: fichier : fichier de la maquette, codeUE : code recherché
 * Retour: position dans le fichier, ou -1 si non trouvée
 */
long rechercher_UE(FILE *fichier, const char *codeUE) {
    if (!fichier) return -1;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    long pos;
    while (1) {
        pos = ftell(fichier);
        if (!lire_UE(fichier, &ue)) break;
        if (strcmp(ue.Code_UE, codeUE) == 0) {
            liberer_EC(&ue);
            return pos;
        }
        liberer_EC(&ue);
    }
    return -1;
}

/*
 * But: Recherche la position de l'UE ayant le plus grand crédit.
 * Paramètres: fichier : fichier de la maquette
 * Retour: position de l'UE, ou -1 si aucun UE
 */
long rechercher_UE_max_credit(FILE *fichier) {
    if (!fichier) return -1;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    long posMax = -1;
    unsigned int maxCredit = 0;
    long pos;
    while (1) {
        pos = ftell(fichier);
        if (!lire_UE(fichier, &ue)) break;
        if (ue.Credit_UE > maxCredit) {
            maxCredit = ue.Credit_UE;
            posMax = pos;
        }
        liberer_EC(&ue);
    }
    return posMax;
}

/*
 * But: Recherche la position de l'UE ayant le plus d'EC.
 * Paramètres: fichier : fichier de la maquette
 * Retour: position de l'UE, ou -1 si aucun UE
 */
long rechercher_UE_max_EC(FILE *fichier) {
    if (!fichier) return -1;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    long posMax = -1;
    unsigned int maxEC = 0;
    long pos;
    while (1) {
        pos = ftell(fichier);
        if (!lire_UE(fichier, &ue)) break;
        if (ue.Nb_EC > maxEC) {
            maxEC = ue.Nb_EC;
            posMax = pos;
        }
        liberer_EC(&ue);
    }
    return posMax;
}

/*
 * But: Saisie en boucle d'ajout d'UE.
 * Paramètres: fichier : fichier de la maquette
 * Retour: aucun
 */
void Enregistrement(FILE *fichier) {
    if (!fichier) {
        fprintf(stderr, "Fichier invalide\n");
        return;
    }
    unsigned int continuer = 1;
    do {
        ajouter_UE(fichier);
        continuer = saisir_choix_binaire("Voulez-vous ajouter une autre UE");
    } while (continuer == 1);
    printf("Enregistrement termine.\n");
}

// ============================================================
// CALCULS ET VÉRIFICATIONS
// ============================================================

/*
 * But: Calcule le volume horaire total d'une UE (somme des EC).
 * Paramètres: ue : pointeur vers l'UE
 * Retour: somme des volumes horaires des EC
 */
float volume_horaire_total(Unite_Enseignement *ue) {
    float total = 0.0f;
    for (unsigned int i = 0; i < ue->Nb_EC; i++)
        total += ue->ECs[i].Volume_Horaire_EC;
    return total;
}

/*
 * But: Teste si un volume horaire est dans un intervalle [min, max].
 * Paramètres: volume : valeur à tester, min : borne inférieure, max : borne supérieure
 * Retour: true si dans l'intervalle, false sinon
 */
bool est_dans_intervalle_volume_horaire(float volume, float min, float max) {
    return (volume >= min && volume <= max);
}

/*
 * But: Teste si un crédit est dans un intervalle [min, max].
 * Paramètres: credit : valeur à tester, min : borne inférieure, max : borne supérieure
 * Retour: true si dans l'intervalle, false sinon
 */
bool est_dans_intervalle_credits(unsigned int credit, unsigned int min, unsigned int max) {
    return (credit >= min && credit <= max);
}

/*
 * But: Compte le nombre total d'UE dans le fichier (restaure la position).
 * Paramètres: fichier : fichier de la maquette
 * Retour: nombre d'UE
 */
unsigned int compter_UE(FILE *fichier) {
    if (!fichier) return 0;
    long pos = ftell(fichier);
    rewind(fichier);
    clearerr(fichier);
    Unite_Enseignement ue;
    unsigned int count = 0;
    while (lire_UE(fichier, &ue)) {
        count++;
        liberer_EC(&ue);
    }
    fseek(fichier, pos, SEEK_SET);
    return count;
}

// ============================================================
// STATISTIQUES
// ============================================================

/*
 * But: Compte le nombre total d'UE qui partagent le même crédit qu'au moins une autre UE.
 * Paramètres: fichier : fichier de la maquette
 * Retour: nombre d'UE concernées
 */
unsigned int nb_UE_meme_credit(FILE *fichier) {
    if (!fichier) return 0;
    FILE *suivi = fopen("temp_suivi.txt", "w+");
    if (!suivi) {
        fprintf(stderr, "Erreur creation fichier temporaire\n");
        return 0;
    }

    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    long pos;
    unsigned int total = 0;

    while (1) {
        pos = ftell(fichier);
        if (!lire_UE(fichier, &ue)) break;

        if (!ue_est_dans_fichier(suivi, &ue)) {
            unsigned int nb = compter_UE_meme_credit_position(fichier, pos, suivi);
            if (nb > 1) total += nb;
        }
        liberer_EC(&ue);
    }

    fclose(suivi);
    remove("temp_suivi.txt");
    return total;
}

/*
 * But: Compte le nombre d'UE dont le volume horaire total > 20 * crédits.
 * Paramètres: fichier : fichier de la maquette
 * Retour: nombre d'UE vérifiant la condition
 */
unsigned int nb_UE_totHoraire_sup20Credit(FILE *fichier) {
    if (!fichier) return 0;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    unsigned int count = 0;
    while (lire_UE(fichier, &ue)) {
        if (volume_horaire_total(&ue) > (20.0f * ue.Credit_UE))
            count++;
        liberer_EC(&ue);
    }
    return count;
}

/*
 * But: Compte le nombre d'EC ayant un coefficient donné dans les UE d'un crédit donné.
 * Paramètres: fichier : fichier de la maquette, coeff : coefficient recherché, credit : crédit de l'UE
 * Retour: nombre d'EC correspondants
 */
unsigned int nb_EC_coeff_credit_precis(FILE *fichier, unsigned int coeff, unsigned int credit) {
    if (!fichier) return 0;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    unsigned int count = 0;
    while (lire_UE(fichier, &ue)) {
        if (ue.Credit_UE == credit) {
            for (unsigned int i = 0; i < ue.Nb_EC; i++) {
                if (ue.ECs[i].Coefficient_EC == coeff)
                    count++;
            }
        }
        liberer_EC(&ue);
    }
    return count;
}

/*
 * But: Compte le nombre d'UE avec crédit dans un intervalle.
 * Paramètres: fichier : fichier de la maquette, min : borne inférieure, max : borne supérieure
 * Retour: nombre d'UE concernées
 */
unsigned int nb_UE_intervalle_credits(FILE *fichier, unsigned int min, unsigned int max) {
    if (!fichier) return 0;
    if (min > max) permutation_entier(&min, &max);
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    unsigned int count = 0;
    while (lire_UE(fichier, &ue)) {
        if (est_dans_intervalle_credits(ue.Credit_UE, min, max))
            count++;
        liberer_EC(&ue);
    }
    return count;
}

/*
 * But: Compte le nombre d'EC dont le libellé est identique à celui de leur UE.
 * Paramètres: fichier : fichier de la maquette
 * Retour: nombre d'EC correspondants
 */
unsigned int nb_EC_meme_nom_UE(FILE *fichier) {
    if (!fichier) return 0;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    unsigned int count = 0;
    while (lire_UE(fichier, &ue)) {
        for (unsigned int i = 0; i < ue.Nb_EC; i++) {
            if (strcmp(ue.ECs[i].Libelle_EC, ue.Libelle_UE) == 0)
                count++;
        }
        liberer_EC(&ue);
    }
    return count;
}

/*
 * But: Compte le nombre d'EC avec volume horaire dans un intervalle.
 * Paramètres: fichier : fichier de la maquette, min : borne inférieure, max : borne supérieure
 * Retour: nombre d'EC concernés
 */
unsigned int nb_EC_intervalle_volume(FILE *fichier, float min, float max) {
    if (!fichier) return 0;
    if (min > max) {
        permutation_reel(&min, &max);
    }
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    unsigned int count = 0;
    while (lire_UE(fichier, &ue)) {
        for (unsigned int i = 0; i < ue.Nb_EC; i++) {
            if (est_dans_intervalle_volume_horaire(ue.ECs[i].Volume_Horaire_EC, min, max))
                count++;
        }
        liberer_EC(&ue);
    }
    return count;
}

/*
 * But: Compte le nombre d'UE ayant exactement nbEC EC.
 * Paramètres: fichier : fichier de la maquette, nbEC : nombre d'EC recherché
 * Retour: nombre d'UE correspondantes
 */
unsigned int nb_UE_nbEC_precis(FILE *fichier, unsigned int nbEC) {
    if (!fichier) return 0;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    unsigned int count = 0;
    while (lire_UE(fichier, &ue)) {
        if (ue.Nb_EC == nbEC) count++;
        liberer_EC(&ue);
    }
    return count;
}

/*
 * But: Compte le nombre d'UE ayant le crédit maximum.
 * Paramètres: fichier : fichier de la maquette
 * Retour: nombre d'UE avec le crédit max
 */
unsigned int nb_UE_max_credits(FILE *fichier) {
    if (!fichier) return 0;
    long posMax = rechercher_UE_max_credit(fichier);
    if (posMax == -1) return 0;
    Unite_Enseignement ueMax;
    fseek(fichier, posMax, SEEK_SET);
    if (!lire_UE(fichier, &ueMax)) return 0;
    unsigned int maxCredit = ueMax.Credit_UE;
    liberer_EC(&ueMax);
    rewind(fichier);
    clearerr(fichier);
    Unite_Enseignement ue;
    unsigned int count = 0;
    while (lire_UE(fichier, &ue)) {
        if (ue.Credit_UE == maxCredit) count++;
        liberer_EC(&ue);
    }
    return count;
}

/*
 * But: Compte le nombre d'UE où crédit = nombre d'EC.
 * Paramètres: fichier : fichier de la maquette
 * Retour: nombre d'UE vérifiant l'égalité
 */
unsigned int nb_UE_credit_egal_nbEC(FILE *fichier) {
    if (!fichier) return 0;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    unsigned int count = 0;
    while (lire_UE(fichier, &ue)) {
        if (ue.Credit_UE == ue.Nb_EC) count++;
        liberer_EC(&ue);
    }
    return count;
}

/*
 * But: Compte le nombre d'UE ayant le plus grand nombre d'EC.
 * Paramètres: fichier : fichier de la maquette
 * Retour: nombre d'UE avec le max d'EC
 */
unsigned int nb_UE_max_EC(FILE *fichier) {
    if (!fichier) return 0;
    long posMax = rechercher_UE_max_EC(fichier);
    if (posMax == -1) return 0;
    Unite_Enseignement ueMax;
    fseek(fichier, posMax, SEEK_SET);
    if (!lire_UE(fichier, &ueMax)) return 0;
    unsigned int maxEC = ueMax.Nb_EC;
    liberer_EC(&ueMax);
    rewind(fichier);
    clearerr(fichier);
    Unite_Enseignement ue;
    unsigned int count = 0;
    while (lire_UE(fichier, &ue)) {
        if (ue.Nb_EC == maxEC) count++;
        liberer_EC(&ue);
    }
    return count;
}

// ============================================================
// LISTES
// ============================================================

/*
 * But: Liste les UE ayant le même nombre de crédits (au moins deux).
 * Paramètres: fichier : fichier de la maquette
 * Retour: aucun
 */
void liste_UE_meme_credit(FILE *fichier) {
    if (!fichier) return;
    FILE *suivi = fopen("temp_suivi.txt", "w+");
    if (!suivi) {
        fprintf(stderr, "Erreur creation fichier temporaire\n");
        return;
    }

    printf("\n--- Liste des UE ayant le meme nombre de credits ---\n");

    // ---------- Première passe : récupérer les crédits en doublon ----------
    unsigned int *credits_groupes = NULL;
    unsigned int nb_groupes = 0;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    long pos;

    while (1) {
        pos = ftell(fichier);
        if (!lire_UE(fichier, &ue)) break;

        if (!ue_est_dans_fichier(suivi, &ue)) {
            unsigned int nb = compter_UE_meme_credit_position(fichier, pos, suivi);
            if (nb > 1) {
                // Éviter les doublons dans le tableau des crédits
                int deja = 0;
                for (unsigned int i = 0; i < nb_groupes; i++) {
                    if (credits_groupes[i] == ue.Credit_UE) {
                        deja = 1;
                        break;
                    }
                }
                if (!deja) {
                    unsigned int *tmp = realloc(credits_groupes,
                                                (nb_groupes + 1) * sizeof(unsigned int));
                    if (tmp) {
                        credits_groupes = tmp;
                        credits_groupes[nb_groupes++] = ue.Credit_UE;
                    }
                }
            }
        }
        liberer_EC(&ue);
    }

    // ---------- Deuxième passe : affichage par groupe ----------
    int trouve = 0;
    for (unsigned int i = 0; i < nb_groupes; i++) {
        printf("\n--- Credits : %u ---\n", credits_groupes[i]);
        rewind(fichier);
        clearerr(fichier);
        while (lire_UE(fichier, &ue)) {
            if (ue.Credit_UE == credits_groupes[i]) {
                affichage_UE(&ue);
                trouve = 1;
            }
            liberer_EC(&ue);
        }
    }

    if (!trouve) {
        printf("Aucune UE ne partage le meme credit avec une autre.\n");
    }

    free(credits_groupes);
    fclose(suivi);
    remove("temp_suivi.txt");
}

/*
 * But: Liste des UE vérifiant volume_horaire > 20 * crédits.
 * Paramètres: fichier : fichier de la maquette
 * Retour: aucun
 */
void liste_UE_totHoraire_sup20Credit(FILE *fichier) {
    if (!fichier) return;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    printf("\n--- Liste des UE dont total horaire > 20 x credits ---\n");
    int trouve = 0;
    while (lire_UE(fichier, &ue)) {
        if (volume_horaire_total(&ue) > (20.0f * ue.Credit_UE)) {
            affichage_UE(&ue);
            trouve = 1;
        }
        liberer_EC(&ue);
    }
    if (!trouve) printf("Aucune UE ne verifie cette condition.\n");
}

/*
 * But: Liste des EC avec coefficient donné dans UE à crédit donné.
 * Paramètres: fichier : fichier de la maquette, coeff : coefficient recherché, credit : crédit de l'UE
 * Retour: aucun
 */
void liste_EC_coeff_credit_precis(FILE *fichier, unsigned int coeff, unsigned int credit) {
    if (!fichier) return;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    printf("\n--- Liste des EC avec coeff %u dans UE a %u credits ---\n", coeff, credit);
    int trouve = 0;
    while (lire_UE(fichier, &ue)) {
        if (ue.Credit_UE == credit) {
            for (unsigned int i = 0; i < ue.Nb_EC; i++) {
                if (ue.ECs[i].Coefficient_EC == coeff) {
                    affichage_EC(&ue.ECs[i]);
                    trouve = 1;
                }
            }
        }
        liberer_EC(&ue);
    }
    if (!trouve) printf("Aucun EC ne correspond a ces criteres.\n");
}

/*
 * But: Liste des UE avec crédit dans un intervalle.
 * Paramètres: fichier : fichier de la maquette, min : borne inférieure, max : borne supérieure
 * Retour: aucun
 */
void liste_UE_intervalle_credits(FILE *fichier, unsigned int min, unsigned int max) {
    if (!fichier) return;
    if (min > max) permutation_entier(&min, &max);
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    printf("\n--- Liste des UE avec credits entre %u et %u ---\n", min, max);
    int trouve = 0;
    while (lire_UE(fichier, &ue)) {
        if (est_dans_intervalle_credits(ue.Credit_UE, min, max)) {
            affichage_UE(&ue);
            trouve = 1;
        }
        liberer_EC(&ue);
    }
    if (!trouve) printf("Aucune UE dans cet intervalle.\n");
}

/*
 * But: Liste des EC ayant même libellé que leur UE.
 * Paramètres: fichier : fichier de la maquette
 * Retour: aucun
 */
void liste_EC_meme_nom_UE(FILE *fichier) {
    if (!fichier) return;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    printf("\n--- Liste des EC ayant le meme nom que leur UE ---\n");
    int trouve = 0;
    while (lire_UE(fichier, &ue)) {
        for (unsigned int i = 0; i < ue.Nb_EC; i++) {
            if (strcmp(ue.ECs[i].Libelle_EC, ue.Libelle_UE) == 0) {
                affichage_EC(&ue.ECs[i]);
                trouve = 1;
            }
        }
        liberer_EC(&ue);
    }
    if (!trouve) printf("Aucun EC ne porte le meme nom que son UE.\n");
}

/*
 * But: Liste des EC avec volume horaire dans un intervalle.
 * Paramètres: fichier : fichier de la maquette, min : borne inférieure, max : borne supérieure
 * Retour: aucun
 */
void liste_EC_intervalle_volume(FILE *fichier, float min, float max) {
    if (!fichier) return;
    if (min > max) permutation_reel(&min, &max);
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    printf("\n--- Liste des EC avec volume entre %.2f et %.2f ---\n", min, max);
    int trouve = 0;
    while (lire_UE(fichier, &ue)) {
        for (unsigned int i = 0; i < ue.Nb_EC; i++) {
            if (est_dans_intervalle_volume_horaire(ue.ECs[i].Volume_Horaire_EC, min, max)) {
                affichage_EC(&ue.ECs[i]);
                trouve = 1;
            }
        }
        liberer_EC(&ue);
    }
    if (!trouve) printf("Aucun EC dans cet intervalle de volume.\n");
}

/*
 * But: Liste des UE ayant exactement nbEC EC.
 * Paramètres: fichier : fichier de la maquette, nbEC : nombre d'EC recherché
 * Retour: aucun
 */
void liste_UE_nbEC_precis(FILE *fichier, unsigned int nbEC) {
    if (!fichier) return;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    printf("\n--- Liste des UE avec %u EC ---\n", nbEC);
    int trouve = 0;
    while (lire_UE(fichier, &ue)) {
        if (ue.Nb_EC == nbEC) {
            affichage_UE(&ue);
            trouve = 1;
        }
        liberer_EC(&ue);
    }
    if (!trouve) printf("Aucune UE avec exactement %u EC.\n", nbEC);
}

/*
 * But: Liste des UE ayant le crédit maximum.
 * Paramètres: fichier : fichier de la maquette
 * Retour: aucun
 */
void liste_UE_max_credits(FILE *fichier) {
    if (!fichier) return;
    long posMax = rechercher_UE_max_credit(fichier);
    if (posMax == -1) { printf("Aucune UE trouvee.\n"); return; }
    Unite_Enseignement ueMax;
    fseek(fichier, posMax, SEEK_SET);
    if (!lire_UE(fichier, &ueMax)) return;
    unsigned int maxCredit = ueMax.Credit_UE;
    liberer_EC(&ueMax);
    rewind(fichier);
    clearerr(fichier);
    Unite_Enseignement ue;
    printf("\n--- Liste des UE avec credit maximum (%u) ---\n", maxCredit);
    int trouve = 0;
    while (lire_UE(fichier, &ue)) {
        if (ue.Credit_UE == maxCredit) {
            affichage_UE(&ue);
            trouve = 1;
        }
        liberer_EC(&ue);
    }
    if (!trouve) printf("Aucune UE avec le credit maximum.\n");
}

/*
 * But: Liste des UE où crédit = nombre d'EC.
 * Paramètres: fichier : fichier de la maquette
 * Retour: aucun
 */
void liste_UE_credit_egal_nbEC(FILE *fichier) {
    if (!fichier) return;
    Unite_Enseignement ue;
    rewind(fichier);
    clearerr(fichier);
    printf("\n--- Liste des UE avec credits = nombre d EC ---\n");
    int trouve = 0;
    while (lire_UE(fichier, &ue)) {
        if (ue.Credit_UE == ue.Nb_EC) {
            affichage_UE(&ue);
            trouve = 1;
        }
        liberer_EC(&ue);
    }
    if (!trouve) printf("Aucune UE ne verifie credits = nb EC.\n");
}

/*
 * But: Liste des UE ayant le plus d'EC.
 * Paramètres: fichier : fichier de la maquette
 * Retour: aucun
 */
void liste_UE_max_EC(FILE *fichier) {
    if (!fichier) return;
    long posMax = rechercher_UE_max_EC(fichier);
    if (posMax == -1) { printf("Aucune UE trouvee.\n"); return; }
    Unite_Enseignement ueMax;
    fseek(fichier, posMax, SEEK_SET);
    if (!lire_UE(fichier, &ueMax)) return;
    unsigned int maxEC = ueMax.Nb_EC;
    liberer_EC(&ueMax);
    rewind(fichier);
    clearerr(fichier);
    Unite_Enseignement ue;
    printf("\n--- Liste des UE avec maximum d EC (%u) ---\n", maxEC);
    int trouve = 0;
    while (lire_UE(fichier, &ue)) {
        if (ue.Nb_EC == maxEC) {
            affichage_UE(&ue);
            trouve = 1;
        }
        liberer_EC(&ue);
    }
    if (!trouve) printf("Aucune UE avec le maximum d EC.\n");
}

// ============================================================
// MODIFICATIONS
// ============================================================

/*
 * But: Modifie le code d'une UE (vérifie unicité).
 * Paramètres: fichier : pointeur vers le pointeur du fichier principal, codeUE : code de l'UE à modifier
 * Retour: aucun
 */
void modifier_code_UE(FILE **fichier, const char *codeUE) {
    if (!*fichier) return;
    unsigned int continuer = saisir_choix_binaire("Voulez-vous vraiment modifier cette UE ?");
    if (continuer == 0) {
        printf("Modification annulee.\n");
        return;
    }
    const char *tempName = "temp_modif.txt";
    FILE *temp = fopen(tempName, "w");
    if (!temp) {
        fprintf(stderr, "Erreur creation fichier temporaire\n");
        return;
    }
    Unite_Enseignement ue;
    rewind(*fichier);
    clearerr(*fichier);
    bool trouve = false;
    bool erreur = false;
    while (lire_UE(*fichier, &ue)) {
        if (strcmp(ue.Code_UE, codeUE) == 0) {
            trouve = true;
            char nouveauCode[MAX_CODE];
            do {
                saisir_chaine("Nouveau code UE : ", nouveauCode, MAX_CODE);
            } while (!verifier_code_UE_unique(*fichier, nouveauCode, ue.Code_UE));
            strcpy(ue.Code_UE, nouveauCode);
            printf("Code UE modifie.\n");
        }
        ecrire_UE(temp, &ue);
        if (ferror(temp)) {
            fprintf(stderr, "Erreur d'ecriture dans le fichier temporaire\n");
            erreur = true;
            liberer_EC(&ue);
            break;
        }
        liberer_EC(&ue);
    }
    if (!trouve) printf("UE %s non trouvee.\n", codeUE);
    fclose(*fichier);
    fclose(temp);
    if (!erreur) {
        remove(FICHIER_MAQUETTE);
        rename(tempName, FICHIER_MAQUETTE);
    } else {
        remove(tempName);
    }
    *fichier = fopen(FICHIER_MAQUETTE, "r+");
    if (!*fichier) exit(1);
}

/*
 * But: Modifie le libellé d'une UE.
 * Paramètres: fichier : pointeur vers le pointeur du fichier principal, codeUE : code de l'UE à modifier
 * Retour: aucun
 */
void modifier_libelle_UE(FILE **fichier, const char *codeUE) {
    if (!*fichier) return;
    unsigned int continuer = saisir_choix_binaire("Voulez-vous vraiment modifier cette UE ?");
    if (continuer == 0) {
        printf("Modification annulee.\n");
        return;
    }
    const char *tempName = "temp_modif.txt";
    FILE *temp = fopen(tempName, "w");
    if (!temp) {
        fprintf(stderr, "Erreur creation fichier temporaire\n");
        return;
    }
    Unite_Enseignement ue;
    rewind(*fichier);
    clearerr(*fichier);
    bool trouve = false;
    bool erreur = false;
    while (lire_UE(*fichier, &ue)) {
        if (strcmp(ue.Code_UE, codeUE) == 0) {
            trouve = true;
            saisir_chaine("Nouveau libelle UE : ", ue.Libelle_UE, MAX_LIBELLE);
            printf("Libelle UE modifie.\n");
        }
        ecrire_UE(temp, &ue);
        if (ferror(temp)) {
            fprintf(stderr, "Erreur d'ecriture dans le fichier temporaire\n");
            erreur = true;
            liberer_EC(&ue);
            break;
        }
        liberer_EC(&ue);
    }
    if (!trouve) printf("UE %s non trouvee.\n", codeUE);
    fclose(*fichier);
    fclose(temp);
    if (!erreur) {
        remove(FICHIER_MAQUETTE);
        rename(tempName, FICHIER_MAQUETTE);
    } else {
        remove(tempName);
    }
    *fichier = fopen(FICHIER_MAQUETTE, "r+");
    if (!*fichier) exit(1);
}

/*
 * But: Modifie le crédit d'une UE.
 * Paramètres: fichier : pointeur vers le pointeur du fichier principal, codeUE : code de l'UE à modifier
 * Retour: aucun
 */
void modifier_credit_UE(FILE **fichier, const char *codeUE) {
    if (!*fichier) return;
    unsigned int continuer = saisir_choix_binaire("Voulez-vous vraiment modifier cette UE ?");
    if (continuer == 0) {
        printf("Modification annulee.\n");
        return;
    }
    const char *tempName = "temp_modif.txt";
    FILE *temp = fopen(tempName, "w");
    if (!temp) {
        fprintf(stderr, "Erreur creation fichier temporaire\n");
        return;
    }
    Unite_Enseignement ue;
    rewind(*fichier);
    clearerr(*fichier);
    bool trouve = false;
    bool erreur = false;
    while (lire_UE(*fichier, &ue)) {
        if (strcmp(ue.Code_UE, codeUE) == 0) {
            trouve = true;
            ue.Credit_UE = saisir_entier("Nouveau credit UE : ");
            printf("Credit UE modifie.\n");
        }
        ecrire_UE(temp, &ue);
        if (ferror(temp)) {
            fprintf(stderr, "Erreur d'ecriture dans le fichier temporaire\n");
            erreur = true;
            liberer_EC(&ue);
            break;
        }
        liberer_EC(&ue);
    }
    if (!trouve) printf("UE %s non trouvee.\n", codeUE);
    fclose(*fichier);
    fclose(temp);
    if (!erreur) {
        remove(FICHIER_MAQUETTE);
        rename(tempName, FICHIER_MAQUETTE);
    } else {
        remove(tempName);
    }
    *fichier = fopen(FICHIER_MAQUETTE, "r+");
    if (!*fichier) exit(1);
}

/*
 * But: Modifie le coefficient d'une UE.
 * Paramètres: fichier : pointeur vers le pointeur du fichier principal, codeUE : code de l'UE à modifier
 * Retour: aucun
 */
void modifier_coefficient_UE(FILE **fichier, const char *codeUE) {
    if (!*fichier) return;
    unsigned int continuer = saisir_choix_binaire("Voulez-vous vraiment modifier cette UE ?");
    if (continuer == 0) {
        printf("Modification annulee.\n");
        return;
    }
    const char *tempName = "temp_modif.txt";
    FILE *temp = fopen(tempName, "w");
    if (!temp) {
        fprintf(stderr, "Erreur creation fichier temporaire\n");
        return;
    }
    Unite_Enseignement ue;
    rewind(*fichier);
    clearerr(*fichier);
    bool trouve = false;
    bool erreur = false;
    while (lire_UE(*fichier, &ue)) {
        if (strcmp(ue.Code_UE, codeUE) == 0) {
            trouve = true;
            ue.Coefficient_UE = saisir_entier("Nouveau coefficient UE : ");
            printf("Coefficient UE modifie.\n");
        }
        ecrire_UE(temp, &ue);
        if (ferror(temp)) {
            fprintf(stderr, "Erreur d'ecriture dans le fichier temporaire\n");
            erreur = true;
            liberer_EC(&ue);
            break;
        }
        liberer_EC(&ue);
    }
    if (!trouve) printf("UE %s non trouvee.\n", codeUE);
    fclose(*fichier);
    fclose(temp);
    if (!erreur) {
        remove(FICHIER_MAQUETTE);
        rename(tempName, FICHIER_MAQUETTE);
    } else {
        remove(tempName);
    }
    *fichier = fopen(FICHIER_MAQUETTE, "r+");
    if (!*fichier) exit(1);
}

/*
 * But: Ajoute un ou plusieurs EC à une UE existante.
 *      L'utilisateur saisit les caractéristiques du nouvel EC,
 *      avec vérification d'unicité du code EC (dans l'UE et dans toute la maquette).
 *      Après chaque ajout, on demande si l'utilisateur souhaite en ajouter un autre.
 * Paramètres: fichier : pointeur vers le pointeur du fichier principal
 *             codeUE  : code de l'UE cible
 * Retour: aucun
 */
void ajouter_EC(FILE **fichier, const char *codeUE) {
    if (!*fichier) return;

    unsigned int continuer;
    do {
        // --- Lire l'UE cible ---
        Unite_Enseignement ue;
        long pos = rechercher_UE(*fichier, codeUE);
        if (pos == -1) {
            printf("UE %s non trouvee.\n", codeUE);
            return;
        }
        fseek(*fichier, pos, SEEK_SET);
        if (!lire_UE(*fichier, &ue)) {
            printf("Erreur de lecture de l'UE.\n");
            return;
        }

        // --- Saisie du nouvel EC ---
        Element_Constitutif nouvelEC;
        do {
            saisir_chaine("Code EC : ", nouvelEC.Code_EC, MAX_CODE);
            if (!verifier_code_EC_unique(&ue, nouvelEC.Code_EC, ue.Nb_EC)) {
                printf("Erreur : ce code EC existe deja dans cette UE.\n");
                continue;
            }
            if (!verifier_code_EC_unique_fichier(*fichier, nouvelEC.Code_EC, NULL)) {
                printf("Erreur : ce code EC existe deja dans une autre UE.\n");
                continue;
            }
            break;
        } while (1);

        saisir_chaine("Libelle EC : ", nouvelEC.Libelle_EC, MAX_LIBELLE);
        nouvelEC.Coefficient_EC = saisir_entier("Coefficient EC : ");
        nouvelEC.Volume_Horaire_EC = saisir_flottant("Volume horaire EC : ");

        // --- Réallouer le tableau d'EC ---
        ue.Nb_EC++;
        ue.ECs = (Element_Constitutif*)realloc(ue.ECs, ue.Nb_EC * sizeof(Element_Constitutif));
        if (!ue.ECs) {
            fprintf(stderr, "Erreur d'allocation memoire.\n");
            exit(EXIT_FAILURE);
        }
        ue.ECs[ue.Nb_EC - 1] = nouvelEC;

        // --- Écrire la maquette mise à jour via fichier temporaire ---
        const char *tempName = "temp_modif.txt";
        FILE *temp = fopen(tempName, "w");
        if (!temp) {
            fprintf(stderr, "Erreur creation fichier temporaire\n");
            liberer_EC(&ue);
            return;
        }

        rewind(*fichier);
        clearerr(*fichier);
        Unite_Enseignement ueCourante;
        bool erreur = false;

        while (lire_UE(*fichier, &ueCourante)) {
            if (strcmp(ueCourante.Code_UE, codeUE) == 0) {
                ecrire_UE(temp, &ue);
            } else {
                ecrire_UE(temp, &ueCourante);
            }
            if (ferror(temp)) {
                fprintf(stderr, "Erreur d'ecriture dans le fichier temporaire\n");
                erreur = true;
                liberer_EC(&ueCourante);
                break;
            }
            liberer_EC(&ueCourante);
        }

        fclose(*fichier);
        fclose(temp);

        if (!erreur) {
            remove(FICHIER_MAQUETTE);
            rename(tempName, FICHIER_MAQUETTE);
            printf("EC ajoute avec succes.\n");
        } else {
            remove(tempName);
        }

        liberer_EC(&ue);  // libère la mémoire

        // --- Rouvrir le fichier principal ---
        *fichier = fopen(FICHIER_MAQUETTE, "r+");
        if (!*fichier) exit(1);

        continuer = saisir_choix_binaire("Voulez-vous ajouter un autre EC");
    } while (continuer == 1);
}

/*
 * But: Supprime un ou plusieurs EC d'une UE existante.
 *      L'utilisateur saisit le code de l'EC à supprimer, avec confirmation.
 *      Après chaque suppression, on demande si l'utilisateur souhaite en supprimer un autre.
 * Paramètres: fichier : pointeur vers le pointeur du fichier principal
 *             codeUE  : code de l'UE cible
 * Retour: aucun
 */
void supprimer_EC(FILE **fichier, const char *codeUE) {
    if (!*fichier) return;

    unsigned int continuer;
    do {
        // --- Lire l'UE cible ---
        Unite_Enseignement ue;
        long pos = rechercher_UE(*fichier, codeUE);
        if (pos == -1) {
            printf("UE %s non trouvee.\n", codeUE);
            return;
        }
        fseek(*fichier, pos, SEEK_SET);
        if (!lire_UE(*fichier, &ue)) {
            printf("Erreur de lecture de l'UE.\n");
            return;
        }

        if (ue.Nb_EC == 0) {
            printf("Cette UE ne contient aucun EC.\n");
            liberer_EC(&ue);
            return;
        }

        // --- Demander le code de l'EC à supprimer ---
        char codeEC[MAX_CODE];
        saisir_chaine("Code EC a supprimer : ", codeEC, MAX_CODE);

        int index = -1;
        for (unsigned int i = 0; i < ue.Nb_EC; i++) {
            if (strcmp(ue.ECs[i].Code_EC, codeEC) == 0) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            printf("EC %s non trouve dans l'UE %s.\n", codeEC, codeUE);
            liberer_EC(&ue);
            return;
        }

        // Demander confirmation
        unsigned int confirmer = saisir_choix_binaire("Voulez-vous vraiment supprimer cet EC");
        if (confirmer == 0) {
            printf("Suppression annulee.\n");
            liberer_EC(&ue);
            return;
        }

        // --- Supprimer l'EC (décalage) ---
        for (unsigned int i = index; i < ue.Nb_EC - 1; i++) {
            ue.ECs[i] = ue.ECs[i + 1];
        }
        ue.Nb_EC--;

        if (ue.Nb_EC == 0) {
            free(ue.ECs);
            ue.ECs = NULL;
        } else {
            ue.ECs = (Element_Constitutif*)realloc(ue.ECs, ue.Nb_EC * sizeof(Element_Constitutif));
            if (!ue.ECs) {
                fprintf(stderr, "Erreur de reallocation memoire.\n");
                exit(EXIT_FAILURE);
            }
        }

        // --- Écrire la maquette mise à jour via fichier temporaire ---
        const char *tempName = "temp_modif.txt";
        FILE *temp = fopen(tempName, "w");
        if (!temp) {
            fprintf(stderr, "Erreur creation fichier temporaire\n");
            liberer_EC(&ue);
            return;
        }

        rewind(*fichier);
        clearerr(*fichier);
        Unite_Enseignement ueCourante;
        bool erreur = false;

        while (lire_UE(*fichier, &ueCourante)) {
            if (strcmp(ueCourante.Code_UE, codeUE) == 0) {
                ecrire_UE(temp, &ue);
            } else {
                ecrire_UE(temp, &ueCourante);
            }
            if (ferror(temp)) {
                fprintf(stderr, "Erreur d'ecriture dans le fichier temporaire\n");
                erreur = true;
                liberer_EC(&ueCourante);
                break;
            }
            liberer_EC(&ueCourante);
        }

        fclose(*fichier);
        fclose(temp);

        if (!erreur) {
            remove(FICHIER_MAQUETTE);
            rename(tempName, FICHIER_MAQUETTE);
            printf("EC supprime avec succes.\n");
        } else {
            remove(tempName);
        }

        liberer_EC(&ue);  // libère la mémoire

        // --- Rouvrir le fichier principal ---
        *fichier = fopen(FICHIER_MAQUETTE, "r+");
        if (!*fichier) exit(1);

        continuer = saisir_choix_binaire("Voulez-vous supprimer un autre EC");
    } while (continuer == 1);
}

/*
 * But: Affiche le menu de modification d'un EC.
 * Paramètres: aucun
 * Retour: choix de l'utilisateur (entier entre 0 et 4)
 */
unsigned int menu_modification_EC(void) {
    unsigned int choix;
    printf("\n+-----------------------------------------+\n");
    printf("|       MODIFICATION D'UN EC (Element)    |\n");
    printf("+-----------------------------------------+\n\n");
    printf(" [1]  Modifier le code EC\n");
    printf(" [2]  Modifier le libelle EC\n");
    printf(" [3]  Modifier le coefficient EC\n");
    printf(" [4]  Modifier le volume horaire EC\n");
    printf(" [0]  Retour au menu principal\n");
    printf("+-----------------------------------------+\n");

    printf("Votre choix -> ");
    choix = saisir_choix(4);
    return choix;
}

/*
 * But: Modifie un EC spécifique (code, libellé, coeff, volume).
 * Paramètres: fichier : pointeur vers le pointeur du fichier principal, codeUE : code de l'UE parente, codeEC : code de l'EC à modifier
 * Retour: aucun
 */
void modifier_EC(FILE **fichier, const char *codeUE, const char *codeEC) {
    if (!*fichier) return;
    unsigned int continuer = saisir_choix_binaire("Voulez-vous vraiment modifier cet EC ?");
    if (continuer == 0) {
        printf("Modification annulee.\n");
        return;
    }
    const char *tempName = "temp_modif.txt";
    FILE *temp = fopen(tempName, "w");
    if (!temp) {
        fprintf(stderr, "Erreur creation fichier temporaire\n");
        return;
    }
    Unite_Enseignement ue;
    rewind(*fichier);
    clearerr(*fichier);
    bool trouveUE = false, trouveEC = false;
    bool erreur = false;
    while (lire_UE(*fichier, &ue)) {
        if (strcmp(ue.Code_UE, codeUE) == 0) {
            trouveUE = true;
            for (unsigned int i = 0; i < ue.Nb_EC; i++) {
                if (strcmp(ue.ECs[i].Code_EC, codeEC) == 0) {
                    trouveEC = true;
                    unsigned int choix = menu_modification_EC();
                    int modifie = 0;
                    switch (choix) {
                        case 0:
                            printf("Retour.\n"); // ← corrigé
                            break;
                        case 1: {
                            char nouveauCode[MAX_CODE];
                            int ok;
                            do {
                                ok = 1;
                                saisir_chaine("Nouveau code EC : ", nouveauCode, MAX_CODE);
                                for (unsigned int j = 0; j < ue.Nb_EC; j++) {
                                    if (j != i && strcmp(ue.ECs[j].Code_EC, nouveauCode) == 0) {
                                        printf("Erreur : ce code EC existe deja dans cette UE.\n");
                                        ok = 0;
                                        break;
                                    }
                                }
                                if (ok && !verifier_code_EC_unique_fichier(*fichier, nouveauCode, ue.ECs[i].Code_EC)) {
                                    printf("Erreur : ce code EC existe deja dans une autre UE.\n");
                                    ok = 0;
                                }
                            } while (!ok);
                            strcpy(ue.ECs[i].Code_EC, nouveauCode);
                            modifie = 1;
                            break;
                        }
                        case 2:
                            saisir_chaine("Nouveau libelle EC : ", ue.ECs[i].Libelle_EC, MAX_LIBELLE);
                            modifie = 1;
                            break;
                        case 3:
                            ue.ECs[i].Coefficient_EC = saisir_entier("Nouveau coefficient EC : ");
                            modifie = 1;
                            break;
                        case 4:
                            ue.ECs[i].Volume_Horaire_EC = saisir_flottant("Nouveau volume horaire EC : ");
                            modifie = 1;
                            break;
                        default:
                            printf("Choix invalide.\n");
                    }
                    if (modifie) printf("EC modifie.\n");
                }
            }
        }
        ecrire_UE(temp, &ue);
        if (ferror(temp)) {
            fprintf(stderr, "Erreur d'ecriture dans le fichier temporaire\n");
            erreur = true;
            liberer_EC(&ue);
            break;
        }
        liberer_EC(&ue);
    }
    if (!trouveUE) printf("UE %s non trouvee.\n", codeUE);
    else if (!trouveEC) printf("EC %s non trouve.\n", codeEC);
    fclose(*fichier);
    fclose(temp);
    if (!erreur) {
        remove(FICHIER_MAQUETTE);
        rename(tempName, FICHIER_MAQUETTE);
    } else {
        remove(tempName);
    }
    *fichier = fopen(FICHIER_MAQUETTE, "r+");
    if (!*fichier) exit(1);
}

// ============================================================
// MENUS
// ============================================================

/*
 * But: Affiche le menu principal et retourne le choix.
 * Paramètres: aucun
 * Retour: choix de l'utilisateur (entier entre 0 et 7)
 */
unsigned int menu_principal(void) {
    printf("\n+--------------------------------------------------+\n");
    printf("|             MENU PRINCIPAL - MAQUETTE            |\n");
    printf("+--------------------------------------------------+\n");
    printf("|  1. Enregistrement                               |\n");
    printf("|  2. Affichage                                    |\n");
    printf("|  3. Recherche                                    |\n");
    printf("|  4. Modification                                 |\n");
    printf("|  5. Suppression                                  |\n");
    printf("|  6. Statistiques                                 |\n");
    printf("|  7. Listes                                       |\n");
    printf("|  0. Quitter                                      |\n");
    printf("+--------------------------------------------------+\n");
    printf("Votre choix : ");
    return saisir_choix(7);
}

/*
 * But: Affiche le sous-menu statistiques et retourne le choix.
 * Paramètres: aucun
 * Retour: choix de l'utilisateur (entier entre 0 et 10)
 */
unsigned int sous_menu_statistiques(void) {
    printf("\n+------------------------------------------------------+\n");
    printf("|            MENU STATISTIQUES DES UE                  |\n");
    printf("+------------------------------------------------------+\n");
    printf("|  1. Nombre d UE avec credits identiques              |\n");
    printf("|  2. Nombre d UE avec total horaire > 20 x credits    |\n");
    printf("|  3. Nombre d EC avec coefficient et credits UE precis|\n");
    printf("|  4. Nombre d UE dans intervalle de credits           |\n");
    printf("|  5. Nombre d EC ayant meme nom que leur UE           |\n");
    printf("|  6. Nombre d EC dans intervalle de volume            |\n");
    printf("|  7. Nombre d UE avec nombre d EC precis              |\n");
    printf("|  8. Nombre d UE avec credits maximum                 |\n");
    printf("|  9. Nombre d UE avec credits = nombre d EC           |\n");
    printf("| 10. Nombre d UE avec maximum d EC                    |\n");
    printf("|  0. Retour                                           |\n");
    printf("+------------------------------------------------------+\n");
    printf("Votre choix : ");
    return saisir_choix(10);
}

/*
 * But: Affiche le sous-menu listes et retourne le choix.
 * Paramètres: aucun
 * Retour: choix de l'utilisateur (entier entre 0 et 10)
 */
unsigned int sous_menu_listes(void) {
    printf("\n+-------------------------------------------------------+\n");
    printf("|                MENU LISTES DES UE/EC                  |\n");
    printf("+-------------------------------------------------------+\n");
    printf("|  1. Liste des UE avec credits identiques              |\n");
    printf("|  2. Liste des UE avec total horaire > 20 x credits    |\n");
    printf("|  3. Liste des EC avec coefficient et credits UE precis|\n");
    printf("|  4. Liste des UE dans intervalle de credits           |\n");
    printf("|  5. Liste des EC ayant meme nom que leur UE           |\n");
    printf("|  6. Liste des EC dans intervalle de volume            |\n");
    printf("|  7. Liste des UE avec nombre d EC precis              |\n");
    printf("|  8. Liste des UE avec credits maximum                 |\n");
    printf("|  9. Liste des UE avec credits = nombre d EC           |\n");
    printf("| 10. Liste des UE avec maximum d EC                    |\n");
    printf("|  0. Retour                                            |\n");
    printf("+-------------------------------------------------------+\n");
    printf("Votre choix : ");
    return saisir_choix(10);
}

/*
 * But: Affiche le sous-menu modification et retourne le choix.
 * Paramètres: aucun
 * Retour: choix de l'utilisateur (entier entre 0 et 5)
 */
unsigned int sous_menu_modification(void) {
    printf("\n+--------------------------------------------------+\n");
    printf("|            MENU MODIFICATION DES UE              |\n");
    printf("+--------------------------------------------------+\n");
    printf("|  1. Modifier code UE                             |\n");
    printf("|  2. Modifier libelle UE                          |\n");
    printf("|  3. Modifier credits UE                          |\n");
    printf("|  4. Modifier coefficient UE                      |\n");
    printf("|  5. Modifier un EC                               |\n");
    printf("|  6. Ajouter un EC                                |\n");
    printf("|  7. Supprimer un EC                              |\n");
    printf("|  0. Retour                                       |\n");
    printf("+--------------------------------------------------+\n");
    printf("Votre choix : ");
    return saisir_choix(7);   // max passe à 7
}

/*
 * But: Message de fin.
 * Paramètres: aucun
 * Retour: aucun
 */
void Quitter(void) {
    printf("\n+--------------------------------------------------+\n");
    printf("|        Merci d'avoir utilise le systeme          |\n");
    printf("+--------------------------------------------------+\n\n");
}