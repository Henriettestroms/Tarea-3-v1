#include "GPSCarSubscriber.h"
#include <QVBoxLayout>
#include <QLabel>
#include <sstream>

GPSCarSubscriber::GPSCarSubscriber(const std::string& name, const std::string& topicName)
    : Subscriber(name, topicName)
{
    view = new QWidget;
    auto* layout = new QVBoxLayout(view);
    timeLabel = new QLabel("t: 0");
    xLabel = new QLabel("x: 0");
    yLabel = new QLabel("y: 0");
    layout->addWidget(timeLabel);
    layout->addWidget(xLabel);
    layout->addWidget(yLabel);
}

void GPSCarSubscriber::update(const std::string& message){
    QString msg = QString::fromStdString(message);
    auto parts = msg.split(' ', Qt::SkipEmptyParts);
    if(parts.size() == 3){
        timeLabel->setText(QString("t: %1").arg(parts[0]));
        xLabel->setText(QString("x: %1").arg(parts[1]));
        yLabel->setText(QString("y: %1").arg(parts[2]));
    }
}

QWidget* GPSCarSubscriber::getView(){
    return view;
}
