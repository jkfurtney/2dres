/**********************************************************************
 *                                                                    * 
 * File Name: ConjGrad.hh                                             *  
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
 * of the sparse matrix A                                             * 
 *                                                                    *
 * Copyright (C) 04/2002  Arthur Moncorge                             *
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

#ifndef _CLASS_CONJGRAD_HH_
#define _CLASS_CONJGRAD_HH_

#include<iostream.h>
#include<math.h>
#include"mylibrary.hh"
#include<stdio.h>

class ConjGrad
{
  
public:
  
  ConjGrad()
  {
    cout << "Class Conjugate Gradient" << endl;
  }
  
  ~ConjGrad()
  {
    int i,j;
    
    cout << "Destructor ConjGrad" << endl;
    delete[] r;
    delete[] w;
    delete[] Aw;
    delete[] q;
    
    for (i=0; i<N; i++)
      {
	delete []  Aspar[i];
	delete []   chol[i];
	delete []  cholt[i];
	delete [] indexA[i];
      }
    delete []   Aspar;
    delete []    chol;
    delete []   cholt;
    delete []  indexA;
    delete [] rowsize;
    delete []    diag;
  }
  
  void init(int iN, int iNt, double itol, int** iCoort);
  int solve(double*** A, double* b, int* mask, double* x);
  
  void sparceA(double*** A, double** Aspar, int** indexA, int* diag, int* rowsize);
  void cholesky_fact(double** Aspar, int** indexA, int* diag, 
		     double** chol, double** cholt);
  void lower(int N, int* diag, int** indexA, 
	     double** chol, double* b, double* x);
  void upper(int N, int* diag, int** indexA, int* rowsize,
	     double** cholt, double* b, double* x);
    
public:
  
  // Constants
  double tol;
  int N;
  int Nt;
  
  int** Coort;
  
  // private

  // Working arrays
  double* r;
  double* w;
  double* Aw;
  double* q;
  
  /* For the incomplete Cholesky preconditionner */
  int Nterm; //=10;
  double** Aspar;
  double**  chol;
  double** cholt;
  int**   indexA;
  int*   rowsize;
  int*      diag;
};


#endif  // End of _CLASS_CONJGRAD_HH_










