#/usr/bin/python

from gencfg import gencfg
from genpoly import genpoly
from os import system, remove
from sys import exit
from jkf import divideList



#l2 is same as l1 with different leak location

# gencgf and genpoly need to be different for leakage


## 19 by 19 injector grid with producer in non symetrical position and a leak

# 82 well points 82 well segments
#injector pressure is 0.12513
# producer pressure is 0.0

# ref for leak is 5
pointsB="""
86 2 0 1
1 1.0 0.0 2
2 1.0 1.0 2
3 0.0 1.0 2
4 0.0 0.0 2
"""

segsB= """
86 1
1	1	2	2
2	2	3	2
3	3	4	2
4	4	1	2
"""


SMALL = 0.0001

c= [0.8/18.*i+0.1 for i in range(19)]

#print c, len(c)

#exit(0)


p = [] # list of producer postitions
i = [] # list of injector positions

runs = [] # [px, py, ix, iy]

for x in c: # this is all positions
    for y in c:        
        p.append([x,y])
            #print x,y

for x in c: # this is all positions
    for y in c:
        i.append([x,y])
        #print x,y


#print p[223]


for pw in [p[223]]:    #[0.58888888888888891, 0.72222222222222221]
    iw = [0.10, 0.4555555555555555555]
    runs.append(pw+iw) 
    




runner = []
prefix = "cont2"
for px,py,ix,iy in runs:    
    rname = "%s_px%lfpy%lfix%lfiy%lfe" % (prefix,px,py,ix,iy)    
    runner.append("2dres %s.cfg > /dev/null  \n" % (rname))
    
    gencfg(rname, rname+".1.amdba")
    genpoly(rname+".poly", ix,iy,px,py,0.01)
    system("triangle -pqa0.0002 %s.poly" % (rname))
    system("triangle2amdba.pl %s.1" % (rname))
    #remove("%s_px%lfpy%lfix%lfiy%lfe.1.poly" % (prefix,px,py,ix,iy))
    #remove("%s_px%lfpy%lfix%lfiy%lfe.poly" % (prefix,px,py,ix,iy))
    #remove("%s_px%lfpy%lfix%lfiy%lfe.1.ele" % (prefix,px,py,ix,iy))
    #remove("%s_px%lfpy%lfix%lfiy%lfe.1.node" % (prefix,px,py,ix,iy)) 

    # call triangle
    # call triangle2amdba

