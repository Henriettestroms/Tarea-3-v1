#include "VideoFollower.h"
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSlider>
#include <QUrl>
#include <QVBoxLayout>
#include <QVideoWidget>

VideoFollower::VideoFollower(const std::string &name,
                             const std::string &topicName)
    : Subscriber(name, topicName) {
  view = new QWidget;
  auto *layout = new QVBoxLayout(view);
  video = new QPushButton(QString::fromStdString(name));
  layout->addWidget(video);

  volumeSlider = new QSlider(Qt::Horizontal);
  volumeSlider->setRange(0, 100);
  volumeSlider->setValue(50);
  layout->addWidget(volumeSlider);

  QObject::connect(video, &QPushButton::clicked, [this]() {
    if (lastUrl.isEmpty())
      return;

    // Objetos de reproducción con el mismo padre para facilitar
    // su liberación de memoria al cerrar la ventana de video
    auto *window = new QWidget;
    auto *player = new QMediaPlayer(window);        // reproductor
    auto *audio = new QAudioOutput(window); // salida de audio
    auto *videoWidget = new QVideoWidget(window);   // área de video

    player->setVideoOutput(videoWidget);
    player->setAudioOutput(audio);
    player->setSource(QUrl(lastUrl));

    // Ventana que contendrá el video y un control de volumen
    auto *layout = new QVBoxLayout(window);
    layout->addWidget(videoWidget);

    QSlider *videoSlider = new QSlider(Qt::Horizontal);
    videoSlider->setRange(0, 100);
    videoSlider->setValue(volumeSlider->value());
    layout->addWidget(videoSlider);
    // Ajustar volumen inicial según la posición del deslizador
    audio->setVolume(videoSlider->value() / 100.0);

    // Sincronizar ambos deslizadores y ajustar el volumen
    QObject::connect(videoSlider, &QSlider::valueChanged,
                     [audio, this](int value) {
                       QSignalBlocker blocker(volumeSlider);
                       volumeSlider->setValue(value);
                       audio->setVolume(value / 100.0);
                     });

    QObject::connect(volumeSlider, &QSlider::valueChanged,
                     [audio, videoSlider](int value) {
                       QSignalBlocker blocker(videoSlider);
                       videoSlider->setValue(value);
                       audio->setVolume(value / 100.0);
                     });

    window->setAttribute(Qt::WA_DeleteOnClose);
    window->resize(640, 520);
    window->show();
    player->play();
  });
}

void VideoFollower::update(const std::string &message) {
  lastUrl = QString::fromStdString(message);
  video->setText(lastUrl);
}

QWidget *VideoFollower::getView() { return view; }