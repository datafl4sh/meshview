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
 * EXprev OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Matteo Cicuttin BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <memory>
#include <QListWidget>
#include <QLabel>
#include <QSlider>
#include <QColorDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include "Mesh.h"

/************************************************************************/
class MainControllerWidget : public QWidget
{
    Q_OBJECT
    
    QLabel          _mesh_path;
    QLabel          _mesh_numpoints;
    QLabel          _mesh_numtets;
    QLabel          _mesh_numtris;
    
    QListWidget     *_bnd_listwidget;
    QListWidget     *_dom_listwidget;
    
    QListWidgetItem *_all_domains_entry;
    QListWidgetItem *_no_surfaces_entry;
    
    std::shared_ptr<NetgenNeutralMesh> _nnm;
    
private:
    QWidget *   makeMeshInfoGroup(void);
    QWidget *   makeWhatToDrawBtnGroup(void);
    QWidget *   makeDomainListWidget(void);
    QWidget *   makeBoundaryListWidget(void);
    
    void        updateMeshInfo(void);
    void        updateLists(void);
    
private slots:
    void        bndListElemSelected(QListWidgetItem *item, QListWidgetItem *prev);
    void        domListElemSelected(QListWidgetItem *item, QListWidgetItem *prev);
    
signals:
    void        drawTetrahedronsRequested();
    void        drawTrianglesRequested();
    void        meshUpdated();
    void        boundarySelected(int);
    void        domainSelected(int);
    
public:
    MainControllerWidget(QWidget *parent = nullptr);
    
public slots:
    void    setMesh(std::shared_ptr<NetgenNeutralMesh>);
    
};

/************************************************************************/
class BoundaryControllerWidget : public QWidget
{
    Q_OBJECT
    
    QLabel  *_workingBndLabel;
    QLabel  *_triangleCountLabel;
    
    QLabel  *_minLengthLabel;
    QLabel  *_avgLengthLabel;
    QLabel  *_maxLengthLabel;
    
    QComboBox   *_groups;
    
    std::shared_ptr<NetgenNeutralMesh> _nnm;
    
    int _workingBnd;

signals:
    void    meshUpdated();
    
private slots:
    void    groupSelected(int);
    
public:
    BoundaryControllerWidget(QWidget *parent = nullptr);
    
public slots:
    void    setMesh(std::shared_ptr<NetgenNeutralMesh>);
    void    setWorkingBoundary(int);
    void    updateGroups(void);
};

/************************************************************************/
class DomainControllerWidget : public QWidget
{
    Q_OBJECT
    
    QLabel          *_workingDomLabel;
    QLabel          *_tetrahedronCountLabel;
    QSlider         *_alphaChannelSlider;
    QPushButton     *_selectColorButton;
    QColorDialog    *_colorDialog;
    

    
    std::shared_ptr<NetgenNeutralMesh> _nnm;
    
    int _workingDom;
    
private slots:
    void        sliderMoved(int);
    void        changeColorButtonClicked(bool);
    void        domainColorChanged(const QColor&);
    
signals:
    void    meshUpdated();
    
public:
    DomainControllerWidget(QWidget *parent = nullptr);
    
public slots:
    void    setMesh(std::shared_ptr<NetgenNeutralMesh>);
    void    setWorkingDomain(int);
};

/************************************************************************/
class BoundaryGroupControllerWidget : public QWidget
{
    Q_OBJECT
    
    QListWidget     *_boundaryGroupList;
    QPushButton     *_addButton;
    QPushButton     *_removeButton;
    QPushButton     *_groupColorButton;
    QLineEdit       *_groupNameEdit;
    QColorDialog    *_colorDialog;
    
    QLineEdit       *_bndList;
    
    std::shared_ptr<NetgenNeutralMesh> _nnm;
    
signals:
    void    meshUpdated();
    void    groupsUpdated();
    
private slots:
    void    addButtonClicked(bool);
    void    removeButtonClicked(bool);
    void    setColorButtonClicked(bool);
    void    groupColorChanged(const QColor&);
    void    boundaryGroupSelected(QListWidgetItem *);
    
public:
    BoundaryGroupControllerWidget(QWidget *parent = nullptr);
    
public slots:
    void    setMesh(std::shared_ptr<NetgenNeutralMesh>);
};










