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

#include "Mesh.h"

#define MIN(a,b) ((a < b) ? a : b)
#define MAX(a,b) ((a < b) ? b : a)

/*****************************************************************************/
Point::Point()
    : _x(0), _y(0), _z(0)
{}

Point::Point(double x, double y, double z)
{
    _x = x;
    _y = y;
    _z = z;
}

void
Point::normalize(double max)
{
    _x /= max;
    _y /= max;
    _z /= max;
}

/*****************************************************************************/
Triangle::Triangle()
{
    bzero(_pts, 3*sizeof(size_t));
}

Triangle::Triangle(size_t p0, size_t p1, size_t p2)
{
    _pts[0] = p0;
    _pts[1] = p1;
    _pts[2] = p2;
    std::sort(_pts, _pts+3);
}

std::vector<size_t>
Triangle::points(void) const
{
    std::vector<size_t> ret;
    ret.assign(_pts, _pts+3);
    return ret;
}

bool
Triangle::operator<(const Triangle& other) const
{
    return std::lexicographical_compare(_pts, _pts+3, other._pts, other._pts+3);
}


/*****************************************************************************/
Tetrahedron::Tetrahedron()
{
    bzero(_pts, 4*sizeof(size_t));
}

Tetrahedron::Tetrahedron(size_t p0, size_t p1, size_t p2, size_t p3)
{
    _pts[0] = p0;
    _pts[1] = p1;
    _pts[2] = p2;
    _pts[3] = p3;
    std::sort(_pts, _pts+4);
}

std::vector<size_t>
Tetrahedron::points(void) const
{
    std::vector<size_t> ret;
    ret.assign(_pts, _pts+4);
    return ret;
}

bool
Tetrahedron::operator<(const Tetrahedron& other) const
{
    return std::lexicographical_compare(_pts, _pts+4, other._pts, other._pts+4);
}

/*****************************************************************************/
NetgenNeutralMesh::NetgenNeutralMesh()
{}

bool
NetgenNeutralMesh::read_points(bool verbose)
{
    size_t      n_items;
    double      x, y, z, max_dim;
    bool        first = true;
    
    
    if (verbose)
    {
        std::cout << "Loading points...";
        std::cout.flush();
    }
    
    _filestream >> n_items;
    _points.reserve(n_items);
    
    while (n_items--)
    {
        _filestream >> x >> y >> z;
        
        if (first)
        {
            _min_x = _max_x = x;
            _min_y = _max_y = y;
            _min_z = _max_z = z;
            first = false;
        }
        
        if ( x < _min_x )
            _min_x = x;
        if ( y < _min_y )
            _min_y = y;
        if ( z < _min_z )
            _min_z = z;
        
        if ( x > _max_x )
            _max_x = x;
        if ( y > _max_y )
            _max_y = y;
        if ( z > _max_z )
            _max_z = z;
        
        _points.push_back( Point(x,y,z) );
    }
    
    if(verbose)
        std::cout << "done" << std::endl;
    
    max_dim = MAX(_max_x - _min_x, MAX(_max_y - _min_y, _max_z - _min_z) );
    

    double mid_x = (_max_x + _min_x)/2;
    double mid_y = (_max_y + _min_y)/2;
    double mid_z = (_max_z + _min_z)/2;
    
    for (size_t i = 0; i < _points.size(); i++)
    {
        Point p = _points.at(i);
        _points.at(i) = Point( (p.x() - mid_x)/max_dim,
                               (p.y() - mid_y)/max_dim,
                               (p.z() - mid_z)/max_dim);
    }
    
    return true;
}

bool
NetgenNeutralMesh::read_tets(bool verbose)
{
    size_t n_items;
    size_t p0, p1, p2, p3, dom;
    
    if (verbose)
    {
        std::cout << "Loading tetrahedrons...";
        std::cout.flush();
    }
    
    _filestream >> n_items;
    
    while (n_items--)
    {
        _filestream >> dom >> p0 >> p1 >> p2 >> p3;
        _domains[dom].add(Tetrahedron(p0-1, p1-1, p2-1, p3-1));
    }
    
    if(verbose)
        std::cout << "done" << std::endl;
    
    return true;
}

bool
NetgenNeutralMesh::read_bndtris(bool verbose)
{
    size_t n_items;
    size_t p0, p1, p2, surf;
    
    if (verbose)
    {
        std::cout << "Loading boundary triangles...";
        std::cout.flush();
    }
    
    _filestream >> n_items;
    
    while (n_items--)
    {
        _filestream >> surf >> p0 >> p1 >> p2;
        _boundaries[surf].add(Triangle(p0-1, p1-1, p2-1));
    }
    
    if(verbose)
        std::cout << "done" << std::endl;
    
    return true;
}

bool
NetgenNeutralMesh::load(const std::string& filename, bool verbose)
{
    _points.clear();
    _boundaries.clear();
    _domains.clear();
    
    _filestream.open(filename.c_str());
    if ( !_filestream.is_open() )
    {
        std::cout << "Cannot open " << filename << std::endl;
        return false;
    }
    
    _filename = filename;
    _filestream.setf(std::ios_base::skipws);
    
    read_points(verbose);
    read_tets(verbose);
    read_bndtris(verbose);
    

    if (verbose)
    {
        std::cout << "Points: " << _points.size() << std::endl;
        
        for (auto& me : _domains)
            std::cout << "Domain " << me.first << ": " << me.second.size()
                      << " tetrahedrons" << std::endl;
        
        for (auto& me : _boundaries)
            std::cout << "Boundary " << me.first << ": " << me.second.size()
                      << " triangles" << std::endl;
    }
    
    _filestream.close();
    
    return true;
}

void
NetgenNeutralMesh::setHighlightBoundariesAll(void)
{
    for (auto& b : _boundaries)
        b.second.setHighlighted(true);
}

void
NetgenNeutralMesh::setHighlightBoundariesNone(void)
{
    for (auto& b : _boundaries)
        b.second.setHighlighted(false);
}

void
NetgenNeutralMesh::setHighlightDomainsAll(void)
{
    for (auto& d : _domains)
        d.second.setHighlighted(true);
}

void
NetgenNeutralMesh::setHighlightDomainsNone(void)
{
    for (auto& d : _domains)
        d.second.setHighlighted(false);
}

void
NetgenNeutralMesh::setDisplayBoundariesAll(void)
{
    for (auto& b : _boundaries)
        b.second.enableDisplay(true);
}

void
NetgenNeutralMesh::setDisplayBoundariesNone(void)
{
    for (auto& b : _boundaries)
        b.second.enableDisplay(false);
}

void
NetgenNeutralMesh::setDisplayDomainsAll(void)
{
    for (auto& d : _domains)
        d.second.enableDisplay(true);
}

void
NetgenNeutralMesh::setDisplayDomainsNone(void)
{
    for (auto& d : _domains)
        d.second.enableDisplay(false);
}





