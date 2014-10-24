/**********************************************************************
 *                                                                    *
 * File Name: MixHy.hh                                                *
 *                                                                    *
 * Class Name: MixHy                                                  *
 *                                                                    *
 * Goal:  Mixed Hybrid Finite Element Method                          *
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
#ifndef _CLASS_MIXHY_HH_
#define _CLASS_MIXHY_HH_

#include <iostream>
#include <math.h>
#include "ConjGrad.hh"
#include "Mesh.hh"

class MixHy {
public:
  struct mixhy_arg{
    int   Nt;
    int   Ne;
    int** Coore;
    double*** Al;
    double** IyRT;
    int segINJE;
    int segPROD;
    double*** invM;
    double**  invMl;
    struct Mesh::Edge**  edge;
    struct Mesh::Edge*** T_edge;

    double mu1;
    double mu2;
    double flux_in;
    double e_g;
    int MAXTIME;
    double dt;
    int production_log;
    char *productionname;
    double *mobility;
  };


public:
  MixHy(struct mixhy_arg mixhy_val);
  ~MixHy();
  void update(double* alpha, double* pressure, double** flux);
  void set_dirichlet();
  void compute_tpress(double* alpha, double** g, double* tpress);
  void compute_flux(double* alpha, double** g, double* tpress, double** flux);
  void compute_pressure(double* alpha, double** g, double* tpress, double* pressure);
  void createg(double* alpha, double** g);

  void createA(double* alpha, double*** A);
  void createb(double* alpha, double** g, double* b);
  void PrintA();

public:
  /* Check which are private */
  // Constants
  int   Nt;
  int   Ne;
  double mu1;
  double mu2;
  double flux_in;
  double e_g;
  int MAXTIME;

  // Working arrays
  double* tpress;
  double**  g;
  int*   mask;
  double*** A;
  double*   b;
  double* tp0;
  double* Atp0;

  // Data from mesh
  int** Coore;
  double*** Al;
  double** IyRT;
  int segINJE;
  int segPROD;
  double*** invM;
  double**  invMl;
  struct Mesh::Edge**  edge;
  struct Mesh::Edge*** T_edge;

  double dt;
  //! boolean value: has fluid 1 (alpha = 1) hit the leak
  int fluid1atProducer;
  //! boolean value: has fluid 1 (alpha = 1) hit the producer
  int fluid1atLeak;
  //! the cumulative volume that has flown from or into the leak
  double leakCumulativeVolume;
  //! the cumulative volume that has flown from or into the producer
  double producerCumulativeVolume;
  //! the cumulative volume of fluid 1 (alpha = 0) that has flown from or into the leak
  double leakCumulativeVolumeAlpha;
  //! the cumulative volume of fluid 1 (alpha = 0) that has flown out of the producer
  double producerCumulativeVolumeAlpha;

  int production_log;
  char *productionname;
  double *mobility_;
  // Other class
  ConjGrad cg;
};


#endif  // End of _CLASS_MIXHY_HH_
