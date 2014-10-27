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
mesh = fp.Grid2D(dx, dy, nx, ny)
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

Q = 7.5e-3
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



# plt.contourf(x, y, pressure.value, 10)
# plt.colorbar()
# plt.gca().set_aspect(1)
# plt.show()


# 1/0
# u, v = (-pressure.getGrad()).value
# u, v = new_mesh.map_cell_variable(u), new_mesh.map_cell_variable(v)
# U = np.sqrt(u**2 + v**2)

# # plt.streamplot(x, y, u, v)
# # plt.gca().set_aspect(1.0)
# # plt.show()


# at = new_mesh.map_cell_variable(pressure)

# np.save('four-point-p', at.data)
# np.save('four-point-x', x)
# np.save('four-point-y', y)
# np.save('four-point-u', u.data)
# np.save('four-point-v', v.data)
# np.save('four-point-m', v.mask)

# # plt.contourf(x,y,U)
# # plt.colorbar()
# # plt.show()

# s = fp.CellVariable(mesh=nm, name='s', value=0.0)
# V = -pressure.getFaceGrad()
# #s.faceGrad.constrain(0.0, mesh.exteriorFaces())
# s.constrain(1.0, in_face_mask)
# s.equation = (fp.TransientTerm() + fp.VanLeerConvectionTerm(coeff=V)==0)

# dt = dx/U.max()/2.0

# front_time = np.zeros_like(pressure.value)
# frontp =     np.zeros_like(pressure.value, dtype='bool')

# for i in range(1400000):
#     rat = 100.0 * frontp.sum() / len(frontp)
#     if i%100==0: print rat
#     s.equation.solve(var=s, dt=dt)
#     assign_mask = np.logical_and((s.value > 0.5), np.logical_not(frontp))
#     front_time[assign_mask] = dt*i
#     frontp[assign_mask] = True
#     if rat>99:
#         break

# # fill in remaining 1%
# assign_mask = np.logical_not(frontp)
# front_time[assign_mask] = i*dt

# sn = new_mesh.map_cell_variable(s)
# ft = new_mesh.map_cell_variable(front_time)
# plt.contourf(x,y,ft,25)
# plt.colorbar()
# plt.gca().set_aspect(1)
# plt.show()

# np.save('four-point', ft.data)
