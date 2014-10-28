/**********************************************************************
 *                                                                    *
 * File Name: IterAlpha.hh                                            *
 *                                                                    *
 * Class Name: IterAlpha                                              *
 *                                                                    *
 * Goal: Advection of alpha with a finite volume method               *
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
#ifndef _CLASS_ITERALPH_HH_
#define _CLASS_ITERALPH_HH_

#include<iostream>
#include "Mesh.hh"  // for T_edge

class IterAlph {
public:
  struct iteralph_arg {
    int   Nt;
    int   Np;
    double dt;
    double flux_in;
    int segINJE;
    int nloop;
    int limitgrad;

    // Data arrays
    double* area;
    struct Mesh::Edge*** T_edge;

    // For limitgrad only
    double** Ihat;
    double** Ixhat;
    double** Iyhat;
    int*     Coort;
    double*  Coorp;
    double*  SumMass;
  };


  //! get global values
  IterAlph(struct iteralph_arg iteralph_val);
  ~IterAlph();

  //! not sure what this is! this could be a mistake
  void init(struct iteralph_arg iteralph_val);

  //! this function is called by prog to start the advection for each timestep
  void iteration_alpha(double** flux, double* alpha);

  //! take weighted averages of previous fluxes fn fnm1 fnm2 for this timesteps flux.
  void extrap_velflux(int N, double frac,
                      double** fnm2, double** fnm1, double** fn, double** f);
  //! given alpha_flux calculated in create_alphaflux advect the fluid into
  void advection_alpha(double t, double* alpha, double** flux,
                       double* new_alpha);
  //! calculate the flux of alpha along the segments (from flux and alpha from provious computation)  populate alpha_flux
  /*! alpha in this function is not `global' alpha. here alpha is a [3][Nt]
      which is the alpha on each segment
      -- this function is called several times.
   */
  void create_alphaflux(double** alpha, double** flux, double** alpha_flux);

  //!
  void limit_grad(double** alpha);

  //!
  void limit_grad3(double** alpha);


public:

  int   Nt;
  int   Np;
  double dt;
  double flux_in;
  int segINJE;
  int nloop;
  int limitgrad;

  // Data arrays
  double* area;    // double[Nt] area of each triangle
  struct Mesh::Edge*** T_edge; //(Edge*)[Nt][3] ptr on edges of triangle

  // For limitgrad only
  // these are test functions from Mesh::compute_mesh
  double** Ihat;
  double** Ixhat;
  double** Iyhat;
  int*     Coort;
  double*  Coorp;

  double*  SumMass;

  // Working arrays
  //! double for each triangle for each edge
  double** alpha_flux;
  //! flux from previous time step (t-3)
  double**       fnm2;
  //! flux from previous time step (t-2)
  double**       fnm1;
  //! flux from previous time step (t-1)
  double**         fn;
  //! [3][Nt] double
  double**  alpha_seg;
  //! alpha on each point used by limitgrad
  double*  alpha_node;
  //! temporary holding place for new alpha values
  double*   new_alpha;
  //! internaly used nonzero for first time step only.
  int test_first;
  double inject_alpha_;
};


#endif  // End of _CLASS_ITERALPH_HH_
