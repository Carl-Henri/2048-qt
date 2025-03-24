#ifndef DAMIERDYN_H
#define DAMIERDYN_H

#include <QObject>
#include <iostream>
#include <QVariant>

using namespace std;

class DamierDyn : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList table READ lireTable NOTIFY tableChangee() )
    Q_PROPERTY(QString scoreQML READ lireScore NOTIFY scoreChange() )

public:

    DamierDyn(int n,int m,int val1=2, int val2=2);
    void Init(int val1 = 2, int val2 = 2);
    Q_INVOKABLE void redim(int n, int m);
    Q_INVOKABLE void reinitialiser();
    Q_INVOKABLE void suivant();
    Q_INVOKABLE bool bas();
    Q_INVOKABLE bool haut();
    Q_INVOKABLE bool gauche();
    Q_INVOKABLE bool droite();
    Q_INVOKABLE bool perdu();
    void sauvegarde();
    Q_INVOKABLE void retour_arriere();
    ~DamierDyn();
    QVariantList lireTable();
    QString lireScore();

    friend ostream& operator<<(ostream& os, const DamierDyn& damier);

    DamierDyn(const DamierDyn &D);
    void Set(int i, int j, int val);
    DamierDyn& operator=(const DamierDyn& D);

private:
    int nombre_lignes;
    int nombre_colonnes;
    int score = 0;
    int** tab = 0;
    int** precedent_tab = 0;
    int precedent_score = 0;

signals:
    void tableChangee();
    void scoreChange();
    void retourArrierePossChange();
};

#endif // DAMIERDYN_H
