#!/usr/bin/env python3

import argparse
import csv
import pathlib
import subprocess

from paraview import simple


def fetch_data_from_ftp(output_dir="input_data"):
    """Fetch data from DLR FTP with wget"""
    parser = argparse.ArgumentParser(description="Credentials to access FTP server")
    parser.add_argument("-u", "--username", type=str, help="Username", required=True)
    parser.add_argument("-p", "--password", type=str, help="Password", required=True)
    cli_args = parser.parse_args()

    dst_dir = pathlib.Path("data")
    if not dst_dir.is_dir():
        dst_dir.mkdir()

    # call wget with the relevant set of parameters
    cmd = [
        "wget",
        "-m",
        "-c",
        "-nd",
        "--directory-prefix=" + dst_dir.name,
        "--user=" + cli_args.username,
        "--password=" + cli_args.password,
        "ftp://ftp.dlr.de/datasets/diseases/Output",
    ]
    subprocess.check_call(cmd)


def generate_persistence_diagrams(data_dir="input_data", cdb="pdiags.cdb"):
    """Generate Persistence Diagrams from FBK data and store them inside a Cinema Database"""

    # create output Cinema Database
    cinema = pathlib.Path(cdb)
    if not cinema.is_dir():
        cinema.mkdir()

    # generate persistence diagrams from NetCDF files
    with open(cinema / "data.csv", "w", newline="") as dst:
        # Cinema Database CSV headers
        fieldnames = ["Type", "It", "Day", "FILE"]
        writer = csv.DictWriter(dst, fieldnames)
        writer.writeheader()

        inpdatadir = pathlib.Path(data_dir)
        outdatadir = cinema / "data"
        # create Cinema Database product folder
        if not outdatadir.is_dir():
            outdatadir.mkdir()

        for nc in inpdatadir.glob("*.nc"):
            # parse simulation parameters from datasets names
            nc_params = nc.name.split(".")[0].split("_")
            Type = nc_params[0]
            It = "It" + nc_params[-3]
            Day = nc_params[-1]

            # output persistence diagram file name
            filename = Type + "_" + It + "_" + Day + ".vtu"

            # write meta-data to Cinema Database index
            writer.writerow(
                {"FILE": "data/" + filename, "Type": Type, "It": It, "Day": Day}
            )

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
            if Type == "prob":
                Type = "probability"
            selection.QueryString = Type + " > 0"
            selection.FieldType = "POINT"
            extractSel = simple.ExtractSelection(Input=ttkId, Selection=selection)

            ttkHarm = simple.TTKHarmonicField(
                InputGeometry=ttkId, InputConstraints=extractSel
            )
            ttkHarm.ConstraintValues = Type
            ttkHarm.Solver = "Iterative"

            # more cleaning: use GaussianResampling to merge topological peaks
            gauss = simple.GaussianResampling(Input=ttkHarm)
            gauss.ResampleField = ["POINTS", "OutputHarmonicField"]
            gauss.ResamplingGrid = [244, 295, 3]
            gauss.GaussianSplatRadius = 0.02
            gauss.GaussianExponentFactor = -10.0
            gauss.SplatAccumulationMode = "Sum"

            slic = simple.Slice(Input=gauss)
            slic.SliceType = "Plane"
            slic.SliceType.Normal = [0.0, 0.0, 1.0]

            # compute persistence diagram
            persdiag = simple.TTKPersistenceDiagram(Input=slic)
            persdiag.DebugLevel = 3

            # save file in Cinema Database
            outfile = str(outdatadir / filename)
            simple.SaveData(outfile, Input=persdiag)
            print(">> Saved " + outfile)


def compute_distances(cdb="pdiags.cdb"):
    """Compute distance matrix between persistence diagrams"""

    # read the Cinema Database index
    cineRead = simple.TTKCinemaReader(DatabasePath=cdb)

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
    datadir = "input_data"
    cinema_db = "pdiags_prob.cdb"
    fetch_data_from_ftp(datadir)
    generate_persistence_diagrams(datadir, cinema_db)
    compute_distances(cinema_db)


if __name__ == "__main__":
    main()
