#include "colors.inc"
#include "textures.inc"

camera { 
  location <0, 15, -40> 
  look_at <-2, 0, 1> 
  angle 10 
}

light_source { <10, 20, 10> color White }

// lay down a boring floor to view the shadow against

plane { y, 0                                           
  pigment { Grey }
}

// here's something to have caustics property applied

sphere { <0, 3, 0>, 2
  texture {
    Glass3 
    finish { caustics .6 }
  }
}
