#include "mapWidget.h"

MapWidget::MapWidget(QWidget *parent) : QWidget(parent)
{
    // Globalni nastaveni
    setAutoFillBackground(true);
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);

    // Nastaveni casovace
    timerInterval = 250;
    timerId = -1;
    timerRunning = false;
}

void MapWidget::SetNewMap(vector<CityPosition> &cityPosition)
{
    this->cityPosition.clear();
    tempBestIndividual.genome.clear();
    bestIndividual.genome.clear();

    int maxPositionX = 0, maxPositionY = 0;
    int minPositionX = INT_MAX, minPositionY = INT_MAX;
    float multConstantX = 0, multConstantY = 0;

    // Vypocet konstant pro prepocitani souradnic mest (aby se vesly do vymezeneho rastru)
    for(int i = 0; i < (int)cityPosition.size(); i++)
    {
        if(cityPosition[i].x > maxPositionX)
            maxPositionX = cityPosition[i].x;
        if(cityPosition[i].x < minPositionX)
            minPositionX = cityPosition[i].x;
        if(cityPosition[i].y > maxPositionY)
            maxPositionY = cityPosition[i].y;
        if(cityPosition[i].y < minPositionY)
            minPositionY = cityPosition[i].y;
    }
    multConstantX = ((double) (this->width() - 12)) / (maxPositionX - minPositionX);
    multConstantY = ((double) (this->height() - 12)) / (maxPositionY - minPositionY);

    // Prepocitani souradnic mest
    CityPosition actCity;
    for(int i = 0; i < (int)cityPosition.size(); i++)
    {
        cityPosition[i].x -= minPositionX;
        cityPosition[i].y -= minPositionY;
        actCity.x = ceil(multConstantX * cityPosition[i].x) + 6;
        actCity.y = ceil(multConstantY * cityPosition[i].y) + 6;
        this->cityPosition.push_back(actCity);
    }

    update();
}

void MapWidget::SetBestIndividual(tIndividual bestIndividual)
{
    mutex.lock();
    tempBestIndividual = bestIndividual;
    mutex.unlock();
}

void MapWidget::paintEvent(QPaintEvent * /* event */)
{
    mutex.lock();
    bestIndividual = tempBestIndividual;
    mutex.unlock();

    QPainter painter(this);

    // Vykresleni okraju
    painter.setPen(Qt::black);
    painter.setBrush(Qt::white);
    painter.drawRect(0, 0, (this->width() - 1), (this->height() - 1));

    // Vykresleni cesty mezi mesty
    painter.setPen(Qt::red);
    if(!bestIndividual.genome.empty())
    {
        int i1, i2;
        for(int i = 0; i < (int)bestIndividual.genome.size(); i++)
        {
            i1 = bestIndividual.genome[i%bestIndividual.genome.size()];
            i2 = bestIndividual.genome[(i+1)%bestIndividual.genome.size()];
            painter.drawLine(cityPosition[i1].x, cityPosition[i1].y, cityPosition[i2].x, cityPosition[i2].y);
        }
    }

    // Vykresleni mest
    painter.setBrush(Qt::black);
    painter.setPen(Qt::black);
    if(!cityPosition.empty())
    {
        for(int i = 0; i < (int)cityPosition.size(); i++)
        {
            painter.drawPie(QRect((cityPosition[i].x - 1), (cityPosition[i].y - 1), 2, 2), 0, (16 * 360));
        }
    }
}

void MapWidget::timerEvent(QTimerEvent *)
{
    update();

    if(!timerRunning)
        killTimer(timerId);
}

void MapWidget::StartTimer()
{
    if(IsTimerRunning())
    {   // Kontrola, zda-li jiz casovac nebezi
        killTimer(timerId);
        timerRunning = false;
    }

    timerRunning = true;
    timerId = startTimer(timerInterval);
}

void MapWidget::KillTimer(bool immidiatelyReaction)
{
    if(IsTimerRunning())
    {
        timerRunning = false;

        if(immidiatelyReaction)
        {   // V pripade pozadovane nezpozdene reakce je timer vypnut ihned (nebude tak zobrazen posledni mezivysledek)
            killTimer(timerId);
        }
    }
}

bool MapWidget::IsTimerRunning()
{
    return timerRunning;
}
