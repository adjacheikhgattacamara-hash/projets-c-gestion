/*
* ============================================================
 *  types.h
 *  Description : Definitions des structures de donnees
 *    - Personne : represente un auteur ou un editeur
 *    - Date     : represente une date d'edition (jour/mois/annee)
 *    - Ouvrage  : represente un livre dans la bibliotheque
 *  Les auteurs sont stockes via allocation dynamique (pointeur)
 * ============================================================
 */



#ifndef TYPES_H
#define TYPES_H

#define MAX_NOM 50
#define MAX_PRENOM 50
#define MAX_TITRE 100
#define MAX_ID 20
#define FICHIER_BIBLIO "bibliotheque.dat"
#define ANNEE_MIN 1500

/*
 * Structure Personne
 * Utilisee pour representer un Auteur ou un Editeur.
 * Champs :
 *   - nom         : nom de famille de la personne
 *   - prenom      : prenom de la personne
 *   - identifiant : identifiant unique de la personne
 */
typedef struct {
    char nom[MAX_NOM];
    char prenom[MAX_PRENOM];
    char identifiant[MAX_ID];
} Personne;

typedef enum {
    JANVIER = 1, FEVRIER, MARS, AVRIL, MAI, JUIN,
    JUILLET, AOUT, SEPTEMBRE, OCTOBRE, NOVEMBRE, DECEMBRE
} Mois;

/*
 * Structure Date
 * Represente la date d'edition d'un ouvrage.
 */
typedef struct {
    unsigned int jour;
    Mois mois;
    unsigned int annee;
} Date;

/*
 * Structure Ouvrage
 * Represente un livre dans la bibliotheque.
 */
typedef struct {
    unsigned int numero;
    char titre[MAX_TITRE];
    Personne editeur;
    Date dateEdition;
    unsigned int quantite;
    unsigned int nbAuteurs;
    Personne **auteurs; /* Allocation dynamique pour les auteurs */
} Ouvrage;

#endif /* TYPES_H */
