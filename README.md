# XSEED-Utils
This repository contains utilities to convert and verify the xSEED standard for miniSEED files

## Install
run ```cmake .``` on the root directory

then ``make`` -- NOTE: Internet connection required
### Dependanceis
1.libmseed v3
2.WJElement

NOTE: These are automatically installed via cmake


## xSEED Validator
Checks a miniSEEDv3 file for: 
1. Valid fixed header
2. Valid payload
3. Valid extra header via user provided JSON schema (optional)

All information on the miniSEED file is printed to the terminal

### Usage:
```./xseedvalidator -f <miniSEEDv3 file to validate> -j <JSON schema to validate extra headers against (optional)>```

## xSEED Converter
Converts a miniSEED file <= v2 to v3 with optional extra header inject via user provided JSON file
### Usage:
```./mseedconvert -j <JSON file for extra header injection (optional) -o <Output miniSEEDv3 fileame> <input miniSEED filename>```

## xSEED2text
Prints the contents of a selected miniseed file in text format to the terminal
### Usage
```./xseed2text <miniSEED input file>```

## xSEED2JSON
Prints the contents of a selected miniseed file in JSON format to the terminal
### Usage
```./xseed2json <miniSEED input file>```

