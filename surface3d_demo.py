#!/usr/bin/env python

#x-run id
#y-no. of iterations
#z-log likelihood
from mpl_toolkits.mplot3d import Axes3D
from scipy.interpolate import griddata
import matplotlib.pyplot as plt
import numpy as np
import itertools
with open('/home/priya/Results-2-27-2014_Alarm/500_DETER_DIST_PC/19/0/0.3/0.3.log') as myfile:
    lines =  myfile.readlines()

head = [lines[0:20] for x in xrange(20)]
    #head = [lines[21:40] for x in xrange(20)]
    #head = [lines[41:60] for x in xrange(20)]
    #head = [lines[61:80] for x in xrange(20)]
print head

x=list(range(20))
print x
y=list(range(20))
for index, line in enumerate(lines[0:20]):
   y[index] = int(line.split()[5])
print(y)
z=list(range(20))
for index, line in enumerate(lines[0:20]):
   z[index] = float(line.split()[6])
print(z)

z = map(float, z)
grid_x, grid_y = np.mgrid[min(x):max(x):100j, min(y):max(y):100j]
grid_z = griddata((x, y), z, (grid_x, grid_y), method='cubic')

fig = plt.figure()
ax = fig.gca(projection='3d')
ax.plot_surface(grid_x, grid_y, grid_z, cmap=plt.cm.Spectral)
plt.show()
