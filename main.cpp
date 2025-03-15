#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "damierdyn.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    DamierDyn Damier(4,4);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("monDamier",&Damier);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Projet_2048", "Main");

    return app.exec();
}
