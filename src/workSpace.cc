#include "workSpace.h"
#include "tsp_evolution.h"

WorkSpace::WorkSpace(QWidget *parent) : QDialog(parent)
{
    // Ziskani zadanych parametru z prikazoveho radku
    QStringList arguments = QCoreApplication::arguments();

    qRegisterMetaType<tIndividual>("tIndividual");

    // Globalni nastaveni aplikace
    setFixedSize(818, 435);
    setFont(QFont("Helvetica", 9, QFont::Normal));
    setWindowTitle(tr("TSP a analyza kvality optimalizace"));

    // Sekce "Parametry evoluce"
    QLabel *evolutionParameterLabel = new QLabel(tr("Parametry evoluce:"), this);
    evolutionParameterLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    evolutionParameterLabel->move(10, 10);
    QLabel *taskChooseLabel = new QLabel(tr("Uloha"), this);
    taskChooseLabel->move(10, 45);
    QLabel *sPopulationLabel = new QLabel(tr("Velikost populace"), this);
    sPopulationLabel->move(10, 75);
    QLabel *nGenerationsLabel = new QLabel(tr("Pocet generaci"), this);
    nGenerationsLabel->setGeometry(10, 101, 200, 23);
    QLabel *pMutationLabel = new QLabel(tr("Pravdepodobnost mutace"), this);
    pMutationLabel->move(10, 135);
    QLabel *tMutationLabel = new QLabel(tr("Typ mutace"), this);
    tMutationLabel->move(10, 165);
    QLabel *tCrossoverLabel = new QLabel(tr("Typ krizeni"), this);
    tCrossoverLabel->move(10, 195);
    QLabel *sTournamentLabel = new QLabel(tr("Ucastnici turnaje"), this);
    sTournamentLabel->move(10, 225);
    sTournamentLabel->setToolTip("Pocet jedincu, z kterych se vybira nejlepe ohodnoceny jedinec pro proces reprodukce pri turnajove selekci.");
    sTournamentLabel->setCursor(Qt::WhatsThisCursor);
    QLabel *sElitismLabel = new QLabel(tr("Elitismus"), this);
    sElitismLabel->move(10, 255);
    sElitismLabel->setToolTip("Pocet nejlepe ohodnocenych jedincu populace, ktere automaticky prechazeji do dalsi generace.");
    sElitismLabel->setCursor(Qt::WhatsThisCursor);
    QLabel *logIntervalLabel = new QLabel(tr("Interval logu"), this);
    logIntervalLabel->move(10, 285);
    logIntervalLabel->setToolTip("Pocet generaci, po kterych dojde k zaznamenani aktualnich statistik.");
    logIntervalLabel->setCursor(Qt::WhatsThisCursor);

    loadTaskButton = new QPushButton(tr("Vybrat ulohu"), this);
    loadTaskButton->setGeometry(145, 40, 180, 25);
    sPopulationStepLine = new QSpinBox(this);
    sPopulationStepLine->setRange(1, (INT_MAX - 1));
    sPopulationStepLine->setGeometry(215, 70, 110, 25);
    nGenerationsStepLine = new QSpinBox(this);
    nGenerationsStepLine->setRange(1, (INT_MAX - 1));
    nGenerationsStepLine->setGeometry(215, 100, 110, 25);
    pMutationDoubleSpinBox = new QDoubleSpinBox(this);
    pMutationDoubleSpinBox->setRange(0, 1);
    pMutationDoubleSpinBox->setDecimals(3);
    pMutationDoubleSpinBox->setSingleStep(0.001);
    pMutationDoubleSpinBox->setGeometry(215, 130, 110, 25);
    tMutationTypeComboBox = new QComboBox(this);
    tMutationTypeComboBox->addItem("SWAP");
    tMutationTypeComboBox->addItem("2-OPT");
    tMutationTypeComboBox->addItem("3-OPT");
    tMutationTypeComboBox->setGeometry(215, 160, 110, 25);
    tCrossoverTypeComboBox = new QComboBox(this);
    tCrossoverTypeComboBox->addItem("PMX");
    tCrossoverTypeComboBox->addItem("ERX");
    tCrossoverTypeComboBox->addItem("OX");
    tCrossoverTypeComboBox->addItem("CX");
    tCrossoverTypeComboBox->setGeometry(215, 190, 110, 25);
    sTournamentStepLine = new QSpinBox(this);
    sTournamentStepLine->setRange(1, (INT_MAX - 1));
    sTournamentStepLine->setGeometry(215, 220, 110, 25);
    sElitismStepLine = new QSpinBox(this);
    sElitismStepLine->setRange(0, (INT_MAX - 1));
    sElitismStepLine->setGeometry(215, 250, 110, 25);
    logIntervalStepLine = new QSpinBox(this);
    logIntervalStepLine->setRange(1, (INT_MAX - 1));
    logIntervalStepLine->setGeometry(215, 280, 110, 25);

    evoluteStartButton = new QPushButton(tr("Start"), this);
    evoluteStartButton->setGeometry(10, 312, 73, 25);
    evoluteStartButton->setEnabled(false);
    evoluteStopButton = new QPushButton(tr("Pozastavit"), this);
    evoluteStopButton->setGeometry(88, 312, 73, 25);
    evoluteStopButton->setEnabled(false);
    evoluteAbortButton = new QPushButton(tr("Zrusit"), this);
    evoluteAbortButton->setGeometry(166, 312, 74, 25);
    evoluteAbortButton->setEnabled(false);
    saveResultButton = new QPushButton(tr("Ulozit"), this);
    saveResultButton->setGeometry(245, 312, 80, 25);
    saveResultButton->setEnabled(false);

    // Sekce "Graf Hamiltonovske kruznice"
    QLabel *hamiltonArcLabel = new QLabel(tr("Graf Hamiltonovske kruznice:"), this);
    hamiltonArcLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    hamiltonArcLabel->move(360, 10);
    evolutionProgressBar = new QProgressBar(this);
    evolutionProgressBar->setGeometry(355, 313, 450, 25);
    evolutionProgressBar->setMinimum(0);
    evolutionProgressBar->setTextVisible(false);
    projectStatus = new QLabel(tr("Prubeh vypoctu trasy"), this);
    projectStatus->setStyleSheet("font-weight: bold");
    projectStatus->setGeometry(520, 318, 300, 18);
    resultTextBox = new QPlainTextEdit(this);
    resultTextBox->setGeometry(10, 345, 796, 80);
    resultTextBox->setFont(QFont("Courier New", 8, QFont::Normal));
    resultTextBox->setReadOnly(true);
    mapWidget = new MapWidget(this);
    mapWidget->setGeometry(355, 40, 449, 264);

    // Nastaveni slotu a signalu
    connect(loadTaskButton, SIGNAL(clicked()), this, SLOT(SetOpenFileName()));
    connect(evoluteStartButton, SIGNAL(clicked()), this, SLOT(StartEvolution()));
    connect(evoluteStopButton, SIGNAL(clicked()), this, SLOT(StopEvolution()));
    connect(evoluteAbortButton, SIGNAL(clicked()), this, SLOT(AbortEvolution()));
    connect(saveResultButton, SIGNAL(clicked()), this, SLOT(SetResultFile()));
    connect(sPopulationStepLine, SIGNAL(valueChanged(int)), this, SLOT(ChangeSPopulation(int)));
    connect(nGenerationsStepLine, SIGNAL(valueChanged(int)), this, SLOT(ChangeNGenerations(int)));
    connect(pMutationDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ChangePMutation(double)));
    connect(tMutationTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeTMutation(int)));
    connect(tCrossoverTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeTCrossover(int)));
    connect(sElitismStepLine, SIGNAL(valueChanged(int)), this, SLOT(ChangeSElitism(int)));
    connect(sTournamentStepLine, SIGNAL(valueChanged(int)), this, SLOT(ChangeSTournament(int)));
    connect(logIntervalStepLine, SIGNAL(valueChanged(int)), this, SLOT(ChangeLogInterval(int)));
    connect(&tspEvolution, SIGNAL(CurrentPathGenerated(tIndividual, int, float, float, float)), this, SLOT(ReceiveCurrentPathGenerated(tIndividual, int, float, float, float)));
    connect(&tspEvolution, SIGNAL(PathGenerated(tIndividual)), this, SLOT(ReceivePathGenerated(tIndividual)));

    // Zpracovani parametru prikazove radky
    if(arguments.size() == 2)
    {
        // Nacteni udaju z konfiguracniho souboru
        string configFile = arguments[1].toStdString();
        Config::GetInstance()->LoadConfiguration(configFile);

        // Nacteni konkretni ulohy
        if(LoadData())
        {
            mapWidget->SetNewMap(cityPosition);
            evolutionProgressBar->setValue(evolutionProgressBar->minimum());
            evoluteStartButton->setEnabled(true);
            evoluteStopButton->setText("Pozastavit");
            update();
        }
        else
        {
            evoluteStartButton->setEnabled(false);
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Vstupni soubor nelze nalezt nebo neobsahuje pozadovane udaje."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
    }

    // Nastaveni hodnot ovladacich prvku
    sPopulationStepLine->setValue(Config::GetInstance()->sPopulation);
    nGenerationsStepLine->setValue(Config::GetInstance()->nGenerations);
    sTournamentStepLine->setValue(Config::GetInstance()->sTournament);
    sElitismStepLine->setValue(Config::GetInstance()->sElitism);
    logIntervalStepLine->setValue(Config::GetInstance()->logInterval);
    pMutationDoubleSpinBox->setValue(Config::GetInstance()->pMutation);
    tCrossoverTypeComboBox->setCurrentIndex(Config::GetInstance()->tCrossover);
    tMutationTypeComboBox->setCurrentIndex(Config::GetInstance()->tMutation);
}

bool WorkSpace::LoadData()
{
    cityPosition.clear();
    cityMatrix.clear();

    unsigned cutAt, actCityIndex;
    bool nodeCoordSection = false;
    string line, paramName, paramValue;
    ifstream file(Config::GetInstance()->problemFile.c_str());

    if(!file.good())
    {
        cerr << "Chyba! Vstupni soubor s daty nelze otevrit." << endl;
        exit(1);
    }

    // Ziskani parametru evoluce a ziskani souradnic mest
    while(getline(file, line))
    {
        if(line.find("EOF") != line.npos)
        {   // ukonceni nacitani, je-li na radku "EOF" (nektere soubory formatu TSPLib tak cini)
            break;
        }
        else if(line.find("NODE_COORD_SECTION") != line.npos)
        {   // zacatek sekce s vyznacenim souradnic mest
            nodeCoordSection = true;
            continue;
        }
        else if(nodeCoordSection)
        {   // parsovani souradnic mest
            actCityIndex = atoi(line.substr(0, line.find_first_of(" ")).c_str());
            line.erase(0, line.find_first_of(" ") + 1);
            cityPosition[actCityIndex - 1].x = atoi(line.substr(0, line.find_first_of(" ")).c_str());
            line.erase(0, line.find_first_of(" ") + 1);
            cityPosition[actCityIndex - 1].y = atoi(line.substr(0, line.find_first_of(" ")).c_str());
            continue;
        }
        else if((cutAt = line.find_first_of(":")) == line.npos)
        {   // neplatny radek konfiguracniho souboru
            continue;
        }

        paramName = line.substr(0, cutAt);
        paramValue = line.substr(cutAt + 1);
        StringTrim(paramName);
        StringTrim(paramValue);

        if(paramName.find("NAME") != paramName.npos)
            info.name = paramValue;
        else if(paramName.find("COMMENT") != paramName.npos)
            info.comment = paramValue;
        else if(paramName.find("DIMENSION") != paramName.npos)
        {
            info.dimension = atoi(paramValue.c_str());

            // Vytvoreni pole souradnic pozice mest na mape
            cityPosition.resize(info.dimension);

            // Vytvoreni matice propojeni mest
            for(int i = 0; i < info.dimension; i++)
            {
                CityMatrixRow row;
                row.resize(info.dimension);
                cityMatrix.push_back(row);
            }
        }
    }

    // Vypocet vzajemnych vzdalenosti mest
    float dX, dY;
    for(int i = 0; i < info.dimension; i++)
    {
        for(int j = 0; j < info.dimension; j++)
        {
            dX = cityPosition[i].x - cityPosition[j].x;
            dY = cityPosition[i].y - cityPosition[j].y;
            cityMatrix[i][j] = sqrt(dX * dX + dY * dY);
        }
    }
    file.close();

    // Vlozeni informaci ziskanych ze souboru do resultBoxu

    string nameText;
    nameText = "<strong><u>Uloha: " + info.name + "<u></strong><br />";
    stringstream out;
    out << info.dimension;
    string commentText = "<strong>Komentar: \t</strong>" + info.comment + "<br />";
    string dimensionText = "<strong>Dimeze: &nbsp;&nbsp;</strong>" + out.str() + "";
    string header = nameText + commentText + dimensionText;
    QString qHeader(header.c_str());
    resultTextBox->appendHtml(qHeader);

    return true;
}

void WorkSpace::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setPen(QPen(Qt::gray, 1));
    painter.drawLine(340, 40, 340, 335);
}

void WorkSpace::SetOpenFileName()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Vyberte ulohu"), "", tr("Vsechny soubory (*);;TSP soubory (*.tsp)"));

    if(!fileName.isEmpty())
    {
        if(fileName.startsWith(QApplication::applicationDirPath(), Qt::CaseSensitive))
        {   // Zmena absolutni cesty na relativni
            fileName = fileName.right(fileName.size() - QApplication::applicationDirPath().size() - 1);
        }

        Config::GetInstance()->problemFile = fileName.toStdString();
        resultTextBox->clear();
        if(LoadData())
        {
            mapWidget->SetNewMap(cityPosition);
            evolutionProgressBar->setValue(evolutionProgressBar->minimum());
            evoluteStartButton->setEnabled(true);
            evoluteStopButton->setText("Pozastavit");
            update();
        }
        else
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Vstupni soubor nelze nalezt nebo neobsahuje pozadovane udaje."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
    }
}

void WorkSpace::SetResultFile()
{
    std::string outputFile= (QFileDialog::getSaveFileName(this, tr("Vyberte soubor pro ulozeni vysledky"), "", tr("Vsechny soubory (*);;SP soubory (*.tsp)"))).toStdString();

    if(!outputFile.empty())
    {
        if(!(tspEvolution.SaveResults(outputFile)))
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Ulozeni vysledku do vystupniho souboru se nezdarilo.\nMozne priciny: Evoluce nebyla dokoncena."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
    }
}

void WorkSpace::StartEvolution()
{
    // Odemknuti / zamknuti ovladacich prvku
    loadTaskButton->setEnabled(false);
    evoluteStartButton->setEnabled(false);
    evoluteStopButton->setEnabled(true);
    evoluteAbortButton->setEnabled(true);
    saveResultButton->setEnabled(false);
    tCrossoverTypeComboBox->setEnabled(false);
    tMutationTypeComboBox->setEnabled(false);
    sPopulationStepLine->setEnabled(false);
    nGenerationsStepLine->setEnabled(false);
    sTournamentStepLine->setEnabled(false);
    sElitismStepLine->setEnabled(false);
    logIntervalStepLine->setEnabled(false);
    pMutationDoubleSpinBox->setEnabled(false);

    // Nastaveni progressBaru
    evolutionProgressBar->setValue(evolutionProgressBar->minimum());
    evolutionProgressBar->setMaximum(Config::GetInstance()->nGenerations);

    // Spusteni evoluce
    tspEvolution.FindSolution(cityMatrix, info);

    resultTextBox->appendHtml(QString("<strong>_______________________________________________</strong>"));
    resultTextBox->appendHtml(QString("<strong>Mezivysledky (actGeneration, Min, Avg, Max</strong>)"));

    mapWidget->StartTimer();
}

void WorkSpace::StopEvolution()
{
    if(evoluteStopButton->text() == "Pozastavit")   // Stridani textu na buttonu Pozastavit / Obnovit
    {
        evoluteStopButton->setText("Obnovit");
        mapWidget->KillTimer(false);
    }
    else
    {
        evoluteStopButton->setText("Pozastavit");
        mapWidget->StartTimer();
    }

    tspEvolution.StopEvolution();    // Pokyn k pozastaveni evoluce
}

void WorkSpace::AbortEvolution()
{
    tspEvolution.AbortEvolution();    // Pokyn k zastaveni evoluce

    // Vypis nalezene trasy a jeji ceny do textoveho pole pro vysledky
    resultTextBox->appendHtml(QString("<strong>_______________________________________________</strong>"));
    string path = "<strong>Vysledna trasa: </strong>";
    for(int i = 0; i < (int)resultPath.genome.size(); i++)
    {
        stringstream out;
        out << resultPath.genome[i];
        path += out.str() + " ";
    }
    QString qPath(path.c_str());
    resultTextBox->appendHtml(qPath);

    string price = "<strong>Cena vysledne trasy: </strong>";
    stringstream out;
    out << resultPath.price;
    price += out.str();
    QString qPrice(price.c_str());
    resultTextBox->appendHtml(qPrice);

    // Odemknuti ovladacich prvku
    loadTaskButton->setEnabled(true);
    evoluteStartButton->setEnabled(true);
    evoluteStopButton->setEnabled(false);
    evoluteAbortButton->setEnabled(false);
    saveResultButton->setEnabled(true);
    evoluteStopButton->setText(QString("Pozastavit"));
    tCrossoverTypeComboBox->setEnabled(true);
    tMutationTypeComboBox->setEnabled(true);
    sPopulationStepLine->setEnabled(true);
    nGenerationsStepLine->setEnabled(true);
    sTournamentStepLine->setEnabled(true);
    sElitismStepLine->setEnabled(true);
    logIntervalStepLine->setEnabled(true);
    pMutationDoubleSpinBox->setEnabled(true);

    mapWidget->KillTimer(true);
}

void WorkSpace::ChangeSPopulation(int newValue)
{
    Config::GetInstance()->sPopulation = newValue;
}

void WorkSpace::ChangeNGenerations(int newValue)
{
    Config::GetInstance()->nGenerations = newValue;
}

void WorkSpace::ChangePMutation(double newValue)
{
    Config::GetInstance()->pMutation = newValue;
}

void WorkSpace::ChangeTMutation(int newValue)
{
    Config::GetInstance()->tMutation = (E_MUTATION)newValue;
}

void WorkSpace::ChangeTCrossover(int newValue)
{
    Config::GetInstance()->tCrossover = (E_CROSSOVER)newValue;
}

void WorkSpace::ChangeSElitism(int newValue)
{
    Config::GetInstance()->sElitism = newValue;
}

void WorkSpace::ChangeSTournament(int newValue)
{
    Config::GetInstance()->sTournament = newValue;
}

void WorkSpace::ChangeLogInterval(int newValue)
{
    Config::GetInstance()->logInterval = newValue;
}

void WorkSpace::ReceiveCurrentPathGenerated(tIndividual resultPath, int actGeneration, float minCurrentPathPrice, float avgCurrentPathPrice, float maxCurrentPathPrice)
{
    mapWidget->SetBestIndividual(resultPath);

    this->resultPath = resultPath;

    // Vypis mezivysledku (minimalni, prumerna a maximalni cena v ramci dane generace) do textoveho pole pro vysledky
    stringstream out1, out2, out3, out4;
    out1 << std::fixed << std::setprecision(4) << actGeneration;
    out2 << std::fixed << std::setprecision(4) << minCurrentPathPrice;
    out3 << std::fixed << std::setprecision(4) << avgCurrentPathPrice;
    out4 << std::fixed << std::setprecision(4) << maxCurrentPathPrice;
    string row = out1.str() + "&nbsp;&nbsp;&nbsp;&nbsp;" + out2.str() + "&nbsp;&nbsp;&nbsp;&nbsp;" + out3.str() + "&nbsp;&nbsp;&nbsp;&nbsp;" + out4.str();
    QString qRow(row.c_str());
    resultTextBox->appendHtml(qRow);

    evolutionProgressBar->setValue(actGeneration);
}

void WorkSpace::ReceivePathGenerated(tIndividual resultPath)
{
    this->resultPath = resultPath;

    mapWidget->SetBestIndividual(resultPath);

    // Vypis nalezene trasy a jeji ceny do textoveho pole pro vysledky
    resultTextBox->appendHtml(QString("<strong>_______________________________________________</strong>"));
    string path = "<strong>Vysledna trasa: </strong>";
    for(int i = 0; i < (int)resultPath.genome.size(); i++)
    {
        stringstream out;
        out << resultPath.genome[i];
        path += out.str() + " ";
    }
    QString qPath(path.c_str());
    resultTextBox->appendHtml(qPath);

    string price = "<strong>Cena vysledne trasy: </strong>";
    stringstream out;
    out << resultPath.price;
    price += out.str();
    QString qPrice(price.c_str());
    resultTextBox->appendHtml(qPrice);

    // Odemknuti / zamknuti ovladacich prvku
    loadTaskButton->setEnabled(true);
    evoluteStartButton->setEnabled(true);
    evoluteStopButton->setEnabled(false);
    evoluteAbortButton->setEnabled(false);
    saveResultButton->setEnabled(true);
    evoluteStopButton->setText(QString("Pozastavit"));

    mapWidget->KillTimer(true);
}
