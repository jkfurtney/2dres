/**********************************************************************
 *                                                                    *
 * File Name: Mesh.hh                                                 *
 *                                                                    *
 * Class Name: Mesh                                                   *
 *                                                                    *
 * Goal: Compute and store all the mesh related data                  *
 *                                                                    *
 * Copyright (C) 04/2002  Arthur Moncorge                             *
 *                        arthur.moncorge@ensta.org                   *
 *                                                                    *
 * This program is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License        *
 * as published by the Free Software Foundation; either version 2     *
 * of the License, or (at your option) any later version.             *
 *                                                                    *
 * This program is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * along with this program; if not, write to the Free Software        *
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,             *
 * MA 02111-1307, USA.                                                *
 *                                                                    *
 *********************************************************************/

#ifndef _CLASS_MESH_HH_
#define _CLASS_MESH_HH_
#include <stdio.h>



class Mesh
{
public:
  //! holds information about each segment(edge) in the mesh
  struct Edge{
    //! the edge number of the segment(edge)
    int n;
    //! node number that makes up one end of the segment
    int vertex1;
    //! node number that makes up one end of the segment
    int vertex2;
    //! this is the number of the triangle that the segment(edge) is a part of
    int k;
    //! not sure but it is set to -1 in Mesh::meshEdge
    int l;
    //! ?? the segment(edge) number[0-2] of the edge in the triangle when encounterd first used in advection routine
    int kseg;
    //! ?? the segment(edge) number[0-2] of the edge in the triangle when encounterd second  used in advection routine
    int lseg;
    //! length of the segment
    double length;
    //! refrence value of the segment
    int Ref;
    //! used during setting of the edges in Mesh::meshEdge
    /*! is seems to provide some kind of unique id to each segment
     *  it is used only in Mesh::meshEdge and is not seen again
     */
    double xp;
    //! used during setting of the edges in Mesh::meshEdge
    double yp;
  };
  //! struct used to pass information about mesh arround
  /*! This structure holds information about the mesh. It is passed back to
   *  the Prog class and is used to initilise the MixHy class
   */
  struct mesh_data{
    int     Np;
    int     Nt;
    int     Ne;
    double  hmin;
    double  hmax;
    int*    Refp;
    double* Coorp;
    int*    Coort;
    int**   Coore;
    double* area;
    double* SumMass;
    double** invMl;
    double*** invM;
    double***   Al;
    double**  Ihat;
    double** Ixhat;
    double** Iyhat;
    double**  IyRT;
    struct Edge*** T_edge;
    struct Edge** edge;
    int segINJE;
    int segPROD;
    int segBORD;
    double total_area;
  };
public:
  Mesh(char* meshfile);
  ~Mesh();
  void meshRead(FILE* fd);
  //! build up data structures that describe the segments(edges) of the mesh
  /*! The member function loops through all the triangles and all the
    possible segments and compiles information needed later in the
    advection and other computations
    specifically T_edges edges Coore and Ne are initialized
  */
  void meshEdge();
  //! this is an internal use function used by meshRead
  void increase_array(int size, double** cx, double** cy);
  //! compute arrays used in matrix calculations
  /*! For each triangle, this subroutine computes matrices that will
   * be use to build the matrices of the Poisson solver or the Darcy's
   *  law */
  void meshCompute();
  void test_orientation(int k);
  //! return information about the mesh
  void get(struct mesh_data *mesh_val);

public:
  int     Np;
  int     Nt;
  int     Ne;
  double  hmin;
  double  hmax;
  int*    Refp;
  double* Coorp;
  int*    Coort;
  int**   Coore;
  double* area;
  double* SumMass;
  double** invMl;
  double*** invM;
  double***   Al;
  double**  Ihat;
  double** Ixhat;
  double** Iyhat;
  double**  IyRT;
  struct Edge*** T_edge; // (Edge*)[Nt][3] ptr on triangle edges
  struct Edge** edge;    // (Edge*)[Ne] ptr on edges
  int segINJE;
  int segPROD;
  int segBORD;
  double total_area;
  double injLength_; // length of all injector segments
  double prodLength_;
  double borderLength_;
};


#endif  // End of _CLASS_MESH_HH_
