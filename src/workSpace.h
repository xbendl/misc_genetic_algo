/**
* \file workSpace.h
* Spravce hlavniho okna.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QtGui>
#include <QDialog>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <iomanip>
#include <time.h>
#include <sys/time.h>
#include "tsp_evolution.h"
#include "config.h"
#include "mapWidget.h"

/// Spravce hlavniho okna.
class WorkSpace : public QDialog
{
    Q_OBJECT

    private:
    tProblemInfo info;                   ///< Informace o resenem problemu.
    vector<CityMatrixRow> cityMatrix;    ///< Matice spojeni mest.
    vector<CityPosition> cityPosition;   ///< Pozice mest na mape.
    TSPEvolution tspEvolution;           ///< Spravce procesu evoluce trasy obchodniho cestujiciho.
    tIndividual resultPath;              ///< Vysledny jedinec reprezentujici nejlepsi nalezene reseni.

    MapWidget *mapWidget;                ///< Mrizka pro zobrazeni zygoty

    QLabel *taskNameLabel;
    QLabel *projectStatus;

    QPushButton *loadTaskButton;
    QPushButton *evoluteStartButton;
    QPushButton *evoluteStopButton;
    QPushButton *evoluteAbortButton;
    QPushButton *saveResultButton;

    QComboBox *tCrossoverTypeComboBox;
    QComboBox *tMutationTypeComboBox;

    QSpinBox *sPopulationStepLine;
    QSpinBox *nGenerationsStepLine;
    QSpinBox *sTournamentStepLine;
    QSpinBox *sElitismStepLine;
    QSpinBox *logIntervalStepLine;
    QDoubleSpinBox *pMutationDoubleSpinBox;

    QPlainTextEdit *resultTextBox;

    QProgressBar *evolutionProgressBar;

    protected:
    void paintEvent(QPaintEvent *);

    /**
     * Nacte ze souboru udaje o poloze jednotlivych mest.
     * @return Urcuje, zda-li  se podarilo data nacist.
     */
    bool LoadData();

    private slots:
    /**
    * Zjisti od uzivatele cestu k souboru s ulohou.
    */
    void SetOpenFileName();

    /**
    * Zjisti od uzivatele cestu k souboru, do ktereho budou ulozeny vysledky.
    */
    void SetResultFile();

    /**
    * Spusti proces evoluce pravidel.
    */
    void StartEvolution();

    /**
    * Pozastavi proces evoluce pravidel.
    */
    void StopEvolution();

    /**
    * Zrusi proces evoluce pravidel.
    */
    void AbortEvolution();

    /**
    * Zmena stavu polozky "Velikost populace".
    * newValue Novy stav policka.
    */
    void ChangeSPopulation(int newValue);

    /**
    * Zmena stavu polozky "Pocet generaci".
    * newValue Novy stav policka.
    */
    void ChangeNGenerations(int newValue);

    /**
    * Zmena stavu polozky "Pravdepodobnost mutace".
    * newValue Novy stav policka.
    */
    void ChangePMutation(double newValue);

    /**
    * Zmena stavu polozky "Typ mutace".
    * newValue Novy stav policka.
    */
    void ChangeTMutation(int newValue);

    /**
    * Zmena stavu polozky "Typ krizeni".
    * newValue Novy stav policka.
    */
    void ChangeTCrossover(int newValue);

    /**
    * Zmena stavu polozky "Elitismus".
    * newValue Novy stav policka.
    */
    void ChangeSElitism(int newValue);

    /**
    * Zmena stavu polozky "Ucastnici turnaje".
    * newValue Novy stav policka.
    */
    void ChangeSTournament(int newValue);

    /**
    * Zmena stavu polozky "Log interval".
    * newValue Novy stav policka.
    */
    void ChangeLogInterval(int newValue);

    /**
    * Prijme informaci o aktualnim stavu evolucniho vypoctu.
    * @param resultPath Nejlepsi reseni nalezene v aktualnim evolucnim kroku.
    * @param actGeneration Cislo aktualni generace.
    * @param minCurrentPathPrice Nejlepsi cena reseni v aktualnim evolucnim kroku.
    * @param avgCurrentPathPrice Prumerna cena reseni v aktualnim evolucnim kroku.
    * @param maxCurrentPathPrice Nejhorsi cena reseni v aktualnim evolucnim kroku.
    */
    void ReceiveCurrentPathGenerated(tIndividual resultPath, int actGeneration, float minCurrentPathPrice, float avgCurrentPathPrice, float maxCurrentPathPrice);

    /**
    * Prijme informaci o stavu dokonceneho evolucniho vypoctu.
    * @param resultPath Nejlepsi reseni nalezene v aktualnim evolucnim kroku.
    * @param timeSec Celkova doba vypoctu.
    * @param timeUsec Celkova doba vypoctu.
    */
    void ReceivePathGenerated(tIndividual resultPath);

    public:
    /**
    * Konstruktor.
    */
    WorkSpace(QWidget *parent = 0);
};

Q_DECLARE_METATYPE(tIndividual)

#endif // WORKSPACE_H
