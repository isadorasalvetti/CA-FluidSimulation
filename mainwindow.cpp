#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->SimButton, SIGNAL(released()), this, SLOT(clickSim()));
    connect(ui->SolverButton, SIGNAL(released()), this, SLOT(clickSolver()));
    connect(ui->kE, SIGNAL(valueChanged(double)), this, SLOT(changekE()));
    connect(ui->kD, SIGNAL(valueChanged(double)), this, SLOT(changekD()));
    connect(ui->ResetButton, SIGNAL(released()), this, SLOT(clickReset()));

    ui->kE->setValue(kE);
    ui->kD->setValue(kD);

    gl = ui->openGLWidget;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clickSolver(){
    bool solverStatus = solver == Particle::VERLET;
    if (solverStatus) {
        ui->SolverButton->setText(QString("Euler"));
        solver = Particle::EULER;
    }
    else {
        ui->SolverButton->setText(QString("Verlet"));
        solver = Particle::VERLET;
    }
}

void MainWindow::clickSim(){
    dimensions++;
    if (dimensions == 3) dimensions = 0;

    if (dimensions == 0) ui->SimButton->setText(QString("Particles"));
    if (dimensions == 1) ui->SimButton->setText(QString("Rope"));
    if (dimensions == 2) ui->SimButton->setText(QString("Cloth"));
}

void MainWindow::changekD(){kD = ui->kE->value();}
void MainWindow::changekE(){kE = ui->kE->value();}

void MainWindow::clickReset(){
    gl->Reset(dimensions, kD, kE, solver);
}

