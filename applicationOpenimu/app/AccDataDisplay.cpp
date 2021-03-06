#include "AccDataDisplay.h"

#include <math.h>
#include <QPropertyAnimation>
#include <QQuickView>

QT_CHARTS_USE_NAMESPACE

AccDataDisplay::AccDataDisplay()
{

}

void AccDataDisplay::showSimplfiedDataDisplay()
{
    checkboxX->hide();
    checkboxY->hide();
    checkboxZ->hide();
    checkboxAccNorm->hide();
    checkboxMovingAverage->hide();
    rSlider->hide();
    pbtn->hide();
    dateRecorded->hide();
}

AccDataDisplay::AccDataDisplay(WimuAcquisition accData){

    this->grabGesture(Qt::PanGesture);
    this->grabGesture(Qt::PinchGesture);

    acceleroData = accData;
    availableData = acceleroData.getData();
    sliceData = availableData;

    if(availableData.size()>0)
    {

        chart = new DataChart();
        chart->legend()->show();
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->setTheme(QChart::ChartThemeDark);

        rSliderValue = 1;
        lSliderValue = 0;

        chart->createDefaultAxes();
        chart->setTitle(tr("Données accéléromètre (en ms)"));
        chart->setAnimationOptions(QChart::SeriesAnimations);

        chartView = new ChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        layout = new QVBoxLayout(this);

        //Initialize Recording Date
        QHBoxLayout *hboxDate = new QHBoxLayout();

        pbtn = new QPushButton("Reset Zoom");
        connect(pbtn, SIGNAL (released()), this, SLOT (handleResetZoomBtn()));

        this->setStyleSheet( "QPushButton{"
                             "background-color: rgba(119, 160, 175,0.7);"
                             "border-style: inset;"
                             "border-width: 2px;"
                             "border-radius: 10px;"
                             "border-color: white;"
                             "font: 12px;"
                             "min-width: 10em;"
                             "padding: 6px; }"
                             "QPushButton:pressed { background-color: rgba(70, 95, 104, 0.7);}"
                             );

        dateRecorded = new QLabel();
        dateRecorded->setText(QString::fromStdString("Journée d'enregistrement: ")+ QString::fromStdString(acceleroData.getDates().back().date));
        hboxDate->addStretch();
        hboxDate->addWidget(dateRecorded);
        hboxDate->addStretch();
        hboxDate->addWidget(pbtn);

        //Initialize Checkbox and Label
        checkboxX = new QCheckBox(tr("Axe X"));
        checkboxY = new QCheckBox(tr("Axe Y"));
        checkboxZ = new QCheckBox(tr("Axe Z"));
        checkboxAccNorm = new QCheckBox(tr("Norme"));
        checkboxMovingAverage = new QCheckBox(tr("Moyenne mobile"));

        checkboxX->setChecked(true);
        checkboxY->setChecked(true);
        checkboxZ->setChecked(true);
        checkboxAccNorm->setChecked(false);
        checkboxMovingAverage->setChecked(false);

        QHBoxLayout *hbox = new QHBoxLayout();
        hbox->addStretch();
        hbox->addWidget(checkboxX);
        hbox->addWidget(checkboxY);
        hbox->addWidget(checkboxZ);
        hbox->addWidget(checkboxAccNorm);
        hbox->addWidget(checkboxMovingAverage);
        hbox->addStretch();


        long long min = WimuAcquisition::minTime(availableData).timestamp;

        rSlider = new RangeSlider(this);
        int tmpmin = int(sliceData.size()*lSliderValue/50);
        tmpmin = min + tmpmin;

        int tmpmax = int(sliceData.size()*rSliderValue/50);
        tmpmax = min + tmpmax;

        rSlider->setStartHour(tmpmin);
        rSlider->setEndHour(tmpmax);

        layout->addLayout(hboxDate);
        layout->addWidget(chartView);
        layout->addLayout(hbox);
        layout->addWidget(rSlider);

        connect(checkboxX, SIGNAL(stateChanged(int)), this, SLOT(slotDisplayXAxis(int)));
        connect(checkboxY, SIGNAL(stateChanged(int)), this, SLOT(slotDisplayYAxis(int)));
        connect(checkboxZ, SIGNAL(stateChanged(int)), this, SLOT(slotDisplayZAxis(int)));
        connect(checkboxAccNorm, SIGNAL(stateChanged(int)), this, SLOT(slotDisplayNorme(int)));
        connect(checkboxMovingAverage, SIGNAL(stateChanged(int)), this, SLOT(slotDisplayMovingAverage(int)));

        fillChartSeries();
    }
}
void AccDataDisplay::handleResetZoomBtn()
{
    chart->zoomReset();
}
void AccDataDisplay::slotDisplayNorme(int value){
    if(value){
        chart->addSeries(lineseriesAccNorm);
        chartView->setChart(chart);
    }else{
        chart->removeSeries(lineseriesAccNorm);
        chartView->setChart(chart);
    }
}
void AccDataDisplay::slotDisplayMovingAverage(int value){
    if(value){
        chart->addSeries(lineseriesMovingAverage);
        chartView->setChart(chart);
    }else{
        chart->removeSeries(lineseriesMovingAverage);
        chartView->setChart(chart);
    }

}
void AccDataDisplay::leftSliderValueChanged(double value)
{
    lSliderValue = value;
    int tmpmin = int(sliceData.size()*lSliderValue/50);
    tmpmin = WimuAcquisition::minTime(availableData).timestamp + tmpmin;
    rSlider->setStartHour(tmpmin);
    chart->removeAllSeries();
    fillChartSeries();
    chartView->setChart(chart);
}
void AccDataDisplay::rightSliderValueChanged(double value)
{
    rSliderValue = value;
    int tmpmax = int(sliceData.size()*rSliderValue/50);
    tmpmax = WimuAcquisition::minTime(availableData).timestamp + tmpmax;
    rSlider->setEndHour(tmpmax);
    chart->removeAllSeries();
    fillChartSeries();
    chartView->setChart(chart);

}
void AccDataDisplay::slotDisplayXAxis(int value){
    if(value){
        chart->addSeries(lineseriesX);
        chartView->setChart(chart);
    }else{
        chart->removeSeries(lineseriesX);
        chartView->setChart(chart);
    }
}
void AccDataDisplay::slotDisplayYAxis(int value){
    if(value){
        chart->addSeries(lineseriesY);
        chartView->setChart(chart);
    }else{
        chart->removeSeries(lineseriesY);
        chartView->setChart(chart);
    }
}
void AccDataDisplay::slotDisplayZAxis(int value){
    if(value){
        chart->addSeries(lineseriesZ);
        chartView->setChart(chart);
    }else{
        chart->removeSeries(lineseriesZ);
        chartView->setChart(chart);
    }
}
std::vector<signed short> AccDataDisplay::movingAverage(int windowSize)
{
    int tmpmin = int(sliceData.size()*lSliderValue);
    int tmpmax = int(sliceData.size()*rSliderValue);
    double sum=0;
    std::vector<signed short> filteredData;
    if(tmpmax-tmpmin < windowSize+1)
    {
        return filteredData;
    }
    for(int j=tmpmin;j<tmpmin+windowSize;j++)
    {
        sum+=sqrt(pow(sliceData.at(j).x,2.0)+ pow(sliceData.at(j).y,2.0) + pow(sliceData.at(j).z,2.0));
    }
    filteredData.push_back(sum/windowSize);
    for (int i=tmpmin+1;i<tmpmax-windowSize;i++)
    {
        sum-=sqrt(pow(sliceData.at(i-1).x,2.0)+pow(sliceData.at(i-1).y,2.0)+ pow(sliceData.at(i-1).z,2.0));
        sum+=sqrt(pow(sliceData.at(i+windowSize-1).x,2.0)+pow(sliceData.at(i+windowSize-1).y,2.0)+ pow(sliceData.at(i+windowSize-1).z,2.0));
        filteredData.push_back(sum/windowSize);
    }
    return filteredData;
}
void AccDataDisplay::fillChartSeries(){

    std::vector<signed short> x;
    std::vector<signed short> y;
    std::vector<signed short> z;
    std::vector<signed short> norm_acc;
    std::vector<signed short> filtered_data;
    filtered_data = movingAverage(10);
    std::vector<float> t;
    int tmpmin = int(sliceData.size()*lSliderValue);
    int tmpmax = int(sliceData.size()*rSliderValue);

    for(int k = tmpmin; k <tmpmax; k++){

        x.push_back(sliceData.at(k).x);
        y.push_back(sliceData.at(k).y);
        z.push_back(sliceData.at(k).z);
        norm_acc.push_back(sqrt(pow(sliceData.at(k).x,2.0) + pow(sliceData.at(k).y,2.0) + pow(sliceData.at(k).z,2.0)));
        t.push_back(k*20); // TO DO Replace w/ real value
    }


    lineseriesX = new QtCharts::QLineSeries();
    QPen penX(QRgb(0xCF000F));
    lineseriesX->setPen(penX);
    lineseriesX->setName(tr("Axe X"));
    lineseriesX->setUseOpenGL(true);


    lineseriesY = new QtCharts::QLineSeries();
    QPen penY(QRgb(0x00b16A));
    lineseriesY->setPen(penY);
    lineseriesY->setName(tr("Axe Y"));
    lineseriesY->setUseOpenGL(true);

    lineseriesZ = new QtCharts::QLineSeries();
    QPen penZ(QRgb(0x4183D7));
    lineseriesZ->setPen(penZ);
    lineseriesZ->setName(tr("Axe Z"));
    lineseriesZ->setUseOpenGL(true);

    lineseriesAccNorm = new QtCharts::QLineSeries();
    lineseriesAccNorm->setName(tr("Norme"));

    QPen pen(QRgb(0xdadfe1));
    lineseriesAccNorm->setPen(pen);
    lineseriesAccNorm->setUseOpenGL(true);

    lineseriesMovingAverage = new QtCharts::QLineSeries();
    QPen penM(QRgb(0xf89406));
    lineseriesMovingAverage->setPen(penM);
    lineseriesMovingAverage->setName(tr("Moyenne mobile"));
    lineseriesMovingAverage->setUseOpenGL(true);

    for(unsigned int i = 0; i <x.size(); i++)
    {
        lineseriesX->append(t.at(i),x.at(i));
        lineseriesY->append(t.at(i),y.at(i));
        lineseriesZ->append(t.at(i),z.at(i));
        lineseriesAccNorm->append(t.at(i),norm_acc.at(i));
    }
    for(unsigned int i = 0; i <filtered_data.size(); i++)
    {
        lineseriesMovingAverage->append(t.at(i),filtered_data.at(i));
    }
    if(checkboxX->isChecked())
        chart->addSeries(lineseriesX);

    if(checkboxY->isChecked())
        chart->addSeries(lineseriesY);

    if(checkboxZ->isChecked())
        chart->addSeries(lineseriesZ);

    if(checkboxAccNorm->isChecked())
        chart->addSeries(lineseriesAccNorm);

    if(checkboxMovingAverage->isChecked())
        chart->addSeries(lineseriesMovingAverage);

    chart->createDefaultAxes();
}

void AccDataDisplay:: firstUpdated(const QVariant &v) {
      leftSliderValueChanged(v.toDouble());
}

void AccDataDisplay:: secondUpdated(const QVariant &v) {
      rightSliderValueChanged(v.toDouble());
}
