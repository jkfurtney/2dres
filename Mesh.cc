/* *********************************************************************
 *                                                                    *
 * File Name: Mesh.cc                                                 *
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
#include <iostream>
#include "Mesh.hh"
#define MAXLINE 100
using namespace std;

void Mesh::meshRead(FILE* fd) {
  char dummyline[MAXLINE];

  int dummy;
  int i;

  /*double Xleak_min=0.175;
  double Xleak_max=0.21;
  double Yleak_min=0.09;
  double Yleak_max=0.14;*/

  fgets(dummyline, MAXLINE, fd);

  memset(Coorp, 0, 2*Np*sizeof(double));
  memset( Refp, 0,   Np*   sizeof(int));
  memset(Coort, 0, 3*Nt*   sizeof(int));

  /* Scan the info for the points */
  for (i=0; i<Np; i++)
    fscanf(fd, "%i %lf %lf %i", &dummy,
           &Coorp[i*2], &Coorp[i*2+1], &Refp[i]);

  /* Scan the info for the triangles */
  for (i=0; i<Nt; i++)
    fscanf(fd, "%i %i %i %i %i", &dummy,
           &Coort[i*3], &Coort[i*3+1], &Coort[i*3+2],
           &dummy);

  /* In a .amdba mesh, the first Triangle is 1, as in a C code,
     the indexes start with 0, I decrease Coort of 1           */
  for (i=0; i<3*Nt; i++)
    Coort[i] -= 1;

  /*  cout << "SWITH OFF NORTH INJECTOR" << endl;
      cout << "DEFINE LEAK POSITION" << endl;
      for (i=0; i<Np; i++)    {
      if ( Refp[i]==INJECTOR && Coorp[2*i+1]>0.5 )      {
      Refp[i]=BORDER;
      }

      if ( Refp[i]==BORDER
      && Coorp[2*i+1] > Yleak_min
      && Coorp[2*i+1] < Yleak_max
           && Coorp[2*i]   > Xleak_min
           && Coorp[2*i]   < Xleak_max ){
           Refp[i]=LEAK;
           }
  */

}


Mesh::Mesh(char* meshfile) {
  FILE* fd;
  int i, j;

  cout << "Init Class Mesh" << endl;

  Np=0;
  Nt=0;
  Ne=0;
  hmin=0.0;
  hmax=0.0;
  Refp=0;
  Coorp=0;
  Coort=0;
  Coore=0;
  area=0;
  SumMass=0;
  invMl=0;
  invM=0;
  Al=0;
  Ihat=0;
  Ixhat=0;
  Iyhat=0;
  IyRT=0;
  T_edge=0;
  edge=0;
  segINJE=0;
  segPROD=0;
  segLEAK=0;
  segBORD=0;
  total_area = 0.0;

  /* Read Np and Nt */
  if ( (fd = fopen(meshfile,"r")) == NULL ){
    cout << "Cannot open mesh" << endl;
    exit(1);
  }
  fscanf(fd, "%i %i", &Np, &Nt);
  Ne=0;
  printf("This mesh has: %i nodes and %i triangles.\n", Np, Nt);


  /* **************** Allocate memory ***********************/
  // edge is allocated in the method ****
  Refp    = new    int[Np];
  Coort   = new    int[3*Nt];
  Coorp   = new double[2*Np];
  area    = new double[Nt];
  SumMass = new double[Np];
  Coore   = new     int*[Nt];
  invMl   = new double *[Nt];
  Ihat    = new double *[Nt];
  Ixhat   = new double *[Nt];
  Iyhat   = new double *[Nt];
  IyRT    = new double *[Nt];
  T_edge  = new struct Edge**[Nt];
  for (i=0; i<Nt; i++) {
    Coore[i]  = new    int[3];
    invMl[i]  = new double[3];
    Ihat[i]   = new double[3];
    Ixhat[i]  = new double[3];
    Iyhat[i]  = new double[3];
    IyRT[i]   = new double[3];
    T_edge[i] = new struct Edge*[3];
  }

  invM = new double **[Nt];
  Al   = new double **[Nt];
  for (i=0; i<Nt; i++) {
    invM[i] = new double *[3];
    Al[i]   = new double *[3];

    for (j=0; j<3; j++) {
      invM[i][j] = new double[3];
      Al[i][j]   = new double[3];
    }
  }

  /* ************* Read data from the mesh file **************/
  meshRead(fd);
  fclose(fd);
  cout << "meshRead OK" << endl;

  /* ***** Compute and allocate for the segments *******/
  meshEdge();
  cout << "meshEdge OK Ne=" << Ne << endl;
  cout << "segINJE=" << segINJE
       << " segPROD=" << segPROD
       << " segLEAK=" << segLEAK
       << " segBORD=" << segBORD<< endl;

  /* ***** Compute the matrixes used in the computation *******/
  meshCompute();
  cout << "meshCompute OK" << endl;
}

Mesh::~Mesh() {
  int i, j;
  cout << "Destructor Mesh" << endl;
  delete[] Refp;
  delete[] Coort;
  delete[] Coorp;
  delete[] area;
  delete[] SumMass;

  for (i=0; i<Ne; i++) {
      delete edge[i];
    }
  delete[] edge;

  for (i=0; i<Nt; i++) {
    delete[] Coore[i];
    delete[] invMl[i];
    delete[] Ihat[i];
    delete[] Ixhat[i];
    delete[] Iyhat[i];
    delete[] IyRT[i];
    delete[] T_edge[i];
  }
  delete[] Coore;
  delete[] invMl;
  delete[] Ihat;
  delete[] Ixhat;
  delete[] Iyhat;
  delete[] IyRT;
  delete[] T_edge;

  for (i=0; i<Nt; i++) {
    for (j=0; j<3; j++) {
      delete[] invM[i][j];
      delete[]   Al[i][j];
    }
    delete[] invM[i];
    delete[]   Al[i];
  }
  delete[] invM;
  delete[]   Al;

}


void Mesh::meshEdge() {

  int     i=0;
  int     j=0;
  int     l=0;
  //! temp index
  int   seg=0;
  //! tmp boolean
  int  find=0;

  double delta=0.0;
  //! tmp variable
  double val=0.0;
  //! size of cx and cy arrays
  int size=0;

  //! x coords of the triangles nodes
  double xi[3];
  //! y coords of the triangles nodes
  double yi[3];
  //! unique identifiers of edge
  double xp=0;
  //! unique identifiers of edge
  double yp=0;

  //! unique identifiers of each edge
  double* cx=0;
  //! unique identifiers of each edge
  double* cy=0;

  //! the node number of the point that makes up each end of a segment
  int PointI=0;
  //! the node number of the point that makes up each end of a segment
  int PointJ=0;

  /* Set the order of magnitude of one element */
  xi[0] = Coorp[ ( Coort[0] )*2 ];
  xi[1] = Coorp[ ( Coort[1] )*2 ];
  yi[0] = Coorp[ ( Coort[0] )*2 + 1 ];
  yi[1] = Coorp[ ( Coort[1] )*2 + 1 ];
  hmin=pow(xi[0]-xi[1],2)+pow(yi[0]-yi[1],2);
  hmax=hmin;

    for (l=0; l<Nt; l++) {
    /* Read the coordinates of the 3 vertices */
      for (i=0; i<3; i++) {
        xi[i] = Coorp[ ( Coort[3*l+i] )*2 ];
        yi[i] = Coorp[ ( Coort[3*l+i] )*2 + 1 ];
      }

      /* Find the smallest and the longest Edge */
      val=pow(xi[0]-xi[1],2)+pow(yi[0]-yi[1],2);
      hmin=MIN(hmin,val); hmax=MAX(hmax,val);

      val=pow(xi[0]-xi[2],2)+pow(yi[0]-yi[2],2);
      hmin=MIN(hmin,val); hmax=MAX(hmax,val);

      val=pow(xi[1]-xi[2],2)+pow(yi[1]-yi[2],2);
      hmin=MIN(hmin,val); hmax=MAX(hmax,val);
    }

  hmin=sqrt(hmin);
  hmax=sqrt(hmax);
  printf("min(dx)=%g max(dx)=%g\n",hmin,hmax);

  /* ****** Set the Edges ******/
  Ne=0;
  size=0;
  for (l=0; l<Nt; l++) {
    /* Read the coordinates of the 3 vertices */
    for (i=0; i<3; i++) {
      xi[i] = Coorp[ ( Coort[3*l+i] )*2 ];
      yi[i] = Coorp[ ( Coort[3*l+i] )*2 + 1 ];
    }

    for (i=0; i<3; i++)  {
      j=i+1;
      if (j==3)
        j=0;
      /* here we loop over each segment in the mesh */

      PointI=Coort[3*l+i];
      PointJ=Coort[3*l+j];


      xp=(xi[i]+xi[j])/2.0;
      yp=(yi[i]+yi[j])/2.0;

      /* ******** Find if the Edge has already been allocated *****/
      seg=0;
      find=0;
      while (seg<Ne && find==0)  {
        if ( fabs(cx[seg]-xp) + fabs(cy[seg]-yp) < hmin/100.0 ){
          /* Edge already allocated */
          T_edge[l][i]=edge[seg];
          Coore[l][i]= edge[seg]->n;
          edge[seg]->l=l;
          edge[seg]->lseg=i;
          find=1;
        }
        seg++;
      }

      if (find==0) { /* This is a new Edge */
        /* Look if we have enough memory to allocate a new segm */
        if (Ne==size){
          size += Np+Nt+1;
          increase_array(size, &cx, &cy);
          }

        /* We have enough place to allocate */
        cx[Ne] = xp;
        cy[Ne] = yp;
        edge[Ne]=new struct Edge[1];
        edge[Ne]->n=Ne;
        T_edge[l][i]=edge[Ne];
        Coore[l][i]=Ne;
        edge[Ne]->vertex1=PointI;
        edge[Ne]->vertex2=PointJ;
        edge[Ne]->k=l;
        edge[Ne]->l=-1;
        edge[Ne]->kseg=i;
        edge[Ne]->length=sqrt( pow(xi[i]-xi[j],2) + pow(yi[i]-yi[j],2) );
        edge[Ne]->xp=xp;
        edge[Ne]->yp=yp;


        /* here we figure out what the refrence of the segment should be*/

        edge[Ne]->Ref=NORMAL;
        if ( Refp[PointI]==INJECTOR &&  Refp[PointJ]==INJECTOR )  {
          edge[Ne]->Ref=INJECTOR;
          segINJE =  segINJE + 1;
        }
        if ( Refp[PointI]==PRODUCER &&  Refp[PointJ]==PRODUCER ) {
          edge[Ne]->Ref=PRODUCER;
          segPROD =  segPROD + 1;
        }
        if ( Refp[PointI]==LEAK &&  Refp[PointJ]==LEAK ) {
          edge[Ne]->Ref=LEAK;
          segLEAK =  segLEAK + 1;
        }
        if ( (Refp[PointI]==BORDER && Refp[PointJ]==BORDER) ||
             (Refp[PointI]!=Refp[PointJ] &&
              Refp[PointI]!=NORMAL && Refp[PointJ]!=NORMAL))  {
          /* For the squared domain */
          /* For the squared domain
             if (Coorp[2*PointI]==Coorp[2*PointJ] )
             {
          */
          edge[Ne]->Ref=BORDER;
          segBORD =  segBORD + 1;
        }
        Ne++;
      }
    }
  }
}


void Mesh::increase_array(int size, double** cx, double** cy)
{
  int                  i=0;
  struct Edge** edge2=0;
  double*            cx2=0;
  double*            cy2=0;

  edge2 = new struct Edge*[size];
  cx2   = new double[size];
  cy2   = new double[size];
  memset(edge2, 0, size*sizeof(struct Edge*));
  memset(cx2,   0, size*sizeof(double));
  memset(cy2,   0, size*sizeof(double));

  for (i=0; i<Ne; i++)
    {
      edge2[i]=edge[i];
      cx2[i]  =(*cx)[i];
      cy2[i]  =(*cy)[i];
    }

  if(Ne!=0)
    {
      delete[] edge;
      delete[] (*cx);
      delete[] (*cy);
    }

  edge=edge2;
  (*cx)=cx2;
  (*cy)=cy2;
}



void Mesh::meshCompute() {
  /* We need ptr on mesh because we give a value to mesh.h */
  int     i=0;
  int     j=0;
  int     l=0;
  double x1=0.0;
  double x2=0.0;
  double x3=0.0;
  double y1=0.0;
  double y2=0.0;
  double y3=0.0;
  double delta=0.0;

  double M11=0.0;
  double M12=0.0;
  double M13=0.0;
  double M22=0.0;
  double M23=0.0;
  double M33=0.0;
  double detM=0.0;
  double trace=0.0;
  double l1_2, l2_2, l3_2, l1dotl2, l1dotl3, l2dotl3;
  double grad[3][3];

  memset(SumMass, 0, Np*sizeof(double));
  for (l=0; l<Nt; l++) {
    test_orientation(l);
    /* Read the coordinates of the 3 vertices */
    x1 = Coorp[ ( Coort[3*l  ] )*2 ];
    x2 = Coorp[ ( Coort[3*l+1] )*2 ];
    x3 = Coorp[ ( Coort[3*l+2] )*2 ];

    y1 = Coorp[ ( Coort[3*l  ] )*2 + 1 ];
    y2 = Coorp[ ( Coort[3*l+1] )*2 + 1 ];
    y3 = Coorp[ ( Coort[3*l+2] )*2 + 1 ];

    delta = (x1-x3)*(y2-y3) - (x2-x3)*(y1-y3);
    area[l] = fabs(delta/2.0);
    total_area += area[l];

    grad[0][0]=(y2-y3)/delta;
    grad[0][1]=(x3-x2)/delta;
    grad[1][0]=(y3-y1)/delta;
    grad[1][1]=(x1-x3)/delta;
    grad[2][0]=(y1-y2)/delta;
    grad[2][1]=(x2-x1)/delta;

    /* construct the matrix for the mixte/hybrid method */

    l1_2=pow(x2-x1,2) + pow(y2-y1,2);
    l2_2=pow(x3-x2,2) + pow(y3-y2,2);
    l3_2=pow(x1-x3,2) + pow(y1-y3,2);

    l1dotl2 = (x1-x2)*(x3-x2) + (y1-y2)*(y3-y2);
    l1dotl3 = (x2-x1)*(x3-x1) + (y2-y1)*(y3-y1);
    l2dotl3 = (x2-x3)*(x1-x3) + (y2-y3)*(y1-y3);

    M11 = ( l2_2 +l3_2   +l2dotl3)/(24.0*area[l]);
    M12 = (-l1_2 -l2_2 +3*l1dotl2)/(24.0*area[l]);
    M13 = (-l1_2 -l3_2 +3*l1dotl3)/(24.0*area[l]);
    M22 = ( l1_2 +l3_2   +l1dotl3)/(24.0*area[l]);
    M23 = (-l2_2 -l3_2 +3*l2dotl3)/(24.0*area[l]);
    M33 = ( l1_2 +l2_2   +l1dotl2)/(24.0*area[l]);

    /* Compute the inverse matrix */
    detM = M11*M22*M33-M11*M23*M23-M22*M13*M13-M33*M12*M12+2*M12*M13*M23;
    invM[l][0][0] = ( M22*M33 - M23*M23 )/detM;
    invM[l][0][1] = (-M12*M33 + M13*M23 )/detM;
    invM[l][0][2] = ( M12*M23 - M13*M22 )/detM;
    invM[l][1][1] = ( M11*M33 - M13*M13 )/detM;
    invM[l][1][2] = (-M11*M23 + M12*M13 )/detM;
    invM[l][2][2] = ( M11*M22 - M12*M12 )/detM;
    invM[l][1][0] = invM[l][0][1];
    invM[l][2][0] = invM[l][0][2];
    invM[l][2][1] = invM[l][1][2];

    /* Compute the lumped matrix */
    trace=0.0;
    for (i=0; i<3; i++) {
      invMl[l][i] = invM[l][i][0] + invM[l][i][1] + invM[l][i][2];
      trace += invMl[l][i];
    }

    /* Compute the contributions of the TP to the Fluxes */
    for (i=0; i<3; i++) {
      for (j=0; j<3; j++) {
        Al[l][i][j] = -(invMl[l][i]*invMl[l][j]/trace - invM[l][i][j]);
      }
    }

    /* ************ Compute other useful matrices **************/
    /* I use them for the interpolation of alpha on the nodes

       Ihat[i] is the value of int on the triangle "l"
       of Phi(i)     for (l=0; l<Nt; l++)
       Ixhat[i] is the value of int on the triangle "l"
       of (d/dx)Phi(i)

       Iyhat[i] is the value of int on the triangle "l"
       of (d/dy)Phi(i)

       IyRT[i] is the same as Iyhat but with the RT elements instead
       of the hat functions SumMass[i] is the int on the total volume
       of Phi(i) (global i)
    */

    IyRT[l][0]=(y2-y3+y1-y3)/(12.0);
    IyRT[l][1]=(y2-y1+y3-y1)/(12.0);
    IyRT[l][2]=(y3-y2+y1-y2)/(12.0);

    for (i=0; i<3; i++) {
      Ihat[l][i]    = area[l]/3.0;
      Ixhat[l][i]   = area[l]*grad[i][0];
      Iyhat[l][i]   = area[l]*grad[i][1];
      SumMass[Coort[3*l+i]] += Ihat[l][i];
    }
  }
}

void Mesh::test_orientation(int k)
{
  /* Test the orientation of triangle k           */

  double x1=0.0;
  double x2=0.0;
  double x3=0.0;
  double y1=0.0;
  double y2=0.0;
  double y3=0.0;

  x1 = Coorp[ ( Coort[3*k  ] )*2 ];
  x2 = Coorp[ ( Coort[3*k+1] )*2 ];
  x3 = Coorp[ ( Coort[3*k+2] )*2 ];
  y1 = Coorp[ ( Coort[3*k  ] )*2 + 1 ];
  y2 = Coorp[ ( Coort[3*k+1] )*2 + 1 ];
  y3 = Coorp[ ( Coort[3*k+2] )*2 + 1 ];

  /* If x3 is at the left hand side of the line (x1,x2),
     it is an anticlockwise triangle, if it is at the
     right hand side, this is a clockwise triangle     */
  if ( (x2-x1)*(y3-y1)-(y2-y1)*(x3-x1) < 0.0 )
    printf("Triangle %i is clockwise\n",k);
  /*else
    printf("Triangle %i is anticlockwise\n",k);
  */
}

void Mesh::get(struct mesh_data *mesh_val) {
  mesh_val->Np=Np;
  mesh_val->Nt=Nt;
  mesh_val->Ne=Ne;
  mesh_val->hmin=hmin;
  mesh_val->hmax=hmax;
  mesh_val->Refp=Refp;
  mesh_val->Coorp=Coorp;
  mesh_val->Coort=Coort;
  mesh_val->Coore=Coore;
  mesh_val->area=area;
  mesh_val->SumMass=SumMass;
  mesh_val->invMl=invMl;
  mesh_val->invM=invM;
  mesh_val->Al=Al;
  mesh_val->Ihat=Ihat;
  mesh_val->Ixhat=Ixhat;
  mesh_val->Iyhat=Iyhat;
  mesh_val->IyRT=IyRT;
  mesh_val->T_edge=T_edge;
  mesh_val->edge=edge;
  mesh_val->segINJE=segINJE;
  mesh_val->segPROD=segPROD;
  mesh_val->segLEAK=segLEAK;
  mesh_val->segBORD=segBORD;
  mesh_val->total_area = total_area;
}
