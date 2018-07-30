---
layout: documentation
category: xSEED Reference Sets
order: 1
title: Overview
---

## Reference Data Set Overview

| File Identifier                                            | Description of File Contents                                 |
| ---------------------------------------------------------- | ------------------------------------------------------------ |
| 1-reference-record-steim2.xseed                            |Includes: <br />- Steim2 encoding<br />- 500 samples<br />- No extra headers |
| 2-reference-record-steim2_EH-FDSN-TQ-ED-TC.xseed           | Includes: <br />- Data Set #1<br />- Time Quality<br />- Event Detection and Time Correction extra headers |
| 3-reference-record-steim2_EH-FDSN-TQ-ED-TC-Oper-Manu.xseed | Includes:<br />- Data Set #2<br />- non-FDSN headers, with root objects for Manufacturer123 and OperatorXYZ |

View examples of the xSEED Reference Data Set:
- [Reference Data Set Example 1](https://xseed.pages.isti.com/xseed-utils/documentation/xSEED%20Reference%20Set/Reference-Set-1/)
- [Reference Data Set Example 2](https://xseed.pages.isti.com/xseed-utils/documentation/xSEED%20Reference%20Set/Reference-Set-2/)
- [Reference Data Set Example 3](https://xseed.pages.isti.com/xseed-utils/documentation/xSEED%20Reference%20Set/Reference-Set-3/)

The xSEED Reference Data Set illustrates the usage of all key aspects of the data, including, but not limited to: 

- An example of data encoded in each non‐deprecated encoding that exercises as many “states” of the encoding as possible (e.g., all encoding  states in Steim compression schemes):

   - Examples covering each variation of each field as allowed by  the  specification (e.g. sample rate and sampling interval)
   - Examples including every defined (reserved) extra header
   - Examples including some extra headers not defined
   - Examples of various record lengths 128 – 4096 bytes and the maximum length
   - Examples both with and without data samples.

- Each data example is represented in three forms:

   - The next generation time series format
   - A human‐readable text format, à la libmseed’s msr_print() output
   - Machine‐readable JSON format, devised by contractor, approved by DMC Data examples:
     - Should not be large in volume
     - Should include a human recognizable signal, such as an earthquake for real data or a sinusoid for generated data, when data samples are included
     - Should use ‘XX’ network and ‘REF’ station codes to clearly identify them as non‐real, reference data.