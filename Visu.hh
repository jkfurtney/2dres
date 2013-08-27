/**********************************************************************
 *                                                                    *
 * File Name: Visu.hh                                                 *
 *                                                                    *
 * Class Name: Visu                                                   *
 *                                                                    *
 * Goal:  Print out .mat files to visualise the results               *
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

#ifndef _CLASS_VISU_HH_
#define _CLASS_VISU_HH_

#include <iostream>
#include "stdio.h"
#include <string.h>
#include "mylibrary.hh"
#include "Mesh.hh"



class Visu {
public:

  struct visu_arg {
    int Nt;
    int Np;
    int* Coort;
    double* Coorp;
    double* SumMass;
    double** Ihat;
    double* area;
    int alpha_out;
    int fracmat;
    int result;
    int coort_file;
    int coorp_file;
    int *Refp;
    double total_area;
    char *coortname;
    char *coorpname;
    char *fracname;
    char *resultname;
  };


  Visu(struct visu_arg visu_val);
  ~Visu();
  void update(double* pressure, double* alpha, int counter);
  void update_arival_time(const double *alpha, const double &time);
  void write_arrival_time(const char *filename);
  void write_well_flux(const char *filename, double **flux,
                       struct Mesh::Edge*** T_edge);


public:
  int   Nt;
  int   Np;
  int* Coort;
  int* Refp;
  double* Coorp;
  double* SumMass;
  double** Ihat;
  double* area;
  double total_area;

  // Working arrays
  double* alpha_node;
  double* press_node;

  // visualization options
  int     coort_file;
  int     coorp_file;
  int     alpha_out;
  int     fracmat;
  int     result;
  char *coortname;
  char *coorpname;
  char *fracname;
  char *resultname;

  bool *arrival_state;
  double *arrival_time;

};


#endif  // End of _CLASS_MIXHY_HH_
