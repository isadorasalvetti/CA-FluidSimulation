HEADERS = \
    mesh.h \
    glwidget.h \
    mainwindow.h \
    particle.h \
    particlespawner.h \
    timer.h \
    collider.h \
    objects.h

SOURCES = \
          main.cpp \
    mesh.cpp \
    glwidget.cpp \
    mainwindow.cpp \
    particle.cpp \
    particlespawner.cpp \
    timer.cpp \
    collider.cpp \
    objects.cpp

QT += gui opengl

CONFIG += c++11

DISTFILES += \
    shaders/basic.frag \
    shaders/basic.vert \
    shaders/particle.vert \
    shaders/particle.frag

FORMS += \
mainwindow.ui
