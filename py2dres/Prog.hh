/**********************************************************************
 *                                                                    *
 * File Name: Prog.hh                                                 *
 *                                                                    *
 * Class Name: Prog                                                   *
 *                                                                    *
 * Goal: Prog class that steers all the others                        *
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
#ifndef _CLASS_Prog_HH_
#define _CLASS_Prog_HH_

#include "Mesh.hh"
#include "MixHy.hh"
#include "IterAlph.hh"
#include <string.h>

class Prog {
public:
  //! struct containing values for the input parameters of the run
  struct prog_arg {
    char    *meshfile;
    double  Rinje;
    double  dt;
    double  mu1;
    double  mu2;
    double  p0;
    double  u_in;
    double  e_g;
    int     MAXTIME;
    int     visu_step;
    int     init_alpha;
    int     coort_file;
    int     coorp_file;
    int     alpha_out;
    int     fracmat;
    int     result;
    int     production_log;
    char    *result_prefix;
  };

public:

  Prog(struct prog_arg prog_val);
  ~Prog();
  void alloc();
  void set_mesh_values(struct Mesh::mesh_data mesh_val);
  void put_mixhy_arg(struct MixHy::mixhy_arg *mixhy_val);
  void put_iteralph_arg(struct IterAlph::iteralph_arg *iteralph_val);

  // python interface to solver
  void updateP();
  void updateA();

public:

  double time_;
  MixHy *mixte_;
  IterAlph *advect_;

  //! The mesh file to read. This must be in .amdba format
  char *meshfile;
  //! saturation value for each triangle on the mesh
  double* alpha;
  //! pressure value for each triangle on the mesh
  double* pressure;
  //! the velocity flux Q across each edge of each triangle
  /*! this is recalculated in each iteration of MixHy
   */
  double** flux;
  //! time step to use
  double dt;
  //! viscosity of fluid 0
  double mu1;
  //! viscosity of fluid 1
  double mu2;
  //! refrence pressure for producer (not currently implemented)
  double p0;
  //! not sure!
  double u_in;
  //! number that the injection velocity is scaled to
  double flux_in;
  //! gravity parameter B
  double e_g;

  //! not sure -- used in AdvectAlpha
  int nloop;
  //! inject fluid 1 into fluid 2 (0 for debugging)
  int inject0_1;
  //! used by AdvectAlpha for greater accuracy
  int limitgrad;
  //! number of time steps to skip between each visualization interval
  int visu_step;
  //! number of time steps to run
  int MAXTIME;
  //! the number of nodes in the mesh
  int     Np;
  //! the number of triangles in the mesh
  int     Nt;
  //! the number of segments(edges) of triangels in the mesh
  int     Ne;
  //! minimum triangle edge length
  double  hmin;
  //! maximum triangle edge length
  double  hmax;
  //! integer array of dim Nt: contains point refrence values
  int*    Refp;
  //! double array of dim 2*Nt: contains x and y values of each point in the mesh
  double* Coorp;
  //! 1d array of doubles dim [3*Nt]: contains the node numbers that make up each triangle
  int*    Coort;
  //! int array dim [Nt][3]: contains segment(edge) refrence numbers
  int**   Coore;
  //! area of each triangle in the mesh
  double* area;
  double* SumMass;
  double** invMl;
  double*** invM;
  double***   Al;
  double**  Ihat;
  double** Ixhat;
  double** Iyhat;
  double**  IyRT;
  //! array of pointers Mesh::Edge structs dim [Nt][3]: give a triangle and an edge and get info on that edge.
  /*! This is a convenience data structure used to get the information
   *  about an edge from knowing the triangle that it is a part of.
   * it points to the appropriate place in the edge data structure
   * */
  struct Mesh::Edge*** T_edge; // (Edge*)[Nt][3] ptr on triangle edges
  //! array of Mesh::Edge structs dim [Ne]: contains the actual Edge struct for each segment(edge) in the mesh
  /*! Ne is < Nt*3 because the triangles share segments(edges) so we
   * want to keep track of the segments(edges)
   */
  struct Mesh::Edge** edge;    // (Edge*)[Ne] ptr on edges
  //! number of injector segments
  int segINJE;
  //! number of producer segments
  int segPROD;
  //! number of border segments
  int segBORD;
  //! the total area of the domain
  /*! the total area of the domain
   *  calculated by summing the area of each triangle that makes up
   *  the mesh. this excludes the wells or other gaps
   */
  double total_area;
  //! radius of the injector used to calculate flux_in
  double Rinje;
  //! permiability of leak in boundary
  int init_alpha;
  //! boolean value should we create a file called coort.mat?
  int  coort_file;
  //! boolean value should we create a file called coorp.mat?
  int  coorp_file;
  //! boolean: parameter should we write saturation(alpha) values to a file after each time step?
  /*! if this value is non zero after each time step a file called
    alpha_out.mat is created an the saturation value for each triangle
    is written. If this file already exists it is destroyed.

    this is different from result.mat because it is written after each time
    step. It should be used to continue a run with different parameters
    etc..
  */
  int  alpha_out;
  //! boolean value should we write a fractional flood area data file?
  /*!
   * If there are no leaks in the boundary then this curve is the same as a
   * fractional production curve. If there are leaks then you need to keep
   * track of what goes in the producer and into or out of the leak
   */
  int fracmat;
  //! write saturation values to result file after each visu_step time steps
  /*!
   * The format of this file is <float> <float>
   * the first number is the presure and the second is the saturation value
   */
  int     result;
  //! Boolean: paramater should we write a file to keep a record of the cumulative volume of fluid that has entered the producer?
  /*! one line is writen to this file for each time step the format is
   * %8.8lf %8.8lf %8.8lf %8.8lf
   *   the first column is the total cumulative volume
   *   and the second is the total cumulative volume of fluid alpha = 0
   *  3rd and 4th are the same for the leak
   *   the units of volume are on the same scale as the domain
   */
  int     production_log;
  //! this string is prefixed to the output files
  char *result_prefix;

  char coortname[128];
  char coorpname[128];
  char fracname[128];
  char resultname[128];
  char productionname[128];
  char alpha_outname[128];

  double *mobility_;

};


#endif  // End of _CLASS_Prog_HH_
