#ifndef OUVRAGE_H
#define OUVRAGE_H

#include <stdio.h>
#include "types.h"

#ifdef _WIN32
    #define EFFACER "cls"
    #define PAUSE "pause"
#endif

// Utilitaires
void Permutation(unsigned int*, unsigned int*);
bool Deja_Compter(Personne, Personne);
void charger_auteurs(FILE*, Ouvrage*);
void liberer_auteurs(Ouvrage*);
void ecrire_ouvrage(FILE*, Ouvrage* );
bool lire_ouvrage(FILE* , Ouvrage* );
FILE* OuvertureFichier(const char*);
void vider_buffer(void);
void attendre_entree(void);
unsigned int annee_courante(void);

// Date
unsigned int est_bissextile(unsigned int);
unsigned int date_valide(Date);
unsigned int Lec_Mois(void);
unsigned int Lec_JourMois(unsigned int, unsigned int);
void lecture_date(Date *d);
void affichage_date(Date);
bool Date_Egale(Date, Date);

// Menus
unsigned int menu_principal(void);
unsigned int sous_menu_statistiques(void);
unsigned int sous_menu_listes(void);
unsigned int sous_menu_modification(void);
void Quitter(void);

// Auteurs
void Lecture_D_Un_Auteur(Ouvrage*, Personne*);
void affichage_d_un_auteur(Ouvrage*, unsigned int );
void Lecture_Auteurs(Ouvrage*, unsigned int);
void affichage_Auteurs(Ouvrage);
void Ajout_Auteur(Ouvrage*);
int recherche_auteur(Ouvrage*, const char* );
bool auteur_existe(FILE* , const char* );
bool editeur_existe(FILE* , const char* );

// Ouvrages
void afficher_Ouvrage_par_pos(FILE*,long );
void affichage_ouvrage(Ouvrage*);
void Lecture_Ouvrage(Ouvrage*);
void afficher_bibliotheque(FILE*);
void ajouter_Ouvrage(FILE*);
long rechercher_Ouvrage(FILE*, unsigned int);
void supprimer_Ouvrage(FILE**, unsigned int);
void Enregistrement(FILE*);
unsigned int compter_ouvrages(FILE*);
bool verifier_numero_unique(FILE*, unsigned int );

// Statistiques
unsigned int Nb_Ouvrages_au_moins_n_auteurs(FILE*, unsigned int);
unsigned int Nb_Ouvrages_par_editeur(FILE*, Personne);
unsigned int Nb_Ouvrages_par_auteur(FILE*, Personne);
unsigned int Nb_Ouvrages_Editeur_Date(FILE*, Personne, Date);
unsigned int Nb_Ouvrages_quantite_egale_auteurs(FILE*);
unsigned int Nb_Ouvrages_Editeur_Entre_Annees(FILE*, Personne, unsigned int, unsigned int);
unsigned int Nb_auteurs_intervalle_annees(FILE*, unsigned int, unsigned int);
unsigned int Nb_Ouvrages_Date_Quantite(FILE*, Date, unsigned int);
unsigned int Nb_Ouvrages_meme_titre(FILE*);
unsigned int Nb_Ouvrages_auteur_editeur(FILE*, Personne, Personne);

// Modifications
void modifier_titre(FILE**, unsigned int);
void modifier_editeur(FILE**, unsigned int);
void modifier_annee(FILE**, unsigned int);
void modifier_quantite(FILE**, unsigned int);
void modifier_date_edition(FILE**, unsigned int);
void modifier_auteur_par_id(FILE**, unsigned int, const char* );

// Listes
void Liste_Ouvrages_au_moins_n_auteurs(FILE*, unsigned int);
void Liste_Ouvrages_par_editeur(FILE*, Personne);
void Liste_Ouvrages_par_auteur(FILE*, Personne);
void Liste_Ouvrages_Editeur_Date(FILE*, Personne, Date);
void Liste_Ouvrages_quantite_egale_auteurs(FILE*);
void Liste_Ouvrages_Editeur_Entre_Annees(FILE*, Personne, unsigned int, unsigned int);
void Liste_auteurs_intervalle_annees(FILE*, unsigned int, unsigned int);
void Liste_Ouvrages_Date_Quantite(FILE*, Date, unsigned int);
void Liste_Ouvrages_meme_titre(FILE*);
void Liste_Ouvrages_auteur_editeur(FILE*, Personne, Personne);

#endif