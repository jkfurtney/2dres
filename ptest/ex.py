import numpy as np
import pylab as plt

Q = 7.5e-3 # m^3/s
a = 1.68e-4

mu = 1.58e-4 # Pa s

p = 12.0 * Q * mu/np.pi /a**3 * np.log(80.0/0.001)
print "{:.3e}".format(p)
