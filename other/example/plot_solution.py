import numpy as np
import pylab as plt
from matplotlib.collections import PolyCollection

# node_id, x, y, node_type = np.loadtxt("cont2_px0.588889py0.722222ix0.100000iy0.455556e.1.node", comments="#", skiprows=1).T

# element_data = np.loadtxt("cont2_px0.588889py0.722222ix0.100000iy0.455556e.1.ele", comments="#", skiprows=1).T

# # the triangle data is index one
# element_data -= 1
# element_data = element_data.astype(int)

# element_id, nn0, nn1, nn2 = element_data

def parse_amdba_file(filename):
    f = open(filename, "r")
    nodes, elements = [], []
    npoints, ne = map(int, f.readline().split())
    print npoints, ne
    for i in range(npoints):
        nodes.append(map(float, f.readline().split()))
    for i in range(ne):
        elements.append(map(int, f.readline().split()))
    return np.array(nodes), np.array(elements)

node_data, element_data = parse_amdba_file("cont2_px0.588889py0.722222ix0.100000iy0.455556e.1.amdba")

element_data -= 1
element_data = element_data.astype(int)
element_id, nn0, nn1, nn2, _ = element_data.T
#node_data.astype(float)
node_id, x, y, node_type = node_data.T



verts = [ [(x[n0], y[n0]), (x[n1], y[n1]), (x[n2], y[n2])]  for n0, n1, n2 in zip(nn0, nn1, nn2) ]
#verts = [[[x[n0], x[n1], x[n2]], [y[n0], y[n1], y[n2]]] for n0, n1, n2 in zip(nn0, nn1, nn2)]
verts = np.array(verts)

_, colorby = np.loadtxt("cont2_px0.588889py0.722222ix0.100000iy0.455556e_result.mat").T

assert len(colorby) % len(x) == 0
res_list =  np.hsplit(colorby, len(colorby)/len(x))

plt.tricontourf(x,y,res_list[-1])
plt.show()

# # need to interpolate to cell centers to do this
# coll = PolyCollection(verts,
#                       array=colorby, linewidths=0,
#                       edgecolors='none')

# plt.gca().add_collection(coll)
# plt.gca().autoscale_view()
# plt.gca().set_aspect(1.0)
# plt.gcf().colorbar(coll, ax=plt.gca())
# plt.show()
