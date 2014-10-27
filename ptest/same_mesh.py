import fipy.tools.numerix as np
import fipy as fp
from scipy.constants import gallon, minute, foot, psi
import pylab as plt
from mesh_creator import MeshCreator

lx = 60.
ly = 120.
#dx = dy = 1e-1
nx = 50
ny = 100
dx, dy = lx/nx, ly/ny
mesh = fp.Gmsh2D("small_inlet.msh")
X, Y = mesh.faceCenters
xx, yy = mesh.cellCenters
y, x = np.mgrid[0:ly:ny*1j, 0:lx:nx*1j]

nm = mesh
pressure = fp.CellVariable(mesh=nm, name='pressure')
from fipy import Viewer
v = Viewer(vars=(pressure))


a = 1.68e-4  #  fracture aperture
mu = 1.58e-4
D = a**2/12.0/mu
pressure.equation = ( fp.DiffusionTerm(coeff=(D)) == 0 )

inj = np.sqrt((Y-25)**2 + (X)**2).argmin()
prod = np.sqrt((Y-105)**2 + (X)**2).argmin()
print "inj at {}, {}".format(X[inj],Y[inj])
print "prod at {}, {}".format(X[prod],Y[prod])
in_faces = [inj]

in_face_mask = np.zeros(X.shape,dtype=bool)
in_face_mask[inj] = True

out_face_mask = np.zeros(X.shape,dtype=bool)
out_face_mask[prod] = True

pressure.constrain(0.0, where=out_face_mask)

# give each face an outward velocity constraint

Q = 7.5e-3/2.0
scalar_velocity = Q / (dy) / a
for face_id in in_faces:
    in_face_mask = np.array([tx==face_id for tx in range(len(X))])
    assert in_face_mask.sum()==1, "mask error"
    #scalar_velocity = 1.0
    # oriented normals seem to be outward facing.
    gradient_boundary = scalar_velocity/D * nm._getOrientedFaceNormals().T[face_id]
    gx, gy = gradient_boundary
    print gx, gy
    pressure.faceGrad.constrain(((gx,),(gy,),), in_face_mask)

in_face_mask =  np.array([tx in in_faces for tx in range(len(X))])

pressure.equation.solve(var=pressure)

print "max pressure {:.3e}".format(pressure.value.max())
print "min pressure {:.3e}".format(pressure.value.min())
