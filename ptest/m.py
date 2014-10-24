from PointSet import PointSet, circleArc, PolyWriter, GeoWriter
import numpy as np
import pylab as plt


p = circleArc(0, 180, 30, dropFirstPoint=False)
p = p.scale(60).shift(0,60)
exterior = PointSet(p._data)


x,y = exterior.get()
# plt.plot(x,y,"*")
# plt.gca().set_aspect(1)
# plt.show()

print ""

for xx,yy in zip(x,y):
    print xx,yy
