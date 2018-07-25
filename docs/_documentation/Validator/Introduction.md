---
layout: documentation
category: Data Validation Tools
order: 2
title: Validator
---
## Description

Validator is a comprehensive validator for xSEED, the next generation time series data format, that can verify all aspects of the data format, with the exception of non‐reserved (user‐defined) extra headers for which no schema is provided. Verification of all aspects of the data format includes proper binary layout and valid contents within those fields.

Validator checks a miniSEEDv3 file for: 

1. Valid fixed header.
2. Valid payload.
3. Valid extra header via user provided JSON schema (optional).

**_Note:_** *All information on the miniSEED file is printed to the terminal.*

## Help





## Typical Usage

To use Validator, execute the following command:

```./xseedvalidator -f <miniSEEDv3 file to validate> -j <JSON schema to validate extra headers against (optional)>```

## Example Usage

