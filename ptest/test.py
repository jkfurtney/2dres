from py2dres import py2dres

model = py2dres(mesh_file="test3.amdba", p0=0, u_in=1)
print model.triangle_count()
