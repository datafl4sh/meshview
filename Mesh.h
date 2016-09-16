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

#include <vector>
#include <map>
#include <fstream>
#include <set>
#include <cmath>
#include <iostream>

#include <QGLWidget>

/*******************************************************************/
class Point
{
    double  _x, _y, _z;
    
public:
    Point();
    Point(double, double, double);
    
    double  x() const { return _x; }
    double  y() const { return _y; }
    double  z() const { return _z; }
    
    double
    distanceFrom(const Point& other) const
    {
        double dx, dy, dz;
        
        dx = _x - other._x;
        dy = _y - other._y;
        dz = _z - other._z;
        
        return sqrt(dx*dx + dy*dy + dz*dz);
    }
    
    void    normalize(double);
};

/*******************************************************************/
class Triangle
{
    size_t  _pts[3];
    
public:
                            Triangle();
                            Triangle(size_t, size_t, size_t);
    std::vector<size_t>     points(void) const;
    bool                    operator<(const Triangle&) const;
};

/*******************************************************************/
class Tetrahedron
{
    size_t  _pts[4];
    
public:
                            Tetrahedron();
                            Tetrahedron(size_t, size_t, size_t, size_t);
    std::vector<size_t>     points(void) const;
    bool                    operator<(const Tetrahedron&) const;
    
};

/*******************************************************************/
template<typename T>
class MeshZone
{
    std::vector<T>          _objects;
    bool                    _display_enabled;
    bool                    _highlighted;
    GLuint                  _list;
    GLfloat                 _red, _green, _blue, _alpha;
    
    std::string             _groupname;
    
    double                  _minEdgeLength, _maxEdgeLength, _avgEdgeLength;
    bool                    _lengthsValid;
    
    void calculateLengths(const std::vector<Point>& _points)
    {
        bool first = true;
        
        std::set<std::pair<size_t, size_t>> _edges;
        
        for(auto& o : _objects)
        {
            auto pts = o.points();
            
            for (size_t i = 0; i < pts.size(); i++)
                for (size_t j = i+1; j < pts.size(); j++)
                    _edges.insert( std::make_pair(pts.at(i), pts.at(j)) );
        }
        
        for (auto& e : _edges)
        {
            double d = _points.at(e.first).distanceFrom(_points.at(e.second));
            
            if (first)
            {
                _minEdgeLength = d;
                _maxEdgeLength = d;
                _avgEdgeLength = d;
                first = false;
                continue;
            }
            
            if ( d < _minEdgeLength )
                _minEdgeLength = d;
            
            if ( _maxEdgeLength < d )
                _maxEdgeLength = d;
            
            _avgEdgeLength += d;
        }
        
        _avgEdgeLength /= _edges.size();
        
        _lengthsValid = true;
    }
    
public:
    MeshZone() : _display_enabled(true),
                 _highlighted(false),
                 _red(1), _green(0), _blue(0),
                 _alpha(0),
                 _lengthsValid(false)
    {}
    
    double minEdgeLength(const std::vector<Point>& _points)
    {
        if (!_lengthsValid)
            calculateLengths(_points);
        
        return _minEdgeLength;
    }
    
    double maxEdgeLength(const std::vector<Point>& _points)
    {
        if (!_lengthsValid)
            calculateLengths(_points);
        
        return _maxEdgeLength;
    }
    
    double avgEdgeLength(const std::vector<Point>& _points)
    {
        if (!_lengthsValid)
            calculateLengths(_points);
        
        return _avgEdgeLength;
    }
    
    void
    add(const T& obj) { _objects.push_back(obj); }
    
    size_t
    size(void) { return _objects.size(); }
    
    bool
    displayEnabled(void) { return _display_enabled; }
    
    void
    enableDisplay(bool en) { _display_enabled = en; }
    
    std::vector<T>&
    objects(void) { return _objects; }
    
    GLuint
    list(void) { return _list; }
    
    void
    setList(GLuint list) { _list = list; }
    
    void
    setHighlighted(bool en) { _highlighted = en; }
    
    bool
    highlighted(void) { return _highlighted; }
    
    GLfloat
    alpha(void) { return _alpha; }

    GLfloat
    red(void) { return _red; }
    
    GLfloat
    green(void) { return _green; }
    
    GLfloat
    blue(void) { return _blue; }
    
    void
    setColor(GLfloat red, GLfloat green, GLfloat blue) {
        _red = red;
        _green = green;
        _blue = blue;
    }
    
    void
    setAlpha(GLfloat alpha) { _alpha = alpha; }
    
    void
    setGroup(const std::string& groupname)
    {
        std::cout << "Assigned group " << groupname << std::endl;
        _groupname = groupname;
    }
    
    std::string
    group(void)
    {
        return _groupname;
    }
};

typedef MeshZone<Triangle>      Boundary;
typedef MeshZone<Tetrahedron>   Domain;

class GroupProperties
{
    GLfloat _red, _green, _blue;

public:
    GroupProperties() : _red(0), _green(0), _blue(0)
    {}
    
    void
    setColor(GLfloat red, GLfloat green, GLfloat blue) {
        _red = red;
        _green = green;
        _blue = blue;
    }
    
    GLfloat red() { return _red; }
    GLfloat green() { return _green; }
    GLfloat blue() { return _blue; }
    
};

/*******************************************************************/
class NetgenNeutralMesh
{
    std::string                     _filename;
    std::ifstream                   _filestream;

    std::vector<Point>              _points;
    std::map<size_t, Boundary>      _boundaries;
    std::map<size_t, Domain>        _domains;
    
    std::map<std::string, GroupProperties>  _elemGroupProps;
    
    double _min_x, _min_y, _min_z, _max_x, _max_y, _max_z;
    
    bool    read_points(bool);
    bool    read_tets(bool);
    bool    read_bndtris(bool);
    
public:
    NetgenNeutralMesh();

    bool    load(const std::string&, bool verbose = false);
    
    std::vector<Point>&             points(void) { return _points; }
    std::map<size_t, Boundary>&     boundaries(void) { return _boundaries; }
    std::map<size_t, Domain>&       domains(void) { return _domains; }
    
    std::map<std::string, GroupProperties>& groupProps(void)
    {
        return _elemGroupProps;
    }
    
    std::string filename(void) { return _filename; }
    
    void        setHighlightBoundariesAll(void);
    void        setHighlightBoundariesNone(void);
    void        setHighlightDomainsAll(void);
    void        setHighlightDomainsNone(void);
    
    void        setDisplayBoundariesAll(void);
    void        setDisplayBoundariesNone(void);
    void        setDisplayDomainsAll(void);
    void        setDisplayDomainsNone(void);
    
    double      min_x() { return _min_x; }
    double      max_x() { return _max_x; }
    double      min_y() { return _min_y; }
    double      max_y() { return _max_y; }
    double      min_z() { return _min_z; }
    double      max_z() { return _max_z; }
    
};


