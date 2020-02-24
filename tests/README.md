# Regression Tests
I've opted by implementing a regression test approach over a unit testing one.

This is a rather dynamic implementation that works as follows:
 1. assuming the current implementation is working as expected (as is the requirement when a regression-tests approach is chosen)
 2. generating regression tests dynamically using `python generate.py`, which works as follows:
      * create a folder to hold the _JSON_ files that describe the regression tests
      * set a specific set of options for each parameter the program receives: `grid`, `seed`, `rho`, `max_clusters`. These include empty parameters (`None` in python), corner cases like 0s and large values, and regular values for that parameter
      * create a _JSON_ file for each combination of the above parameters (currently a total of **2268** tests)
      * run the code with those parameters and save the real output in the same _JSON_ file
 3. running the tests with `python test.py` which will:
      * compile the code using the previously written `Makefile` through python
      * look for _JSON_ files inside the directory specified in `TESTS_PATTERN`
      * for each test pass the params that are not `None` (or `null` in _JSON_ representation) and run the code through python
      * use the XUnit framework `unittest` to assert that the `.dat` and `.pgm` output files content matches the expected values

Naturally, it is expected that between steps **2.** and **3.** there might be code changes that will then be tested by the regression tests. 
