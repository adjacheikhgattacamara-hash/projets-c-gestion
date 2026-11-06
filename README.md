# Projets C : gestion de données avec fichiers binaires

Deux projets de fin de cours de **Programmation en langage C**, réalisés à l'UNCHK (Sénégal) par **Adja Cheikh Gatta Camara**, au sein de **ACGC Innov'Bot**.

Ils reprennent la même approche : programme en console avec menus, structures de données, allocation dynamique, persistance dans un fichier binaire, contrôle des saisies.

## 1. Gestion de bibliothèque (`Gestion_Bibliotheque_ACGC`)

Gestion d'un fonds d'ouvrages stocké dans un fichier binaire (`bibliotheque.dat`).

- Structures `Personne` (auteur ou éditeur), `Date` et `Ouvrage` ; les auteurs sont stockés par allocation dynamique
- Enregistrement, affichage, recherche par numéro et suppression d'un ouvrage
- Modification du titre, de l'éditeur, de l'année, de la quantité, de la date d'édition ou d'un auteur
- Validation des dates (années bissextiles, mois, jours) et unicité des numéros d'ouvrage
- Statistiques et listes par critères : nombre d'auteurs, éditeur, auteur, période d'édition, titres identiques, couple auteur/éditeur

## 2. Gestion de maquette pédagogique (`Gestion_Maquette_Pedagogique_ACGC`)

Gestion d'une maquette de formation : unités d'enseignement (UE) et leurs éléments constitutifs (EC), stockées dans un fichier binaire.

- Structures `Unite_Enseignement` (code, crédits, coefficient, libellé) et `Element_Constitutif` (code, coefficient, volume horaire) ; les EC d'une UE sont alloués dynamiquement
- Ajout, recherche, modification et suppression d'une UE et de ses EC
- Contrôle de l'unicité des codes d'UE et d'EC
- Calcul du volume horaire total d'une UE
- Statistiques et listes : UE de même crédit, UE dans un intervalle de crédits, UE ayant le plus de crédits ou d'EC, EC dans un intervalle de volume horaire, EC de même nom

## Technologies

Langage C (C standard) · fichiers binaires · allocation dynamique · CMake (CLion)

## Compilation et lancement

Dans le dossier d'un projet :

    gcc -Wall -o programme *.c
    ./programme

Ou avec CMake :

    cmake -S . -B build
    cmake --build build

## Structure

- `Gestion_Bibliotheque_ACGC/` : `main.c`, `Ouvrage.c`, `Ouvrage.h`, `types.h`, `CMakeLists.txt`
- `Gestion_Maquette_Pedagogique_ACGC/` : `main.c`, `Maquette.c`, `Maquette.h`, `types.h`, `CMakeLists.txt`

## Licence

MIT, voir [LICENSE](LICENSE).
