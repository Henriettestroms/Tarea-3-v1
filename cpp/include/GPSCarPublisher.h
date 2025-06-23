#ifndef GPSCARPUBLISHER_H
#define GPSCARPUBLISHER_H

#include "Publisher.h"
#include <QWidget>
#include <vector>

class QLabel;
class QTimer;
class GPSCarPublisher : public Publisher {
public:
    GPSCarPublisher(const std::string& name, Broker& broker, const std::string& topicName);
    QWidget* getView();

private:
    struct Position { double time; double x; double y; };
    Position interpolate(const std::vector<Position>& pts, double t);
    void loadFile(const QString& fileName);
    void startEmitting();

    QWidget* view{nullptr};
    QLabel* fileLabel{nullptr};
    std::vector<Position> points;
    QTimer* timer{nullptr};
    int index{0};
};

#endif