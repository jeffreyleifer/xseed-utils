---
layout: documentation
category: Data Validation Tools
order: 2
title: Validator
---
## Description

Validator checks an xSEED file for: 

1. Valid fixed header.
2. Valid payload.
3. Valid extra header via user provided JSON schema (optional).

***Note:***  *All information on the xSEED file is printed to the terminal.*

## Help

Validator usage information:

```
Usage: ./xseedValidator

Program to Validate xSEED format files
          --help -h Display usage information
          --file -f File to validate
          --json -j Json schema
          --verbose -v Verbosity level
```

## Typical Usage

```
./xseedValidator -f reference-baseline-record-steim2.mseed3 -j ExtraHeaders-FDSN.schema.json -vvv
```

## Example Usage