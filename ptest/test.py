from py2dres import py2dres
model = py2dres(mesh_file="./geo.1.amdba", p0=0, u_in=1)
print model.triangle_count()
arr = model.mobility()

assert arr.shape == (model.triangle_count(),)
assert (arr==1.0).all()

print model.pressure().max()
