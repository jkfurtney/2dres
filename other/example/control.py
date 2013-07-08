# what plots to do?
# conts of pressure for several external pressures
# conts of alpha f of time
# conts of alpha for constant time but different ext press

from feread import mesh, data, data_get2
from pylab import *
import os

def filldata(d):
    t1 = [[x for x in range(len(X))] for y in range(len(Y))]
    t0 = [[x for x in range(len(X))] for y in range(len(Y))]

    for x in range(len(X)):
        for y in range(len(Y)):
            t0[x][y] = data_get2(d, X[x], Y[y], 0)
            t1[x][y] = data_get2(d, X[x], Y[y], 1)
    return t0, t1


#lop = [0, -0.0005, -0.001, -0.0015, -0.002, -0.0025, -0.003, -0.0035, -0.004, -0.0045, -0.005]

# datas = []
# l= os.listdir("./data/")
# for li in l:
#     if li.endswith("mat"):datas.append(li)



wells = [ [0.588889, 0.100000],
          [0.500000, 0.500000],
          [0.100000, 0.544444],
          [0.100000, 0.455556]]

prod = [0.588889, 0.722222]

m = mesh("cont.amdba",10,10)
d = data("cont_result.mat",m)

X = arange(0,1.00001,0.01)
Y = arange(0,1.00001,0.01)
XG, YG = meshgrid(X,Y)

t0p, t0a = filldata(d)
d.advance()#2
d.advance()
d.advance()
d.advance()
d.advance()
d.advance()
t0p, t1a = filldata(d)
d.advance()#9
d.advance()
d.advance()
d.advance()
d.advance()
d.advance()
t0p, t2a = filldata(d)
d.advance()#16
d.advance()
d.advance()
d.advance()
d.advance()
d.advance()
t0p, t3a = filldata(d)
d.advance()#22
d.advance()
d.advance()
d.advance()
d.advance()
d.advance()
t0p, t4a = filldata(d)
d.advance()#28
d.advance()
d.advance()
d.advance()
d.advance()
t0p, t5a = filldata(d)



Z1p=array(t0p)
Z1=array(t0a)
Z2=array(t1a)
Z3=array(t2a)
Z4=array(t3a)
Z5=array(t4a)
Z6=array(t5a)



def max2(a):
    return max(max(a))
def min2(a):
    return min(min(a))

def norm(a):
    a=a - min2(a)
    a= a/(max2(a))
    return a

wx1 = [wells[0][1], prod[1]]
wy1 = [wells[0][0], prod[0]]

wx2 = [wells[1][1], prod[1]]
wy2 = [wells[1][0], prod[0]]

wx3 = [wells[3][1], prod[1]]
wy3 = [wells[3][0], prod[0]]


figure(1)
subplot(221)
title("Injected Fluid Front at Several Times")

scatter(wx3, wy3)
cnt = contour(XG, YG, Z1, [0.5])
cnt = contour(XG, YG, Z2, [0.5])
cnt = contour(XG, YG, Z3, [0.5])
cnt = contour(XG, YG, Z4, [0.5])
cnt = contour(XG, YG, Z5, [0.5])
cnt = contour(XG, YG, Z6, [0.5])

subplot(222)
title("Pore Pressure")
scatter(wx3, wy3)
p = norm(Z1p)
cnts = [0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9]
C = contour(XG, YG, p, cnts, colors=('k',) )
#clabel(C, fontsize=9, inline=1, fmt='%1.5f')
show()
