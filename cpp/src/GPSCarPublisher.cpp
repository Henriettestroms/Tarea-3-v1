#include "GPSCarPublisher.h"
#include "Broker.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QHBoxLayout>
#include <QLabel>

GPSCarPublisher::GPSCarPublisher(const std::string& name, Broker& broker, const std::string& topicName)
    : Publisher(name, broker, topicName)
{
    view = new QWidget;
    auto* layout = new QHBoxLayout(view);
    layout->addWidget(new QLabel(QString::fromStdString(name)));
    fileLabel = new QLabel;
    layout->addWidget(fileLabel);

    QString fileName = QFileDialog::getOpenFileName(view, "Archivo de coordenadas", QString(), "Text Files (*.txt)");
    if(!fileName.isEmpty()){
        fileLabel->setText(fileName);
        loadFile(fileName);
        startEmitting();
    } else {
        fileLabel->setText("Sin archivo");
    }
}

GPSCarPublisher::Position GPSCarPublisher::interpolate(const std::vector<Position>& pts, double t){
    if(pts.empty()) return {t,0,0};
    if(t <= pts.front().time) return {t, pts.front().x, pts.front().y};
    for(size_t i=0; i<pts.size()-1; ++i){
        if(t >= pts[i].time && t <= pts[i+1].time){
            double alpha = (t - pts[i].time)/(pts[i+1].time - pts[i].time);
            double x = pts[i].x + alpha*(pts[i+1].x - pts[i].x);
            double y = pts[i].y + alpha*(pts[i+1].y - pts[i].y);
            return {t,x,y};
        }
    }
    return {t, pts.back().x, pts.back().y};
}

void GPSCarPublisher::loadFile(const QString& fileName){
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    std::vector<Position> raw;
    while(!in.atEnd()){
        double t,x,y;
        in >> t >> x >> y;
        if(in.status() == QTextStream::Ok)
            raw.push_back({t,x,y});
    }
    if(raw.empty())
        return;
    for(int t = static_cast<int>(raw.front().time); t <= static_cast<int>(raw.back().time); ++t){
        points.push_back(interpolate(raw, static_cast<double>(t)));
    }
}

void GPSCarPublisher::startEmitting(){
    if(points.empty())
        return;
    timer = new QTimer(view);
    QObject::connect(timer, &QTimer::timeout, [this](){
        if(index >= static_cast<int>(points.size())){
            timer->stop();
            return;
        }
        const auto& p = points[index++];
        QString msg = QString("%1 %2 %3").arg(p.time).arg(p.x).arg(p.y);
        publishNewEvent(msg.toStdString());
    });
    timer->start(1000);
}

QWidget* GPSCarPublisher::getView(){
    return view;
}