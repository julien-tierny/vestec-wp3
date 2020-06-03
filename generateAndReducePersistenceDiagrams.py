#!/usr/bin/env python3

# pylint: disable=invalid-name, missing-docstring, import-error, too-many-locals

import argparse
import pathlib
import subprocess

import cv2
import numpy as np

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
        "ftp://ftp.dlr.de/datasets/persistence_diagrams/D3.2/fire_original_data.cdb",
    ]
    subprocess.check_call(cmd)


def generate_persistence_diagrams(args):
    """Generate Persistence Diagrams from FBK data and store them inside a Cinema Database"""

    inpdatadir = pathlib.Path(args.input_dir)

    # group images by the form of the fire propagation
    case = {
        "t1": 0,
        "t2": 1,
        "t3": 2,
        "t4": 3,
        "t5": 3,
        "t6": 1,
        "t7": 1,
        "t8": 1,
        "t9": 1,
        "t10": 2,
    }

    for tif in sorted(inpdatadir.glob("*.tif")):
        bak = pathlib.Path(str(tif) + ".bak")
        if not bak.exists():
            print("saved", tif)
            tif.rename(bak)
        img = cv2.imread(str(bak), cv2.IMREAD_ANYDEPTH)
        k = np.ones((5, 5), np.uint8)
        close = cv2.morphologyEx(img, cv2.MORPH_CLOSE, k)
        cv2.imwrite(str(tif), close)

        # load TIFF image
        fire = simple.TIFFReader(FileName=str(tif))

        # "simulation parameters"
        tifname = tif.name.split(".")[0]
        params = {"Name": tifname, "CaseId": str(case.get(tifname, -1))}

        # threshold to keep only the propagation map
        thr = simple.Threshold(Input=fire)
        thr.Scalars = ["POINTS", "Tiff Scalars"]
        thr.ThresholdRange = [0.0, 8.0]

        # compute eigen functions of the mesh laplacian graph
        eigen = simple.TTKEigenField(InputGeometry=simple.Tetrahedralize(thr))
        eigen.Numberofeigenfunctions = 200
        eigen.Computestatistics = 1

        # extract the sum of all eigen functions
        extrComp = simple.ExtractComponent(Input=eigen)
        extrComp.InputArray = ["POINTS", "Statistics"]
        extrComp.Component = 3  # Sum

        # compute persistence diagram
        persdiag = simple.TTKPersistenceDiagram(Input=extrComp)
        persdiag.ScalarField = "Result"
        persdiag.DebugLevel = 3

        # add file name as Field Data
        arred = simple.TTKArrayEditor(Target=persdiag)
        arred.TargetAttribute = "Field Data"
        arred.DataString = "\n".join([",".join(tup) for tup in params.items()])

        # save file in Cinema Database
        cinewriter = simple.TTKCinemaWriter(Input=arred)
        cinewriter.DatabasePath = args.pdiags_cdb_dir
        cinewriter.ForwardInput = False

        # trigger the pipeline by saving the empty output of TTKCinemaWriter
        simple.SaveData("empty.vtu", Input=cinewriter)
        print(">> Processed " + str(tif))


def compute_distances_and_clustering(args):
    """Compute distance matrix between persistence diagrams"""

    # read the Cinema Database index
    cineRead = simple.TTKCinemaReader(DatabasePath=args.pdiags_cdb_dir)

    # perform SQL query on the Cinema Database index
    query = simple.TTKCinemaQuery(InputTable=cineRead)
    query.SQLStatement = "SELECT * FROM InputTable0"

    # load the filtered products (persistence diagrams) from the database
    prodRead0 = simple.TTKCinemaProductReader(Input=query)

    ###########################################
    # DISTANCE MATRIX AND DIMENSION REDUCTION #
    ###########################################

    # compute the distance matrix between the persistence diagrams
    distMat = simple.TTKPersistenceDiagramDistanceMatrix(Input=prodRead0)
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

    cluster = simple.TTKPersistenceDiagramClustering(Input=prodRead0)
    cluster.Numberofclusters = 5  # one for each "iteration"
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
-- clustering tuples
-- with distance matrix data
SELECT dm.*, cl.ClusterId
FROM InputTable1 AS dm
JOIN InputTable0 AS cl
USING (Name)"""

    # generate points from 3-dimensional coordinates
    t2p = simple.TableToPoints(Input=mergeQuery)
    t2p.XColumn = "Component_0"
    t2p.YColumn = "Component_1"
    t2p.ZColumn = "Component_2"
    t2p.KeepAllDataArrays = 1
    t2p.a2DPoints = True

    # converts from vtkPolyData to vtkUnstructuredGrid
    tetra = simple.Tetrahedralize(Input=t2p)

    # generate a heat map from the distance matrix
    hm = simple.TTKMatrixToHeatMap(Input=distMat)
    hm.SelectFieldswithaRegexp = 1
    hm.Regexp = "Diagram.*"

    # save output
    simple.SaveData("fire_trajectories.vtu", Input=tetra)
    simple.SaveData("fire_heatmap.vtu", Input=hm)
    simple.SaveData("fire_distmat.csv", Input=dimRed)


def main():
    parser = argparse.ArgumentParser(
        description=(
            "Simulate the topological analysis performed "
            "for the Forest Fire Use Case"
        )
    )
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
    # force use of subcommand, display help without one
    if "func" in cli_args.__dict__:
        cli_args.func(cli_args)
    else:
        parser.parse_args(["--help"])


if __name__ == "__main__":
    main()
