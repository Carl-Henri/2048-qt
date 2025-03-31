#ifndef DAMIERDYN_H
#define DAMIERDYN_H

#include <QObject>
#include <iostream>
#include <QVariant>

using namespace std;

class DamierDyn : public QObject {
    Q_OBJECT
    // On communique le damier sous forme de QVariantList à l'interface
    Q_PROPERTY(QVariantList table READ lireTable NOTIFY tableChangee() )
    Q_PROPERTY(QVariantList deplacement READ getDeplacement NOTIFY deplace )
    // On communique le score
    Q_PROPERTY(QString scoreQML READ lireScore NOTIFY scoreChange() )
    Q_PROPERTY(QVariantList lastAddedTile READ lireLastAddedTile NOTIFY lastAddedTileChange())




public:

    DamierDyn(int n,int m,int val1=2, int val2=2);
    void Init(int val1 = 2, int val2 = 2);
    Q_INVOKABLE void redim(int n, int m);

    // Méthodes pour gérer le jeu
    Q_INVOKABLE void reinitialiser();
    Q_INVOKABLE void suivant();
    Q_INVOKABLE bool perdu();

    // Actions possibles
    Q_INVOKABLE bool bas();
    Q_INVOKABLE bool haut();
    Q_INVOKABLE bool gauche();
    Q_INVOKABLE bool droite();

    // Méthodes pour gérer la fonctionnalité de retour arrière
    void sauvegarde();
    Q_INVOKABLE void retour_arriere();

    ~DamierDyn();
    QVariantList lireTable();
    QString lireScore();
    QVariantList lireLastAddedTile();
    QVariantList getDeplacement() const {
        return m_deplacement;
    }
    Q_INVOKABLE void emitTableChangee() {
        emit tableChangee();
    }

    friend ostream& operator<<(ostream& os, const DamierDyn& damier);

    DamierDyn& operator=(const DamierDyn& D);

private:
    int lastAddedRow = -1;
    int lastAddedCol = -1;
    QVariantList m_deplacement = QVariantList(); // Enregistre les mouvements

    // On fait un tableau dynamique à deux dimensions pour représenter le damier du 2048
    int nombre_lignes;
    int nombre_colonnes;
    int** tab = 0;

    int score = 0;

    // Pour la fonctionnalité "retour arrière"
    int** precedent_tab = 0;
    int precedent_score = 0;

signals:
    void deplace();
    void tableChangee();
    void scoreChange();
    void retourArrierePossChange();
    void lastAddedTileChange();
};

#endif // DAMIERDYN_H
