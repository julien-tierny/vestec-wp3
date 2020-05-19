#!/usr/bin/env python3

import argparse
import csv
import pathlib
import subprocess

from paraview import simple


def fetch_data_from_ftp(args):
    """Fetch data from DLR FTP with wget"""
    dst_dir = pathlib.Path(args.datadir)
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
        "ftp://ftp.dlr.de/datasets/diseases/Output",
    ]
    subprocess.check_call(cmd)


def generate_persistence_diagrams(args):
    """Generate Persistence Diagrams from FBK data and store them inside a Cinema Database"""

    inpdatadir = pathlib.Path(args.datadir)

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

        # compute persistence diagram
        persdiag = simple.TTKPersistenceDiagram(Input=slic)
        persdiag.DebugLevel = 3

        # add parameters as Field Data
        arred = simple.TTKArrayEditor(Target=persdiag)
        arred.TargetAttribute = "Field Data"
        arred.DataString = "\n".join([",".join(tup) for tup in params.items()])

        # save file in Cinema Database
        cinewriter = simple.TTKCinemaWriter(Input=arred)
        cinewriter.DatabasePath = args.cdbdir
        cinewriter.ForwardInput = False

        # trigger the pipeline by saving the empty output of TTKCinemaWriter
        simple.SaveData("empty.vtu", Input=cinewriter)
        print(">> Processed " + str(nc))


def compute_distances(args):
    """Compute distance matrix between persistence diagrams"""

    # read the Cinema Database index
    cineRead = simple.TTKCinemaReader(DatabasePath=args.cdbdir)

    # perform SQL query on the Cinema Database index
    query = simple.TTKCinemaQuery(InputTable=cineRead)
    query.SQLStatement = (
        "SELECT * FROM InputTable0"  # WHERE It='it2' OR It='it3' OR It='it4'"
    )

    # load the filtered products (persistence diagrams) from the database
    prodRead = simple.TTKCinemaProductReader(Input=query)

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

    # generate points from 3-dimensional coordinates
    t2p = simple.TableToPoints(Input=dimRed)
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
    simple.SaveData("mosq_distmat.vtu", Input=tetra)
    simple.SaveData("mosq_heatmap.vtu", Input=hm)


def main():
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers()

    datadir = "input_dir"
    cdbdir = "pdiags.cdb"

    fetchpars = subparsers.add_parser("fetch")
    fetchpars.add_argument("-u", "--username", type=str, required=True)
    fetchpars.add_argument("-p", "--password", type=str, required=True)
    fetchpars.add_argument("-d", "--destdir", type=str, default=datadir)
    fetchpars.set_defaults(func=fetch_data_from_ftp)

    generate_pd = subparsers.add_parser("gen")
    generate_pd.add_argument("-i", "--input_dir", type=str, default=datadir)
    generate_pd.add_argument("-c", "--cdb_dir", type=str, default=cdbdir)
    generate_pd.set_defaults(func=generate_persistence_diagrams)

    cluster_pd = subparsers.add_parser("cluster")
    cluster_pd.add_argument("-c", "--cdb_dir", type=str, default=cdbdir)
    cluster_pd.set_defaults(func=compute_distances)

    parser.parse_args()


if __name__ == "__main__":
    main()
