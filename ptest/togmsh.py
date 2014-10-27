import numpy as np
import pylab as plt

ndata = np.loadtxt("geo.1.node",skiprows=1)
_,x,y,_ = ndata.T
edata = np.loadtxt("geo.1.ele", skiprows=1)
_,t0,t1,t2 = edata.T

header = """$MeshFormat
2.2 0 8
$EndMeshFormat
$Nodes
{number_of_nodes}""".format(number_of_nodes=len(x))

print header

for i,xx in enumerate(x):
    print i+1, xx, y[i], 0.0

mid = """$EndNodes
$Elements
{number_of_elements}""".format(number_of_elements=len(t0))
print mid

for i, t in enumerate(t0):
    print i+1, 2, 2, 1, 2, int(t0[i]), int(t1[i]), int(t2[i])

footer ="""$EndElements"""
print footer
