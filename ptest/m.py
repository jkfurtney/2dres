from PointSet import PointSet, circleArc, PolyWriter, GeoWriter
import numpy as np
import pylab as plt

n=60
p = circleArc(0, 360, n, dropFirstPoint=True)
p = p.scale(60).shift(0,60)
exterior = PointSet(p._data)


x,y = exterior.get()
# plt.plot(x,y,"*")
# plt.gca().set_aspect(1)
# plt.show()

print "{n} 2 0 1".format(n=n)
i=1
for xx,yy in zip(x,y):
    print i, xx,yy, 2
    i+=1


p0, p1 = np.array([0,25.0]), np.array([0,105.0])
s = 1
for p in [p0,p1]:
    ne = p + (s,s)
    se = p + (s,-s)
    sw = p + (-s,-s)
    nw = p + (-s,s)
    for pp in [ne,se,sw,nw]:
        print i, pp[0], pp[1], 3
        i+=1


print "{n}".format(n=n)

for i in range(n-1):
    nn = 1+i
    if i==n-2:
        print nn, nn, 1, 2
    else:
        print nn, nn, nn+1, 2
