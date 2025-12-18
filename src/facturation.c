#include <stdio.h>
#include "facturation.h"

float calculerTotal(int nbNuits, float prixNuit) {
return nbNuits * prixNuit;
}

void creerFacture(Facture *f) {
printf("Numero de facture : ");
scanf("%d", &f->numeroFacture);

printf("Nom du client : ");
scanf("%s", f->nomClient);

printf("Nombre de nuits : ");
scanf("%d", &f->nbNuits);

printf("Prix par nuit : ");
scanf("%f", &f->prixNuit);

f->total = calculerTotal(f->nbNuits, f->prixNuit);
}

void afficherFacture(Facture f) {
printf("\n----- FACTURE HOTEL -----\n");
printf("Numero : %d\n", f.numeroFacture);
printf("Client : %s\n", f.nomClient);
printf("Nuits : %d\n", f.nbNuits);
printf("Prix/nuit : %.2f\n", f.prixNuit);
printf("TOTAL : %.2f\n", f.total);
}

void enregistrerFacture(Facture f) {
FILE *file = fopen("factures.txt", "a");
if (file == NULL) {
printf("Erreur d'ouverture du fichier !\n");
return;
}

fprintf(file, "%d %s %d %.2f %.2f\n",
f.numeroFacture,
f.nomClient,
f.nbNuits,
f.prixNuit,
f.total);

fclose(file);
}
