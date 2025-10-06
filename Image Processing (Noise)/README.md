# F28HS Coursework 1

HPDEC NOISE EDGE
Program takes a hpdec file, adds noise to the image of the file, prints the maximum and minimum differences between each adjacent pixel of the image and saves the image to a new hpdec file.
To run:
    If there is a make file enter "make" in the command line and return otherwise type:
        "gcc -o process process.c"
        
    Enter "./process" followed by two arguments per the number of hpdec files that needs to be processed i.e. both input and output file names (or path):
        "input1 output1 input2 output2 ... inputN outputN"
    finally followed by a positive decimal value that will determine the noise strength added to the image.

    Format:
        - "./process input1 output1 input2 output2 ... inputN outputN noise_strength" for multiple
        - "./process input output noise_intensity" for single
    
    Both input and output files entered should be HPDEC and noise intensity must be a positive decimal value.

    Examples of use (tested on image files "wildcat.hpdec" and "coffee.hpdec"):
        Single: "./process wildcat.hpdec wildcatout.hpdec 50"
        Multiple: "./process wildcat.hpdec wildcatout.hpdec coffee.hpdec coffeeout.hpdec 70"