from py2dres import py2dres
import numpy as np
import pylab as plt
from matplotlib.collections import PolyCollection

Q = 7.5e-3 # m^3/s
a = 1.68e-4
u_in = Q/a  # m^2/s
mu = 1.58e-4 # Pa s

mobility_factor = a**2/mu/12.0

mesh_file = "/home/jkf/src/2dres/ptest/small_inlet.1.amdba"
mesh_file = "/home/jkf/src/2dres/ptest/geo.1.amdba"
mesh_file = "/home/jkf/src/2dres/test3.amdba"
mesh_file = "/home/jkf/src/2dres/ptest/full.1.amdba"

model = py2dres(mesh_file=mesh_file,
                p0=0, u_in=u_in)
dt = 0.5*model.area().min()/u_in*4.0
model.set_dt(dt)
print "new timestep ", dt
print model.triangle_count()
mobility = model.mobility()
assert mobility.shape == (model.triangle_count(),)
assert (mobility==1.0).all()

mobility *= mobility_factor
assert (model.mobility()==mobility_factor).all()

model.update_p()
print "{:.4e}".format(model.pressure().max())
print "{:.4e}".format(model.flux().flatten().max())

def parse_amdba_file(filename):
    f = open(filename, "r")
    nodes, elements = [], []
    npoints, ne = map(int, f.readline().split())
    #print npoints, ne
    for i in range(npoints):
        nodes.append(map(float, f.readline().split()))
    for i in range(ne):
        elements.append(map(int, f.readline().split()))
    return np.array(nodes), np.array(elements)

node_data, element_data = parse_amdba_file(mesh_file)
element_data -= 1
element_data = element_data.astype(int)
element_id, nn0, nn1, nn2, _ = element_data.T
#node_data.astype(float)
node_id, x, y, node_type = node_data.T
verts = [ [(x[n0], y[n0]), (x[n1], y[n1]), (x[n2], y[n2])]  for n0, n1, n2 in zip(nn0, nn1, nn2) ]
#verts = [[[x[n0], x[n1], x[n2]], [y[n0], y[n1], y[n2]]] for n0, n1, n2 in zip(nn0, nn1, nn2)]
verts = np.array(verts)

colorby = model.pressure()

# plt.tricontourf(x,y,colorby)
# plt.gca().set_aspect(1.0)
# plt.colorbar()
# plt.show()

# coll = PolyCollection(verts,
#                       array=colorby, linewidths=0,
#                       edgecolors='none')

# plt.gca().add_collection(coll)
# plt.gca().autoscale_view()
# plt.gca().set_aspect(1.0)
# plt.gcf().colorbar(coll, ax=plt.gca())
# plt.show()


alpha = model.alpha()
for i in range(100000):
    model.update_a()
print alpha.max(), alpha.min()

coll = PolyCollection(verts,
                      array=model.alpha(), linewidths=0,
                      edgecolors='none')

plt.gca().add_collection(coll)
plt.gca().autoscale_view()
plt.gca().set_aspect(1.0)
plt.gcf().colorbar(coll, ax=plt.gca())
plt.show()
