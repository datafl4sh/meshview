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

#pragma once

#include <memory>

#include <QtGui>
#include <QGLWidget>
#include "Mesh.h"

enum WhatToDraw {
    DRAW_TRIANGLES,
    DRAW_TETRAHEDRONS
};

class MeshGLWidget : public QGLWidget
{
    Q_OBJECT
    
private:
    void            prepare_tritet_view(void);
    void            draw_axes(void);
    void            draw_triangles(void);
    void            compile_triangles(void);
    void            draw_tetrahedrons(void);
    void            compile_tetrahedrons(void);
    
    GLfloat         _rotX, _rotY;
    GLfloat         _tranX, _tranY;
    GLfloat         _zoom;
    
    double          _zoom_max;
    
    int             _prevX, _prevY;
    
    WhatToDraw      whatToDraw;
    
    std::shared_ptr<NetgenNeutralMesh> _nnm;
    
protected:
    virtual void    initializeGL();
    virtual void    paintGL();
    virtual void	resizeGL(int, int);
    virtual void    mousePressEvent(QMouseEvent *);
    virtual void    mouseReleaseEvent(QMouseEvent *);
    virtual void    mouseMoveEvent(QMouseEvent *);
    virtual void    wheelEvent(QWheelEvent *);
    
public slots:
    void    setDrawTetrahedrons(void);
    void    setDrawTriangles(void);
    
public:
    MeshGLWidget( QWidget *parent = 0 );
    
    void            setMesh(std::shared_ptr<NetgenNeutralMesh>);
    
};


