/*
 * This source file is part of EMT, the ElectroMagneticTool.
 *
 * Copyright (C) 2013, Matteo Cicuttin - matteo.cicuttin@uniud.it
 * Department of Electrical Engineering, University of Udine
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of Udine nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Matteo Cicuttin ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Matteo Cicuttin BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>

#include <QFileDialog>

#include "MainWindow.h"
#include "MeshGLWidget.h"

MainWindow::MainWindow(QWidget *parent)
{
    setWindowTitle( "EMT Geometry Frontend (OpenGL) || (C) 2013-2014 Matteo Cicuttin" );
    
    _meshWidget = new MeshGLWidget();
    
    setCentralWidget( _meshWidget );
    setMinimumSize(200, 200);
    resize(1100, 600);
    
    create_actions();
    create_menus();
    
    /* Main controller */
    QDockWidget *mainDW = new QDockWidget();
    _mainController = new MainControllerWidget();
    mainDW->setWidget(_mainController);
    mainDW->setWindowTitle("Main Controller");
    connect(_mainController, SIGNAL(drawTetrahedronsRequested(void)),
            _meshWidget, SLOT(setDrawTetrahedrons(void)));
    connect(_mainController, SIGNAL(drawTrianglesRequested(void)),
            _meshWidget, SLOT(setDrawTriangles(void)));
    connect(_mainController, SIGNAL(meshUpdated(void)),
            _meshWidget, SLOT(updateGL(void)));
    addDockWidget(Qt::LeftDockWidgetArea, mainDW);
    
    /* Boundary controller */
    QDockWidget *boundaryDW = new QDockWidget();
    _boundaryController = new BoundaryControllerWidget();
    boundaryDW->setWidget(_boundaryController);
    boundaryDW->setWindowTitle("Boundary controller");
    connect(_mainController, SIGNAL(boundarySelected(int)),
            _boundaryController, SLOT(setWorkingBoundary(int)));
    connect(_boundaryController, SIGNAL(meshUpdated(void)),
            _meshWidget, SLOT(updateGL(void)));
    addDockWidget(Qt::RightDockWidgetArea, boundaryDW);
    
    /* Boundary Group Controller */
    QDockWidget *bgDW = new QDockWidget();
    _boundaryGroupController = new BoundaryGroupControllerWidget();
    bgDW->setWidget(_boundaryGroupController);
    bgDW->setWindowTitle("Boundary group controller");
    addDockWidget(Qt::RightDockWidgetArea, bgDW);
    connect(_boundaryGroupController, SIGNAL(groupsUpdated()),
            _boundaryController, SLOT(updateGroups()));
    connect(_boundaryGroupController, SIGNAL(meshUpdated(void)),
            _meshWidget, SLOT(updateGL(void)));
    
    
    
    /* Domain controller */
    QDockWidget *domainDW = new QDockWidget();
    _domainController = new DomainControllerWidget();
    domainDW->setWidget(_domainController);
    domainDW->setWindowTitle("Domain controller");
    connect(_mainController, SIGNAL(domainSelected(int)),
            _domainController, SLOT(setWorkingDomain(int)));
    connect(_domainController, SIGNAL(meshUpdated(void)),
            _meshWidget, SLOT(updateGL(void)));
    addDockWidget(Qt::RightDockWidgetArea, domainDW);
    
    
    statusBar()->showMessage("Ready");
};

void
MainWindow::create_actions(void)
{
    _openAction = new QAction("&Open mesh", this);
    connect(_openAction, SIGNAL(triggered()), this, SLOT(open_action()));
}

void
MainWindow::create_menus(void)
{
    _fileMenu = menuBar()->addMenu("&File");
    _fileMenu->addAction(_openAction);
}

void
MainWindow::open_action(void)
{
    bool is_ok;
    
    QString meshPath = QFileDialog::getOpenFileName(NULL, "Select a mesh to open...", QDir::homePath());
    
    if (meshPath == "")
        return;
    
    QString message;
    QTextStream(&message) << "Opening mesh " << meshPath << ", please wait";
    statusBar()->showMessage(message);
    
    std::shared_ptr<NetgenNeutralMesh> new_nnm(new NetgenNeutralMesh());
    is_ok = new_nnm->load(meshPath.toStdString(), true);
    
    message.clear();
    
    if (!is_ok)
    {
        QTextStream(&message) << "Problem loading mesh";
        return;
    }
    
    _nnm = new_nnm;
    
    _meshWidget->setMesh(_nnm);
    _mainController->setMesh(_nnm);
    _boundaryController->setMesh(_nnm);
    _domainController->setMesh(_nnm);
    _boundaryGroupController->setMesh(_nnm);
    
    QTextStream(&message) << "Mesh loaded";

    statusBar()->showMessage(message);
}





