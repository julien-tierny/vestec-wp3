#!/usr/bin/env python3

# pylint: disable=invalid-name, missing-docstring, import-error, too-many-locals

import argparse
import pathlib
import subprocess

from paraview import simple


def fetch_data_from_ftp(args):
    """Fetch data from DLR FTP with wget"""
    dst_dir = pathlib.Path(args.destdir)
    if not dst_dir.is_dir():
        dst_dir.mkdir()

    # call wget with the relevant set of parameters
    cmd = [
        "wget",
        "-m",
        "-c",
        "-nd",
        "--directory-prefix=" + dst_dir.name,
        "--user=" + args.username,
        "--password=" + args.password,
        "ftp://ftp.dlr.de/datasets/diseases/Output/probability",
    ]
    subprocess.check_call(cmd)


def generate_persistence_diagrams(args):
    """Generate Persistence Diagrams from FBK data and store them inside a Cinema Database"""

    inpdatadir = pathlib.Path(args.input_dir)

    for nc in inpdatadir.glob("*.nc"):
        # parse simulation parameters from datasets names
        nc_params = nc.name.split(".")[0].split("_")
        Type = nc_params[0]
        It = "It" + nc_params[-3]
        Day = nc_params[-1]
        FieldName = "probability" if Type == "prob" else Type
        params = {"Type": Type, "It": It, "Day": Day}

        # use ParaView to read NetCDF files
        mosq = simple.NetCDFReader(FileName=[str(nc)])
        mosq.Dimensions = "(lat, lon)"
        mosq.SphericalCoordinates = False

        # transform NetCDF to vtkImageData
        resample = simple.ResampleToImage(Input=mosq)
        resample.SamplingDimensions = [244, 295, 1]

        # use TTKHarmonicField to generate missing pixel values
        # without messing with topology
        ttkId = simple.TTKIdentifiers(Input=resample)

        selection = simple.SelectPoints()
        selection.QueryString = FieldName + " > 0"
        selection.FieldType = "POINT"
        extractSel = simple.ExtractSelection(Input=ttkId, Selection=selection)

        ttkHarm = simple.TTKHarmonicField(
            InputGeometry=ttkId, InputConstraints=extractSel
        )
        ttkHarm.ConstraintValues = FieldName
        ttkHarm.Solver = "Iterative"

        # more cleaning: use GaussianResampling to merge topological peaks
        gauss = simple.GaussianResampling(Input=ttkHarm)
        gauss.ResampleField = ["POINTS", "OutputHarmonicField"]
        gauss.ResamplingGrid = [488, 590, 3]
        gauss.GaussianSplatRadius = 0.02
        gauss.GaussianExponentFactor = -10.0
        gauss.SplatAccumulationMode = "Sum"

        slic = simple.Slice(Input=gauss)
        slic.SliceType = "Plane"
        slic.SliceType.Normal = [0.0, 0.0, 1.0]
        rsi = simple.ResampleToImage(Input=slic)
        rsi.SamplingDimensions = [488, 590, 1]

        # add parameters as Field Data
        arred = simple.TTKArrayEditor(Target=rsi)
        arred.TargetAttribute = "Field Data"
        arred.DataString = "\n".join([",".join(tup) for tup in params.items()])

        # store pre-processing result with topological compression
        cinewriter0 = simple.TTKCinemaWriter(Input=rsi)
        cinewriter0.DatabasePath = args.tcomp_cdb_dir
        cinewriter0.Storeas = 2
        cinewriter0.ForwardInput = False

        # compute persistence diagram
        persdiag = simple.TTKPersistenceDiagram(Input=rsi)
        persdiag.DebugLevel = 3

        # save file in Cinema Database
        cinewriter1 = simple.TTKCinemaWriter(Input=persdiag)
        cinewriter1.DatabasePath = args.pdiags_cdb_dir
        cinewriter1.ForwardInput = False

        # trigger the pipeline by saving the empty output of TTKCinemaWriter
        simple.SaveData("empty.vtu", Input=cinewriter0)
        simple.SaveData("empty.vtu", Input=cinewriter1)
        print(">> Processed " + str(nc))


def compute_distances_and_clustering(args):
    """Compute distance matrix between persistence diagrams"""

    # read the Cinema Database index
    cineRead = simple.TTKCinemaReader(DatabasePath=args.cdb_dir)

    # perform SQL query on the Cinema Database index
    query = simple.TTKCinemaQuery(InputTable=cineRead)
    query.SQLStatement = (
        "SELECT * FROM InputTable0"  # WHERE It='it2' OR It='it3' OR It='it4'"
    )

    # load the filtered products (persistence diagrams) from the database
    prodRead = simple.TTKCinemaProductReader(Input=query)

    ###########################################
    # DISTANCE MATRIX AND DIMENSION REDUCTION #
    ###########################################

    # compute the distance matrix between the persistence diagrams
    distMat = simple.TTKPersistenceDiagramDistanceMatrix(Input=prodRead)
    distMat.NumberofPairs = 50

    # use MDS to reduce the distance into a 3-dimensional space
    dimRed = simple.TTKDimensionReduction(Input=distMat)
    dimRed.SelectFieldswithaRegexp = 1
    dimRed.Regexp = "Diagram.*"
    dimRed.Components = 3
    dimRed.InputIsaDistanceMatrix = 1
    dimRed.UseAllCores = 0

    ##############
    # CLUSTERING #
    ##############

    # filter input
    filterDiags = simple.TTKCinemaQuery(InputTable=query)
    filterDiags.SQLStatement = """SELECT * FROM InputTable0
WHERE Day >= 170 AND Day <= 190"""

    cluster = simple.TTKPersistenceDiagramClustering(Input=filterDiags)
    cluster.NumberOfClusters = 5  # one for each "iteration"
    cluster.Maximalcomputationtimes = 100.0  # do it in less than 100s

    #################################################
    # MERGE CLUSTERING INTO REDUCED DISTANCE MATRIX #
    #################################################

    # convert clustering FieldData to vtkTable
    ds2t = simple.TTKDataSetToTable(Input=cluster)
    ds2t.DataAssociation = "Field"

    mergeQuery = simple.TTKCinemaQuery(InputTable=[ds2t, dimRed])
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
USING (It, Day)
WHERE cl.ClusterId is null

UNION

-- clustering tuples
-- with distance matrix data
SELECT dm.*, cl.ClusterId
FROM InputTable1 AS dm
JOIN InputTable0 AS cl
USING (It, Day)"""

    # generate points from 3-dimensional coordinates
    t2p = simple.TableToPoints(Input=mergeQuery)
    t2p.XColumn = "Component_0"
    t2p.YColumn = "Component_1"
    t2p.ZColumn = "Component_2"
    t2p.KeepAllDataArrays = 1

    # converts from vtkPolyData to vtkUnstructuredGrid
    tetra = simple.Tetrahedralize(Input=t2p)

    # generate a heat map from the distance matrix
    hm = simple.TTKMatrixToHeatMap(Input=distMat)
    hm.SelectFieldswithaRegexp = 1
    hm.Regexp = "Diagram.*"

    # save output
    simple.SaveData("disease_trajectories.vtu", Input=tetra)
    simple.SaveData("disease_heatmap.vtu", Input=hm)
    simple.SaveData("disease_distmat.csv", Input=dimRed)


def main():
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers()

    datadir = "input_dir"
    pdiags_cdbdir = "pdiags.cdb"
    tcomp_cdbdir = "tcomp.cdb"

    fetchpars = subparsers.add_parser("fetch")
    fetchpars.add_argument("-u", "--username", type=str, required=True)
    fetchpars.add_argument("-p", "--password", type=str, required=True)
    fetchpars.add_argument("-d", "--destdir", type=str, default=datadir)
    fetchpars.set_defaults(func=fetch_data_from_ftp)

    generate_pd = subparsers.add_parser("generate")
    generate_pd.add_argument("-i", "--input_dir", type=str, default=datadir)
    generate_pd.add_argument("-p", "--pdiags_cdb_dir", type=str, default=pdiags_cdbdir)
    generate_pd.add_argument("-t", "--tcomp_cdb_dir", type=str, default=tcomp_cdbdir)
    generate_pd.set_defaults(func=generate_persistence_diagrams)

    cluster_pd = subparsers.add_parser("cluster")
    cluster_pd.add_argument("-p", "--pdiags_cdb_dir", type=str, default=pdiags_cdbdir)
    cluster_pd.set_defaults(func=compute_distances_and_clustering)

    cli_args = parser.parse_args()
    cli_args.func(cli_args)


if __name__ == "__main__":
    main()
