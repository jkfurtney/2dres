#include"ConjGrad.hh"
/**********************************************************************
 *                                                                    *
 * File Name: ConjGrad.cc                                             *
 *                                                                    *
 * Class Name: ConjGrad                                               *
 *                                                                    *
 * Goal: This a conjugate gradient with incomplete Cholesky           *
 *       preconditioner for sparse matrix                             *
 *                                                                    *
 * Solve A*x=b with A[Nt][3][3] the local matrix                      *
 *                                                                    *
 * Coort[Nt][3] has values between 0 and N-1, gives                   *
 *  the 3 vertices or the 3 nodes of each triangle                    *
 *                                                                    *
 * b[N] is the left hand side,                                        *
 *                                                                    *
 * x[N] is in input a initialized vector, (if we rougthly know the    *
 * expected value) and the solution in output                         *
 *                                                                    *
 * mask[N] int array, mask[i]=1 if we compute the ith value in x, =0  *
 * if not. It is used for Diriclet conditions when we already know    *
 * the value.                                                         *
 *                                                                    *
 * x=xD+x0, as we do not compute xD (Dirichlet), we have A*x=b        *
 * equivalent to A*x0=b-A*xD.                                         *
 * This is this last system that we compute.                          *
 *                                                                    *
 * If the code crashes, first increase Nterm                          *
 * This is the maximum number of non-0 values for a row               *
 * of the sparse matrix A                                             */
#define Nterm 10

/* Copyright (C) 04/2002  Arthur Moncorge                             *
 *                        arthur.moncorge@ensta.org                   *
 *                        from Mats Nigam's Fortran code              *
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

void  ConjGrad::init(int iN, int iNt, double itol, int** iCoort)
{
  int i;

  // Steering parameter, maximum of non-zero values on a line
  N=iN;
  Nt=iNt;
  tol=itol;
  Coort=iCoort;

  r  = new double[N];
  w  = new double[N];
  Aw = new double[N];
  q  = new double[N];

  Aspar  = new double *[N];
  chol   = new double *[N];
  cholt  = new double *[N];
  indexA = new int *[N];

  for (i=0; i<N; i++)
    {
      Aspar[i] = new double[Nterm];
       chol[i] = new double[Nterm];
      cholt[i] = new double[Nterm];
     indexA[i] = new int[Nterm];
    }

  rowsize = new int[N];
  diag    = new int[N];
}

int ConjGrad::solve(double*** A, double* b, int* mask, double* x)
{
  int       k=0;
  int       i=0;
  int    node=0;
  int    loop=0;
  double alpha=0.0;
  double beta=0.0;
  double qdotr=0.0;
  double qdotr_save=0.0;
  double wdotAw;
  double err=0.0;
  double tol2=tol*tol;

  memset( r, 0, N*sizeof(double));
  memset( w, 0, N*sizeof(double));
  memset(Aw, 0, N*sizeof(double));
  memset( q, 0, N*sizeof(double));

  /* Create the preconditioning matrixes
     chol and cholt                       */
  sparceA(A, Aspar, indexA, diag, rowsize);
  cholesky_fact(Aspar, indexA, diag, chol, cholt);

  /* Initialisation */
  qdotr=0.0;
  err=0.0;
  multiply(Nt, N, Coort, A, x, r);
  for (i=0; i<N; i++)
    {
      r[i]  =  b[i]-r[i];
      r[i] *= (double) mask[i];
    }

  /* Preconditioning */
  lower(N, diag, indexA, chol, r, b);
  upper(N, diag, indexA, rowsize, cholt, b, q);

  for (i=0; i<N; i++)
    {
      qdotr += q[i]*r[i];
      err   += r[i]*r[i];
      w[i] = q[i];
    }

  loop=0;
  while( err > tol2 )/*&& loop < N )*/
    {
      loop++;

      memset(Aw, 0, N*sizeof(double));
      multiply(Nt, N, Coort, A, w, Aw);

      wdotAw=0.0;
      for (i=0; i<N; i++)
        {
          Aw[i] *= (double) mask[i];
          wdotAw += Aw[i]*w[i];
        }

      if (qdotr==0.0)
        alpha=0.0;
      else if (wdotAw==0.0)
        cout << "Error in conjgrad_chol" << endl;
      else
        alpha=qdotr/wdotAw;

      qdotr_save=qdotr;
      err=0.0;

      qdotr=0.0;
      for (i=0; i<N; i++)
        {
          /* Iteration of x and r */
          x[i] +=  alpha* w[i];
          r[i] += -alpha*Aw[i];
          r[i] *=  (double) mask[i];
        }

      /* Preconditioning */
      lower(N, diag, indexA, chol, r, b);
      upper(N, diag, indexA, rowsize, cholt, b, q);

      for (i=0; i<N; i++)
        {
          qdotr += q[i]*r[i];
          err   += r[i]*r[i];
        }

      /* Compute the new direction of the iteration */
      if (qdotr==0.0)
        beta=0.0;
      else if (qdotr_save==0.0)
        printf("Error in conjgrad_chol\n");
      else
        beta=qdotr/qdotr_save;

      for (i=0; i<N; i++)
        {
          w[i] = q[i]+beta*w[i];
          w[i] *= (double) mask[i];
        }
    } /* while */
  cout << "sqrt=" << sqrt(err) << ", tol=" << tol << ", loop="
       << loop << ", N=" << N << endl;
  if( loop < N )
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

void ConjGrad::sparceA(double*** A, double** Aspar,
                       int** indexA, int* diag, int* rowsize)
{
  int i=0;
  int j=0;
  int k=0;
  int l=0;
  int L=0;
  int node1=0;
  int node2=0;
  int find=0;

  for (i=0; i<N; i++)
    {
      for (j=0; j<Nterm; j++)
        {
          Aspar[i][j]=0.0;
          indexA[i][j]=-1;
        }
      rowsize[i]=0;
      diag[i]=-1;
    }

  for (k=0; k<Nt; k++)
    {
      for (i=0; i<3; i++)
        {
          node1=Coort[k][i];

          for (j=0; j<3; j++)
            {
              node2=Coort[k][j];

              if ( rowsize[node1]==0 )
                {
                  /* This is a new value */
                  indexA[node1][0] = node2;
                   Aspar[node1][0] = A[k][i][j];
                  if( node1==node2 )
                    {
                      diag[node1]=0;
                    }
                  rowsize[node1]=1;
                }
              else if( node2 > indexA[node1][rowsize[node1]-1] )
                {
                  /* This is a new value */
                  indexA[node1][ rowsize[node1] ] = node2;
                   Aspar[node1][ rowsize[node1] ] = A[k][i][j];
                  if( node1==node2 )
                    {
                      diag[node1]=rowsize[node1];
                    }
                  rowsize[node1]++;
                }
              else
                {
                  /* Either we already have a contribution to
                     A[node1][node2] in Aspar or we have a
                     contribution of a A[node1][node] with
                     node>node2                               */
                  find=0;
                  L=0;
                  while( L<rowsize[node1] && find==0 )
                    {
                      if ( node2 <= indexA[node1][L] )
                        {
                          find=1;
                          if ( node2 < indexA[node1][L] )
                            {
                              /* this is a contribution to A[node1][node2]
                                 As we already have values of A[node1][node]
                                 with node>node2, we make a place for node2
                                 to keep sparceA sorted                     */
                              for ( l=rowsize[node1]-1; l>=L; l--)
                                {
                                   Aspar[node1][l+1] =  Aspar[node1][l];
                                  indexA[node1][l+1] = indexA[node1][l];
                                }
                               Aspar[node1][L] = 0.0;
                              indexA[node1][L] = -1;

                              if ( L <= diag[node1] )
                                {
                                  /* We have shifted one step
                                     to the right the value   */
                                  diag[node1]++;
                                }
                              rowsize[node1]++;
                            }
                          indexA[node1][L]=node2;
                           Aspar[node1][L] += A[k][i][j];
                          if( node1==node2 )
                            {
                              diag[node1]=L;
                            }
                        }
                      L++;
                    }

                  if (find==0)
                    {
                      printf("Error in SparceA\n");
                    }
                }
            }
        }
    }
}

void ConjGrad::cholesky_fact(double** Aspar, int** indexA, int* diag,
                             double** chol, double** cholt)
{
  int i=0;
  int j=0;
  int node=0;
  int l=0;
  int ind=0;
  double sum=0.0;

  for (i=0; i<N; i++)
    {
      memset( chol[i], 0, Nterm*sizeof(double));
      memset(cholt[i], 0, Nterm*sizeof(double));
    }

  if (Aspar[0][0] < 0.0 )
    {
      printf("Error in cholesky_fact, Aspar[0][0]<0\n");
      printf("The system is definite positive or definite negative?\n");
    }
   chol[0][0]=sqrt(fabs(Aspar[0][0]));
  cholt[0][0]=chol[0][0];

  for (node=1; node<N; node++)
    {
      for (i=0; i<diag[node]-1; i++)
        {
          ind=indexA[node][i];
          if (chol[ind][diag[ind]]==0.0)
            printf("Error in cholesky_fact\n");
          else
            chol[node][i]=Aspar[node][i]/chol[ind][diag[ind]];

          l=0;
          j=0;
          while( j<diag[ind] && l<diag[ind] )
            {
              if( indexA[node][j]==indexA[ind][l] )
                {
                  if (chol[ind][diag[ind]]==0.0)
                    printf("Error in cholesky_fact\n");
                  else
                    chol[node][i] -= chol[node][j]*chol[ind][l]/chol[ind][diag[ind]];
                  j++;
                  l++;
                }
              else if( indexA[node][j] > indexA[ind][l] )
                {
                  l++;
                }
              else
                {
                  j++;
                }
            }
          j=0;
          while( node != indexA[ind][j] )
            {
              j++;
            }
          cholt[ind][j]=chol[node][i];
        }
      sum=Aspar[node][diag[node]];
      for (i=0; i<diag[node]-1; i++)
        {
          sum -= pow(chol[node][i], 2);
        }
      if (sum < 0.0 )
        {
          printf("Error in cholesky_fact, sum<0\n");
          printf("The system is definite positive or definite negative?\n");
        }
       chol[node][diag[node]]=sqrt(fabs(sum));
      cholt[node][diag[node]]=sqrt(fabs(sum));
    }
}


void ConjGrad::lower(int N, int* diag, int** indexA,
                     double** chol, double* b, double* x) {
  /* solve Chol*x=b, gives x in output */
  int i=0;
  int j=0;

  memset(x, 0, N*sizeof(double));
  for(i=0; i<N; i++) {
    x[i]=b[i];
    for (j=0; j<diag[i]-1; j++) {
      x[i] -= chol[i][j]*x[indexA[i][j]];
    }
    x[i] = x[i] / (chol[i][diag[i]]);
  }
}


void ConjGrad::upper(int N, int* diag, int** indexA, int* rowsize,
                     double** cholt, double* b, double* x) {
  /* solve Cholt*x=b, gives x in output */
  int i=0;
  int j=0;

  memset(x, 0, N*sizeof(double));
  for(i=N-1; i>=0; i--) {
    x[i]=b[i];
    for (j=diag[i]+1; j<rowsize[i]; j++) {
      x[i] -= cholt[i][j]*x[indexA[i][j]];
    }
    x[i] = x[i] / (cholt[i][diag[i]]);
  }
}
