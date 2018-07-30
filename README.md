# XSEED-Utils
This repository contains utilities to convert and verify files in the xSEED standard

## Install
run ```cmake .``` on the root directory

then ``make`` -- NOTE: Internet connection required
### Dependanceis
1. libmseed >= v3
2. WJElement

NOTE: These are automatically installed via cmake


## xSEED Validator
Checks xSEED file for: 
1. Valid fixed header
2. Valid payload
3. Valid extra header via user provided JSON schema (optional)

All information on the xSEED file is printed to the terminal

### Usage:
```./xseedvalidator -f <xSEED file to validate> -j <JSON schema to validate extra headers against (optional)>```
add ```-v``` for verbosity level, add ```v```s for more verbose

## xSEED2text
Prints the contents of a selected xSEED file in text format to the terminal
### Usage
```./xseed2text <xSEED input file>```
add ```-v``` for verbosity level, add ```v```s for more verbose

## xSEED2JSON
Prints the contents of a selected xSEED file in JSON format to the terminal
### Usage
```./xseed2json <xSEED input file>```
add ```-v``` for verbosity, add ```v```s for more verbose

