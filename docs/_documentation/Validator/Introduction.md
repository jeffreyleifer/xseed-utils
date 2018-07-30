---
layout: documentation
category: Data Validation Tools
order: 2
title: Validator
---
## Introduction

Validator checks a xSEED file for: 

1. Valid fixed header.
2. Valid payload.
3. Valid extra header via user provided JSON schema (optional).

All information on the xSEED file is printed to the terminal.



## Usage

```./xseedvalidator -f <xSEED file to validate> -j <JSON schema to validate extra headers against (optional)>```

