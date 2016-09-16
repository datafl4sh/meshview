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

#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QTextStream>

#include <iostream>

#include "ControllerWidget.h"

/************************************************************************/
MainControllerWidget::MainControllerWidget(QWidget *parent)
    : QWidget(parent)
{
    _mesh_numpoints.setText("Mesh not yet loaded");
    
    QGridLayout *layout = new QGridLayout();
    
    layout->addWidget(makeMeshInfoGroup(), 0, 0);
    layout->addWidget(makeWhatToDrawBtnGroup(), 1, 0);
    layout->addWidget(makeDomainListWidget(), 2, 0);
    layout->addWidget(makeBoundaryListWidget(), 3, 0);
    
    setLayout(layout);
    
    _nnm = nullptr;
}

QWidget *
MainControllerWidget::makeMeshInfoGroup(void)
{
    QGroupBox *groupbox = new QGroupBox(tr("Mesh info"));
    
    QVBoxLayout *vbox = new QVBoxLayout();
    //vbox->addWidget(&_mesh_path);
    vbox->addWidget(&_mesh_numpoints);
    vbox->addWidget(&_mesh_numtets);
    vbox->addWidget(&_mesh_numtris);
    vbox->addStretch(1);
    
    groupbox->setLayout(vbox);
    return groupbox;
}

QWidget *
MainControllerWidget::makeWhatToDrawBtnGroup(void)
{
    QGroupBox *whatToDrawBtnGroup = new QGroupBox(tr("Elems to draw"));
    QRadioButton *radio_tets = new QRadioButton(tr("Tetrahedrons"));
    QRadioButton *radio_tris = new QRadioButton(tr("Triangles"));
    radio_tets->setChecked(true);
    
    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(radio_tets);
    vbox->addWidget(radio_tris);
    vbox->addStretch(1);
    
    whatToDrawBtnGroup->setLayout(vbox);
    
    QObject::connect(radio_tris, SIGNAL(clicked(bool)),
                     this, SIGNAL(drawTrianglesRequested()));
    
    QObject::connect(radio_tets, SIGNAL(clicked(bool)),
                     this, SIGNAL(drawTetrahedronsRequested()));
    
    return whatToDrawBtnGroup;
}

QWidget *
MainControllerWidget::makeDomainListWidget(void)
{
    QGroupBox *groupbox = new QGroupBox(tr("Mesh domains"));
    
    QVBoxLayout *vbox = new QVBoxLayout();
    _dom_listwidget = new QListWidget();
    vbox->addWidget(_dom_listwidget);
    vbox->addStretch(1);
    groupbox->setLayout(vbox);
    
    connect(_dom_listwidget,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this,
            SLOT(domListElemSelected(QListWidgetItem *, QListWidgetItem *)));
    
    return groupbox;
}

QWidget *
MainControllerWidget::makeBoundaryListWidget(void)
{
    QGroupBox *groupbox = new QGroupBox(tr("Mesh boundaries"));
    
    QVBoxLayout *vbox = new QVBoxLayout();
    _bnd_listwidget = new QListWidget();
    vbox->addWidget(_bnd_listwidget);
    vbox->addStretch(1);
    groupbox->setLayout(vbox);
    
    connect(_bnd_listwidget,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this,
            SLOT(bndListElemSelected(QListWidgetItem *, QListWidgetItem *)));
    
    return groupbox;
}

void
MainControllerWidget::bndListElemSelected(QListWidgetItem *item,
                                          QListWidgetItem *prev)
{
    if (!item)
        return;
    
    _nnm->setHighlightBoundariesNone();
    
    if (item == _no_surfaces_entry)
    {
        emit meshUpdated();
        return;
    }
    
    auto num = item->data(Qt::UserRole).toUInt();
    _nnm->boundaries().at(num).setHighlighted(true);
    
    emit boundarySelected(num);
    emit meshUpdated();
    return;
}

void
MainControllerWidget::domListElemSelected(QListWidgetItem *item,
                                          QListWidgetItem *prev)
{
    if (!item)
        return;
    
    if (item == _all_domains_entry)
    {
        _nnm->setDisplayDomainsAll();
        emit meshUpdated();
        return;
    }
    
    _nnm->setDisplayDomainsNone();
    auto num = item->data(Qt::UserRole).toUInt();
    _nnm->domains().at(num).enableDisplay(true);

    emit domainSelected(num);
    emit meshUpdated();
    return;
}


void
MainControllerWidget::updateMeshInfo(void)
{
    if (!_nnm)
        return;
    
    QString str;
    size_t num;
    
    /*
    str.clear();
    QTextStream(&str) << "Path: " << _nnm->filename().c_str();
    _mesh_path.setText(str);
     */
    
    str.clear();
    num = 0;
    QTextStream(&str) << "Points: " << _nnm->points().size();
    _mesh_numpoints.setText(str);
    
    str.clear();
    num = 0;
    for (auto& b : _nnm->boundaries())
        num += b.second.size();
    QTextStream(&str) << "Triangles: " << num;
    _mesh_numtris.setText(str);
    
    str.clear();
    num = 0;
    for (auto& d : _nnm->domains())
        num += d.second.size();
    QTextStream(&str) << "Tetrahedrons: " << num;
    _mesh_numtets.setText(str);
}

void
MainControllerWidget::updateLists(void)
{
    if (_bnd_listwidget)
        _bnd_listwidget->clear();
    
    if (_dom_listwidget)
        _dom_listwidget->clear();
    
    if (!_nnm)
        return;
    
    
    _no_surfaces_entry = new QListWidgetItem("None");
    _bnd_listwidget->addItem(_no_surfaces_entry);
    for ( auto& b : _nnm->boundaries() )
    {
        QString entry_str;
        entry_str.sprintf("Boundary %lu", b.first);
        
        QListWidgetItem *item = new QListWidgetItem(entry_str);
        item->setData(Qt::UserRole, QVariant( uint(b.first) ));
        
        _bnd_listwidget->addItem(item);
    }
    
    _all_domains_entry = new QListWidgetItem("All domains");
    _dom_listwidget->addItem(_all_domains_entry);
    for ( auto& d : _nnm->domains() )
    {
        QString entry_str;
        entry_str.sprintf("Domain %lu", d.first);
        
        QListWidgetItem *item = new QListWidgetItem(entry_str);
        item->setData(Qt::UserRole, QVariant( uint(d.first) ));
        
        _dom_listwidget->addItem(item);
    }
}

void
MainControllerWidget::setMesh(std::shared_ptr<NetgenNeutralMesh> nnm)
{
    _nnm = nnm;
    updateMeshInfo();
    updateLists();
}

/************************************************************************/
BoundaryControllerWidget::BoundaryControllerWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Boundary View");
    
    _workingBndLabel = new QLabel();
    _triangleCountLabel = new QLabel();
    
    _minLengthLabel = new QLabel();
    _avgLengthLabel = new QLabel();
    _maxLengthLabel = new QLabel();

    
    QGroupBox *groupbox = new QGroupBox(tr("Boundary info"));
    
    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(_workingBndLabel);
    vbox->addWidget(_triangleCountLabel);
    vbox->addWidget(_minLengthLabel);
    vbox->addWidget(_avgLengthLabel);
    vbox->addWidget(_maxLengthLabel);
    vbox->addStretch(1);
    groupbox->setLayout(vbox);
    
    _groups = new QComboBox();
    connect(_groups, SIGNAL(activated(int)),
            this, SLOT(groupSelected(int)));
    
    QGridLayout *layout = new QGridLayout();
    
    layout->addWidget(groupbox, 0, 0, 1, 2);
    layout->addWidget(new QLabel("Boundary group:"), 1, 0);
    layout->addWidget(_groups, 1, 1);
    
    setLayout(layout);
    
    _nnm = nullptr;
}

void
BoundaryControllerWidget::updateGroups(void)
{
    if (!_nnm)
        return;
    
    _groups->clear();
    
    _groups->addItem( QString("None"), QVariant(QString("")) );
    
    for ( auto& b : _nnm->groupProps() )
    {
        QString s(b.first.c_str());
        _groups->addItem(s, QVariant(s) );
    }
}

void
BoundaryControllerWidget::groupSelected(int sel)
{
    QString group = _groups->itemData(sel).toString();
    
    _nnm->boundaries().at(_workingBnd).setGroup(group.toStdString());
}

void
BoundaryControllerWidget::setMesh(std::shared_ptr<NetgenNeutralMesh> nnm)
{
    _nnm = nnm;
}

void
BoundaryControllerWidget::setWorkingBoundary(int bnd)
{
    _workingBnd = bnd;
    
    QString str;
    QTextStream(&str) << "Boundary " << bnd;
    _workingBndLabel->setText(str);
    
    str.clear();
    QTextStream(&str) << "Triangles: " << _nnm->boundaries().at(bnd).size();
    _triangleCountLabel->setText(str);
    
    
    str.clear();
    QTextStream(&str)
    << "Min edge length: " << _nnm->boundaries().at(bnd).minEdgeLength(_nnm->points());
    _minLengthLabel->setText(str);
    
    str.clear();
    QTextStream(&str)
    << "Avg edge length: " << _nnm->boundaries().at(bnd).avgEdgeLength(_nnm->points());
    _avgLengthLabel->setText(str);
    
    str.clear();
    QTextStream(&str)
    << "Max edge length: " << _nnm->boundaries().at(bnd).maxEdgeLength(_nnm->points());
    
    _maxLengthLabel->setText(str);
    
    QString group(_nnm->boundaries().at(bnd).group().c_str());
    
    int item = _groups->findData( QVariant(group) );
    _groups->setCurrentIndex(item);
    
}

/************************************************************************/
DomainControllerWidget::DomainControllerWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Domain View");
    
    _workingDomLabel = new QLabel();
    _tetrahedronCountLabel = new QLabel();
    
    _alphaChannelSlider = new QSlider(Qt::Horizontal);
    _alphaChannelSlider->setMinimum(0);
    _alphaChannelSlider->setMaximum(100);
    _alphaChannelSlider->setTracking(true);
    connect(_alphaChannelSlider, SIGNAL(valueChanged(int)),
            this, SLOT(sliderMoved(int)));
    
    QGroupBox *groupbox = new QGroupBox(tr("Domain info"));
        QVBoxLayout *vbox = new QVBoxLayout();
        vbox->addWidget(_workingDomLabel);
        vbox->addWidget(_tetrahedronCountLabel);
        vbox->addStretch(1);
        groupbox->setLayout(vbox);
    
    _selectColorButton = new QPushButton("Set color");
    connect(_selectColorButton, SIGNAL(clicked(bool)),
            this, SLOT(changeColorButtonClicked(bool)));
    
    QGridLayout *layout = new QGridLayout();
        layout->addWidget(groupbox, 0, 0, 1, 2);
        layout->addWidget(new QLabel("Transparency"), 1, 0);
        layout->addWidget(_alphaChannelSlider, 1, 1);
        layout->addWidget(_selectColorButton, 2, 0);
    
    
        setLayout(layout);
    
    _colorDialog = new QColorDialog();
    connect(_colorDialog, SIGNAL(currentColorChanged(const QColor&)),
            this, SLOT(domainColorChanged(const QColor&)));
    
    _nnm = nullptr;
}

void
DomainControllerWidget::setMesh(std::shared_ptr<NetgenNeutralMesh> nnm)
{
    _nnm = nnm;
}

void
DomainControllerWidget::setWorkingDomain(int dom)
{
    _workingDom = dom;
    
    int alpha = _nnm->domains().at(_workingDom).alpha()*100;
    _alphaChannelSlider->setValue(alpha);
    
    QString str;
    QTextStream(&str) << "Domain " << dom;
    _workingDomLabel->setText(str);
    
    str.clear();
    QTextStream(&str) << "Tetrahedrons: " << _nnm->domains().at(dom).size();
    _tetrahedronCountLabel->setText(str);
}

void
DomainControllerWidget::sliderMoved(int value)
{
    _nnm->domains().at(_workingDom).setAlpha( value/100.0 );
    emit meshUpdated();
}

void
DomainControllerWidget::domainColorChanged(const QColor& color)
{
    _nnm->domains().at(_workingDom).setColor(color.red()/255.0,
                                             color.green()/255.0,
                                             color.blue()/255.0);
    emit meshUpdated();
}

void
DomainControllerWidget::changeColorButtonClicked(bool)
{
    _colorDialog->show();
}

/************************************************************************/
BoundaryGroupControllerWidget::BoundaryGroupControllerWidget(QWidget *parent)
    : QWidget(parent)
{
    _boundaryGroupList = new QListWidget();
    connect(_boundaryGroupList,
            SIGNAL(itemClicked(QListWidgetItem *)),
            this,
            SLOT(boundaryGroupSelected(QListWidgetItem *)));
    
    _bndList = new QLineEdit();
    
    _addButton = new QPushButton("Add");
    connect(_addButton, SIGNAL(clicked(bool)),
            this, SLOT(addButtonClicked(bool)));
    
    _removeButton = new QPushButton("Remove");
    connect(_removeButton, SIGNAL(clicked(bool)),
            this, SLOT(removeButtonClicked(bool)));
    
    _groupColorButton = new QPushButton("Set color");
    connect(_groupColorButton, SIGNAL(clicked(bool)),
            this, SLOT(setColorButtonClicked(bool)));
    
    _groupNameEdit = new QLineEdit();
    
    QLabel *label = new QLabel("Group name:");
    
    QVBoxLayout *vbox_left = new QVBoxLayout();
        vbox_left->addWidget(_boundaryGroupList);
        QHBoxLayout *hbox = new QHBoxLayout();
            hbox->addWidget(label);
            hbox->addWidget(_groupNameEdit);
        vbox_left->addLayout(hbox);
    
    
    QVBoxLayout *vbox_right = new QVBoxLayout();
        vbox_right->addWidget(_addButton);
        vbox_right->addWidget(_removeButton);
        vbox_right->setAlignment(_removeButton, Qt::AlignTop);
        vbox_right->addWidget(_groupColorButton);

    
    QGridLayout *layout = new QGridLayout();
        layout->addLayout(vbox_left, 0, 0);
        layout->addLayout(vbox_right, 0, 1);
        QHBoxLayout *hbox_bot = new QHBoxLayout();
            hbox_bot->addWidget(new QLabel("Group bnds:"));
            hbox_bot->addWidget(_bndList);
        layout->addLayout(hbox_bot,1,0,1,2);
    
    setLayout(layout);
    
    _colorDialog = new QColorDialog();
    connect(_colorDialog, SIGNAL(currentColorChanged(const QColor&)),
            this, SLOT(groupColorChanged(const QColor&)));
}

void
BoundaryGroupControllerWidget::setMesh(std::shared_ptr<NetgenNeutralMesh> nnm)
{
    _nnm = nnm;
}

void
BoundaryGroupControllerWidget::addButtonClicked(bool)
{
    if (!_nnm)
        return;
    
    QString group = _groupNameEdit->text();
    
    if (group == "")
        return;
    
    QListWidgetItem *item = new QListWidgetItem( group );
    _boundaryGroupList->addItem(item);
    
    _nnm->groupProps()[group.toStdString()];
    
    _groupNameEdit->clear();
    emit groupsUpdated();
}

void
BoundaryGroupControllerWidget::removeButtonClicked(bool)
{
    if (!_nnm)
        return;
    
    QListWidgetItem *item = _boundaryGroupList->currentItem();
    if (!item)
        return;
    
    _nnm->groupProps().erase(item->text().toStdString());
    
    _boundaryGroupList->takeItem(_boundaryGroupList->currentRow());
    emit groupsUpdated();
}

void
BoundaryGroupControllerWidget::setColorButtonClicked(bool)
{
    _colorDialog->show();
}

void
BoundaryGroupControllerWidget::groupColorChanged(const QColor& color)
{
    QListWidgetItem *item = _boundaryGroupList->currentItem();
    if (!item)
        return;
    
    std::string group = item->text().toStdString();
    
    if (_nnm->groupProps().find(group) == _nnm->groupProps().end())
        return;
    
    _nnm->groupProps().at(group).setColor(color.red()/255.0,
                                          color.green()/255.0,
                                          color.blue()/255.0);
    
    emit meshUpdated();
}

void
BoundaryGroupControllerWidget::boundaryGroupSelected(QListWidgetItem *item)
{
    if (!item || !_nnm)
        return;
    
    std::string group = item->text().toStdString();
    
    QString str;
    
    _nnm->setHighlightBoundariesNone();
    
    for (auto& b : _nnm->boundaries())
    {
        if (b.second.group() == group)
        {
            QTextStream(&str) << b.first << " ";
            b.second.setHighlighted(true);
        }
    }
    emit meshUpdated();
    
    _bndList->setText(str);
}













