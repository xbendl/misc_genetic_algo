/**
* \file mapWidget.h
* 2D mapa pro zobrazeni pruchodu mesty u problemu obchodniho cestujiciho
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef MAPWIDGET_H_INCLUDED
#define MAPWIDGET_H_INCLUDED

#include <QtGui>
#include <QWidget>
#include <QMetaType>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "tsp_evolution.h"

/// 2D mrizka pro zobrazeni stavu bunek celularniho automatu.
class MapWidget : public QWidget
{
    Q_OBJECT

    vector<CityPosition> cityPosition;   ///< Pozice mest na mape.
    tIndividual tempBestIndividual;
    tIndividual bestIndividual;          ///< Jedinec evolvovane populace s nejlepsim ohodnocenim cesty.
    int timerInterval;    ///< Interval tiku casovace
    int timerId;          ///< ID casovace
    bool timerRunning;

    QMutex mutex;

    protected:
    void timerEvent(QTimerEvent *timerEvent);
    void paintEvent(QPaintEvent *event);

    public:
    /**
    * Konstruktor.
    * @param parent Rodicovsky objekt.
    */
    MapWidget(QWidget *parent = 0);

    /**
    * Nastavi mapu podle predaneho seznamu s pozicemi mest
    * @param cityPosition Pozice mest na mape.
    */
    void SetNewMap(vector<CityPosition> &cityPosition);

    /**
    * Nastavi na mape trasu pruchodu mesty podle predaneho jedince.
    * @param individual Jedinec populace s nejlepe ohodnocenou trasou pruchodu mesty.
    */
    void SetPath(tIndividual &individual);

    /**
    * Nastavi nejlepsiho jedince.
    * @param bestIndividual Jedinec populace s nejlepe ohodnocenou trasou pruchodu mesty.
    */
    void SetBestIndividual(tIndividual bestIndividual);

    /**
    * Spusti casovac.
    */
    void StartTimer();

    /**
    * Vypne casovac.
    * @param immidiatelyReaction Urci rychlost reakce (zda ma dobehnout aktualni krok).
    */
    void KillTimer(bool immidiatelyReaction);

    /**
    * Zjisti stav casovace.
    * @return Stav casovace.
    */
    bool IsTimerRunning();
};

#endif // MAPWIDGET_H_INCLUDED
