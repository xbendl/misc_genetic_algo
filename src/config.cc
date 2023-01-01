#include "config.h"

using namespace std;

Config::Config()
{
    sPopulation = 100;
    nGenerations = 1000;
    sElitism = 1;
    sTournament = 2;
    logInterval = 10;
    pMutation = 0.1;
    tMutation = OPT_2;
    tCrossover = OX;
}

Config::~Config()
{
    if(configInstance != 0)
        delete configInstance;
}

bool Config::LoadConfiguration(string configFile)
{
    unsigned int cutAt;
    string line, paramName, paramValue;
    ifstream file(configFile.c_str());

    if(!file.good())
    {
        cerr << "Chyba! Konfiguracni soubor nelze otevrit." << endl;
        exit(1);
    }

    while(getline(file, line))
    {
        if((cutAt = line.find_first_of("=")) == line.npos)   // neplatny radek konfiguracniho souboru
            continue;

        paramName = line.substr(0, cutAt);
        paramValue = line.substr(cutAt + 1);
        StringTrim(paramName);
        StringTrim(paramValue);

        if(paramName == "PROBLEM_FILE")
            problemFile = paramValue;
        else if(paramName == "S_POPULATION")
            sPopulation = atoi(paramValue.c_str());
        else if(paramName == "N_GENERATIONS")
            nGenerations = atoi(paramValue.c_str());
        else if(paramName == "P_MUTATION")
            pMutation = atof(paramValue.c_str());
        else if(paramName == "S_TOURNAMENT")
            sTournament = atoi(paramValue.c_str());
        else if(paramName == "S_ELITISM")
            sElitism = atoi(paramValue.c_str());
        if(paramName == "LOG_INTERVAL")
            logInterval = atoi(paramValue.c_str());
        else if(paramName == "T_MUTATION")
        {
            if(paramValue == "OPT_2")
                tMutation = OPT_2;
            else if(paramValue == "OPT_3")
                tMutation = OPT_3;
            else if(paramValue == "SWAP")
                tMutation = SWAP;
        }
        else if(paramName == "T_CROSSOVER")
        {
            if(paramValue == "PMX")
                tCrossover = PMX;
            else if(paramValue == "ERX")
                tCrossover = ERX;
            else if(paramValue == "OX")
                tCrossover = OX;
            else if(paramValue == "CX")
                tCrossover = CX;
        }
    }

    // Osetreni nespravne konfigurace
    if(sPopulation < sElitism)
    {
        cerr << "CHYBA! Pocet jedincu pro elitismus musi byt mensi nez velikost populace." << endl;
        exit(1);
    }

    file.close();

    return true;
}

void StringTrim(string& str)
{
    string::size_type pos1 = str.find_first_not_of(' ');
    string::size_type pos2 = str.find_last_not_of(' ');
    str = str.substr(pos1 == string::npos ? 0 : pos1, pos2 == string::npos ? str.length() - 1 : pos2 - pos1 + 1);
}

Config *Config::configInstance = 0;
