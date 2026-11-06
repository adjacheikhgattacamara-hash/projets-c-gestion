#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Maquette.h"


/* ============================================================
 * Projet : Système de Gestion de Maquette Pédagogique
 * Auteur : Adja Cheikh Gatta Camara
 * Date   : 19 juin 2026
 * Fichiers : Maquette.c, Maquette.h, types.h, main.c
 *
 * Objectif :
 *   Ce projet permet de gérer une maquette pédagogique avec les
 *   fonctionnalités suivantes :
 *     - Enregistrement des UE (libellé, coefficient, code, etc.)
 *     - Affichage complet du catalogue
 *     - Recherche par code
 *     - Suppression et modification des UE
 *     - Statistiques (nombre de UE, par maximum d'auteurs, par nombre d'EC précis …)
 *     - Listes selon critères (par même crédits, par intervalle crédits, par maximum d'EC…)
 *
 * Particularités :
 *   - Vérification d’unicité des codes UE
 *   - Vérification d’unicité des codes EC
 *   - Gestion mémoire dynamique pour les structures
 *   - Menus interactifs pour l’utilisateur
 *   - Respect des contraintes académiques (robustesse, modularité)
 *
 * ============================================================ */
int main(void) {
    FILE* fichier = OuvertureFichier(FICHIER_MAQUETTE);
    if (!fichier) {
        fprintf(stderr, "Erreur d'ouverture du fichier.\n");
        return 1;
    }
    unsigned int choix;

    do {
        choix = menu_principal();

        switch (choix) {
            case 1:
                Enregistrement(fichier);
                rewind(fichier);
                printf("Nombre total d'UE : %u\n", compter_UE(fichier));
                attendre_entree();
                break;

            case 2:
                afficher_maquette(fichier);
                attendre_entree();
                break;

            case 3: {
                char code[MAX_CODE];
                saisir_chaine("Code UE a rechercher : ", code, MAX_CODE);
                long pos = rechercher_UE(fichier, code);
                if (pos != -1) {
                    afficher_UE_par_pos(fichier, pos);
                } else {
                    printf("L UE avec le code %s n existe pas dans la maquette actuelle \n", code);
                }
                attendre_entree();
                break;
            }

            case 4: {
                unsigned int choixModif;
                do {
                    choixModif = sous_menu_modification();
                    if (choixModif == 0) break;
                    char code[MAX_CODE];
                    saisir_chaine("Code UE a modifier : ", code, MAX_CODE);
                    switch (choixModif) {
                        case 1: modifier_code_UE(&fichier, code); break;
                        case 2: modifier_libelle_UE(&fichier, code); break;
                        case 3: modifier_credit_UE(&fichier, code); break;
                        case 4: modifier_coefficient_UE(&fichier, code); break;
                        case 5: {
                            char codeEC[MAX_CODE];
                            saisir_chaine("Entrez le code de l EC a modifier : ", codeEC, MAX_CODE);
                            modifier_EC(&fichier, code, codeEC);
                            break;
                        }
                        case 6: ajouter_EC(&fichier, code); break;
                        case 7: supprimer_EC(&fichier, code); break;
                        default: printf("Choix invalide.\n");
                    }
                    attendre_entree();
                } while (choixModif != 0);
                break;
            }

            case 5: {
                char code[MAX_CODE];
                saisir_chaine("Code UE a supprimer : ", code, MAX_CODE);
                supprimer_UE(&fichier, code);
                printf("Total restant : %u UE\n", compter_UE(fichier));
                attendre_entree();
                break;
            }

            case 6: {
                unsigned int choixStats;
                do {
                    choixStats = sous_menu_statistiques();
                    switch (choixStats) {
                        case 1:
                            printf("Resultat : %u UE\n", nb_UE_meme_credit(fichier));
                            break;
                        case 2:
                            printf("Resultat : %u UE\n", nb_UE_totHoraire_sup20Credit(fichier));
                            break;
                        case 3: {
                            unsigned int coeff = saisir_entier("Coefficient recherche : ");
                            unsigned int credit = saisir_entier("Credit UE recherche : ");
                            printf("Resultat : %u EC\n", nb_EC_coeff_credit_precis(fichier, coeff, credit));
                            break;
                        }
                        case 4: {
                            unsigned int min = saisir_entier("Credit minimum : ");
                            unsigned int max = saisir_entier("Credit maximum : ");
                            printf("Resultat : %u UE\n", nb_UE_intervalle_credits(fichier, min, max));
                            break;
                        }
                        case 5:
                            printf("Resultat : %u EC\n", nb_EC_meme_nom_UE(fichier));
                            break;
                        case 6: {
                            float min = saisir_flottant("Volume horaire minimum : ");
                            float max = saisir_flottant("Volume horaire maximum : ");
                            printf("Resultat : %u EC\n", nb_EC_intervalle_volume(fichier, min, max));
                            break;
                        }
                        case 7: {
                            unsigned int nbEC = saisir_entier("Nombre d EC precis : ");
                            printf("Resultat : %u UE\n", nb_UE_nbEC_precis(fichier, nbEC));
                            break;
                        }
                        case 8:
                            printf("Resultat : %u UE\n", nb_UE_max_credits(fichier));
                            break;
                        case 9:
                            printf("Resultat : %u UE\n", nb_UE_credit_egal_nbEC(fichier));
                            break;
                        case 10:
                            printf("Resultat : %u UE\n", nb_UE_max_EC(fichier));
                            break;
                        case 0: break;
                        default: printf("Choix invalide.\n");
                    }
                    attendre_entree();
                } while (choixStats != 0);
                break;
            }

            case 7: {
                unsigned int choixListes;
                do {
                    choixListes = sous_menu_listes();
                    switch (choixListes) {
                        case 1:
                            liste_UE_meme_credit(fichier);
                            break;
                        case 2:
                            liste_UE_totHoraire_sup20Credit(fichier);
                            break;
                        case 3: {
                            unsigned int coeff = saisir_entier("Coefficient recherche : ");
                            unsigned int credit = saisir_entier("Credit UE recherche : ");
                            liste_EC_coeff_credit_precis(fichier, coeff, credit);
                            break;
                        }
                        case 4: {
                            unsigned int min = saisir_entier("Credit minimum : ");
                            unsigned int max = saisir_entier("Credit maximum : ");
                            liste_UE_intervalle_credits(fichier, min, max);
                            break;
                        }
                        case 5:
                            liste_EC_meme_nom_UE(fichier);
                            break;
                        case 6: {
                            float min = saisir_flottant("Volume horaire minimum : ");
                            float max = saisir_flottant("Volume horaire maximum : ");
                            liste_EC_intervalle_volume(fichier, min, max);
                            break;
                        }
                        case 7: {
                            unsigned int nbEC = saisir_entier("Nombre d EC precis : ");
                            liste_UE_nbEC_precis(fichier, nbEC);
                            break;
                        }
                        case 8:
                            liste_UE_max_credits(fichier);
                            break;
                        case 9:
                            liste_UE_credit_egal_nbEC(fichier);
                            break;
                        case 10:
                            liste_UE_max_EC(fichier);
                            break;
                        case 0: break;
                        default: printf("Choix invalide.\n");
                    }
                    attendre_entree();
                } while (choixListes != 0);
                break;
            }

            case 0:
                fclose(fichier);
                Quitter();
                break;

            default:
                printf("Choix invalide.\n");
                attendre_entree();
        }
    } while (choix != 0);

    return 0;
}