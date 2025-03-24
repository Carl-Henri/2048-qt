#include "damierdyn.h"
#include <iostream>
#include <QVariant>
#include <random>
#include <iomanip>
using namespace std;

// Constructeurs

DamierDyn::DamierDyn(int n, int m, int val1, int val2) {
    redim(n,m);
    Init(val1, val2);
}

void DamierDyn::Init(int val1, int val2) {

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distribRow(0, nombre_lignes - 1);
    uniform_int_distribution<> distribCol(0, nombre_colonnes - 1);

    int i1, j1, i2, j2;

    i1 = distribRow(gen);
    j1 = distribCol(gen);

    do {
        i2 = distribRow(gen);
        j2 = distribCol(gen);
    } while (i1 == i2 && j1 == j2);

    for (int i = 0 ; i < nombre_lignes ; i++) {
        for (int j = 0 ; j < nombre_colonnes ; j++) {
            tab[i][j] = 0;
        }
    };

    tab[i1][j1] = val1;
    tab[i2][j2] = val2;
    emit tableChangee();
}

void DamierDyn::reinitialiser() {
    Init();
    score = 0;
    emit scoreChange();
}

void DamierDyn::Set(int i, int j, int val) {
    if (i>=0 && i < nombre_lignes && j>=0 && j<nombre_colonnes)
        tab[i][j] = val;
}

void DamierDyn::suivant() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distribRow(0, nombre_lignes - 1);
    uniform_int_distribution<> distribCol(0, nombre_colonnes - 1);
    int val = 2;
    uniform_int_distribution<> distribVal(0, 9);
    int d;
    d = distribVal(gen);
    if (d == 9) {
        val = 4;
    }
    int i, j;
    do {
        i = distribRow(gen);
        j = distribCol(gen);
    } while (tab[i][j] != 0);
    tab[i][j] = val;

    // Mise à jour des coordonnées de la dernière tuile ajoutée
    lastAddedRow = i;
    lastAddedCol = j;

    // Notifier QML que la table et la dernière tuile ajoutée ont changé
    emit tableChangee();
    emit lastAddedTileChange();
}

QVariantList DamierDyn::lireLastAddedTile() {
    return QVariantList{lastAddedRow, lastAddedCol};
}

bool DamierDyn::bas() {
    bool modif = false;
    sauvegarde();
    m_deplacement = QVariantList(nombre_colonnes*nombre_lignes, 0);

    for (int j = 0; j < nombre_colonnes; j++) {
        bool* fusionne = new bool[nombre_lignes]();

        // Add the last line to the QML list
        if (tab[(nombre_lignes-1)][j] == 0) {
            m_deplacement[(nombre_lignes - 1)*nombre_lignes+j] = QVariant(-1);
        } else {
            m_deplacement[(nombre_lignes - 1)*nombre_lignes+j] = QVariant(nombre_lignes-1);
        }

        for (int i = nombre_lignes - 2; i >= 0; i--) {
            if (tab[i][j] == 0) {
                m_deplacement[i*nombre_lignes+j] = QVariant(-1);
                continue;}



            int nouvelle_pos = i;
            while (nouvelle_pos + 1 < nombre_lignes && tab[nouvelle_pos + 1][j] == 0) {
                nouvelle_pos++;
            }

            if (nouvelle_pos + 1 < nombre_lignes && tab[nouvelle_pos + 1][j] == tab[i][j] && !fusionne[nouvelle_pos + 1]) {
                tab[nouvelle_pos + 1][j] *= 2;
                m_deplacement[i*nombre_lignes+j] = QVariant(nouvelle_pos + 1);
                tab[i][j] = 0;
                fusionne[nouvelle_pos + 1] = true;
                score += tab[nouvelle_pos + 1][j];
                emit scoreChange();
                modif = true;
            }
            else if (nouvelle_pos != i) {
                tab[nouvelle_pos][j] = tab[i][j];
                m_deplacement[i*nombre_lignes+j] = QVariant(nouvelle_pos);
                tab[i][j] = 0;
                modif = true;
            }
            else{m_deplacement[i*nombre_lignes+j] = QVariant(nouvelle_pos);}

        }

        delete[] fusionne;
    }
    if (modif) emit deplace();
    return(modif);
}

bool DamierDyn::haut() {
    bool modif = false;
    sauvegarde();
    m_deplacement = QVariantList(nombre_colonnes*nombre_lignes, 0);

    for (int j = 0; j < nombre_colonnes; j++) {
        bool* fusionne = new bool[nombre_lignes](); // Suivi des fusions

        // Ajout de la première ligne à la liste QML.
        if (tab[0][j] == 0) {
            m_deplacement[j] = QVariant(-1);
        } else {
            m_deplacement[j] = QVariant(0);
        }

        for (int i = 1; i < nombre_lignes; i++) { // On commence à la deuxième ligne
            if (tab[i][j] == 0){
                m_deplacement[i*nombre_lignes+j] = QVariant(-1);
                continue;} // Ignore les cases vides

            int nouvelle_pos = i;
            while (nouvelle_pos - 1 >= 0 && tab[nouvelle_pos - 1][j] == 0) {
                nouvelle_pos--; // On cherche la case non vide au dessus de la case en cours de traitement
            }

            if (nouvelle_pos - 1 >= 0 && tab[nouvelle_pos - 1][j] == tab[i][j] && !fusionne[nouvelle_pos - 1]) {
                // On peut fusionner si la case trouvée a la même valeur que la case traitée et qu'elle n'a pas déjà fusionnée
                tab[nouvelle_pos - 1][j] *= 2;
                m_deplacement[i*nombre_lignes+j] = QVariant(nouvelle_pos - 1);
                tab[i][j] = 0;
                fusionne[nouvelle_pos - 1] = true; // Marquer la fusion
                score += tab[nouvelle_pos - 1][j]; // Augmenter le score
                emit scoreChange();
                modif = true;
            }
            else if (nouvelle_pos != i) {
                // Si pas de fusion possible, on fait un simple déplacement
                tab[nouvelle_pos][j] = tab[i][j];
                m_deplacement[i*nombre_lignes+j] = QVariant(nouvelle_pos);
                tab[i][j] = 0;
                modif = true;
            }
            else{m_deplacement[i*nombre_lignes+j]=QVariant(nouvelle_pos);}

        }

        delete[] fusionne; // Libération de la mémoire
    }
    if (modif) emit deplace();
    return(modif);
}

bool DamierDyn::gauche() {
    bool modif = false;
    sauvegarde();
    m_deplacement = QVariantList(nombre_colonnes*nombre_lignes, 0);

    for (int i = 0; i < nombre_lignes; i++) {
        bool* fusionne = new bool[nombre_colonnes]();

        // Add the first column to the QML list
        if (tab[i][0] == 0) {
            m_deplacement[i*nombre_lignes] = QVariant(-1);
        } else {
            m_deplacement[i*nombre_lignes] = QVariant(0);
        }

        for (int j = 1; j < nombre_colonnes; j++) {

            if (tab[i][j] == 0){
                m_deplacement[i*nombre_lignes+j] = QVariant(-1);
                continue;}

            int nouvelle_pos = j;
            while (nouvelle_pos - 1 >= 0 && tab[i][nouvelle_pos - 1] == 0) {
                nouvelle_pos--;
            }


            if (nouvelle_pos - 1 >= 0 && tab[i][nouvelle_pos - 1] == tab[i][j] && !fusionne[nouvelle_pos - 1]) {
                tab[i][nouvelle_pos - 1] *= 2;
                m_deplacement[i*nombre_lignes+j] = QVariant(nouvelle_pos - 1);
                tab[i][j] = 0;
                fusionne[nouvelle_pos - 1] = true;
                score += tab[i][nouvelle_pos - 1];
                emit scoreChange();
                modif = true;
            }
            else if (nouvelle_pos != j) {
                m_deplacement[i*nombre_lignes+j] = QVariant(nouvelle_pos);
                tab[i][nouvelle_pos] = tab[i][j];
                tab[i][j] = 0;
                modif = true;
            }
            else{m_deplacement[i*nombre_lignes+j] = QVariant(nouvelle_pos);}
        }
        delete[] fusionne;

    }
    if (modif) emit deplace();
    return(modif);

}

bool DamierDyn::droite() {
    bool modif = false;
    sauvegarde();
    m_deplacement = QVariantList(nombre_colonnes*nombre_lignes, 0);

    for (int i = 0; i < nombre_lignes; i++) {
        bool* fusionne = new bool[nombre_colonnes]();

        // Add the first column to the QML list
        if (tab[i][nombre_colonnes-1] == 0) {
            m_deplacement[i*nombre_lignes+nombre_colonnes-1] = QVariant(-1);
        } else {
            m_deplacement[i*nombre_lignes+nombre_colonnes-1] = QVariant(nombre_colonnes-1);
        }


        for (int j = nombre_colonnes - 2; j >= 0; j--) {
            if (tab[i][j] == 0){
                m_deplacement[i*nombre_lignes+j] = QVariant(-1);
                continue;}

            int nouvelle_pos = j;
            while (nouvelle_pos + 1 < nombre_colonnes && tab[i][nouvelle_pos + 1] == 0) {
                nouvelle_pos++;
            }

            if (nouvelle_pos + 1 < nombre_colonnes && tab[i][nouvelle_pos + 1] == tab[i][j] && !fusionne[nouvelle_pos + 1]) {
                tab[i][nouvelle_pos + 1] *= 2;
                m_deplacement[i*nombre_lignes+j] = QVariant(nouvelle_pos + 1);
                tab[i][j] = 0;
                fusionne[nouvelle_pos + 1] = true;
                score += tab[i][nouvelle_pos + 1];
                emit scoreChange();
                modif = true;
            }
            else if (nouvelle_pos != j) {
                tab[i][nouvelle_pos] = tab[i][j];
                m_deplacement[i*nombre_lignes+j] = QVariant(nouvelle_pos);
                tab[i][j] = 0;
                modif = true;
            }
            else{m_deplacement[i*nombre_lignes+j] = QVariant(nouvelle_pos);}
        }

        delete[] fusionne;
    }
    if (modif) emit deplace();
    return(modif);
}

bool DamierDyn::perdu() {
    bool perdu = true;
    // Si le damier n'est pas plein ou qu'il a y deux cases adjacentes de même valeur,
    // on n'a pas encore perdu.
    bool stop = false;
    for (int i = 0; i < nombre_lignes && !stop; i++) {
        for (int j = 0; j < nombre_colonnes; j++) {
            if (tab[i][j] == 0) {
                stop = true;
                perdu = false;
                break;
            }
            else if (i+1 < nombre_lignes && tab[i][j] == tab[i+1][j]) {
                stop = true;
                perdu = false;
                break;
            }

            else if (j+1 < nombre_colonnes && tab[i][j] == tab[i][j+1]) {
                stop = true;
                perdu = false;
                break;
            }
        }
    }
    return perdu;
}

QVariantList DamierDyn::lireTable() {
    QVariantList res;
    for (int i = 0; i<nombre_lignes;i++) {
        QVariantList ligneQML;
        for (int j = 0; j<nombre_colonnes;j++) {
            ligneQML.append(QVariant(tab[i][j]));
        }
        res.append(ligneQML);
    }
    return res;
}

void DamierDyn::retour_arriere() {
    if (precedent_tab != 0) {
        for (int i = 0; i<nombre_lignes;i++) {
            for (int j = 0; j<nombre_colonnes;j++) {
                    tab[i][j] = precedent_tab[i][j];
            }
        }
        score = precedent_score;
        emit tableChangee();
        emit scoreChange();
    }
}

QString DamierDyn::lireScore() {
    QString res = QString::number(score);
    return(res);
}

// Destructeur
DamierDyn::~DamierDyn() {
    if (tab != 0) {
        for (int i=0;i<nombre_lignes;i++)
            delete [] tab[i];
        delete[] tab;
        tab = 0;
    }
}

void DamierDyn::redim(int n,int m) {
    if (tab !=0) {
        for (int i=0;i<nombre_lignes;i++)
            delete [] tab[i];
        delete [] tab;
        tab = 0;
    }

    if (precedent_tab !=0) {
        for (int i=0;i<nombre_lignes;i++)
            delete [] precedent_tab[i];
        delete [] precedent_tab;
        precedent_tab = 0;
    }

    nombre_lignes = n;
    nombre_colonnes = m;

    tab = new int*[nombre_lignes];
    for (int i=0;i<nombre_lignes;i++) {
        tab[i] = new int[nombre_colonnes];
    };
}

void DamierDyn::sauvegarde() {
    if (precedent_tab !=0) {
        for (int i=0;i<nombre_lignes;i++)
            delete [] precedent_tab[i];
        delete [] precedent_tab;
        precedent_tab = 0;
    }

    precedent_tab = new int*[nombre_lignes];
    for (int i=0;i<nombre_lignes;i++) {
        precedent_tab[i] = new int[nombre_colonnes];
    };
    for (int i=0;i<nombre_lignes;i++)
        for (int j=0;j<nombre_colonnes;j++)
            precedent_tab[i][j] = tab[i][j];
    precedent_score = score;
}


// Surcharge des opérateurs

DamierDyn& DamierDyn::operator=(const DamierDyn& D) {
    if (this != &D) {
        redim(D.nombre_lignes,D.nombre_colonnes);
        for (int i=0;i<nombre_lignes;i++)
            for (int j=0;j<nombre_colonnes;j++)
                tab[i][j] = D.tab[i][j];
    }
    return(*this);
}

ostream& operator<<(ostream& os, const DamierDyn& damier) {
    os << "Score : " << damier.score << endl;
    for (int i = 0; i < damier.nombre_lignes; ++i) {
        for (int j = 0; j < damier.nombre_colonnes; ++j) {
            os << setw(6) << damier.tab[i][j] << " ";
        }
        os << endl;
    }
    return os;
}
