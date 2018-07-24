---
layout: documentation
category: xSEED Reference Sets
order: 1
title: Introduction
---
Three data reference sets exist for xSEED, the next generation time series data format. Intended for use by implementers, data centers, and users of the data format, the reference sets illustrate the usage of all key aspects of the data, including, but not limited to: 

- An example of data encoded in each non‐deprecated encoding that exercises as many “states” of the encoding as possible (e.g., all encoding  states in Steim compression schemes):

   - Examples covering each variation of each field as allowed by  the  specification (e.g. sample rate and sampling interval)
   - Examples including every defined (reserved) extra header
   - Examples including some extra headers not defined
   - Examples of various record lengths 128 – 4096 bytes and the maximum length
   - Examples both with and without data samples.

- Each data example must be represented in 3 forms:

   - The next generation time series format
   - A human‐readable text format, à la libmseed’s msr_print() output
   - Machine‐readable JSON format, devised by contractor, approved by DMC Data examples:
     - Should not be large in volume
     - Should include a human recognizable signal, such as an earthquake for real data or a sinusoid for generated data, when data samples are included
     - Should use ‘XX’ network and ‘REF’ station codes to clearly identify them as non‐real, reference data.

The three reference data sets are:


- [Reference Data Set 1](https://xseed.pages.isti.com/xseed-utils/documentation/xSEED%20Reference%20Set/Reference-Set-1/): Contains Steim2 encoding, 500 samples, no extra headers
- [Reference Data Set 2](https://xseed.pages.isti.com/xseed-utils/documentation/xSEED%20Reference%20Set/Reference-Set-2/): Includes Data Set #1, plus Time Quality, Event Detection and Time Correction extra headers
- [Reference Data Set 3](https://xseed.pages.isti.com/xseed-utils/documentation/xSEED%20Reference%20Set/Reference-Set-3/): Includes Data Set #2, plus non-FDSN headers, with root objects for Manufacturer123 and OperatorXYZ