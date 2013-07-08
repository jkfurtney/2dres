#include <stdlib.h>
#include <iostream>

#include "Prog.hh"

extern "C" {
  /* cfp is a configuration parsing module it is writen in c
     and needs to be declared as extern "C" as below
     for more information on cfp see cfp/cfp.h
  */
#include "cfp/cfp.h"
}
main(int argc, char *argv[]) {

  int info = 0;
  cfpObject c = cfpNew(1);
  struct Prog::prog_arg prog_val;

  if (argc != 2) {
    cout << " You must provide 1 commandline argument which is the name of the configuration file for this run " << endl;
    exit (1);
  }

  if (0 == cfpParseFile(c, argv[1])) {
    cerr << "error opening file: " << argv[1] << " exiting..." << endl;
    exit(1);
  }

  if (info) {
    cout << "**************************************************************" << endl;
    cout << "*                   2D reservoir simulator                   *" << endl;
    cout << "**************************************************************" << endl;
    cout << endl;
    cout << "alpha is the saturation of injected fluid" << endl;
    cout << "=1 corresponds to the injected fluid," << endl;
    cout << "=0 corresponds to the original fluid" << endl;
    cout << endl;
    cout << "The equations that govern the flow are:" << endl;
    cout << endl;
    cout << "1) Conservation of injected fluid mass" << endl;
    cout << "d(alpha)/dt + div(alpha*u) = 0" << endl;
    cout << endl;
    cout << "2) Darcy's Law" << endl;
    cout << "(mu/k)*u = -grad(p) - B*alpha*e_y" << endl;
    cout << "with e_y normal vector in the y direction  " << endl;
    cout << endl;
    cout << "3) Incompressibility condition" << endl;
    cout << "div(u)=0" << endl;
    cout << endl;
    cout << "4) Boundary conditions:" << endl;
    cout << "no flow at BORDER" << endl;
    cout << "pressure fixed at PRODUCER" << endl;
    cout << "flow fixed at INJECTOR" << endl;
    cout << "outflow at LEAK = K_ext(p-p_ext)" << endl;
    cout << endl;
    exit(0);
  }

  prog_val.meshfile           = cfpGetString(c, "meshfile");
  prog_val.Rinje              = cfpGetDouble(c, "Rinje");
  prog_val.K_ext              = cfpGetDouble(c, "K_ext");
  prog_val.p_ext              = cfpGetDouble(c, "p_ext");
  prog_val.dt                 = cfpGetDouble(c, "dt");
  prog_val.k_p                = cfpGetDouble(c, "k_p");
  prog_val.mu1                = cfpGetDouble(c, "mu1");
  prog_val.mu2                = cfpGetDouble(c, "mu2");
  prog_val.p0                 = cfpGetDouble(c, "p0");
  prog_val.u_in               = cfpGetDouble(c, "u_in");
  prog_val.e_g                = cfpGetDouble(c, "e_g");
  prog_val.MAXTIME            = cfpGetInt(c, "MAXTIME");
  prog_val.visu_step          = cfpGetInt(c, "visu_step");
  prog_val.init_alpha         = cfpGetInt(c, "init_alpha");
  prog_val.coort_file         = cfpGetBoolean(c, "make_coort.mat");
  prog_val.coorp_file         = cfpGetBoolean(c, "make_coorp.mat");
  prog_val.alpha_out          = cfpGetBoolean(c, "make_alpha_out.mat");
  prog_val.fracmat            = cfpGetBoolean(c, "make_frac.mat");
  prog_val.result             = cfpGetBoolean(c, "make_result.mat");
  prog_val.production_log     = cfpGetBoolean(c, "production_log");
  prog_val.result_prefix      = cfpGetString(c, "result_prefix");

  cfpDelete(c);
  Prog program(prog_val);
  program.compute();
}
