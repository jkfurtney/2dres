from numpy.distutils.core import setup
from numpy.distutils.misc_util import Configuration

def configuration(parent_package='', top_path=None):
    config = Configuration('py2dres', parent_package, top_path)
    config.add_extension("py2dres",
                          sources=["py2dres.cpp",
                                   "Prog.cc",
                                   "MixHy.cc",
                                   "Mesh.cc",
                                   "IterAlph.cc",
                                   "ConjGrad.cc",
                                   "mylibrary.cc"],
                          include_dirs=['.'])

    return config

if __name__ == '__main__':
    setup(**configuration(top_path='').todict())
