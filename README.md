# IRM
Reference implementation of the IRM (Interval Rotation Map) data structure in C++11.

# Dependencies
Requires CMake to build, but only uses the C++ standard libraries.

# Building
To build, enter the project directory in the terminal/command prompt and enter:
```
mkdir build
cd build
cmake .
```
This will generate the benchmark application. To run, enter:
```
./IRM
```

# Credit
IRM data structure, implementation, and benchmark code by [0x22fe](https://github.com/0x22fe).
[Interval Tree](https://github.com/ekg/intervaltree) implementation by [Erik Garrison](https://github.com/ekg).

# Citation
For this repository (BibTeX):
```
@misc{ashok2019,
  author = {Rohan Ashok},
  title = {IRM},
  year = {2020},
  publisher = {GitHub},
  journal = {GitHub repository},
  howpublished = {\url{https://github.com/0x22fe/IRM}},
  commit = {6c55728f9d992e1ea26a37c0123127f9db34dc3b}
}
```

For the paper:
```
@misc{ashok2019,
    title={Accelerating 2D Line-Segment Queries in Sparse Scenes},
    author={Rohan Ashok},
    year={2019},
    note = {\url{https://0x22fe.com}}
}
```

# License
This implementation is under the MIT License.
