#include "tsp_evolution.h"

TSPEvolution::TSPEvolution(QObject *) : QThread()
{
    // Nastaveni priznaku ukonceni a uspani
    abortFlag = false;
    sleepFlag = false;
}

void TSPEvolution::FindSolution(vector<CityMatrixRow> &cityMatrix, tProblemInfo info)
{
    // Zpracovani predanych parametru
    this->cityMatrix = cityMatrix;
    this->info = info;

    // Priprava pouzivanych vektoru ve vypoctu
    parentPop.clear();
    childPop.clear();
    tempPop.clear();
    actGeneration  = 0;

    result.minPathPrice = result.minInitialPathPrice = result.minCurrentPathPrice = FLT_MAX;
    result.crossoverCount = result.mutationCount = result.genomeEvaluationCount =
         result.avgPathPrice = result.avgInitialPathPrice = result.avgCurrentPathPrice =
         result.maxPathPrice = result.maxInitialPathPrice = result.maxCurrentPathPrice = 0;

    // Nastaveni priznaku ukonceni a uspani
    this->abortFlag = false;
    this->sleepFlag = false;

    // Spusteni evoluce
    if(!isRunning())
        start();
}

void TSPEvolution::run()
{
    GenerateInitPopulation();      // vytvori pocatecni populaci jedincu

    while(!Done())
    {
        Step();

        if(actGeneration % Config::GetInstance()->logInterval == 0)
        {   // prubezne vypisovani aktualni generace vypoctu
            SetGenerationStatistics();
            cout.flush();

            mutex.lock();
            if(!abortFlag)
                emit CurrentPathGenerated(resultPath, actGeneration, result.minCurrentPathPrice, result.avgCurrentPathPrice, result.maxCurrentPathPrice);
            mutex.unlock();
        }

        // Uspani pri nastaveni priznaku uspani
        mutex.lock();
        if(sleepFlag)
        {
            condition.wait(&mutex);
            sleepFlag = false;
        }
        mutex.unlock();

        // Ukonceni pri nastaveni priznaku ukonceni
        mutex.lock();
        if(abortFlag)
        {
            // Vypocet a ulozeni doby behu
            struct timeval endTime;
            gettimeofday(&endTime, NULL);
            result.runningTime.tv_sec = endTime.tv_sec - startTime.tv_sec;
            if((endTime.tv_usec - startTime.tv_usec) < 0)
            {
                result.runningTime.tv_sec--;
                result.runningTime.tv_usec = (startTime.tv_usec - endTime.tv_usec) + 1;
            }
            else
            {
                result.runningTime.tv_usec = endTime.tv_usec - startTime.tv_usec;
            }
            mutex.unlock();
            return;
        }
        mutex.unlock();
    }

    // Vygenerovani signalu; zasilame nejlepsi nalezenou cestu pro problem obchodniho cestujiciho
    emit PathGenerated(resultPath);
}

void TSPEvolution::StopEvolution()
{
    if(sleepFlag)
    {   // Obnoveni prehravani
        mutex.lock();
        sleepFlag = false;
        condition.wakeOne();
        mutex.unlock();
    }
    else
    {   // Pozastaveni prehravani
        mutex.lock();
        sleepFlag = true;
        mutex.unlock();
    }
}

void TSPEvolution::AbortEvolution()
{
    mutex.lock();
    abortFlag = true;      // Nastaveni priznaku pro ukonceni
    condition.wakeOne();   // Probuzeni spiciho vlakna
    mutex.unlock();

    wait();                // Pockani na dobehnuti vlakna
}

TSPEvolution::~TSPEvolution()
{
    mutex.lock();
    abortFlag = true;      // Nastaveni priznaku pro ukonceni
    condition.wakeOne();   // Probuzeni spiciho vlakna
    mutex.unlock();

    wait();                // Pockani na dobehnuti vlakna
}

void TSPEvolution::GenerateInitPopulation()
{
    vector<int> cityPattern, cityPermutation;
    for(int i = 0; i < info.dimension; i++)
        cityPattern.push_back(i);

    // Alokace prostoru pro rodice i potomky (sdileji stejne pole)
    int randIndex;
    tIndividual actCity;
    for(int i = 0; i < Config::GetInstance()->sPopulation; i++)
    {
        actCity.price = 0;
        actCity.genome.clear();
        cityPermutation = cityPattern;
        if(i < Config::GetInstance()->sPopulation)
        {   // generovani genomu probiha pouze pro rodice
            for(int j = 0; j < info.dimension; j++)
            {
                randIndex = rand() % cityPermutation.size();
                actCity.genome.push_back(cityPermutation[randIndex]);
                cityPermutation.erase(cityPermutation.begin() + randIndex);
            }
        }
        parentPop.push_back(actCity);
    }

    // Ohodnoceni pocatecni populace jedincu
    EvaluatePopulation(parentPop);

    // Ulozeni statistik pocatecni populace jedincu
    result.minInitialPathPrice = FLT_MAX;
    result.maxInitialPathPrice = 0;
    result.avgInitialPathPrice = 0;
    for(int i = 0; i < (int)parentPop.size(); i++)
    {
        if(parentPop[i].price > result.maxInitialPathPrice)
            result.maxInitialPathPrice = parentPop[i].price;
        if(parentPop[i].price < result.minInitialPathPrice)
            result.minInitialPathPrice = parentPop[i].price;
        result.avgInitialPathPrice += parentPop[i].price;
    }
    result.avgInitialPathPrice = result.avgInitialPathPrice / parentPop.size();
    tempMinPathPrice.push_back(result.minInitialPathPrice);
    tempAvgPathPrice.push_back(result.avgInitialPathPrice);
    tempMaxPathPrice.push_back(result.maxInitialPathPrice);

    mutex.lock();
    if(!abortFlag)
        emit CurrentPathGenerated(resultPath, 0, result.minInitialPathPrice, result.avgInitialPathPrice, result.maxInitialPathPrice);
    mutex.unlock();
}

void TSPEvolution::SetGenerationStatistics()
{
    result.minCurrentPathPrice = INT_MAX;
    result.maxCurrentPathPrice = 0;
    result.avgCurrentPathPrice = 0;

    for(int i = 0; i < (int)parentPop.size(); i++)
    {
        if(parentPop[i].price > result.maxCurrentPathPrice)
            result.maxCurrentPathPrice = parentPop[i].price;
        if(parentPop[i].price > result.maxPathPrice)
            result.maxPathPrice = parentPop[i].price;
        if(parentPop[i].price < result.minCurrentPathPrice)
            result.minCurrentPathPrice = parentPop[i].price;
        if(parentPop[i].price < result.minPathPrice)
        {
            result.minPathPrice = parentPop[i].price;
            resultPath = parentPop[i];
        }
        result.avgCurrentPathPrice += parentPop[i].price;
    }
    result.avgCurrentPathPrice = result.avgCurrentPathPrice / parentPop.size();
    tempAvgPathPrice.push_back(result.avgCurrentPathPrice);
    tempMinPathPrice.push_back(result.minCurrentPathPrice);
    tempMaxPathPrice.push_back(result.maxCurrentPathPrice);
}

void TSPEvolution::EvaluatePopulation(vector<tIndividual> &population)
{
    for(int i = 0; i < (int)population.size(); i++)
    {   // ohodnoti se vsechny jedinci populace
        EvaluateGenome(population[i]);
    }
}

void TSPEvolution::EvaluateGenome(tIndividual &individual)
{
    individual.price = 0;
    for(int i = 0; i < info.dimension; i++)
    {   // ohodnoti se prechod mezi kazdymi dvema mesty
        individual.price += cityMatrix[individual.genome[i]][individual.genome[(i + 1) % info.dimension]];
    }

    result.genomeEvaluationCount++;
}

bool TSPEvolution::Done()
{
    bool done = (actGeneration == Config::GetInstance()->nGenerations);

    // Akce po skonceni vypoctu
    if(done)
    {
        // Vypocet a ulozeni doby behu
        struct timeval endTime;
        gettimeofday(&endTime, NULL);
        result.runningTime.tv_sec = endTime.tv_sec - startTime.tv_sec;
        if((endTime.tv_usec - startTime.tv_usec) < 0)
        {
            result.runningTime.tv_sec--;
            result.runningTime.tv_usec = (startTime.tv_usec - endTime.tv_usec) + 1;
        }
        else
        {
            result.runningTime.tv_usec = endTime.tv_usec - startTime.tv_usec;
        }
    }

    return done;
}

void TSPEvolution::Step()
{
    // Nastartovani casovace (pokud jde o prvni generaci)
    if(actGeneration == 0)
        gettimeofday(&startTime, NULL);

    int pIndex1, pIndex2;
    childPop.clear();
    tempPop.clear();

    // Vytvoreni mnoziny potomku
    while((int)childPop.size() < Config::GetInstance()->sPopulation)
    {
        // Vyber rodicu
        pIndex1 = TournamentSelection(parentPop);
        pIndex2 = TournamentSelection(parentPop);

        // Tvorba novych potomku z vybranych potomku pomoci operatoru krizeni a mutace
        CreateNewIndividuals(parentPop[pIndex1], parentPop[pIndex2]);
    }

    // Ohodnoceni mnoziny potomku
    EvaluatePopulation(childPop);
    tempPop.resize(childPop.size() + parentPop.size());
    copy(parentPop.begin(), parentPop.end(), tempPop.begin());
    copy(childPop.begin(), childPop.end(), (tempPop.begin() + parentPop.size()));
    parentPop.clear();

    // Aplikace elitismu - nejlepsi potomci jsou automaticky zkopirovani do nove populace
    parentPop.resize(Config::GetInstance()->sElitism);
    sort(tempPop.begin(), tempPop.end(), FitnessSort);
    copy(tempPop.begin(), (tempPop.begin() + Config::GetInstance()->sElitism), parentPop.begin());

    // Obnova populace
    Survive(parentPop, tempPop);

    this->actGeneration++;
}

void TSPEvolution::CreateNewIndividuals(tIndividual &parent1, tIndividual &parent2)
{
    tIndividual child1, child2;

    switch(Config::GetInstance()->tCrossover)
    {
        case PMX:
            CrossoverPMX(parent1, parent2, child1);  // krizeni rodicu -> vznik potomku
            Mutation(child1);            // mutace potomku
            childPop.push_back(child1);  // zarazeni novych jedincu do populace potomku
            break;
        case ERX:
            CrossoverERX(parent1, parent2, child1);
            Mutation(child1);
            childPop.push_back(child1);
            break;
        case OX:
            CrossoverOX(parent1, parent2, child1);
            Mutation(child1);
            childPop.push_back(child1);
            break;
        case CX:
            CrossoverCX(parent1, parent2, child1, child2);
            Mutation(child1);
            Mutation(child2);
            childPop.push_back(child1);
            childPop.push_back(child2);
            break;
    }
}

void TSPEvolution::Mutation(tIndividual &individual)
{
    double randomValue = (double)rand() / RAND_MAX;

    if(randomValue < Config::GetInstance()->pMutation)
    {   // mutace je provadena pouze s urcitou pravdepodobnosti
        switch(Config::GetInstance()->tMutation)
        {
            case SWAP:
                MutationSwap(individual);
                break;
            case INVERSION:
                MutationInversion(individual);
                break;
            case OPT_2:
                MutationOpt2(individual);
                break;
            case OPT_3:
                MutationOpt3(individual);
                break;
        }
    }
}

void TSPEvolution::MutationSwap(tIndividual &individual)
{
    // Nahodne vygenerovani indexu dvou mest, ve kterych dojde k zamene
    int randCity1 = rand() % info.dimension;
    int randCity2 = rand() % info.dimension;
    int tempCity;

    // Zamena vybranych mest
    tempCity = individual.genome[randCity1];
    individual.genome[randCity1] = individual.genome[randCity2];
    individual.genome[randCity2] = tempCity;

    result.mutationCount++;
}

void TSPEvolution::MutationInversion(tIndividual &individual)
{
    // Nahodne vygenerovani indexu dvou mest, mezi kterymi bude provedena inverze
    int randCity1 = rand() % info.dimension;
    int randCity2 = rand() % info.dimension;
    int tempCity;
    if(randCity1 > randCity2)
    {
        tempCity = randCity1;
        randCity1 = randCity2;
        randCity2 = tempCity;
    }

    // Inverze mezi nahodne vygenerovanymi body
    vector<int> dummyVector;
    dummyVector.resize(randCity2 - randCity1 + 1);
    copy(&(individual.genome[randCity1]), &(individual.genome[randCity2])+1, dummyVector.begin());
    reverse(dummyVector.begin(), dummyVector.end());
    individual.genome.erase((individual.genome.begin() + randCity1), (individual.genome.begin() + randCity2 + 1));
    individual.genome.insert((individual.genome.begin() + randCity1), dummyVector.begin(), dummyVector.end());

    result.mutationCount++;
}

void TSPEvolution::MutationOpt2(tIndividual &individual)
{
    // Nahodne vygenerovani indexu dvou mest, mezi bude provedena 2-zamena
    int randCity1, randCity2;
    do
    {
        randCity1 = rand() % info.dimension;
        randCity2 = rand() % info.dimension;
    } while(randCity1 == randCity2);

    // Vytvoreni pomocneho vektoru s obracenym poradim mest vybraneho useku
    vector<int> dummyVector;
    int actIndex = randCity2;
    do
    {
        dummyVector.push_back(individual.genome[actIndex]);
        actIndex--;
        actIndex = (actIndex + info.dimension) % (info.dimension);
    }
    while(actIndex != randCity1);

    // Inverze poradi mezi naslednikem 1.bodu 2-vymeny a 2.bodem 2-vymeny
    if(randCity2 > randCity1)
    {
        individual.genome.erase ((individual.genome.begin() + randCity1 + 1), (individual.genome.begin() + randCity2 + 1));
        individual.genome.insert((individual.genome.begin() + randCity1 + 1), dummyVector.begin(), dummyVector.end());
    }
    else
    {
        individual.genome.erase ((individual.genome.begin() + randCity1+1), individual.genome.end());
        individual.genome.insert((individual.genome.begin() + randCity1 + 1), dummyVector.begin(), (dummyVector.begin() + info.dimension - randCity1 - 1));
        individual.genome.erase (individual.genome.begin(), (individual.genome.begin() + randCity2 + 1));
        individual.genome.insert(individual.genome.begin(), dummyVector.begin() + info.dimension - 1 - randCity1, dummyVector.end());
    }

    result.mutationCount++;
}

void TSPEvolution::MutationOpt3(tIndividual &individual)
{
    // Nahodne vygenerovani indexu tri ruznych mest tak, aby platila nerovnost randCity1 < randCity2 < randCity3
    int randCity1, randCity2, randCity3;
    while(true)
    {
        randCity1 = rand() % info.dimension;
        randCity2 = rand() % info.dimension;
        randCity3 = rand() % info.dimension;
        if((randCity1 < randCity3) && (randCity2 > randCity1) && (randCity2 < randCity3))
            break;
        if((randCity1 > randCity3) && ((randCity2 > randCity1) || (randCity2 < randCity3)))
            break;
    }

    // Naplneni pomocneho vektoru s obracenym poradim mest vybraneho useku mezi naslednikem 1.bodu 3-vymeny a 2.bodem 3-vymeny
    vector<int> dummyVector;
    int actIndex = randCity2;
    do
    {
        dummyVector.push_back(individual.genome[actIndex]);
        actIndex--;
        actIndex = (actIndex + info.dimension) % (info.dimension);
    }
    while(actIndex != randCity1);

    // Inverze poradi mezi naslednikem 1.bodu 3-vymeny a 2.bodem 3-vymeny
    if(randCity2 > randCity1)
    {
        individual.genome.erase ((individual.genome.begin() + randCity1 + 1), (individual.genome.begin() + randCity2 + 1));
        individual.genome.insert((individual.genome.begin() + randCity1 + 1), dummyVector.begin(), dummyVector.end());
    }
    else
    {
        individual.genome.erase ((individual.genome.begin() + randCity1+1), individual.genome.end());
        individual.genome.insert((individual.genome.begin() + randCity1 + 1), dummyVector.begin(), (dummyVector.begin() + info.dimension - randCity1 - 1));
        individual.genome.erase (individual.genome.begin(), (individual.genome.begin() + randCity2 + 1));
        individual.genome.insert(individual.genome.begin(), dummyVector.begin() + info.dimension - 1 - randCity1, dummyVector.end());
    }

    // Naplneni pomocneho vektoru s obracenym poradim mest vybraneho useku mezi naslednikem 2.bodu 3-vymeny a 3.bodem 3-vymeny
    dummyVector.clear();
    actIndex = randCity3;
    do
    {
        dummyVector.push_back(individual.genome[actIndex]);
        actIndex--;
        actIndex = (actIndex + info.dimension) % (info.dimension);
    }
    while(actIndex != randCity2);

    // Inverze poradi mezi naslednikem 2.bodu 3-vymeny a 3.bodem 3-vymeny
    if(randCity3 > randCity2)
    {
        individual.genome.erase ((individual.genome.begin() + randCity2 + 1), (individual.genome.begin() + randCity3 + 1));
        individual.genome.insert((individual.genome.begin() + randCity2 + 1), dummyVector.begin(), dummyVector.end());
    }
    else
    {
        individual.genome.erase ((individual.genome.begin() + randCity2+1), individual.genome.end());
        individual.genome.insert((individual.genome.begin() + randCity2 + 1), dummyVector.begin(), (dummyVector.begin() + info.dimension - randCity2 - 1));
        individual.genome.erase (individual.genome.begin(), (individual.genome.begin() + randCity3 + 1));
        individual.genome.insert(individual.genome.begin(), dummyVector.begin() + info.dimension - 1 - randCity2, dummyVector.end());
    }

    result.mutationCount++;
}

void TSPEvolution::CrossoverERX(tIndividual &parent1, tIndividual &parent2, tIndividual &child)
{
    // Inicializace hranove tabulky
    vector<tEdgeTable> edgeTable(info.dimension);
    for(int i = 0; i < info.dimension; i++)
        edgeTable[i].cityIndex = i;

    int k, leftNeighbour1, leftNeighbour2, rightNeighbour1, rightNeighbour2;
    vector<int>::iterator it;

    // Vytvoreni hranove tabulky
    for(int i = 0; i < info.dimension; i++)
    {   // hranova tabulka musi obsahovat vsechna mesta
        leftNeighbour1  = parent1.genome[(i-1+info.dimension)%info.dimension];  // sousedi aktualniho mesta v obou rodicovskych genomech
        rightNeighbour1 = parent1.genome[(i+1)%info.dimension];
        leftNeighbour2  = parent2.genome[(i-1+info.dimension)%info.dimension];
        rightNeighbour2 = parent2.genome[(i+1)%info.dimension];

        // Nastaveni leveho souseda aktualniho mesta prvniho rodicovskeho genomu
        k = parent1.genome[i];
        it = find(edgeTable[k].neighbourCity.begin(), edgeTable[k].neighbourCity.end(), leftNeighbour1);
        if((it == edgeTable[k].neighbourCity.end()) && ((edgeTable[k].neighbourCity.empty()) || (edgeTable[k].neighbourCity[edgeTable[k].neighbourCity.size()-1] != leftNeighbour1)))
            edgeTable[k].neighbourCity.push_back(leftNeighbour1);

        // Nastaveni praveho souseda aktualniho mesta prvniho rodicovskeho genomu
        it = find(edgeTable[k].neighbourCity.begin(), edgeTable[k].neighbourCity.end(), rightNeighbour1);
        if((it == edgeTable[k].neighbourCity.end()) && ((edgeTable[k].neighbourCity.empty()) || (edgeTable[k].neighbourCity[edgeTable[k].neighbourCity.size()-1] != rightNeighbour1)))
            edgeTable[k].neighbourCity.push_back(rightNeighbour1);

        // Nastaveni leveho souseda aktualniho mesta druheho rodicovskeho genomu
        k = parent2.genome[i];
        it =find(edgeTable[k].neighbourCity.begin(), edgeTable[k].neighbourCity.end(), leftNeighbour2);
        if((it == edgeTable[k].neighbourCity.end()) && ((edgeTable[k].neighbourCity.empty()) || (edgeTable[k].neighbourCity[edgeTable[k].neighbourCity.size()-1] != leftNeighbour2)))
            edgeTable[k].neighbourCity.push_back(leftNeighbour2);

        // Nastaveni praveho souseda aktualniho mesta druheho rodicovskeho genomu
        it = find(edgeTable[k].neighbourCity.begin(), edgeTable[k].neighbourCity.end(), rightNeighbour2);
        if((it == edgeTable[k].neighbourCity.end()) && ((edgeTable[k].neighbourCity.empty()) || (edgeTable[k].neighbourCity[edgeTable[k].neighbourCity.size()-1] != rightNeighbour2)))
            edgeTable[k].neighbourCity.push_back(rightNeighbour2);
    }

    // Vlozeni prvniho mesta do genomu potomka (nahodne se vybere prvni mesto bud z prvniho nebo z druheho rodice)
    int actCity = ((rand() % 2) == 0) ? parent1.genome[0] : parent2.genome[0];
    child.genome.push_back(actCity);

    // Smazani prvniho mesta ze seznamu sousedu ostatnich mest
    for(int i = 0; i < (int)edgeTable.size(); i++)
    {
        for(int j = (edgeTable[i].neighbourCity.size() - 1); j >=0; j--)
        {
            if(edgeTable[i].neighbourCity[j] == actCity)
            {
                edgeTable[i].neighbourCity.erase(edgeTable[i].neighbourCity.begin() + j);
                break;
            }
        }
    }

    // Pomucka pro urceni indexu mesta po mazani ruznych sousedu
    int convertor[info.dimension];
    for(int i = 0; i < info.dimension; i++)
        convertor[i] = i;

    // Doplneni dalsich mest do genomu potomka
    int minNeighbourCount, neighbourIndex, isNeighbour;
    vector<int> edgeTableOption;
    while((int)child.genome.size() < info.dimension)
    {
        isNeighbour = !edgeTable[actCity].neighbourCity.empty();  // pomucka pro urceni, ze uzel jiz nema zadneho souseda

        // Nalezeni sousedu aktualniho mesta s nejmensim poctem jinych sousedu
        minNeighbourCount = INT_MAX;
        for(int i = 0; i < (int)edgeTable[actCity].neighbourCity.size(); i++)
        {
            neighbourIndex = convertor[edgeTable[actCity].neighbourCity[i]];
            if((int)edgeTable[neighbourIndex].neighbourCity.size() < minNeighbourCount)
            {
                if(neighbourIndex > actCity)
                    neighbourIndex--;
                edgeTableOption.clear();
                edgeTableOption.push_back(neighbourIndex);
                minNeighbourCount = edgeTable[neighbourIndex].neighbourCity.size();
            }
            else if((int)edgeTable[neighbourIndex].neighbourCity.size() == minNeighbourCount)
            {
                if(neighbourIndex > actCity)
                    neighbourIndex--;
                edgeTableOption.push_back(neighbourIndex);
            }
        }
        for(int i = info.dimension - 1; i > edgeTable[actCity].cityIndex; i--)
            convertor[i]--;

        // Smazani predchoziho aktualniho mesta
        edgeTable.erase(edgeTable.begin() + actCity);

        // Urceni noveho aktualniho mesta
        if(!isNeighbour)  // pokud dosavadni mesto nema zadneho souseda, je vybran nahodny, jeste neprirazany, uzel z tabulky
            actCity = rand() % edgeTable.size();
        else              // jinak je vybrano mesto, ktery ma nejmensi pocet sousedu (je-li jich vice, bere se nahodne jeden z nich)
            actCity = edgeTableOption[rand() % edgeTableOption.size()];

        // Vybrane mesto se umisti do genomu potomka
        child.genome.push_back(edgeTable[actCity].cityIndex);

        // Smazani noveho aktualniho mesta ze seznamu sousedu ostatnich mest
        for(int i = 0; i < (int)edgeTable.size(); i++)
        {
            int len = edgeTable[i].neighbourCity.size() - 1;
            for(int j = len; j >=0; j--)
            {
                if(edgeTable[i].neighbourCity[j] == edgeTable[actCity].cityIndex)
                {
                    edgeTable[i].neighbourCity.erase(edgeTable[i].neighbourCity.begin() + j);
                    break;
                }
            }
        }
    }

    result.crossoverCount++;
}

void TSPEvolution::CrossoverOX(tIndividual &parent1, tIndividual &parent2, tIndividual &child)
{
    vector<int> usedCities;

    // Ziskani indexu pro body krizeni
    int crossPoint1 = rand() % info.dimension;
    int crossPoint2 = rand() % info.dimension;
    int tempPoint;
    if(crossPoint1 > crossPoint2)
    {
        tempPoint = crossPoint1;
        crossPoint1 = crossPoint2;
        crossPoint2 = tempPoint;
    }

    // Kopirovani useku vymezeneho krizicimi body geny z genomu rodice 1
    child.genome.resize(info.dimension, -1);
    for(int i = crossPoint1; i <= crossPoint2; i++)
    {
        child.genome[i] = parent1.genome[i];
        usedCities.push_back(child.genome[i]);
    }

    // Doplneni useku mimo krizici body geny z genomu rodice 2 (zacina se za druhym krizicim bodem)
    vector<int>::iterator it;
    int indexChild  = (crossPoint2 + 1) % info.dimension;
    int indexParent = (crossPoint2 + 1) % info.dimension;
    while((int)usedCities.size() != info.dimension)
    {   // dokud neni vyplneny cely genom potomka
        it = find(usedCities.begin(), usedCities.end(), parent2.genome[indexParent]);
        if((it == usedCities.end()) && (usedCities[usedCities.size()-1] != parent2.genome[indexParent]))
        {   // mesto zatim neni u potomka pouzito - pouzijeme jej
            child.genome[indexChild] = parent2.genome[indexParent];
            usedCities.push_back(parent2.genome[indexParent]);
            indexChild = (indexChild + 1) % info.dimension;
        }

        indexParent = (indexParent + 1) % info.dimension;
    }

    result.crossoverCount++;
}

void TSPEvolution::CrossoverCX(tIndividual &parent1, tIndividual &parent2, tIndividual &child1, tIndividual &child2)
{
    vector<int> usedCities;
    int indexParent1, indexParent2;
    int valueParent2;
    indexParent1 = indexParent2 = 0;
    child1.genome.resize(info.dimension, -1);
    child2.genome.resize(info.dimension, -1);

    vector<tCitySection> sectionParent;
    bool foundUnusedCity;
    int freeCityIndex;
    while((int)usedCities.size() != info.dimension)
    {
        // Nalezeni mesta, ktere jeste neni v zadne sekci
        for(freeCityIndex = 0; freeCityIndex < info.dimension; freeCityIndex++)
        {
            foundUnusedCity = true;
            for(int j = 0; j < (int)usedCities.size(); j++)
            {
                if(freeCityIndex == usedCities[j])
                {
                    foundUnusedCity = false;
                    break;
                }
            }
            if(foundUnusedCity)
                break;
        }

        // Rozdeleni genomu na sekce
        indexParent1 = freeCityIndex;
        tCitySection section;
        do
        {
            valueParent2 = parent2.genome[indexParent1];

            for(indexParent1 = 0; indexParent1 < (int)parent1.genome.size(); indexParent1++)
                if(parent1.genome[indexParent1] == valueParent2)
                    break;

            usedCities.push_back(indexParent1);
            section.push_back(indexParent1);
        }
        while(indexParent1 != freeCityIndex);

        sectionParent.push_back(section);
    }

    // Vytvoreni potomku vymenou rodicovskych sekci useku genomu
    int dummy = 0;
    for(int i = 0; i < (int)sectionParent.size(); i++)
    {
        if(sectionParent[i].size() == 1)
            dummy++;

        for(int j = 0; j < (int)sectionParent[i].size(); j++)
        {
            if(((i + dummy) % 2) == 0)
            {
                child1.genome[sectionParent[i][j]] = parent1.genome[sectionParent[i][j]];
                child2.genome[sectionParent[i][j]] = parent2.genome[sectionParent[i][j]];
            }
            else
            {
                child1.genome[sectionParent[i][j]] = parent2.genome[sectionParent[i][j]];
                child2.genome[sectionParent[i][j]] = parent1.genome[sectionParent[i][j]];
            }
        }
    }

    result.crossoverCount++;
}


void TSPEvolution::CrossoverPMX(tIndividual &parent1, tIndividual &parent2, tIndividual &child)
{
    // Ziskani indexu pro body krizeni
    int crossPoint1 = rand() % info.dimension;
    int crossPoint2 = rand() % info.dimension;

    int tempPoint;
    if(crossPoint1 > crossPoint2)
    {
        tempPoint = crossPoint1;
        crossPoint1 = crossPoint2;
        crossPoint2 = tempPoint;
    }

    // Vymena podretezcu urcena body krizeni
    child.genome.resize(info.dimension, -1);
    for(int i = crossPoint1; i <= crossPoint2; i++)
    {
        child.genome[i] = parent1.genome[i];      // zamena prostredni casti mezi krizicimi body
    }

    // Aktualni gen je spojen s protilehlym genem a s genem se stejnou hodnotou v pomocnem chromozomu
    int temp;
    tIndividual parent = parent2;
    vector<int> dummyVector;
    for(int i = crossPoint1; i <= crossPoint2; i++)
    {
        for(int j = 0; j < info.dimension; j++)
        {
            if(parent.genome[j] == child.genome[i])
            {
                temp = parent.genome[i];
                parent.genome[i] = parent.genome[j];
                parent.genome[j] = temp;
                dummyVector.push_back(i);
                dummyVector.push_back(j);
                break;
            }
        }
    }

    // Na potomka jsou zkopirovany pouze ty geny z pomocneho chromozomu, ktere jsou mimo body krizeni
    for(int i = 0; i < (int)dummyVector.size(); i++)
        if((dummyVector[i] < crossPoint1) || (dummyVector[i] > crossPoint2))
            child.genome[dummyVector[i]] = parent.genome[dummyVector[i]];

    // Zbyla mista doplnime podle templatu - druheho rodicovskeho chromozomu
    for(int i = crossPoint2 + 1; i < info.dimension; i++)
        if(child.genome[i] == -1)
            child.genome[i] = parent2.genome[i];
    for(int i = 0; i < crossPoint1; i++)
        if(child.genome[i] == -1)
            child.genome[i] = parent2.genome[i];

    result.crossoverCount++;
}

void TSPEvolution::Survive(vector<tIndividual> &population, vector<tIndividual> &selectedPopulation)
{
    int championIndex;
    while((int)population.size() < Config::GetInstance()->sPopulation)
    {
        championIndex = TournamentSelection(selectedPopulation);
        population.push_back(selectedPopulation[championIndex]);
        //selectedPopulation.erase(selectedPopulation.begin() + championIndex);
    }
}

int TSPEvolution::TournamentSelection(vector<tIndividual> &population)
{
    tIndividual champion;

    // Nahodny vyber jednotlivcu z populace (ucastniku "turnaje")
    int randIndex, minIndex = 0;
    float minPrice = FLT_MAX;

    for(int i = 0; i < Config::GetInstance()->sTournament; i++)
    {
        randIndex = rand() % population.size();
        if(population[randIndex].price < minPrice)
        {
            minPrice = population[randIndex].price;
            minIndex = randIndex;
        }
    }

    return minIndex;
}

bool TSPEvolution::SaveResults(string outputFile)
{
    ofstream file;
    file.open(outputFile.c_str());

    if(!file.good())
        return false;

    // Vypocet prumerne ceny za vsechny generace
    float multConstant = (float)1 / ((float)actGeneration / Config::GetInstance()->logInterval);
    for(int i = 0; i < (int)tempAvgPathPrice.size(); i++)
        result.avgPathPrice += tempAvgPathPrice[i] * multConstant;

    file << "***** NALEZENA CESTA *****\n";
    for(int i = 0; i < (int)resultPath.genome.size(); i++)
        file << resultPath.genome[i]  << " ";

    file << "\n\n***** PARAMETRY BEHU GA *****\n";
    file << Config::GetInstance()->problemFile   << "\t # cesta k vstupnimu datovemu souboru\n";
    file << actGeneration                << "\t # aktualni generace behu vypoctu\n";
    file << result.crossoverCount        << "\t # pocet aplikaci operatoru krizeni od inicializace\n";
    file << result.mutationCount         << "\t # pocet aplikaci operatoru mutace od inicializace\n";
    file << result.genomeEvaluationCount << "\t # pocet ohodnoceni genomu od inicializace\n\n";

    file << "***** CELKOVE STATISTIKY *****\n";
    file << result.runningTime.tv_sec << "." << result.runningTime.tv_usec << "s\t # doba behu vypoctu\n";
    file << result.minPathPrice          << "\t # nejlepsi cena cesty od inicializace\n";
    file << result.maxPathPrice          << "\t # nejhorsi cena cesty od inicializace\n";
    file << result.avgPathPrice          << "\t # prumerna cena cesty od inicializace\n\n";

    file << "***** STATISTIKY K POCATECNI POPULACI *****\n";
    file << result.minInitialPathPrice   << "\t # nejlepsi cena cesty v pocatecni populaci\n";
    file << result.maxInitialPathPrice   << "\t # nejhorsi cena cesty v pocatecni populaci\n";
    file << result.avgInitialPathPrice   << "\t # prumerna cena cesty v pocatecni populaci\n\n";

    file << "***** STATISTIKY K AKTUALNI POPULACI *****\n";
    file << result.minCurrentPathPrice   << "\t # nejlepsi cena cesty v aktualni populaci\n";
    file << result.maxCurrentPathPrice   << "\t # nejhorsi cena cesty v aktualni populaci\n";
    file << result.avgCurrentPathPrice   << "\t # prumerna cena cesty v aktualni populaci\n\n";

    file << "***** MEZIVYSLEDKY *****\n";
    for(int i = 0; i < (int)tempMinPathPrice.size(); i++)
    {
        file << (i * Config::GetInstance()->logInterval) << " \t";
        file << tempMinPathPrice[i] << " \t";
        file << tempAvgPathPrice[i] << " \t";
        file << tempMaxPathPrice[i] << "\n";
    }

    file.close();

    return true;
}

bool FitnessSort(tIndividual inv1, tIndividual inv2)
{
    return (inv1.price < inv2.price);
}
