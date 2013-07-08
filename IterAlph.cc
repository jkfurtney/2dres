/**********************************************************************
 *                                                                    * 
 * File Name: IterAlpha.cc                                            *  
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
#include <iostream.h>
#include "IterAlph.hh"


IterAlph::IterAlph(struct iteralph_arg iteralph_val) {
  int i=0;
  
  cout << "Init class Advection of alpha" << endl;
  
  Nt        = iteralph_val.Nt;
  Np        = iteralph_val.Np;
  dt        = iteralph_val.dt; 
  flux_in   = iteralph_val.flux_in;
  segINJE   = iteralph_val.segINJE;
  nloop     = iteralph_val.nloop; 
  limitgrad = iteralph_val.limitgrad;
  inject0_1 = iteralph_val.inject0_1;
  area      = iteralph_val.area;
  T_edge    = iteralph_val.T_edge;
  Ihat      = iteralph_val.Ihat;
  Ixhat     = iteralph_val.Ixhat;
  Iyhat     = iteralph_val.Iyhat;
  Coort     = iteralph_val.Coort;
  Coorp     = iteralph_val.Coorp;
  SumMass   = iteralph_val.SumMass;
  
  alpha_flux = new (double*)[Nt];
  fnm2       = new (double*)[Nt];
  fnm1       = new (double*)[Nt];
  fn         = new (double*)[Nt];
  for (i=0; i<Nt; i++) {
      alpha_flux[i] = new double[3];
      fnm2[i]       = new double[3];
      fnm1[i]       = new double[3];
      fn[i]         = new double[3];
    }
  
  alpha_seg    = new (double*)[3];
  alpha_seg[0] = new (double)[Nt];
  alpha_seg[1] = new (double)[Nt];
  alpha_seg[2] = new (double)[Nt];
  
  alpha_node = new double[Np];
  new_alpha  = new double[Nt];
  
  test_first=1;
}


IterAlph::~IterAlph() {
  int i=0;
  
  cout << "Destructor IterAlph" << endl;
  for (i=0; i<Nt; i++) {
      delete[] alpha_flux[i];
      delete[] fnm2[i];
      delete[] fnm1[i];
      delete[] fn[i];
    }
  delete[] alpha_flux;
  delete[] fnm2;
  delete[] fnm1;
  delete[] fn;
  
  delete[] alpha_seg[0];
  delete[] alpha_seg[1];
  delete[] alpha_seg[2];
  delete[] alpha_seg;
  
  delete[] alpha_node;
  delete[] new_alpha;
}


void IterAlph::iteration_alpha(double** flux, double* alpha) {
  int testcounter=0;
  
  int i=0;
  int k=0;
  int seg=0;
  int loop=0;
  int limit=0;
  double frac=0.0;
  double dt_adv=0.0;
  // used by limit grad as temp space
  double alp_seg[3];

  double nloop_db= (double) nloop;
    
  /* Upgrade velocity fluxes: fnm1 -> fnm2, fn -> fnm1, flux->fn */
  if ( test_first==0 ) {
    for (i=0; i<Nt; i++) {
      memcpy(fnm2[i], fnm1[i], 3*sizeof(double));
      memcpy(fnm1[i],   fn[i], 3*sizeof(double));
      memcpy(  fn[i], flux[i], 3*sizeof(double));
    } 
  }
  else {  
    /* This is the first time, flux in fnm2, fnm1 and fn */
    test_first=0;
    for (i=0; i<Nt; i++) {
      memcpy(fnm2[i], flux[i], 3*sizeof(double));
      memcpy(fnm1[i], flux[i], 3*sizeof(double));
      memcpy(  fn[i], flux[i], 3*sizeof(double));
    }
  }
  
  for (loop=0; loop<nloop; loop++) {
    // this loop is done nloop times which is in Proc.cc as 4
    frac=loop*dt/nloop_db;
    extrap_velflux(Nt, frac, fnm2, fnm1, fn, flux);
    
    /* Create the alpha fluxes from the velocity fluxes */
    memcpy(alpha_seg[0], alpha, Nt*sizeof(double));
    memcpy(alpha_seg[1], alpha, Nt*sizeof(double));
    memcpy(alpha_seg[2], alpha, Nt*sizeof(double));
    create_alphaflux(alpha_seg, flux, alpha_flux);
    
    if ( limitgrad==0 ) {
      /* We only advect alpha, no limitgrad */
      dt_adv = dt/(nloop_db);
      advection_alpha(dt_adv, alpha, alpha_flux, new_alpha);
    }
    else {
      /* We use a limitGrad function */
      /* advection of alpha */
      dt_adv = dt/(2*nloop_db);
      advection_alpha(dt_adv, alpha, alpha_flux, new_alpha);
      
      /* Change alpha to limit gradient */
      memcpy(alpha_seg[0], new_alpha, Nt*sizeof(double));
      memcpy(alpha_seg[1], new_alpha, Nt*sizeof(double));
      memcpy(alpha_seg[2], new_alpha, Nt*sizeof(double));
      
      limit_grad(alpha_seg);
      //cout << "limit grad OK" << endl;
      
      /* Take the average of alpha for each segment */
      for (k=0; k<Nt; k++)  {
	alp_seg[0]      = (alpha_seg[0][k]+alpha_seg[1][k])/2;
	alp_seg[1]      = (alpha_seg[1][k]+alpha_seg[2][k])/2;
	alp_seg[2]      = (alpha_seg[2][k]+alpha_seg[0][k])/2;
	alpha_seg[0][k] =  alp_seg[0];
	alpha_seg[1][k] =  alp_seg[1];
	alpha_seg[2][k] =  alp_seg[2];
      }
      
      frac=(loop+0.5)*dt/nloop_db;
      extrap_velflux(Nt, frac, fnm2, fnm1, fn, flux);
      
      /* Create the alpha fluxes from the velocity fluxes */
      create_alphaflux(alpha_seg, flux, alpha_flux);
      
      /* advection of alpha */
      dt_adv = dt/(nloop_db);
      advection_alpha(dt_adv, alpha, alpha_flux, new_alpha);
    }
    // put new_alpha values into global alpha
    memcpy(alpha, new_alpha, Nt*sizeof(double));
  }
}

void IterAlph::extrap_velflux(int Nt, double frac, double** fnm2,
			      double** fnm1, double** fn, double** f) {
  int i=0;
  int j=0;
  double frac2=0.0;
  
  for (i=0; i<Nt; i++) {
    for (j=0; j<3; j++) {
      frac2=frac*frac; // this could probably be outside this loop
      f[i][j]  = (1+(frac2+3*frac)/2)*fn[i][j];
      f[i][j] += -(frac2+2*frac)*fnm1[i][j];
      f[i][j] += ((frac2+frac)/2)*fnm2[i][j];
    }
  }  
} 


void IterAlph::advection_alpha(double t, double* alpha, double** flux, double* new_alpha) {
  int k=0;
  int seg=0;
  /******************************************************************
   * alpha in this scope is global alpha but 
   # flux is not global flux it is alpha_flux !				      
   ******************************************************************/								     
  for (k=0; k<Nt; k++) {
    new_alpha[k]=0.0;
    for (seg=0; seg<3; seg++) {
      new_alpha[k] += flux[k][seg];
    }
    new_alpha[k] *= -t/area[k];
    new_alpha[k] += alpha[k];
    
    // 0 < alpha < 1
    new_alpha[k] = MIN (ALPHA_MAX, MAX(ALPHA_MIN, new_alpha[k]));
  }
}

void IterAlph::create_alphaflux(double** alpha, double** flux, 
				double** alpha_flux) {
  /*******************************************************************
   * alpha in this routine is in the local scope                     *
   * not related to global alpha                                     *
   *******************************************************************/	     
  int l=0;
  int k=0;
  int seg=0;
  int kseg=0;
  double fluxINJ=flux_in/segINJE;
  int Ref=0;
  int tri=0;

  for (l=0; l<Nt; l++) {
    memset(alpha_flux[l], 0, 3*sizeof(double));
    for (seg=0; seg<3; seg++) {
      Ref=T_edge[l][seg]->Ref;
      if (Ref!=NORMAL && Ref!=LEAK) {
	if (Ref==INJECTOR) {
	  // new fluid comeing in from the injector
	  alpha_flux[l][seg]=(1.0-inject0_1)*fluxINJ;
	}
	else if (Ref==PRODUCER) {
	  // fluid leaving the system by the producer
	  alpha_flux[l][seg]=flux[l][seg]*alpha[seg][l];
	  if ( flux[l][seg] < 0.0 )
	    cout << "Problem, outflow at producer" << endl;
	}
	else  {  /* BORDER */
	  // zero flux boundary conditions so there should be none
	  alpha_flux[l][seg]=0.0;
	}
      }
      else {
	if (flux[l][seg] >= 0.0) {
	  // the flux is positive so we take 
	  alpha_flux[l][seg] = flux[l][seg]*alpha[seg][l];
	}
	else {
	  if (Ref==LEAK) {
	    // alpha = 0 should come through the fluid
	    alpha_flux[l][seg] = inject0_1*flux[l][seg];
	  }
	  else {
	    /* The flux goes in, we take the 
	       alpha of the other triangle   */
	    if( T_edge[l][seg]->l==l ) {
	      k=T_edge[l][seg]->k;
	      kseg=T_edge[l][seg]->kseg;
	    }
	    else {
	      if( T_edge[l][seg]->k==l )   {
		k=T_edge[l][seg]->l;
		kseg=T_edge[l][seg]->lseg;
	      }
	      else{
		cout << "Error in the segments" << endl;
	      }
	    }
	    
	    if (k==-1) {
	      cout << "Problem in alpha_flux" << endl;
	    }
	    else {
	      alpha_flux[l][seg] = flux[l][seg]*alpha[kseg][k];
	    }
	  }
	}
      }
    }
  }
}

void IterAlph::limit_grad(double** alpha) {
  int k=0;
  int i=0;
  int seg=0;
  int node=0;
  int tri=0;
  double xc=0.0;
  double yc=0.0;
  double alpha_x=0.0;
  double alpha_y=0.0;
  double alpha_max=0.0;
  double alpha_min=0.0;
  double alpham=0.0;
  double Dxmax=0.0;
  double Dxmin=0.0;
  double Dymax=0.0;
  double Dymin=0.0;
  double Txmax=0.0;
  double Txmin=0.0;
  double Tymax=0.0;
  double Tymin=0.0;
  double phix=0.0;
  double phiy=0.0;
  
  /* Interpolate alpha on the nodes */
  memset(alpha_node, 0, Np*sizeof(double));
  for (k=0; k<Nt; k++) {
    if ( alpha[0][k]!=alpha[1][k] || alpha[0][k]!=alpha[2][k] ) {
      cout << "Error in limit_alpha: " 
	   << alpha[0][k] << " " 
	   << alpha[1][k] << " " 
	   << alpha[2][k] << endl; 
    }
    
    for (i=0; i<3; i++) {
      node=Coort[3*k+i];
      if (SumMass[node]==0.0)
	cout << "Error SumMass, limit_grad" << endl;
      alpha_node[node] += Ihat[k][i]*alpha[0][k]/SumMass[node];
    }
  }
  
  /* Limit the gradient */
  for (k=0; k<Nt; k++) {
    /* Compute center of mass [xc yc] and (d/dx)alpha & (d/dy)alpha */
    xc=0.0;
    yc=0.0;
    alpha_x=0.0;
    alpha_x=0.0;
    for (i=0; i<3; i++) {
      node = Coort[3*k+i];
      if (area[node]==0.0)
	cout << "Error area, limit_grad" << endl;
      xc += Ihat[k][i]*Coorp[2*node+0]/area[k]; 
      yc += Ihat[k][i]*Coorp[2*node+1]/area[k]; 
      alpha_x += Ixhat[k][i]*alpha_node[node]/area[k]; 
      alpha_y += Iyhat[k][i]*alpha_node[node]/area[k]; 
    }

    /* Compute the maximum averaged alpha on the 
       surrounding triangles                     */
    alpha_max=ALPHA_MIN;
    alpha_min=ALPHA_MAX;
    for (seg=0; seg<3; seg++) {
      if (T_edge[k][seg]->k != k) {
	tri=T_edge[k][seg]->k;
      }
      else {
	tri=T_edge[k][seg]->l;
      }
      
      if( tri!=-1) {
	alpha_max=MAX( alpha_max, alpha[0][tri] );
	alpha_min=MIN( alpha_min, alpha[0][tri] );
      }
    }
    
    alpham=MAX(alpha_min, MIN(alpha[0][k], alpha_max));
    /*printf("amin=%g, amax=%g, a=%g, am=%g\n", alpha_min, alpha_max, alpha[0][k], alpham);
     */
    /*-----See if alpha_x or y can change of sign, if not we can simplify----*/
    node = Coort[3*k+0]; 
    
    Dxmax = (Coorp[2*node+0]-xc)*alpha_x;
    Dxmin = Dxmax;
    Dymax = (Coorp[2*node+1]-yc)*alpha_y;
    Dymin = Dymax;
    
    for (i=1; i<3; i++) {
      node = Coort[3*k+i];
      Dxmax = MAX(Dxmax, (Coorp[2*node+0]-xc)*alpha_x );
      Dxmin = MIN(Dxmin, (Coorp[2*node+0]-xc)*alpha_x );
      Dymax = MAX(Dymax, (Coorp[2*node+1]-yc)*alpha_y );
      Dymin = MIN(Dymin, (Coorp[2*node+1]-yc)*alpha_y );
    }
    
      
      /*-------------------                      */
    Txmax=(alpha_max-alpham)/(fabs(Dxmax+SMALL));
    Tymax=(alpha_max-alpham)/(fabs(Dymax+SMALL));
    Txmin=(alpham-alpha_min)/(fabs(Dxmin+SMALL));
    Tymin=(alpham-alpha_min)/(fabs(Dymin+SMALL));
    
    phix=MIN(Txmin, MIN(Txmax, 1.0));
    /*printf("Tmax=%g, Tmin=%g, phi=%g\n",Txmax,Txmin,phix); 
     */ 
    
    phiy=MIN(Tymin, MIN(Tymax, 1.0));
    /*printf("Tmax=%g, Tmin=%g, phi=%g\n",Tymax,Tymin,phiy);
     */ 
    
    /* Our new alpha!! */
    for (i=0; i<3; i++) {
      node=Coort[3*k+i];
      alpha[i][k] += phix*alpha_x*(Coorp[2*node+0]-xc);
      alpha[i][k] += phiy*alpha_y*(Coorp[2*node+1]-yc);
    }
  } /* for(k=0; k<Nt */
}


void IterAlph::limit_grad3(double** alpha) {
  int k=0;
  int i=0;
  int seg=0;
  int node=0;
  int tri=0;
  double xc=0.0;
  double yc=0.0;
  double xc_tri=0.0;
  double yc_tri=0.0;
  double     x2=0.0;
  double     y2=0.0;
  double    amx=0.0;
  double    amy=0.0;
  double alpha_x=0.0;
  double alpha_y=0.0;
  double alpha_x_max=0.0;
  double alpha_y_max=0.0;
  double alpha_x_min=0.0;
  double alpha_y_min=0.0;
  double alphamx=0.0; 
  double alphamy=0.0;
  double Dxmax=0.0;
  double Dxmin=0.0;
  double Dymax=0.0;
  double Dymin=0.0;
  double Txmax=0.0;
  double Txmin=0.0;
  double Tymax=0.0;
  double Tymin=0.0;
  double phix=0.0;
  double phiy=0.0;
  
  /* Interpolate alpha on the nodes */
  memset(alpha_node, 0, Np*sizeof(double));
  for (k=0; k<Nt; k++) {
    if ( alpha[0][k]!=alpha[1][k] || alpha[0][k]!=alpha[2][k] ){
      cout << "Error in limit_alpha: " 
	   << alpha[0][k] << " " 
	   << alpha[1][k] << " " 
	   << alpha[2][k] << endl; 
    }
    
    for (i=0; i<3; i++) {
      node=Coort[3*k+i];
      if (SumMass[node]==0.0)
	cout << "Error SumMass, limit_grad" << endl;
      alpha_node[node] += Ihat[k][i]*alpha[0][k]/SumMass[node];
    }
  }
  
  /* Limit the gradient */
  for (k=0; k<Nt; k++) {
    /* Compute center of mass [xc yc] and (d/dx)alpha & (d/dy)alpha */
    xc=0.0;
    yc=0.0;
    alpha_x=0.0;
    alpha_x=0.0;
    for (i=0; i<3; i++){
      node = Coort[3*k+i];
      if (area[node]==0.0)
	cout << "Error area, limit_grad" << endl;
      xc += Ihat[k][i]*Coorp[2*node+0]/area[k]; 
      yc += Ihat[k][i]*Coorp[2*node+1]/area[k]; 
      alpha_x += Ixhat[k][i]*alpha_node[node]/area[k]; 
      alpha_y += Iyhat[k][i]*alpha_node[node]/area[k]; 
    }

      /* Compute the maximum averaged alpha on the 
	 surrounding triangles                     */
    alpha_x_max=ALPHA_MIN; 
    alpha_y_max=ALPHA_MIN;
    alpha_x_min=ALPHA_MAX; 
    alpha_y_min=ALPHA_MAX;
    for (seg=0; seg<3; seg++){
      xc_tri=0.0;
      yc_tri=0.0;
      
      if (T_edge[k][seg]->k != k) {
	tri=T_edge[k][seg]->k;
      }
      else {
	tri=T_edge[k][seg]->l;
      }
      
      if( tri!=-1) { 
	for (i=0; i<3; i++) {
	  node = Coort[3*tri+i];
	  if (area[node]==0.0)
	    cout << "Error area, limit_grad" << endl;
	  xc_tri += Ihat[tri][i]*Coorp[2*node+0]/area[tri]; 
	  yc_tri += Ihat[tri][i]*Coorp[2*node+1]/area[tri]; 
	}
	x2=(xc_tri-xc)*(xc_tri-xc);
	y2=(yc_tri-yc)*(yc_tri-yc);
	amx=alpha[0][tri]*sqrt(x2/(x2+y2));
	amx=alpha[0][tri]*sqrt(y2/(x2+y2));
	alpha_x_max=MAX( alpha_x_max, amx );
	alpha_x_min=MIN( alpha_x_min, amx );
	alpha_y_max=MAX( alpha_y_max, amy );
	alpha_y_min=MIN( alpha_y_min, amy );
      }
    }
    
    alphamx = MAX(alpha_x_min, MIN(alpha[0][k], alpha_x_max));
    alphamy = MAX(alpha_y_min, MIN(alpha[0][k], alpha_y_max));
    
    node = Coort[3*k+0]; 
    
    Dxmax = (Coorp[2*node+0]-xc)*alpha_x;
    Dxmin = Dxmax;
    Dymax = (Coorp[2*node+1]-yc)*alpha_y;
    Dymin = Dymax;
    
    for (i=1; i<3; i++) {
      node = Coort[3*k+i];
      Dxmax = MAX(Dxmax, (Coorp[2*node+0]-xc)*alpha_x );
      Dxmin = MIN(Dxmin, (Coorp[2*node+0]-xc)*alpha_x );
      Dymax = MAX(Dymax, (Coorp[2*node+1]-yc)*alpha_y );
      Dymin = MIN(Dymin, (Coorp[2*node+1]-yc)*alpha_y );
    }
    
    
    /*-------------------                      */
    Txmax=(alpha_x_max-alphamx)/(fabs(Dxmax+SMALL));
    Tymax=(alpha_y_max-alphamy)/(fabs(Dymax+SMALL));
    Txmin=(alphamx-alpha_x_min)/(fabs(Dxmin+SMALL));
    Tymin=(alphamy-alpha_y_min)/(fabs(Dymin+SMALL));
    
    phix=MIN(Txmin, MIN(Txmax, 1.0));
    /*printf("Tmax=%g, Tmin=%g, phi=%g\n",Txmax,Txmin,phix); 
     */ 
    
    phiy=MIN(Tymin, MIN(Tymax, 1.0));
    /*printf("Tmax=%g, Tmin=%g, phi=%g\n",Tymax,Tymin,phiy);
     */ 
 
    /* Our new alpha!! */
    for (i=0; i<3; i++) {
      node=Coort[3*k+i];
      alpha[i][k] += phix*alpha_x*(Coorp[2*node+0]-xc);
      alpha[i][k] += phiy*alpha_y*(Coorp[2*node+1]-yc);
    }
  } /* for(k=0; k<Nt */
}







