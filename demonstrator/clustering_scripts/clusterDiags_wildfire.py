# pylint: disable=invalid-name, missing-docstring, import-error
from paraview import simple

# read every other persistence diagram
CinemaReader = simple.TTKCinemaReader(DatabasePath="../precomputed_diagrams/wildfire/pdiags.cdb")
CinemaFilter = simple.TTKCinemaQuery(InputTable=CinemaReader)
CinemaFilter.SQLStatement = """SELECT * FROM InputTable0
WHERE TimeValue != 1
"""
ProductReader = simple.TTKCinemaProductReader(Input=CinemaFilter)


############
# CLUSTERING
############

Cluster = simple.TTKPersistenceDiagramClustering(Input=ProductReader)
Cluster.Numberofclusters = 4
Cluster.Maximalcomputationtimes = 10.0
Cluster.Displayingmethod = 'Clusters as stars' 
Cluster.Spacing = 1.05

simple.SaveData("./wildfire_clustered_diagrams.vtu", proxy=simple.OutputPort(Cluster,0))
simple.SaveData("./wildfire_centroids.vtu", proxy=simple.OutputPort(Cluster,1))
