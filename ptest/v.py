import numpy as np

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
