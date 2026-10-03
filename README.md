# HPDEC Noise Edge

A C utility that reads HPDEC image files, applies configurable noise to the image, and analyses the result, printing the maximum and minimum differences between adjacent pixels before saving the noisy image to a new HPDEC file.

## Overview

The program processes images stored in the HPDEC format at the binary level. For each image, it:

1. Reads the input HPDEC file.
2. Applies noise to the image at a user-specified strength.
3. Calculates and prints the maximum and minimum differences between adjacent pixels, a simple measure of how much the noise has disrupted local pixel continuity.
4. Writes the noisy image out to a new HPDEC file.

It supports processing a single image or multiple images in one run.

## Tech Stack

- **Language:** C

## Getting Started

### Prerequisites
- GCC (or any standard C compiler)

### Building

If a Makefile is present:
```bash
make
```

Otherwise, compile directly:
```bash
gcc -o process process.c
```

### Usage

```bash
./process input1 output1 input2 output2 ... inputN outputN noise_strength
```

- Provide one or more **input/output HPDEC file pairs**.
- Follow them with a single **noise strength** value (a positive decimal), applied to every image in that run.
- Both input and output files must be in HPDEC format.

**Single file:**
```bash
./process input output noise_strength
```

**Multiple files:**
```bash
./process input1 output1 input2 output2 ... inputN outputN noise_strength
```

### Examples

Tested using `wildcat.hpdec` and `coffee.hpdec`:

**Single image:**
```bash
./process wildcat.hpdec wildcatout.hpdec 50
```

**Multiple images:**
```bash
./process wildcat.hpdec wildcatout.hpdec coffee.hpdec coffeeout.hpdec 70
```

## Output

For each image processed, the program prints the maximum and minimum pixel-to-pixel differences found in the resulting noisy image, then writes the modified image to the specified output HPDEC file.

## What I Learned

Building this project involved working directly with binary image data, reading and writing a custom file format at the byte level, and handling variable-length argument parsing in C to support processing multiple images in a single run.
