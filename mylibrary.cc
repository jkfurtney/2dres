#include "mylibrary.hh"

double MIN(double a, double b)
{
  if(a<b)
    return a;
  else
    return b;
}

double MAX(double a, double b)
{
  if(a<b)
    return b;
  else
    return a;
}

void multiply(int Nt, int N, int** Coort, double*** A, double* b, double* Ab)
{
  int i=0;
  int j=0;
  int k=0;
  int node=0;
  int node2=0;

  memset(Ab, 0, N*sizeof(double));
  for (k=0; k<Nt; k++)
    {
      for (i=0; i<3; i++)
        {
          node=Coort[k][i];
          
          for (j=0; j<3; j++)
            {  
              node2=Coort[k][j];
              Ab[node] += A[k][i][j]*b[node2];
            }
        }
    }
}


