# rts
Random triangular matrix sampler

This program samples an input matrix of binary values, building a square matrix from a random sample of row and column indices. 

The resulting square matrix is tested to determine if it is an upper- or lower-triangular matrix. 

A square matrix is called lower-triangular if all the entries above the diagonal are zero, or called upper-triangular if all the entries below the diagonal are zero.

If it an upper- or lower-triangular matrix, it is printed to standard output. 

If the `--preserve-metadata` option is used, the row and column names from the original input matrix are included in the output.

## Example

We start with the following example matrix `test.mtx`:

```
$ less test.mtx
        feature01       feature02       feature03       feature04       feature05       feature06       feature07       feature08       feature09       feature10       feature11
elementA        1       0       0       1       1       1       0       0       0       0       0
elementB        0       0       1       1       1       0       1       0       0       0       1
elementC        0       0       1       1       0       0       1       0       1       1       1
elementD        0       0       0       0       1       1       1       1       0       0       1
elementE        1       0       1       1       0       0       1       1       0       0       0
elementF        0       0       0       0       0       0       0       0       0       0       0
```

We can sample this test matrix for any 3x3 lower-triangular matrices we can find within, from a random selection of rows and columns:

```
$ make clean && make && make test-lower
rm -rf *~
rm -rf rts
rm -rf rts.o
clang++ -g -Wall -Wextra -std=c++14 -D__STDC_CONSTANT_MACROS -D__STDINT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -O3 -c rts.cpp -o rts.o
clang++ -g -Wall -Wextra -std=c++14 -D__STDC_CONSTANT_MACROS -D__STDINT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -O3 -I/usr/include rts.o -o rts
set -e; \
        ROWS=$(wc -l ./test.mtx | awk '{print ($1-1)}'); \
        COLS=$(tail -1 ./test.mtx | awk '{print NF-1}'); \
        ./rts --rows ${ROWS} --cols ${COLS} --samples 20 --order 3 --rng-seed 123 --lower --preserve-metadata < ./test.mtx
        feature03       feature08       feature05
elementC        1       0       0
elementE        1       1       0
elementD        0       1       1
        feature02       feature07       feature05
elementF        0       0       0
elementE        0       1       0
elementD        0       1       1
        feature11       feature03       feature02
elementA        0       0       0
elementE        0       1       0
elementC        1       1       0
        feature06       feature01       feature02
elementF        0       0       0
elementE        0       1       0
elementD        1       0       0
        feature04       feature01       feature02
elementD        0       0       0
elementF        0       0       0
elementA        1       1       0
```

Likewise, we can sample the input matrix for 4x4 upper-triangular matrices:

```
$ make clean && make && make test-upper
rm -rf *~
rm -rf rts
rm -rf rts.o
clang++ -g -Wall -Wextra -std=c++14 -D__STDC_CONSTANT_MACROS -D__STDINT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -O3 -c rts.cpp -o rts.o
clang++ -g -Wall -Wextra -std=c++14 -D__STDC_CONSTANT_MACROS -D__STDINT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -O3 -I/usr/include rts.o -o rts
set -e; \
        ROWS=$(wc -l ./test.mtx | awk '{print ($1-1)}'); \
        COLS=$(tail -1 ./test.mtx | awk '{print NF-1}'); \
        ./rts --rows ${ROWS} --cols ${COLS} --samples 50 --order 4 --rng-seed 123 --upper --preserve-metadata < ./test.mtx
        feature08       feature02       feature09       feature04
elementF        0       0       0       0
elementA        0       0       0       1
elementC        0       0       1       1
elementB        0       0       0       1
        feature08       feature07       feature02       feature04
elementD        1       1       0       0
elementB        0       1       0       1
elementF        0       0       0       0
elementA        0       0       0       1
```

## Performance characteristics

### Memory usage

The input matrix of binary values is read into a bit array in single-byte increments. Using a bit array reduces storage overhead considerably, which is an issue for very large input matrices. The memory usage of the bit array is `ceil((rows * cols)/8)` bytes.

### Sampling

Depending on the specified type of matrix we are interested in, we only search the upper or lower triangle for disqualifying bits. If one is found, we immediately drop the sample and try the next, instead of searching through the rest of the matrix. This reduces overall lookup time.