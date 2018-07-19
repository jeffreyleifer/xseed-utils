---
layout: documentation
title: Home
permalink: /documentation/
---

## Welcome to xSEED!

## xSeed Preamble

### Background and Context

Adopted by the International Federation of Digital Seismograph Networks (FDSN) in 1987, the SEED format has become the canonical format for passive source seismic (and other) data. For continuous data collection, archiving, and delivery, it has become common to handle the time series and metadata separately. [Read the canonical specification for a next generation version format for the time series portion, known as miniSEED](https://iris-edu.github.io/xseed-specification/xSEED%20-%20DRAFT%2020180511.pdf).

 The key issues motivating a change to miniSEED 2.4 are limitations with identifiers for a) deployments with a very large number of nodes and b) new instruments and other data source types, e.g. synthetics. Changing the fundamental identifiers requires changes to key fields in miniSEED that render it incompatible with the current release. Such a small, but disruptive change affords the opportunity to address a number of historical issues and create new capability to address future needs.

This repository contains utilities to convert and verify the xSEED standard for miniSEED files.