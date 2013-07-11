/**********************************************************************
 *                                                                    *
 * File Name: Visu.cc                                                 *
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
#include <iostream>
#include "Visu.hh"
using namespace std;

Visu::Visu(struct visu_arg visu_val)
{
  Nt         =  visu_val.Nt;
  Np         =  visu_val.Np;
  Coort      =  visu_val.Coort;
  Coorp      =  visu_val.Coorp;
  SumMass    =  visu_val.SumMass;
  Ihat       =  visu_val.Ihat;
  area       =  visu_val.area;
  alpha_out  =  visu_val.alpha_out;
  fracmat    =  visu_val.fracmat;
  result     =  visu_val.result;
  coort_file =  visu_val.coort_file;
  coorp_file =  visu_val.coorp_file;
  Refp       =  visu_val.Refp;
  total_area =  visu_val.total_area;
  coortname  =  visu_val.coortname;
  coorpname  =  visu_val.coorpname;
  fracname   =  visu_val.fracname;
  resultname =  visu_val.resultname;

  alpha_node = new double[Np];
  press_node = new double[Np];
  arrival_state = new bool[Np];
  arrival_time = new double[Np];
  for (int i=0; i<Np; i++)
  {
    arrival_time[i] = 1e300;
  }
}


Visu::~Visu()
{
  cout << "Destructor Visu" << endl;
  delete[] alpha_node;
  delete[] press_node;
  delete[] arrival_state;
  delete[] arrival_time;
}

void Visu::update_arival_time(const double *alpha, const double &time)
{
  memset(alpha_node, 0, Np*sizeof(double));
  for (int l=0; l<Nt; l++)
  {
    for (int i=0; i<3; i++)
    {
      int node = Coort[3*l+i];
      alpha_node[node] += Ihat[l][i]*alpha[l]/SumMass[node];
    }
  }

  for (int n=0; n<Np; n++)
  {
    if (! arrival_state[n])
    {
      if (alpha_node[n] > 0.4)
      {
        arrival_state[n]=true;
        arrival_time[n]=time;
      }
    }
  }
}

void Visu::write_arrival_time(const char* result_prefix)
{
  FILE *file;
  char arrival_outname[128];

  strcpy(arrival_outname, result_prefix);
  strcat(arrival_outname, "_arrival_time.mat");
  file=fopen(arrival_outname ,"w");
  for (int i=0; i<Np; i++)
  {
    fprintf(file, "%g\n", arrival_time[i]);
  }
  fclose(file);
}

void Visu::update(double* pressure, double* alpha, int counter) {
  FILE* file;
  int i=0;
  int l=0;
  int node=0;
  double inj_area=0.0;
  int numLeakNodes, numProdNodes, numInjNodes;
  double pLeak, pProd, pInj;

  // save alpha interpolated on nodes
  for (l=0; l<Nt; l++){
    inj_area   += area[l]*alpha[l];
  }

  if (fracmat) {
    file = fopen(fracname,"a");
    fprintf(file, "%i %g\n", counter, inj_area/total_area);
    fclose(file);
  }

  memset(alpha_node, 0, Np*sizeof(double));
  memset(press_node, 0, Np*sizeof(double));

  numLeakNodes =  numProdNodes = numInjNodes = 0;
  pLeak = pProd = pInj = 0.0;

  for (l=0; l<Nt; l++) {
    for (i=0; i<3; i++){
      node=Coort[3*l+i];
      alpha_node[node] += Ihat[l][i]*alpha[l]/SumMass[node];
      press_node[node] += Ihat[l][i]*pressure[l]/SumMass[node];
      //if ( Refp[node] == PRODUCER) {numProdNodes++; pProd +=press_node[node];}
      //if ( Refp[node] == INJECTOR) {numInjNodes++; pInj +=press_node[node];}
      //if ( Refp[node] == LEAK) {numLeakNodes++; pLeak +=press_node[node];}
    }
  }
  for (l=0; l<Np; l++) {
    if ( Refp[l] == PRODUCER) {numProdNodes++; pProd +=press_node[l];}
    if ( Refp[l] == INJECTOR) {numInjNodes++; pInj +=press_node[l];}
    if ( Refp[l] == LEAK) {numLeakNodes++; pLeak +=press_node[l];}
  }

  if (numProdNodes){printf("Pressure producer = %lf from %i points\n",(pProd/(numProdNodes+0.0)), numProdNodes);}
  if (numInjNodes) {printf("Pressure injector = %lf from %i points\n",  (pInj/(numInjNodes+0.0)), numInjNodes);}
  if (numLeakNodes) {printf("Pressure leak = %lf from %i points\n",  (pLeak/(numLeakNodes+0.0)), numLeakNodes);}

  /* For the first time, we print the mesh values in the file */
  if (counter==0)  {
    if (coorp_file) {
      if ( (file = fopen(coorpname,"w")) == NULL ) {
        cerr << "can't open " <<  coorpname << endl;
      }else {
        for (i=0; i<Np; i++) {
          fprintf(file, "%g %g\n", Coorp[2*i], Coorp[2*i+1]);
        }
      fclose(file);
      }
    }

    if (coort_file) {
      if ((file=fopen("coort.mat","w")) == NULL ) {
        cerr << "can't open coort.mat" << endl;
      }else{
        for (i=0; i<Nt; i++) {
          fprintf(file, "%i %i %i\n", Coort[3*i]+1,
                  Coort[3*i+1]+1, Coort[3*i+2]+1);
        }
        fclose(file);
      }
    }

    if (result) {
      file=fopen(resultname ,"w");
      for (i=0; i<Np; i++) {
        fprintf(file, "%g %g\n", press_node[i], alpha_node[i]);
      }
      fclose(file);
    }
  }
  else {
    /* Print the values of the pressure and alpha */
    if (result) {
      file=fopen(resultname,"a");
      for (i=0; i<Np; i++)
        {
          fprintf(file, "%g %g\n", press_node[i], alpha_node[i]);
        }
      fclose(file);
    }
  }
  /* Instruction to see in Matlab in the file FIG/movie_advect.m */
}
