/**
* \file tsp_evolution.h
* Modul pro vypocet optimalni cesty.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef TSP_EVOLUTION_H_INCLUDED
#define TSP_EVOLUTION_H_INCLUDED

#include <QMetaType>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <float.h>
#include <algorithm>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <sstream>
#include "config.h"

using namespace std;

/// Informace o resenem problemu.
typedef struct
{
    string name;      ///< Pojmenovani (identifikace) problemu.
    string comment;   ///< Komentar k problemu.
    int dimension;    ///< Dimenze (pocet mest)
} tProblemInfo;

/// Pozice mesta na mape.
typedef struct
{
    int x;  ///< Pozice X
    int y;  ///< Pozice Y
} CityPosition;

/// Radek hodnot v matici propojeni mest.
typedef vector<float> CityMatrixRow;

/// Genom jednotlivce v populaci.
typedef struct
{
    vector<int> genome;  ///< Genom (permutace indexu mest).
    float price;         ///< Cena cesty mezi mesty.
} tIndividual;

/// Trida pro optimalizaci trasy obchodniho cestujiciho.
class TSPEvolution : public QThread
{
    Q_OBJECT

    private:

    /// Tabulka sekci (pro operator krizeni CX)
    typedef struct vector<int> tCitySection;

    /// Prepisovaci pravidlo (pro operator krizeni PMX)
    typedef struct
    {
        int cityPosition1;    ///< Index prvni mesta.
        int cityPosition2;    ///< Index druheho mesta.
    } tTranscriptRule;

    /// Hranova tabulka (pro operator krizeni ERX).
    typedef struct
    {
        int cityIndex;              ///< Index centralniho mesta
        vector<int> neighbourCity;  ///< Index sousedniho mesta.
    } tEdgeTable;

    /// Vysledky vypoctu.
    typedef struct
    {
        int crossoverCount;          ///< Pocet aplikaci operatoru krizeni od inicializace.
        int mutationCount;           ///< Pocet aplikaci operatoru mutace od inicializace.
        int genomeEvaluationCount;   ///< Pocet ohodnoceni genomu od inicializace.
        float avgPathPrice;          ///< Prumerna cena cesty od inicializace.
        float maxPathPrice;          ///< Maximalni cena cesty od inicializace.
        float minPathPrice;          ///< Minimalni cene cesty od inicializace.
        float avgInitialPathPrice;   ///< Prumerna cena cesty v pocatecni populaci.
        float minInitialPathPrice;   ///< Nejlepsi cena cesty v pocatecni populaci.
        float maxInitialPathPrice;   ///< Nejhorsi cena cesty v pocatecni populaci.
        float avgCurrentPathPrice;   ///< Prumerna cena cesty v aktualni populaci.
        float minCurrentPathPrice;   ///< Nejlepsi cena cesty v aktualni populaci.
        float maxCurrentPathPrice;   ///< Nejhorsi cena cesty v aktualni populaci.
        struct timeval runningTime;  ///< Cas behu vypoctu.
    } tResult;

    tProblemInfo info;           ///< Informace o resenem problemu.
    tResult result;              ///< Vysledky vypoctu.
    int actGeneration;           ///< Cislo aktualni generace behu evoluce.
    vector<tIndividual> parentPop;     ///< Genomy jednotlivcu populace rodicu (permutace mest).
    vector<tIndividual> childPop;      ///< Genomy jednotlivcu populace deti (permutace mest).
    vector<tIndividual> tempPop;       ///< Genomy rodicu a deti vybrane do dalsiho cyklu behu GA.
    vector<CityMatrixRow> cityMatrix;  ///< Matice spojeni mest.
    struct timeval startTime;    ///< Cas zacatku spusteni vypoctu.
    tIndividual resultPath;      ///< Vysledna cesta obchodniho cestujiciho.
    vector<float> tempAvgPathPrice;
    vector<float> tempMinPathPrice;
    vector<float> tempMaxPathPrice;

    bool abortFlag;                       //< Priznak ukonceni evolucnich vypoctu
    bool sleepFlag;                       //< Priznak pozastaveni evolucnich vypoctu

    QMutex mutex;
    QWaitCondition condition;

    /**
    * Smycka evolucniho vypoctu.
    */
    void run();

    /**
     * Ohodnoti populaci jedincu.
     * @param population Populace jedincu.
     */
    void EvaluatePopulation(vector<tIndividual> &population);

    /**
     * Ohodnoti jedince v populaci.
     * @param individual Ohodnocovany jedinec v populaci.
     */
    void EvaluateGenome(tIndividual &individual);

    /**
     * Vytvori populaci jedincu (mnozinu permutaci mest).
     */
    void GenerateInitPopulation();

    /**
     * Vytvori noveho potomka podle vybraneho type krizeni a mutace.
     * @param parent1 Prvni rodic.
     * @param parent2 Druhy rodic.
     */
    void CreateNewIndividuals(tIndividual &parent1, tIndividual &parent2);

    /**
     * Mutuje genom vybraneho jedince populace.
     * @param individual Vybrany jedinec populace urceny k mutaci.
     */
    void Mutation(tIndividual &individual);

    /**
     * Mutace typu jednoduchy swap (zamena dvou nahodne vybranych mest).
     * @param individual Vybrany jedinec populace urceny k mutaci.
     */
    void MutationSwap(tIndividual &individual);

    /**
     * Mutace typu inverze (zamena dvou nahodne vybranych mest).
     * @param individual Vybrany jedinec populace urceny k mutaci.
     */
    void MutationInversion(tIndividual &individual);

    /**
     * Mutace typu 2-OPT.
     * @param individual Vybrany jedinec populace urceny k mutaci.
     */
    void MutationOpt2(tIndividual &individual);

    /**
     * Mutace typu 3-OPT.
     * @param individual Vybrany jedinec populace urceny k mutaci.
     */
    void MutationOpt3(tIndividual &individual);

    /**
     * Krizeni typu PMX (partially matched crossover).
     * @param parent1 Prvni rodic.
     * @param parent2 Druhy rodic.
     * @param child Potomek vznikly krizenim.
     */
    void CrossoverPMX(tIndividual &parent1, tIndividual &parent2, tIndividual &child1);

    /**
     * Krizeni typu ERX (edge recombination crossover).
     * @param parent1 Prvni rodic.
     * @param parent2 Druhy rodic.
     * @param child Potomek vznikly krizenim.
     */
    void CrossoverERX(tIndividual &parent1, tIndividual &parent2, tIndividual &child);

    /**
     * Krizeni typu OX (order crossover).
     * @param parent1 Prvni rodic.
     * @param parent2 Druhy rodic.
     * @param child Potomek vznikly krizenim.
     */
    void CrossoverOX(tIndividual &parent1, tIndividual &parent2, tIndividual &child);

    /**
     * Krizeni typu CX (cycle crossover).
     * @param parent1 Prvni rodic.
     * @param parent2 Druhy rodic.
     * @param child1 Prvni potomek vznikly krizenim.
     * @param child2 Druhy potomek vznikly krizenim.
     */
    void CrossoverCX(tIndividual &parent1, tIndividual &parent2, tIndividual &child1, tIndividual &child2);

    /**
     * Provede turnajovou selekci nad danou populaci a urci viteze.
     * @param population Populace, nad kterou selekce probiha.
     * @param tournamentSize Pocet ucastniku turnaje.
     * @return Vitez turnaje (permutace cest s nejnizsi cenou).
     */
    int TournamentSelection(vector<tIndividual> &population);

    /**
     * Zajisti obnovu populace.
     * @param population Populace, nad kterou selekce probiha.
     * @param selectedPopulation
     * @return Vitez turnaje (permutace cest s nejnizsi cenou).
     */
    void Survive(vector<tIndividual> &population, vector<tIndividual> &selectedPopulation);

    /**
     * Ulozi vysledky k dane generaci behu vypoctu
     */
    void SetGenerationStatistics();

    /**
     * Ziska cislo aktualni generace behu vypoctu.
     * @return Urcuje, zda-li jiz bylo dosazeno pozadovaneho poctu generaci.
     */
    bool Done();

    /**
     * Provede jeden evolucni krok.
     */
    void Step();

    public:

    /**
    * Hleda trasu obchodniho cestujiciho pro zadanou mnozinu mest.
    * @param cityMatrix Mnozina mest.
    * @param info Informace o problemu.
    */
    void FindSolution(vector<CityMatrixRow> &cityMatrix, tProblemInfo info);

    /**
    * Pozastavi evoluci pravidel.
    */
    void StopEvolution();

    /**
    * Zrusi evoluci pravidel.
    */
    void AbortEvolution();

    /**
     * Ulozi vysledky behu vypoctu.
     * @param outputFile Cesta k vystupimu souboru, do ktereho budou ulozeny vysledky behu evoluce.
     * @return Urcuje, zda-li se zapsani vysledku do vystupniho souboru podarilo.
     */
    bool SaveResults(string outputFile);

    /**
    * Konstruktor.
    * @param parent Rodicovsky objekt.
    */
    TSPEvolution(QObject *parent = 0);

    /**
     * Destruktor.
     */
    ~TSPEvolution();

    signals:
    /**
    * Signal zaslany po provedeni urceneho poctu evolucnich kroku.
    * @param resultPath Nejlepsi reseni nalezene v aktualnim evolucnim kroku.
    * @param actGeneration Cislo aktualni generace.
    * @param minCurrentPathPrice Nejlepsi cena reseni v aktualnim evolucnim kroku.
    * @param avgCurrentPathPrice Prumerna cena reseni v aktualnim evolucnim kroku.
    * @param maxCurrentPathPrice Nejhorsi cena reseni v aktualnim evolucnim kroku.
    */
    void CurrentPathGenerated(tIndividual resultPath, int actGeneration, float minCurrentPathPrice, float avgCurrentPathPrice, float axCurrentPathPrice);

    /**
    * Signal zaslany po dokonceni behu evoluce.
    * @param resultPath Nejlepsi reseni nalezene v aktualnim evolucnim kroku.
    */
    void PathGenerated(tIndividual resultPath);
};

/**
 * Porovna dve ruzne permutace na zaklade ceny cesty mezi mesty.
 * @param inv1 Prvni porovnavany jedinec.
 * @param inv2 Druhy porovnavany jedinec.
 * @return True, pokud je cena cesty prvniho jedince mensi nez cena cesty druheho jedince.
 */
bool FitnessSort(tIndividual inv1, tIndividual inv2);

#endif // TSP_EVOLUTION_H_INCLUDED
