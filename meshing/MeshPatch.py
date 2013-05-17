
from Cuboid import *

# A mesh patch is a subset of the finite element mesh region which is covered
# by a structured mesh. The whole finite element is the constructed by a 
# consistent meshing without hanging nodes
#
# @warning Currently only axis parallel cuboids are supported
# 
class MeshPatch(Cuboid):
	
