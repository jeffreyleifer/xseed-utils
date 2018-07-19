---
layout: documentation
category: Data Validation Tools
order: 3
title: xSEED Converter
---

## Introduction

xSEED Converter converts a miniSEED file <= v2 to v3 with optional extra header inject via a user-provided JSON file.

## Usage

```./mseedconvert -j <JSON file for extra header injection (optional) -o <Output miniSEEDv3 fileame> <input miniSEED filename>```