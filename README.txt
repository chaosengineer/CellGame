Copy the Boost libraries in '.\boost' or make it a symbolic link to your Boost
location. Any version from Boost 1.38 and onward would be sufficient.
The '.\boost' directory should have the following structure:

boost
     |_boost
            |_accumulators
            |_algorithm
            |_...
            |_spirit
            |_...


This project requires LLVM 3.4.2. Place LLVM header files and libraries in the
'.\llvm' directory under the following structure:

llvm
    |_debug
    |      |_...all debug libraries go here
    |
    |_release
    |        |_...all release libraries go here
    |         
    |
    |_include
             |_llvm
                   |_ADT
                   |_Analysis
                   |_...
