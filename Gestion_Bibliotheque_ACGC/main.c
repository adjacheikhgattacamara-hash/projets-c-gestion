#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Ouvrage.h"
#include<stdbool.h>

/* ============================================================
 * Projet : Système de Gestion de Bibliothèque
 * Auteur : Adja Cheikh Gatta Camara
 * Date   :21 Avril 2026
 * Fichiers : Ouvrage.c, Ouvrage.h, types.h, main.c
 *
 * Objectif :
 *   Ce projet permet de gérer une bibliothèque avec les
 *   fonctionnalités suivantes :
 *     - Enregistrement des livres (titre, auteur, code, etc.)
 *     - Affichage complet du catalogue
 *     - Recherche par code
 *     - Suppression et modification des livres
 *     - Statistiques (nombre de livres, par auteur, par date d'édition …)
 *     - Listes selon critères (par auteur, par éditeur, par date d'édition …)
 *
 * Particularités :
 *   - Vérification d’unicité des codes livres
 *   - Gestion mémoire dynamique pour les structures
 *   - Menus interactifs pour l’utilisateur
 *   - Respect des contraintes académiques (robustesse, modularité)
 *
 * ============================================================ */


int main(void) {
    FILE* fichier = OuvertureFichier(FICHIER_BIBLIO);
    if (!fichier) {
        fprintf(stderr, "Erreur d'ouverture du fichier.\n");
        return 1;
    }
    unsigned int choix;

    do {
        choix = menu_principal();

        switch (choix) {
            case 1: // Enregistrement
                Enregistrement(fichier);
                printf("Nombre total d'ouvrages mis a jour : %u\n", compter_ouvrages(fichier));
                attendre_entree();
                break;

            case 2: // Affichage
                afficher_bibliotheque(fichier);
                attendre_entree();
                break;

            case 3: { // Recherche
                unsigned int numero;
                do {
                    printf("Numero d'ouvrage a rechercher : ");
                    if (scanf("%u", &numero) != 1) {
                        printf("Entree invalide. Veuillez entrer un nombre.\n");
                        while (getchar() != '\n');
                        continue;
                    }
                    vider_buffer();
                    break;
                } while (1);

                long pos = rechercher_Ouvrage(fichier, numero);
                if (pos != -1) {
                    afficher_Ouvrage_par_pos(fichier, pos);
                } else {
                    printf("Ouvrage #%u non trouve.\n", numero);
                }
                break;
            }

            case 4: { // Modification
                unsigned int choixModif;
                do {
                    choixModif = sous_menu_modification();

                    if (choixModif == 0) break;

                    unsigned int numero;
                    printf("Numero de l'ouvrage a modifier : ");
                    scanf("%u", &numero);
                    vider_buffer();

                    switch (choixModif) {
                        case 1: modifier_titre(&fichier, numero); break;
                        case 2: modifier_editeur(&fichier, numero); break;
                        case 3: modifier_annee(&fichier, numero); break;
                        case 4: modifier_quantite(&fichier, numero); break;
                        case 5: modifier_date_edition(&fichier, numero); break;
                        case 6: {
                            char id_auteur[MAX_ID];
                            printf("Identifiant de l'auteur a modifier : ");
                            fgets(id_auteur, sizeof(id_auteur), stdin);
                            id_auteur[strcspn(id_auteur, "\n")] = '\0';
                            if (!auteur_existe(fichier, id_auteur)) {
                                printf("Auteur [%s] inexistant dans la bibliothèque.\n",id_auteur);
                                break;
                            }
                            modifier_auteur_par_id(&fichier, numero, id_auteur);
                            break;
                        }
                        default: printf(" Choix invalide.\n");
                    }
                    attendre_entree();
                } while (choixModif != 0);
                break;
            }


            case 5: { // Suppression

                unsigned int numero;
                do {
                    printf("Numero d'ouvrage a supprimer : ");
                    if (scanf("%u", &numero) != 1) {
                        printf("Entree invalide. Veuillez entrer un nombre.\n");
                        while (getchar() != '\n');
                        continue;
                    }
                    vider_buffer();
                    break;
                } while (1);

                supprimer_Ouvrage(&fichier, numero);
                break;
            }

            case 6: { // Statistiques

                unsigned int choixStats;
                do{
                 choixStats = sous_menu_statistiques();
                switch (choixStats) {
                    case 1: {
                        unsigned int n;
                        printf("Entrez n : ");
                        scanf("%u", &n);
                        vider_buffer();
                        printf("Resultat : %u ouvrages\n", Nb_Ouvrages_au_moins_n_auteurs(fichier, n));
                        break;
                    }
                    case 2: {
                        Personne editeur;
                        printf("Identifiant editeur : ");
                        fgets(editeur.identifiant, MAX_ID, stdin);
                        editeur.identifiant[strcspn(editeur.identifiant, "\n")] = '\0';
                        if (!editeur_existe(fichier, editeur.identifiant)) {
                            printf("Editeur [%s] inexistant dans la bibliothèque.\n", editeur.identifiant);
                            break;
                        }
                        printf("Resultat : %u ouvrages\n", Nb_Ouvrages_par_editeur(fichier, editeur));
                        break;
                    }
                    case 3: {
                        Personne auteur;
                        printf("Identifiant auteur : ");
                        fgets(auteur.identifiant, MAX_ID, stdin);
                        auteur.identifiant[strcspn(auteur.identifiant, "\n")] = '\0';
                        if (!auteur_existe(fichier, auteur.identifiant)) {
                            printf("Auteur [%s] inexistant dans la bibliothèque.\n", auteur.identifiant);
                            break;
                        }
                        printf("Resultat : %u ouvrages\n", Nb_Ouvrages_par_auteur(fichier, auteur));
                        break;
                    }
                    case 4: {
                        Personne editeur;
                        Date d;
                        printf("Identifiant editeur : ");
                        fgets(editeur.identifiant, MAX_ID, stdin);
                        editeur.identifiant[strcspn(editeur.identifiant, "\n")] = '\0';
                        if (!editeur_existe(fichier, editeur.identifiant)) {
                            printf("Editeur [%s] inexistant dans la bibliothèque.\n", editeur.identifiant);
                            break;
                        }
                        lecture_date(&d);
                        printf("Resultat : %u ouvrages\n", Nb_Ouvrages_Editeur_Date(fichier, editeur, d));
                        break;
                    }
                    case 5:
                        printf("Resultat : %u ouvrages\n", Nb_Ouvrages_quantite_egale_auteurs(fichier));
                        break;
                    case 6: {
                        Personne editeur;
                        unsigned int debut, fin;
                        printf("Identifiant editeur : ");
                        fgets(editeur.identifiant, MAX_ID, stdin);
                        editeur.identifiant[strcspn(editeur.identifiant, "\n")] = '\0';
                        if (!editeur_existe(fichier, editeur.identifiant)) {
                            printf("Editeur [%s] inexistant dans la bibliothèque.\n", editeur.identifiant);
                            break;
                        }
                        printf("Annee debut : "); scanf("%u", &debut);
                        printf("Annee fin : "); scanf("%u", &fin);
                        vider_buffer();
                        printf("Resultat : %u ouvrages\n", Nb_Ouvrages_Editeur_Entre_Annees(fichier, editeur, debut, fin));
                        break;
                    }
                    case 7: {
                        unsigned int debut, fin;
                        printf("Annee debut : "); scanf("%u", &debut);
                        printf("Annee fin : "); scanf("%u", &fin);
                        vider_buffer();
                        printf("Resultat : %u ouvrages\n", Nb_auteurs_intervalle_annees(fichier, debut, fin));
                        break;
                    }
                    case 8: {
                        Date d; unsigned int q;
                        lecture_date(&d);
                        printf("Quantite : "); scanf("%u", &q);
                        vider_buffer();
                        printf("Resultat : %u ouvrages\n", Nb_Ouvrages_Date_Quantite(fichier, d, q));
                        break;
                    }
                    case 9:
                        printf("Resultat : %u ouvrages\n", Nb_Ouvrages_meme_titre(fichier));
                        break;
                    case 10: {

                        Personne auteur, editeur;

                        printf("Identifiant auteur : ");
                        fgets(auteur.identifiant, MAX_ID, stdin);
                        auteur.identifiant[strcspn(auteur.identifiant, "\n")] = '\0';
                        if (!auteur_existe(fichier, auteur.identifiant)) {
                            printf("Auteur [%s] inexistant dans la bibliothèque.\n", auteur.identifiant);
                            break;
                        }
                        printf("Identifiant editeur : ");
                        fgets(editeur.identifiant, MAX_ID, stdin);
                        editeur.identifiant[strcspn(editeur.identifiant, "\n")] = '\0';
                        if (!editeur_existe(fichier, editeur.identifiant)) {
                            printf("Editeur [%s] inexistant dans la bibliothèque.\n", editeur.identifiant);
                            break;
                        }

                        unsigned int nb = Nb_Ouvrages_auteur_editeur(fichier, auteur, editeur);
                        printf("Resultat : %u ouvrage(s) trouve(s).\n", nb);
                        break;
                    }

                    case 0:
                        break;
                    default:
                        printf("Choix invalide.\n");
                }
                attendre_entree();
            }while (choixStats!= 0);
                break;
            }

            case 7: { // Listes
                unsigned int choixListes;
                do {
                    choixListes = sous_menu_listes();
                    switch (choixListes) {
                        case 1: {
                            unsigned int n;
                            printf("Entrez n : ");
                            scanf("%u", &n);
                            vider_buffer();
                            Liste_Ouvrages_au_moins_n_auteurs(fichier, n);
                            break;
                        }
                        case 2: {
                            Personne editeur;
                            printf("Identifiant editeur : ");
                            fgets(editeur.identifiant, MAX_ID, stdin);
                            editeur.identifiant[strcspn(editeur.identifiant, "\n")] = '\0';
                            if (!editeur_existe(fichier, editeur.identifiant)) {
                                printf("Editeur [%s] inexistant dans la bibliothèque.\n", editeur.identifiant);
                                break;
                            }
                            Liste_Ouvrages_par_editeur(fichier, editeur);
                            break;
                        }
                        case 3: {
                            Personne auteur;
                            printf("Identifiant auteur : ");
                            fgets(auteur.identifiant, MAX_ID, stdin);
                            auteur.identifiant[strcspn(auteur.identifiant, "\n")] = '\0';
                            if (!auteur_existe(fichier, auteur.identifiant)) {
                                printf("Auteur [%s] inexistant dans la bibliothèque.\n", auteur.identifiant);
                                break;
                            }
                            Liste_Ouvrages_par_auteur(fichier, auteur);
                            break;
                        }
                        case 4: {
                            Personne editeur;
                            Date d;
                            printf("Identifiant editeur : ");
                            fgets(editeur.identifiant, MAX_ID, stdin);
                            editeur.identifiant[strcspn(editeur.identifiant, "\n")] = '\0';
                            if (!editeur_existe(fichier, editeur.identifiant)) {
                                printf("Editeur [%s] inexistant dans la bibliothèque.\n", editeur.identifiant);
                                break;
                            }
                            lecture_date(&d);
                            Liste_Ouvrages_Editeur_Date(fichier, editeur, d);
                            break;
                        }
                        case 5:
                            Liste_Ouvrages_quantite_egale_auteurs(fichier);
                            break;
                        case 6: {
                            Personne editeur;
                            unsigned int debut, fin;
                            printf("Identifiant editeur : ");
                            fgets(editeur.identifiant, MAX_ID, stdin);
                            editeur.identifiant[strcspn(editeur.identifiant, "\n")] = '\0';
                            if (!editeur_existe(fichier, editeur.identifiant)) {
                                printf("Editeur [%s] inexistant dans la bibliothèque.\n", editeur.identifiant);
                                break;
                            }
                            printf("Annee debut : "); scanf("%u", &debut);
                            printf("Annee fin : "); scanf("%u", &fin);
                            vider_buffer();
                            Liste_Ouvrages_Editeur_Entre_Annees(fichier, editeur, debut, fin);
                            break;
                        }
                        case 7: {
                            unsigned int debut, fin;
                            printf("Annee debut : "); scanf("%u", &debut);
                            printf("Annee fin : "); scanf("%u", &fin);
                            vider_buffer();
                            Liste_auteurs_intervalle_annees(fichier, debut, fin);
                            break;
                        }
                        case 8: {
                            Date d; unsigned int q;
                            lecture_date(&d);
                            printf("Quantite : "); scanf("%u", &q);
                            vider_buffer();
                            Liste_Ouvrages_Date_Quantite(fichier, d, q);
                            break;
                        }
                        case 9:
                            Liste_Ouvrages_meme_titre(fichier);
                            break;
                        case 10: {
                            Personne auteur, editeur;
                            printf("Identifiant auteur : ");
                            fgets(auteur.identifiant, MAX_ID, stdin);
                            auteur.identifiant[strcspn(auteur.identifiant, "\n")] = '\0';
                            if (!auteur_existe(fichier, auteur.identifiant)) {
                                printf("Auteur [%s] inexistant dans la bibliothèque.\n", auteur.identifiant);
                                break;
                            }
                            printf("Identifiant editeur : ");
                            fgets(editeur.identifiant, MAX_ID, stdin);
                            editeur.identifiant[strcspn(editeur.identifiant, "\n")] = '\0';
                            if (!editeur_existe(fichier, editeur.identifiant)) {
                                printf("Editeur [%s] inexistant dans la bibliothèque.\n", editeur.identifiant);
                                break;
                            }
                            Liste_Ouvrages_auteur_editeur(fichier, auteur, editeur);
                            break;
                        }
                        case 0:
                            break;
                        default:
                            printf("Choix invalide.\n");
                        }
                        attendre_entree();
                    }while (choixListes!= 0);
                    break;
                }

            case 0:
                Quitter();
                break;

            default:
                printf("Choix invalide.\n");
                attendre_entree();
        }
    } while (choix != 0);

    fclose(fichier);
    return 0;
}