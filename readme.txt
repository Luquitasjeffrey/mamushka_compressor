Mamushka is a compressor inspired by lz77, and huffman encoding.
I put the name mamushka because each step performed by the algorithm is like
substracting a layer of redundancy.

If you compress at mode 0, the compressor will pass the data 4 times in lz77
compressor, each one with a slightly different algorithm that makes data entirely
recoverable without lossing any information.

It has some scalability problems (currently is not able to process more than 64 mb
of data, (i am less than 1 year coding), so don´t use this code in a serious project.
In wich case, try to modify the code for your purposes.

************************************************************************************

USSAGE:

put this in the command prompt:

start mamushka.exe mode inputdir outputfilename flags

mode can take value eiher 'c' or 'd'.
'c' stands for compress, and 'd' stands for decompress.

in inputdir you must put the access route to the file you want to compress.
in outputfilename, put the access route to the output.

flags:

4: fast compression (slow ratio)
0: maximal compression possible configuration (slow execution time)
1: must like standard deflate algorithm (good compression ratio and runtime)
2: a little slower than 1 but archeives more compression ratio
3: a little slower than 2 but archeives more compression ratio

example:

for compression{
start mamushka.exe c example dir/example.example 0
}

for decompression{
start mamushka.exe d dir/example.example example 
}
note that you dont need to put flags in decompression side.

Enjoy ;)

