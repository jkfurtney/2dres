#!/usr/bin/python

#  this program makes a list of nodes and segments for holes suitable for
#  inserting into a .poly file. the user has control over the following
#  paramaters via command line arguments

#  -r [float] the radius of the injector well (default is 1)
#  -x [float] the x coordinate of the center of the ciricle (default 0)
#  -y [float] the y coordinate of the center of the ciricle (default 0)
#  -n [int] the number of points (default is 50)
#  -s [int] starting node number (default is 0)
#  -a [int] refrece number       (default is 0)

#  values of n below 6 do not produce meaningfull results

#  the actual number of points may vary if an odd number is given for n


import sys
from math import *
import getopt

optlist, args = getopt.getopt(sys.argv[1:], 'r:x:y:n:s:a:')
n = 50
r = 1
x0 = 0
y0 = 0
start = 1
a = 0      # refrence number
for opt in optlist:
    if opt[0] == '-r': r = float(opt[1])
    elif opt[0] == '-x': x0 = float(opt[1])
    elif opt[0] == '-y': y0 = float(opt[1])
    elif opt[0] == '-n': n = int(opt[1])
    elif opt[0] == '-s': start = int(opt[1])
    elif opt[0] == '-a': a = int(opt[1])

n = int(n / 2.0)
print "# r, x0, y0, n ", r , x0, y0, n
p = []

for i in range(n):
    theta = pi*(i)*(1.0/float(n))
    s = sin(theta)
    c = cos(theta)
    tx = c * r + x0
    ty = s * r 
    print (start + i), tx, ty + y0, a
    p.append([tx,ty])

print start + n, x0-r, y0, a

p.reverse()
i=0
#draw the lower half
for px, py in p[:-1]:
    i = i + 1;
    print (i+n+start), px, -1*py + y0, a

print 
#link all the segments
for i in range(2*n)[:-1]:
    j = i + start
    print j, j, j+1, a

# final point
print 2*n+start-1, 2*n+start-1, start, a
