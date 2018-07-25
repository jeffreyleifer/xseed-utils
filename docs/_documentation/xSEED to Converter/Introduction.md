---
layout: documentation
category: Data Validation Tools
order: 3
title: xSEED Converter
---

## Introduction

xSEED Converter converts a miniSEED file to xSEED: <= v2 to v3 with optional extra header inject via a user-provided JSON file.

## Usage Instructions

To run xSEED Converter, run the following script:

```./mseedconvert -j <JSON file for extra header injection (optional) -o <Output miniSEEDv3 fileame> <input miniSEED filename>```