# pylint: disable=invalid-name, missing-docstring, import-error
from paraview import simple

# read every other persistence diagram
CinemaReader = simple.TTKCinemaReader(DatabasePath="./data/pdiags.cdb")
CinemaFilter = simple.TTKCinemaQuery(InputTable=CinemaReader)
CinemaFilter.SQLStatement = """SELECT it.*, printf(
  "%s_%s_(%s-%s-%s)_%s",
  it.CaseName, it.ScalarField, it.B0x, it.B0y, it.B0z, it.ns
) AS Case_Field_B0_ns
FROM InputTable0 AS it
WHERE TimeStep % 2 = 0 AND ScalarField = 'mag(B)'"""
ProductReader = simple.TTKCinemaProductReader(Input=CinemaFilter)

#################
# DISTANCE MATRIX
#################

DistMat = simple.TTKPersistenceDiagramDistanceMatrix(Input=ProductReader)
DistMat.FilterPairs = "Number Of Pairs"
DistMat.NumberOfPairs = 20

#####################
# DIMENSION REDUCTION
#####################

DimRed = simple.TTKDimensionReduction(Input=DistMat)
DimRed.SelectFieldswithaRegexp = 1
DimRed.Regexp = "Diagram.*"
DimRed.Components = 3
DimRed.InputIsaDistanceMatrix = 1
DimRed.UseAllCores = 0

############
# CLUSTERING
############

# filter the input diagrams to cluster only the last ones
CineFilter2 = simple.TTKCinemaQuery(InputTable=CinemaFilter)
CineFilter2.SQLStatement = """SELECT * FROM InputTable0
WHERE TimeStep > 2450 AND (TimeStep % 10 = 0)"""
ProdRead2 = simple.TTKCinemaProductReader(Input=CineFilter2)

# cluster the last diagrams
Cluster = simple.TTKPersistenceDiagramClustering(Input=ProdRead2)
Cluster.Numberofclusters = 4
Cluster.Maximalcomputationtimes = 100.0

###############################################
# MERGE CLUSTERING INTO REDUCED DISTANCE MATRIX
###############################################

# convert clustering FieldData to vtkTable
ds2t = simple.TTKDataSetToTable(Input=Cluster)
ds2t.DataAssociation = "Field"

mergeQuery = simple.TTKCinemaQuery(InputTable=[ds2t, DimRed])
# exclude distance matrix from the result, too many columns for SQLite
mergeQuery.ExcludecolumnswithaRegexp = 1
mergeQuery.Regexp = "Diagram.*"
mergeQuery.SQLStatement = """
-- distance matrix tuples
-- with dummy clustering data
-- without clustering tuples
SELECT dm.*, -1 AS ClusterId
FROM InputTable1 AS dm
LEFT OUTER JOIN InputTable0 AS cl
USING (Case_Field_B0_ns, TimeStep)
WHERE cl.ClusterId is null

UNION

-- clustering tuples
-- with distance matrix data
SELECT dm.*, cl.ClusterId
FROM InputTable1 AS dm
JOIN InputTable0 AS cl
USING (Case_Field_B0_ns, TimeStep)"""

# generate points from 3D coordinates
t2p = simple.TableToPoints(Input=mergeQuery)
t2p.XColumn = "Component_0"
t2p.YColumn = "Component_1"
t2p.ZColumn = "Component_2"
t2p.KeepAllDataArrays = 1
tetra = simple.Tetrahedralize(Input=t2p)

# generate a heat map from distance matrix
# save output distance matrix and point cloud trajectories
simple.SaveData("./spaceWeather_distmat.csv", Input=DimRed)
simple.SaveData("./spaceWeather_trajectories.vtu", simple.CleantoGrid(Input=tetra))
