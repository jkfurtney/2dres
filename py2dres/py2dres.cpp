// This file provides the interface between Python and the C++
// implementation of the fast marching method in fast_marching.cpp

#include "Python.h"
#include "numpy/noprefix.h"
#include <structmember.h>

#include "Prog.hh"

typedef struct {
  PyObject_HEAD
  Prog *prog;
} py2dres;

#define CHECK_SELF ;
static PyObject *py2dres_triangle_count(py2dres *self, PyObject *)
{
  CHECK_SELF;
  return PyInt_FromLong(self->prog->Nt);
}

static PyObject *py2dres_update_p(py2dres *self, PyObject *){
  self->prog->updateP();
  Py_RETURN_NONE;
}

static PyObject *py2dres_update_a(py2dres *self, PyObject *){
  self->prog->updateA();
  Py_RETURN_NONE;
}

static PyObject *mobility(py2dres *self, PyObject *){
  npy_intp dims[12];
  dims[0] = self->prog->Nt;
  void *data = (void *)self->prog->mobility_;
  return PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, data);
}

static PyObject *alpha(py2dres *self, PyObject *){
  npy_intp dims[12];
  dims[0] = self->prog->Nt;
  void *data = (void *)self->prog->alpha;
  return PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, data);
}

static PyObject *pressure(py2dres *self, PyObject *){
  npy_intp dims[12];
  dims[0] = self->prog->Nt;
  void *data = (void *)self->prog->pressure;
  return PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, data);
}

static PyObject *flux(py2dres *self, PyObject *){
  npy_intp dims[12];
  dims[0] = self->prog->Nt;
  dims[1] = 3;
  void *data = (void *)self->prog->flux;
  return PyArray_SimpleNewFromData(2, dims, NPY_DOUBLE, data);
}

static PyObject *edge_pressure(py2dres *self, PyObject *){
  npy_intp dims[12];
  dims[0] = self->prog->Nt;
  dims[1] = 3;
  void *data = (void *)self->prog->mixte_->tpress;
  return PyArray_SimpleNewFromData(2, dims, NPY_DOUBLE, data);
}

static PyObject *x(py2dres *self, PyObject *){
  npy_intp dims[12];
  dims[0] = self->prog->Nt;
  PyArrayObject *array =  (PyArrayObject *)PyArray_SimpleNew(1,
                                                             dims,
                                                             NPY_DOUBLE);
  if (!array) return NULL;

  double *data = (double *)PyArray_DATA(array);
  for (int i=0; i<dims[0]; i++)
  {
    int n0 = self->prog->Coort[3*i];
    int n1 = self->prog->Coort[3*i + 1];
    int n2 = self->prog->Coort[3*i + 2];

    data[i] = (self->prog->Coorp[2*n0] +
               self->prog->Coorp[2*n1] +
               self->prog->Coorp[2*n2]) / 3.0;
  }
  return (PyObject *)array;
}

static PyObject *y(py2dres *self, PyObject *){
  npy_intp dims[12];
  dims[0] = self->prog->Nt;
  PyArrayObject *array =  (PyArrayObject *)PyArray_SimpleNew(1,
                                                             dims,
                                                             NPY_DOUBLE);
  if (!array) return NULL;

  double *data = (double *)PyArray_DATA(array);
  for (int i=0; i<dims[0]; i++)
  {
    int n0 = self->prog->Coort[3*i];
    int n1 = self->prog->Coort[3*i + 1];
    int n2 = self->prog->Coort[3*i + 2];

    data[i] = (self->prog->Coorp[2*n0+1] +
               self->prog->Coorp[2*n1+1] +
               self->prog->Coorp[2*n2+1]) / 3.0;
  }
  return (PyObject *)array;
}

static PyObject *area(py2dres *self, PyObject *){
  npy_intp dims[12];
  dims[0] = self->prog->Nt;
  void *data = (void *)self->prog->mesh_->area;
  return PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, data);
}

static PyObject *set_dt(py2dres *self, PyObject *args)
{
  double dt;
  if (!PyArg_ParseTuple(args,"d", &dt)) return NULL;
  self->prog->dt = dt;
  self->prog->advect_->dt = dt;
  Py_RETURN_NONE;
}

static PyObject *in_alpha(py2dres *self, PyObject *args)
{
  double ina;
  if (!PyArg_ParseTuple(args,"d", &ina)) return NULL;
  self->prog->advect_->inject_alpha_ = ina;
  Py_RETURN_NONE;
}

static PyObject *set_flux(py2dres *self, PyObject *args)
{
  double flux;
  if (!PyArg_ParseTuple(args,"d", &flux)) return NULL;
  double new_flux = -flux / self->prog->mesh_->injLength_; // m/s
  self->prog->flux_in = new_flux;
  self->prog->mixte_->flux_in = new_flux;
  self->prog->advect_->flux_in = new_flux;
  Py_RETURN_NONE;
}

static PyMethodDef py2dres_methods[] = {
  {"triangle_count", (PyCFunction)py2dres_triangle_count, METH_NOARGS, "Return the number if triangles"},
  {"update_p", (PyCFunction)py2dres_update_p, METH_NOARGS, "update pressure solution"},
  {"update_a", (PyCFunction)py2dres_update_a, METH_NOARGS, "update advection"},
  {"mobility", (PyCFunction)mobility, METH_NOARGS, "return mobility array"},
  {"alpha", (PyCFunction)alpha, METH_NOARGS, "return advected scalar"},
  {"pressure", (PyCFunction)pressure, METH_NOARGS, "return advected scalar"},
  {"flux", (PyCFunction)flux, METH_NOARGS, "flux on triangle edges (Nt,3)"},
  {"edge_pressure", (PyCFunction)edge_pressure, METH_NOARGS,
   "pressure on triangle edges (Nt,3)"},
  {"x", (PyCFunction)x, METH_NOARGS, "x coordinate of element centroids [m]"},
  {"y", (PyCFunction)y, METH_NOARGS, "y coordinate of element centroids [m]"},
  {"area", (PyCFunction)area, METH_NOARGS, "mesh element areas [m^2]"},
  {"set_dt", (PyCFunction)set_dt, METH_VARARGS, "set timestep [seconds]"},
  {"in_alpha", (PyCFunction)in_alpha, METH_VARARGS, "set inflow alpha [] (between 0 and 1)"},
  {"set_flux", (PyCFunction)set_flux, METH_VARARGS, "set total influx [m^2/s]"},

  {NULL}  /* Sentinel */
};

static int py2dres_init(py2dres *self, PyObject *args, PyObject *kwds) {
  char *mesh_file;
  double p0, u_in;
  double dt = 1e-4;
  struct Prog::prog_arg prog_val;

  static char *kwlist[] = {"mesh_file", "p0", "u_in", NULL};

  if (! PyArg_ParseTupleAndKeywords(args, kwds, "sdd", kwlist,
                                    &mesh_file, &p0, &u_in))
    return -1;

  prog_val.meshfile           = mesh_file;
  prog_val.Rinje              = 1.0;
  prog_val.dt                 = dt;
  prog_val.mu1                = 0;
  prog_val.mu2                = 0;
  prog_val.p0                 = p0;
  prog_val.u_in               = u_in;
  prog_val.e_g                = 0.0;
  prog_val.MAXTIME            = 0.0;
  prog_val.visu_step          = 0;
  prog_val.init_alpha         = 0;
  prog_val.coort_file         = false;
  prog_val.coorp_file         = false;
  prog_val.alpha_out          = false;
  prog_val.fracmat            = false;
  prog_val.result             = false;
  prog_val.production_log     = false;
  prog_val.result_prefix      = "";

  self->prog = new Prog(prog_val);
  return 0;
}

static void
py2dres_dealloc(py2dres *self)
{
  Prog *p = self->prog;
  if (p)
    delete p;
  self->ob_type->tp_free((PyObject*)self);
}


static PyObject *py2dres_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  py2dres *self;
  self = (py2dres *)type->tp_alloc(type, 0);

  return (PyObject *)self;
}

static PyTypeObject py2dresType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "py2dres.py2dres",             /*tp_name*/
    sizeof(py2dres),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)py2dres_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "py2dres object",           /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    py2dres_methods,             /* tp_methods */
    0,                          /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)py2dres_init,      /* tp_init */
    0,                         /* tp_alloc */
    py2dres_new,                 /* tp_new */
};

static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC  /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initpy2dres(void)
{
    PyObject* m;

    if (PyType_Ready(&py2dresType) < 0)
        return;

    m = Py_InitModule3("py2dres", module_methods,
                       "module for py2dres wrapper");

    if (m == NULL)
      return;
    import_array();
    Py_INCREF(&py2dresType);
    PyModule_AddObject(m, "py2dres", (PyObject *)&py2dresType);
}
