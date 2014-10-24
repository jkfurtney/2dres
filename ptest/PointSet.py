import string
import numpy as np
import pylab as plt
from scipy.constants import inch

def circleArc(a0, a1, n, dropFirstPoint=True):
    """PointSet Factory, create point set as a circle segment. Angle of
    zero deg is defined as vertical. Input is in Degrees.

    """
    theta = np.linspace(np.deg2rad(a0), np.deg2rad(a1), n )
    x, y = np.sin(theta), np.cos(theta)
    if dropFirstPoint:
        x,y = x[1:], y[1:]
    return PointSet(np.array([x,y]).T)


class PointSet(object):
    """
    A collection of 2d points and common operations on these points
    """
    def __init__(self, data=None):
        """ Data shape should be (n,2)"""
        self._data = np.copy(data)

    def get(self):
        return self._data.T

    def scale(self,factor):
        """ Scale all points by factor: Returns a new PointSet
        """
        return PointSet(self._data*factor)

    def shift(self,x,y):
        """ translate all points by x,y: Returns a new PointSet
        """
        return PointSet(self._data + [x,y])

    def rotate(self, angle):
        """rotate points by angle about the origin: Returns a new PointSet
        Counter-Clockwise rotation for positive angles.

        """
        theta = np.deg2rad(angle)
        R = np.matrix([[np.cos(theta), -np.sin(theta)],
                       [np.sin(theta), np.cos(theta)]])
        return PointSet(np.array(R*np.mat(self._data).T).T)

    def append(self, other):
        """Add PointSet other to the current PointSet, changes are made inplace
        """
        assert type(other)==PointSet
        self._data = np.vstack((self._data, other._data))

    def length(self):
        """return a characteristic length between points """
        return np.sqrt(((self._data[0]-self._data[1])**2).sum())

    def __len__(self): return self._data.shape[0]

    def reflect(self, theta):
        """Reflect points about a line through the origin at angle theta.
        Input is in degrees, a zero angle is defined as vertical.

        """
        if theta==0:  #  reflection about x=0
            return PointSet(self._data * [-1.0, 1.0])
        if theta==90:
            return PointSet(self._data * [1.0, -1.0])
        a = 1.0/np.tan(np.deg2rad(theta))
        assert not a == 0
        x, y = self._data.T
        d = (x + a*y) / (1+a**2)
        xnew = 2*d - x
        ynew = 2*d*a - y
        return PointSet(np.array([xnew, ynew]).T)
        #return PointSet(np.array([y/a, a*x]).T)

class GeoWriter(object):
    """Take two PointSets representing the outer boundary and well
    locations, write them out as a geo file for gmsh to tetmesh

    """
    def __init__(self, ps_exterior, ps_wells):
        assert type(ps_exterior)==PointSet
        assert type(ps_wells)==PointSet
        self.exterior = ps_exterior._data
        self.elen = ps_exterior.length()
        self.wells = ps_wells._data
        self.count = 1
        self.loop_count = 1

    def write_line_loop(self, f, points):
        i=0
        for x,y in points:
            print >> f, "Point(%i) = {%lf, %lf,0,%lf};" % (i + self.count,
                                                           x,y,self.elen)
            i+=1
        N = len(points)
        for i in range(N):
            ii, jj = i, (i+1)%N
            print >> f, "Line(%i) = {%i, %i};" % (ii+self.count,
                                                  ii+self.count,
                                                  jj+self.count)
        print >> f, "Line Loop(%i) = {" % (self.loop_count),
        print >> f,  ", ".join(map(str,np.arange(N)+self.count)), "};"
        print >> f, "Physical Line(\"%i\") = {" % (self.loop_count),
        print >> f,  ", ".join(map(str,np.arange(N)+self.count)), "};"
        self.loop_count += 1
        self.count += N

    def write(self, filename='test.geo', well_scale=None):
        f = open(filename, "w")
        self.write_line_loop(f, self.exterior)
        if well_scale is None:
            offsets = self.elen * np.array([[0.5,   0.5],
                                            [0.5,  -0.5],
                                            [-0.5, -0.5],
                                            [-0.5,  0.5]])
        else:
            well_scale = well_scale/2.0
            hexagon = np.array(((-0.866025403784440, 1.50000000000000),
                                (0.866025403784440, 1.50000000000000),
                                (1.73205080756888, 0),
                                (0.866025403784440, -1.50000000000000),
                                (-0.866025403784440, -1.50000000000000),
                                (-1.73205080756888, 0)))/1.50
            # 8.75 diameter well bore,
            perimeter = 8.75 * inch * np.pi
            apothem = perimeter / 4.0 / np.sqrt(3.0)
            offsets = hexagon * apothem
            # offsets = np.array([[well_scale,   well_scale],
            #                     [well_scale,  -well_scale],
            #                     [-well_scale, -well_scale],
            #                     [-well_scale,  well_scale]])

        for p in self.wells:
            self.write_line_loop(f,[p+o for o in offsets])
        # wells now linesegment count is N
        print >> f, "Plane Surface(1) = {%s};" % \
            (", ".join(map(str, np.arange(self.loop_count-1)+1)))
        print >> f, "Physical Surface(1) = {%s};" % \
            (", ".join(map(str, np.arange(self.loop_count-1)+1)))
        f.close()

class PolyWriter(object):
    """This class is a different version of the GeoWriter, It writes the
    mesh data out as a poly file suitable for use with the triangle program.

    """
    def __init__(self, ps_exterior, ps_wells, size):
        assert type(ps_exterior)==PointSet
        assert type(ps_wells)==PointSet
        self.exterior = ps_exterior._data
        self.elen = size
        self.wells = ps_wells._data
        self.node_count = 1
        self.loop_count = 1
        self.node_section = []
        self.segment_section = []


    def write_line_loop(self, points, boundary_code):
        i=0
        for x,y in points:
            self.node_section.append("%i %lf %lf %s" % (i + self.node_count,
                                                           x,y, boundary_code))
            i+=1

        N = len(points)
        for i in range(N):
            ii, jj = i, (i+1)%N
            self.segment_section.append("%i %i %i %s" % (ii+self.node_count,
                                                         ii+self.node_count,
                                                         jj+self.node_count, boundary_code))
        self.loop_count += 1
        self.node_count += N

    def write(self, filename='test.poly', well_scale=None):

        self.write_line_loop(self.exterior, "2")

        if well_scale is None:
            offsets = self.elen * np.array([[0.5,   0.5],
                                           [0.5,  -0.5],
                                           [-0.5, -0.5],
                                           [-0.5,  0.5]])
        else:
            well_scale = well_scale/2.0
            offsets = np.array([[well_scale,   well_scale],
                                [well_scale,  -well_scale],
                                [-well_scale, -well_scale],
                                [-well_scale,  well_scale]])


        for i, p in enumerate(self.wells):
            boundary_code = "well%02i" % (i+2)
            self.write_line_loop([p+o for o in offsets], boundary_code)

        f = open(filename, "w")

        print >> f, "%i 2 0 1" % (len(self.node_section))
        for line in self.node_section:
            print >> f, line

        print >> f, "\n %i 1" % (len(self.segment_section))
        for line in self.segment_section:
            print >> f, line

        print >> f, "\n %i" % (len(self.wells))
        for i, well in enumerate(self.wells):
            print >> f, "%i %lf %lf" % (i+1, well[0], well[1])


        f.close()


def test_reflection():
    p = PointSet([[1.0,1.0]])

    data = []
    data.append(p.reflect(-60.0)._data[0])
    data.append(p.reflect(-30.0)._data[0])
    data.append(p.reflect(0.0)._data[0])
    data.append(p.reflect(30.0)._data[0])
    data.append(p.reflect(60.0)._data[0])
    data.append(p.reflect(90.0)._data[0])
    data = np.array(data)

    known = np.array([(-0.366025403784440, -1.36602540378444),
                       (-1.36602540378444, -0.366025403784441),
                       (-1.00000000000000, 1.00000000000000),
                       (0.366025403784440, 1.36602540378444),
                       (1.36602540378444, 0.366025403784440),
                       (1.00000000000000, -1.00000000000000)])

    np.testing.assert_allclose(data, known)




if __name__ == '__main__':
    test_reflection()

    sin60 = np.sin(np.deg2rad(60.0))
    p = PointSet(np.array([[0.5,sin60], [1,0], [0.5,-sin60], [-0.5,-sin60],
                           [-1,0], [-0.5,sin60], [0,0]]))
    n=0
    for angle in [0, 60, 120, 180, 240, 300]:
        n += 1
        plt.subplot(3, 4, n)
        x, y = p.rotate(angle).get()
        plt.text(0.2, 0.2, "%i"%n)
        for xx, yy in zip(x,y):
            plt.plot(xx,yy,"o")

        plt.xticks([])
        plt.yticks([])
        plt.gca().set_frame_on(False)
        plt.gca().set_aspect(1.0)
        plt.xlim((-1.2,1.2))
        plt.ylim((-1.2,1.2))



    for angle in [60, 0, -60, 90, -30, 30]:
        n += 1
        plt.subplot(3, 4, n)
        plt.text(0.2, 0.2, "%i"%n)
        x, y = p.reflect(angle).get()
        for xx, yy in zip(x,y):
            plt.plot(xx,yy,"o")
        plt.xticks([])
        plt.yticks([])
        plt.gca().set_frame_on(False)
        plt.gca().set_aspect(1.0)
        plt.xlim((-1.2,1.2))
        plt.ylim((-1.2,1.2))



    plt.show()
