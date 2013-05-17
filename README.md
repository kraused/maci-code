maci-code
=========

Maci is a C++ code for coupling atomistic and continuum simulation codes. 

See `doc/main.pdf` for more information.

The code is provided "as is" in the hope that it will be useful for other 
researchers.
Since we cannot open-source certain required third-party libraries (in
particular the finite element package UG), the code will not run out of the box. 
However, interfacing Maci with a different finite element package should be
possible with decent effort.

Please note that active developed of Maci was ceased in 2010.

References
==========

Please cite the following references when using MACI:

* _Coupling Molecular Dynamics and Continua with Weak Constraints._
  Konstantin Fackeldey, Dorian Krause, Rolf Krause and Christoph Lenzen.
  SIAM Multiscale Modeling & Simulation 2011; 9(4):1459 - 1494.
* _Numerical validation of a constraints-based multiscale simulation method for solids._
  Konstantin Fackeldey, Dorian Krause and Rolf Krause.
  Meshfree Methods for Partial Differential Equations V, Lecture Notes in Computational Science and Engineering. Springer (2011).
* _Parallel Scale-Transfer in Multiscale MD-FE Coupling Using Remote Memory Access._
  Dorian Krause and Rolf Krause.
  IEEE 7th International Conference on E-Science, e-Science 2011, Workshop Proceedings, IEEE Computer Society (2011).
