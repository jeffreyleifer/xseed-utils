---
layout: documentation
category: Data Validation Tools
order: 2
title: Validator
---
## Introduction

Validator checks a miniSEEDv3 file for: 

1. Valid fixed header.
2. Valid payload.
3. Valid extra header via user provided JSON schema (optional).

All information on the miniSEED file is printed to the terminal.



## Usage

```./xseedvalidator -f <miniSEEDv3 file to validate> -j <JSON schema to validate extra headers against (optional)>```

