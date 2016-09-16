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

#include <iostream>
#include <algorithm>

#include "MeshGLWidget.h"

#define MIN(a,b) ((a < b) ? a : b)
#define MAX(a,b) ((a < b) ? b : a)

MeshGLWidget::MeshGLWidget(QWidget *parent)
    : QGLWidget(parent),
      _nnm(nullptr)
{
    _rotX = _rotY = 0.0;
    _tranX = _tranY = 0.0;
    _prevX = _prevY = 0;
    
    _zoom = 1.0;
    _zoom_max = 3.0;
    
    resize(700,700);
    
    whatToDraw = DRAW_TETRAHEDRONS;
}

void
MeshGLWidget::initializeGL()
{
    //glShadeModel( GL_SMOOTH );
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //RGBA
    glClearDepth(1.0f);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glEnable(GL_LIGHTING);
    
    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void
MeshGLWidget::resizeGL( int w, int h )
{
    glViewport(0, 0, w, h);
 
    /*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)w/ (float)h;
    glOrtho(-2*aspect, 2*aspect, -2, 2, -4, 4);
    
    glMatrixMode(GL_MODELVIEW);
     */
}

void
MeshGLWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    
    draw_axes();
    prepare_tritet_view();
    switch (whatToDraw)
    {
            
        case DRAW_TRIANGLES:
            draw_triangles();
            break;
            
        case DRAW_TETRAHEDRONS:
            draw_tetrahedrons();
            break;
    }
}

void
MeshGLWidget::draw_axes()
{
    glLineWidth(3);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width()/2, width()/2, -height()/2, height()/2, -100, 100);
    
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glTranslatef(-width()/2 + 40, -height()/2 + 40, 0.0f);
    glRotatef(_rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(_rotY, 0.0f, 1.0f, 0.0f);
    
    glBegin(GL_LINES);
        glColor4f(1.0, 0.0, 0.0, 0.3);
        glVertex3f(0, 0, 0);
        glVertex3f(30, 0, 0);
    
        glColor4f(0.0, 1.0, 0.0, 0.3);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 30, 0);
        
        glColor4f(0.0, 0.0, 1.0, 0.3);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 30);
    glEnd();
    
    //glPopMatrix();
}

void
MeshGLWidget::prepare_tritet_view(void)
{
    double scalefact = 4.0/5.0;
    double aspect = width()/(double)height();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-scalefact*aspect, scalefact*aspect,
            -scalefact, scalefact,
            -_zoom_max*scalefact, _zoom_max*scalefact);
    
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glTranslatef(_tranX, _tranY, 0);
    glRotatef(_rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(_rotY, 0.0f, 1.0f, 0.0f);
    glScalef(_zoom, _zoom, _zoom);
}

void
MeshGLWidget::compile_triangles(void)
{
    if (!_nnm)
        return;
    
    for ( auto& b : _nnm->boundaries() )
    {
        GLuint list;
        
        list = glGenLists(1);
        
        glNewList(list, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        
        for (auto& t : b.second.objects())
        {
            auto pts = t.points();
            glVertex3f(_nnm->points()[pts.at(0)].x(),
                       _nnm->points()[pts.at(0)].y(),
                       _nnm->points()[pts.at(0)].z());
            
            glVertex3f(_nnm->points()[pts.at(1)].x(),
                       _nnm->points()[pts.at(1)].y(),
                       _nnm->points()[pts.at(1)].z());

            glVertex3f(_nnm->points()[pts.at(2)].x(),
                       _nnm->points()[pts.at(2)].y(),
                       _nnm->points()[pts.at(2)].z());
            
        }
        
        glEnd();
        glEndList();
        
        b.second.setList(list);
    }
}

void
MeshGLWidget::draw_triangles(void)
{
    if (!_nnm)
        return;
    
    glLineWidth(1);
    
    for ( auto& b : _nnm->boundaries() )
    {
        
        if ( !b.second.displayEnabled() )
            continue;
        
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
        
        
        if (b.second.highlighted())
            glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
        else
            glColor4f(0.3f, 0.3f, 0.3f, 0.0f);
        
        glCallList( b.second.list() );
    }
}

void
MeshGLWidget::compile_tetrahedrons(void)
{
    if (!_nnm)
        return;
    
    for ( auto& d : _nnm->domains() )
    {
        GLuint list;
        
        list = glGenLists(1);
        
        glNewList(list, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        
        for (auto& t : d.second.objects())
        {
            auto pts = t.points();
            ///////////////////////////////////////////
            glVertex3f(_nnm->points()[pts.at(0)].x(),
                       _nnm->points()[pts.at(0)].y(),
                       _nnm->points()[pts.at(0)].z());
            
            glVertex3f(_nnm->points()[pts.at(1)].x(),
                       _nnm->points()[pts.at(1)].y(),
                       _nnm->points()[pts.at(1)].z());
            
            glVertex3f(_nnm->points()[pts.at(2)].x(),
                       _nnm->points()[pts.at(2)].y(),
                       _nnm->points()[pts.at(2)].z());
            
            ///////////////////////////////////////////
            glVertex3f(_nnm->points()[pts.at(0)].x(),
                       _nnm->points()[pts.at(0)].y(),
                       _nnm->points()[pts.at(0)].z());
            
            glVertex3f(_nnm->points()[pts.at(1)].x(),
                       _nnm->points()[pts.at(1)].y(),
                       _nnm->points()[pts.at(1)].z());
            
            glVertex3f(_nnm->points()[pts.at(3)].x(),
                       _nnm->points()[pts.at(3)].y(),
                       _nnm->points()[pts.at(3)].z());
            
            ///////////////////////////////////////////
            glVertex3f(_nnm->points()[pts.at(0)].x(),
                       _nnm->points()[pts.at(0)].y(),
                       _nnm->points()[pts.at(0)].z());
            
            glVertex3f(_nnm->points()[pts.at(2)].x(),
                       _nnm->points()[pts.at(2)].y(),
                       _nnm->points()[pts.at(2)].z());
            
            glVertex3f(_nnm->points()[pts.at(3)].x(),
                       _nnm->points()[pts.at(3)].y(),
                       _nnm->points()[pts.at(3)].z());
            
            ///////////////////////////////////////////
            glVertex3f(_nnm->points()[pts.at(1)].x(),
                       _nnm->points()[pts.at(1)].y(),
                       _nnm->points()[pts.at(1)].z());
            
            glVertex3f(_nnm->points()[pts.at(2)].x(),
                       _nnm->points()[pts.at(2)].y(),
                       _nnm->points()[pts.at(2)].z());
            
            glVertex3f(_nnm->points()[pts.at(3)].x(),
                       _nnm->points()[pts.at(3)].y(),
                       _nnm->points()[pts.at(3)].z());
            
        }
        
        glEnd();
        glEndList();
        
        d.second.setList(list);
    }
}

void
MeshGLWidget::draw_tetrahedrons(void)
{
    if (!_nnm)
        return;
    
    glLineWidth(1);
    
    for ( auto& d : _nnm->domains() )
    {
        GLfloat alpha = 0.0;
        
        if ( !d.second.displayEnabled() )
            alpha=0.98;
        
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(d.second.red(), d.second.green(),
                  d.second.blue(), /*d.second.alpha()*/ alpha);
        
        /*
         if (sd.second.highlight)
         glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
         else
         glColor4f(0.3f, 0.3f, 0.3f, _mesh_alpha);
         */
        glCallList( d.second.list() );
    }
}


void
MeshGLWidget::setDrawTetrahedrons(void)
{
    whatToDraw = DRAW_TETRAHEDRONS;
    updateGL();
}

void
MeshGLWidget::setDrawTriangles(void)
{
    whatToDraw = DRAW_TRIANGLES;
    updateGL();
}

void
MeshGLWidget::mousePressEvent(QMouseEvent *e)
{
    _prevX = e->x();
    _prevY = e->y();
}

void
MeshGLWidget::mouseReleaseEvent(QMouseEvent *)
{
}

void
MeshGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    Qt::MouseButtons btns = e->buttons();
    
    //std::cout << e->x() << " " << e->y() << std::endl;
    
    
    if (btns == Qt::LeftButton)
    {
        if (e->x() > _prevX)
            _rotY += 6/_zoom;
        else if (e->x() < _prevX)
            _rotY -= 6/_zoom;
        
        if (e->y() > _prevY)
            _rotX += 6/_zoom;
        else if (e->y() < _prevY)
            _rotX -= 6/_zoom;
    }
    
    /*
    if (btns == Qt::RightButton)
    {
        if (e->x() > prevX) tranX += 0.1;
        else if (e->x() < prevX) tranX -= 0.1;
        
        if (e->y() > prevY) tranY -= 0.1;
        else if (e->y() < prevY) tranY += 0.1;
    }
    */
    _prevX = e->x();
    _prevY = e->y();
    
    updateGL();
}

void
MeshGLWidget::wheelEvent(QWheelEvent *e)
{
    if (e->delta() > 0)
        _zoom += 0.1*_zoom;
    else if (e->delta() < 0)
        if (_zoom > 0.2)
            _zoom -= 0.1*_zoom;
    
    if (_zoom < 1/_zoom_max)
        _zoom = 1/_zoom_max;
    
    if (_zoom > _zoom_max)
        _zoom = _zoom_max;
    
    e->accept();
    
    updateGL();
}

void
MeshGLWidget::setMesh(std::shared_ptr<NetgenNeutralMesh> nnm)
{
    _nnm = nnm;
    compile_triangles();
    compile_tetrahedrons();
    update();
}







