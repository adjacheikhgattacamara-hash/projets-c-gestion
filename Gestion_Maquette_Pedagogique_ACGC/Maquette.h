#ifndef GESTION_MAQUETTE_PEDAGOGIQUE_ACGC_MAQUETTE_H
#define GESTION_MAQUETTE_PEDAGOGIQUE_ACGC_MAQUETTE_H

#include <stdio.h>
#include "types.h"
#include <stdbool.h>
// Nom du fichier principal de la maquette
#define FICHIER_MAQUETTE "maquette.txt"

// Utilitaires

FILE* OuvertureFichier(const char *);
void vider_buffer(void);
void attendre_entree(void);
void permutation_entier(unsigned int *, unsigned int *);
void permutation_reel(float *, float *);

// Vérifications avec exclusion possible
bool verifier_code_UE_unique(FILE *, const char*, const char*);
bool verifier_code_EC_unique(Unite_Enseignement *, const char *, unsigned int);
bool verifier_code_EC_unique_fichier(FILE *, const char *, const char*);
bool ue_est_dans_fichier(FILE *, Unite_Enseignement *);
unsigned int compter_UE_meme_credit_position(FILE *, long, FILE *);

// Saisies sécurisées
unsigned int saisir_entier(const char *);
float saisir_flottant(const char *);
void saisir_chaine(const char *, char *, size_t);
unsigned int saisir_choix_binaire(const char *);
unsigned int saisir_choix(unsigned int);

// Gestion mémoire EC
void liberer_EC(Unite_Enseignement *);
bool charger_EC(FILE *, Unite_Enseignement *);

// Gestion des ECs
void lecture_EC(Element_Constitutif *, Unite_Enseignement *, FILE *, unsigned int);
void affichage_EC(Element_Constitutif *);
void lecture_tableau_EC(Unite_Enseignement *, FILE *);

// Fichier texte
void ecrire_UE(FILE *, Unite_Enseignement *);
bool lire_UE(FILE *, Unite_Enseignement *);

// Gestion de la maquette
void ajouter_UE(FILE *);
void afficher_maquette(FILE *);
void lecture_UE(Unite_Enseignement *, FILE *);
void affichage_UE(Unite_Enseignement *);
void afficher_UE_par_pos(FILE *, long);
void supprimer_UE(FILE **, const char *);
long rechercher_UE(FILE *, const char *);
long rechercher_UE_max_credit(FILE *);
long rechercher_UE_max_EC(FILE *);
void Enregistrement(FILE *);

// Calculs
float volume_horaire_total(Unite_Enseignement *);
bool est_dans_intervalle_volume_horaire(float, float, float);
bool est_dans_intervalle_credits(unsigned int, unsigned int, unsigned int);
unsigned int compter_UE(FILE *);

// Statistiques
unsigned int nb_UE_meme_credit(FILE *);
unsigned int nb_UE_totHoraire_sup20Credit(FILE *);
unsigned int nb_EC_coeff_credit_precis(FILE *, unsigned int, unsigned int);
unsigned int nb_UE_intervalle_credits(FILE *, unsigned int, unsigned int);
unsigned int nb_EC_meme_nom_UE(FILE *);
unsigned int nb_EC_intervalle_volume(FILE *, float, float);
unsigned int nb_UE_nbEC_precis(FILE *, unsigned int);
unsigned int nb_UE_max_credits(FILE *);
unsigned int nb_UE_credit_egal_nbEC(FILE *);
unsigned int nb_UE_max_EC(FILE *);

// Listes
void liste_UE_meme_credit(FILE *);
void liste_UE_totHoraire_sup20Credit(FILE *);
void liste_EC_coeff_credit_precis(FILE *, unsigned int, unsigned int);
void liste_UE_intervalle_credits(FILE *, unsigned int, unsigned int);
void liste_EC_meme_nom_UE(FILE *);
void liste_EC_intervalle_volume(FILE *, float, float);
void liste_UE_nbEC_precis(FILE *, unsigned int);
void liste_UE_max_credits(FILE *);
void liste_UE_credit_egal_nbEC(FILE *);
void liste_UE_max_EC(FILE *);

// Modifications
void modifier_code_UE(FILE **, const char *);
void modifier_libelle_UE(FILE **, const char *);
void modifier_credit_UE(FILE **, const char *);
void modifier_coefficient_UE(FILE **, const char *);
unsigned int menu_modification_EC(void);
void ajouter_EC(FILE **fichier, const char *codeUE);
void supprimer_EC(FILE **fichier, const char *codeUE);
void modifier_EC(FILE **, const char *, const char *);

// Menus
unsigned int menu_principal(void);
unsigned int sous_menu_statistiques(void);
unsigned int sous_menu_listes(void);
unsigned int sous_menu_modification(void);
void Quitter(void);

#endif