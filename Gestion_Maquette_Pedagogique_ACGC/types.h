//
// Created by user on 13/04/2026.
//

#ifndef GESTION_MAQUETTE_PEDAGOGIQUE_ACGC_TYPES_H
#define GESTION_MAQUETTE_PEDAGOGIQUE_ACGC_TYPES_H

#define MAX_CODE 20
#define MAX_LIBELLE 100

typedef struct {
    char Code_EC[MAX_CODE];
    char Libelle_EC[MAX_LIBELLE];
    unsigned int Coefficient_EC;
    float Volume_Horaire_EC;
} Element_Constitutif;


typedef struct {
    char Code_UE[MAX_CODE];
    unsigned int Credit_UE;
    unsigned int Coefficient_UE;
    char Libelle_UE[MAX_LIBELLE];
    unsigned int Nb_EC;
    Element_Constitutif *ECs; // allocation dynamique
} Unite_Enseignement;



#endif //GESTION_MAQUETTE_PEDAGOGIQUE_ACGC_TYPES_H
