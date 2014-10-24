/**********************************************************************
 *                                                                    *
 * File Name: MixHy.cc                                                *
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
#include "MixHy.hh"

MixHy::MixHy(struct mixhy_arg mixhy_val) : cg() {
  int i=0;
  int j=0;

  cout << "Init class Mixed Hybrid" << endl;

  // Initialisation of the values
  Nt      = mixhy_val.Nt;
  Ne      = mixhy_val.Ne;
  Coore   = mixhy_val.Coore;
  Al      = mixhy_val.Al;
  IyRT    = mixhy_val.IyRT;
  segINJE = mixhy_val.segINJE;
  segPROD = mixhy_val.segPROD;
  invM    = mixhy_val.invM;
  invMl   = mixhy_val.invMl;
  edge    = mixhy_val.edge;
  T_edge  = mixhy_val.T_edge;

  k_p     = mixhy_val.k_p;
  mu1     = mixhy_val.mu1;
  mu2     = mixhy_val.mu2;
  flux_in = mixhy_val.flux_in;
  e_g     = mixhy_val.e_g;

  MAXTIME = mixhy_val.MAXTIME;
  dt      = mixhy_val.dt;
  production_log = mixhy_val.production_log;
  productionname = mixhy_val.productionname;

  leakCumulativeVolume = 0.0;
  producerCumulativeVolume = 0.0;
  leakCumulativeVolumeAlpha = 0.0;
  producerCumulativeVolumeAlpha = 0.0;

  fluid1atProducer = 0;
  fluid1atLeak = 0;

  // Allocate memory in ConjGrad
  cg.init(Ne, Nt, SMALL, Coore);

  // Allocate private arrays
  A  = new double **[Nt];
  g  = new double *[Nt];
  for (i=0; i<Nt; i++){
      A[i] = new double *[3];
      g[i] = new double[3];

      for (j=0; j<3; j++) {
          A[i][j] = new double[3];
        }
    }

  tpress = new double[Ne];
  b      = new double[Ne];
  tp0    = new double[Ne];
  Atp0   = new double[Ne];
  mask   = new int[Ne];

  set_dirichlet();
}


MixHy::~MixHy() {
  int i,j;
  cout << "Destructor MixHy" << endl;

  for (i=0; i<Nt; i++){
      for (j=0; j<3; j++){
          delete [] A[i][j];
        }

      delete[] A[i];
    }
  delete[] A;
  delete[] b;
  delete[] tp0;
  delete[] Atp0;
  delete[] mask;
}

void MixHy::update(double* alpha, double* pressure, double** flux){
  compute_tpress(alpha, g, tpress);
  compute_flux(alpha, g, tpress, flux);
  compute_pressure(alpha, g, tpress, pressure);
}

void MixHy::set_dirichlet() {
  int i=0;
  /*------- Case of the Mixte/Hybrid with the pressure---------------*/
  /* The pressure is fixed on the producer.
     We set mask[i]=0 for those points and 1 for the others.
     This point won't be computed in the conjugate gradient method.  */

  for (i=0; i<Ne; i++) {
      mask[i]=1;
      tp0[i]=0.0;
    }

  for (i=0; i<Ne; i++) {
    if (edge[i]->Ref==PRODUCER) {
      mask[i]=0;
      tp0[i]=0.0;
    }
  }
}



void MixHy::compute_tpress(double* alpha, double** g, double* tpress)
{
  int i=0;
  int val=0;

  /* Creation of the  matrix A (A*tpress=b) */
  createA(alpha, A);

  /* Creation of the g */
  createg(alpha, g);

  /* Creation of the right hand side term (A*tpress=b)  */
  createb(alpha, g, b);

  multiply(Nt, Ne, Coore, A, tp0, Atp0);
  for (i=0; i<Ne; i++) {
    b[i] = b[i] - Atp0[i];
  }

  val=cg.solve(A, b, mask, tpress);


  /* Impose the Dirichlet conditions on the new solution */
  for (i=0; i<Ne; i++) {
    if (mask[i]==0) {
      tpress[i]=tp0[i];
    }
  }
}

void MixHy::compute_flux(double* alpha, double** g, double* tpress, double** flux) {
  /* PUT THE k/mu */
  int l=0;
  int i=0;
  int j=0;
  int Ref=0;
  double trace=0.0;
  double int_prod=0.0;
  double int_inje=0.0;
  double int_leak=0.0;
  double k_mu=0.0;

  for (l=0; l<Nt; l++) {
    k_mu=k_p/( alpha[l]*mu1 + (1.0-alpha[l])*mu2 );
    trace = invMl[l][0] + invMl[l][1] + invMl[l][2];
    for (i=0; i<3; i++) {
      flux[l][i]=0.0;
      for (j=0; j<3; j++) {
        flux[l][i] += k_mu*(invMl[l][i]*invMl[l][j]/trace - invM[l][i][j])*( tpress[Coore[l][j]]+ g[l][j] );
      }

      // integrate the velocity to check global conservation of mass
      Ref=T_edge[l][i]->Ref;
      if ( Ref==PRODUCER ) {
        int_prod += flux[l][i];
        producerCumulativeVolumeAlpha += flux[l][i]*(1-alpha[l])*dt;
        producerCumulativeVolume += flux[l][i]*dt;
        if (alpha[l] > 0.9  && (! fluid1atProducer)) {
          fluid1atProducer++;
          cout << "fluid 1 hit producer"<< endl;
        }
      }
      if ( Ref==INJECTOR ) {
        int_inje += flux[l][i];
      }
    }

    // Test if mass conserved
    if((flux[l][0] + flux[l][1] + flux[l][2])/(flux[l][0]+SMALL) > SMALL &&
       (flux[l][0] + flux[l][1] + flux[l][2])/(flux[l][1]+SMALL) > SMALL &&
       (flux[l][0] + flux[l][1] + flux[l][2])/(flux[l][2]+SMALL) > SMALL  ) {
      cout << "Problem in " << flux[l][0] << " " << flux[l][1] << " "
           << flux[l][2] << " SumFlux="
           << flux[l][0]+flux[l][1]+flux[l][2] << endl;
    }
  }
  cout << "Integration Inj=" << int_inje
       << " Prod="           << int_prod
       << " Leak="           << int_leak
       << " relative=" << (int_inje+int_prod+int_leak)/int_inje << endl;

  if (production_log){
    // write out the cumulative volumes. this shoule be done
    // after the advection routine so it matches well with the
    // other output
    FILE *fp;
    if ( NULL == (fp = fopen(productionname, "a"))) {
      cerr << "error opening " << productionname << endl;
      exit(1);
    }
    fprintf(fp, "%8.8lf %8.8lf %8.8lf %8.8lf\n", producerCumulativeVolume, producerCumulativeVolumeAlpha, leakCumulativeVolume, leakCumulativeVolumeAlpha);

    fclose (fp);
  }
}


void MixHy::compute_pressure(double* alpha, double** g, double* tpress, double* pressure)
{
  int l=0;
  int i=0;
  double k_mu=0.0;

  memset(pressure, 0, Nt*sizeof(double));
  for (l=0; l<Nt; l++)
    {
      k_mu=k_p/( alpha[l]*mu1 + (1.0-alpha[l])*mu2 );
      for (i=0; i<3; i++)
        {
          pressure[l] += invMl[l][i]*(tpress[ Coore[l][i] ] + g[l][i]);
        }
      pressure[l]=k_mu*pressure[l]/(invMl[l][0]+invMl[l][1]+invMl[l][2]);
    }
}



void MixHy::createA(double* alpha, double*** A)
{
  double  k_mu=0.0;
  int     l=0;
  int     i=0;
  int     j=0;

  /* Loop on all triangles */
  for (l=0; l<Nt; l++) {
    k_mu=k_p/( alpha[l]*mu1 + (1.0-alpha[l])*mu2 );
    for (i=0; i<3; i++) {
      for (j=0; j<3; j++) {
        A[l][i][j] = k_mu*Al[l][i][j];
      }
    }
  }
}


void MixHy::createg(double* alpha, double** g)
{
  int        l=0;
  int        i=0;
  double maxgli=0.0;

  for (l=0; l<Nt; l++)
    {
      for (i=0; i<3; i++)
        {
          g[l][i] = alpha[l]*e_g*IyRT[l][i];
          maxgli  = amMAX( maxgli, fabs( g[l][i] ) );
        }
    }
  //cout << "Max gravity contribution = " << maxgli << endl;
}


void MixHy::createb(double* alpha, double** g, double* b)
{
  int        l=0;
  int        i=0;
  int        j=0;
  int      Ref=0;
  double  K_press0=0.1*0.0;

  memset(b, 0, Ne*sizeof(double));

  /* Add the buoyancy contribution given by g */
  for (l=0; l<Nt; l++) {
    for (i=0; i<3; i++) {
      for (j=0; j<3; j++) {
        b[ Coore[l][i] ] += (-A[l][i][j])*g[l][j];
      }
    }
  }

  /* Look at the Neumann condition for the velocity */
  for (l=0; l<Nt; l++) {
    for (i=0; i<3; i++) {
      Ref=T_edge[l][i]->Ref;
      double length = T_edge[l][i]->length;
      if ( Ref != NORMAL ) {
        if ( Ref==BORDER ||  Ref==PRODUCER ) {
          b[ Coore[l][i] ] += 0.0;
        }
        else if ( Ref==INJECTOR ) {
          if ( segINJE != 0 ) {
            /* We have -flux because we take - the equation
               to have a finite POSITIVE matrix             */
            b[ Coore[l][i] ] += -flux_in * length;
            //printf("setting flux at %i %i to %lf\n", l, i, -flux_in * length);
          }
          else {
            printf("Error, segINJE=0\n");
          }
        }
        else { // Ref==LEAK)
          printf("Error, unknown segment code\n");
        }
      }
    }
  }
}


void MixHy::PrintA()
{
  // For debugging purposes
  // Not to use for large systems !!
  FILE* file;
  int i=0;
  int     j=0;
  int     k=0;
  double** AMT=0;

  AMT= new double*[Ne];
  for (i=0; i<Ne; i++)
    {
      AMT[i]=new double[Ne];
      memset(AMT[i], 0, Ne*sizeof(double) );
    }

  for (k=0; k<Nt; k++)
    for (i=0; i<3; i++)
      for (j=0; j<3; j++)
        AMT[ Coore[k][i] ][ Coore[k][j] ] += A[k][i][j];

  for (i=0; i<Ne; i++)
    {
      for (j=0; j<Ne; j++)
        {
          printf("%g ", AMT[i][j]);
        }
      printf("\n");
    }
  printf("\n\n\n");

   for (i=0; i<Ne; i++)
     {
       delete[] AMT[i];
     }
   delete[] AMT;
}
