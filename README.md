Signature Analysis Command Line Tool
====================================

This command line application, written in C++, analyzes files within a specified directory and its subdirectories to determine if they are PDF or JPG files based on their file signatures. 
It also calculates the MD5 hash of the file contents for PDF and JPG files and writes them to a specified output file.


Features
--------

-   Analyzes files in a directory and its subdirectories (optional).
-   Detects PDF and JPG files based on file signatures.
-   Computes MD5 hash of file contents for PDF and JPG files.
-   Outputs results to a CSV file containing the following information:
    -   Full path to the file
    -   Actual file type (PDF or JPG)
    -   MD5 hash of the file contents

Prerequisites
-------------

-   C++ compiler
-   Botan library (for MD5 hash calculation)


Usage
-----

Takes the following command line arguments


-   `<analyzedDir>`: The full path to the directory containing the files to be analyzed.
-   `<outputPath>`: The path to the output CSV file.
-   `-s=<value>` (optional): Specify 't' or 'true' to include subdirectories in the analysis. Omit it to analyze only the top-level directory.

Example
-------

To analyze files in the directory `/path/to/files` (including subdirectories) and save the results to `output.csv`, use the following command:


`./sample_app /path/to/files output.csv -s=true`

File Signatures
---------------
Inspects the files for the following file signatures.

-   PDF Signature: `25504446`
-   JPG Signature: `ffd8`
