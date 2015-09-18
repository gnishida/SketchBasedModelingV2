﻿#include "MainWindow.h"
#include <QFileDialog>
#include <QDate>
#include <time.h>

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	QActionGroup* sketchStepGroup = new QActionGroup(this);
	sketchStepGroup->addAction(ui.actionStepFloor);
	sketchStepGroup->addAction(ui.actionStepWindow);
	ui.actionStepFloor->setChecked(true);
	ui.actionStepWindow->setChecked(false);

	// メニューハンドラ
	connect(ui.actionNewSketch, SIGNAL(triggered()), this, SLOT(onNewSketch()));
	connect(ui.actionSaveImage, SIGNAL(triggered()), this, SLOT(onSaveImage()));
	connect(ui.actionLoadCamera, SIGNAL(triggered()), this, SLOT(onLoadCamera()));
	connect(ui.actionSaveCamera, SIGNAL(triggered()), this, SLOT(onSaveCamera()));
	connect(ui.actionResetCamera, SIGNAL(triggered()), this, SLOT(onResetCamera()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionStepFloor, SIGNAL(triggered()), this, SLOT(onStepFloor()));
	connect(ui.actionStepWindow, SIGNAL(triggered()), this, SLOT(onStepWindow()));

	glWidget = new GLWidget3D(this);
	setCentralWidget(glWidget);
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
	glWidget->keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent* e) {
	glWidget->keyReleaseEvent(e);
}

void MainWindow::onNewSketch() {
	glWidget->clear();
	glWidget->update();
}

void MainWindow::onSaveImage() {
	if (!QDir("screenshots").exists()) QDir().mkdir("screenshots");
	QString fileName = "screenshots/" + QDate::currentDate().toString("yyMMdd") + "_" + QTime::currentTime().toString("HHmmss") + ".png";
	glWidget->grabFrameBuffer().save(fileName);
	printf("Save %s\n", fileName.toAscii().constData());
}

void MainWindow::onLoadCamera() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open camera file..."), "", tr("Camera Files (*.cam)"));
	if (filename.isEmpty()) return;

	glWidget->camera.loadCameraPose(filename.toUtf8().data());
	glWidget->update();
}

void MainWindow::onSaveCamera() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save camera file..."), "", tr("Camera Files (*.cam)"));
	if (filename.isEmpty()) return;

	glWidget->camera.saveCameraPose(filename.toUtf8().data());
}

void MainWindow::onResetCamera() {
	glWidget->camera.resetCamera();
	glWidget->update();
}

void MainWindow::onStepFloor() {
	glWidget->sketch_step = cga::STEP_FLOOR;
}

void MainWindow::onStepWindow() {
	glWidget->sketch_step = cga::STEP_WINDOW;
}