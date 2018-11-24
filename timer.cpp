#include "timer.h"

Timer::Timer(QOpenGLWidget *s, particleSpawner *sp){
    widget = s;
    spawner = sp;
    t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(update()));
    t->start(30);
}

void Timer::update(){
   spawner->updateParticles();
   widget->update();
}

Timer::~Timer(){
    delete t;
}
