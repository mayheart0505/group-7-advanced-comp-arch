#include "colors.inc"

camera {  
  angle 20
  location <2, 10, -30>
  look_at <0, 1, 0>
}

light_source { <20, 20, -20> color White }

prism { 
  linear_sweep
  cubic_spline
  0,  // sweep the following shape from here ...
  1,  // ... up through here
  18, // the number of points making up the shape ...

  <3,-5>, <3,5>, <-5,0>, <3, -5>, <3,5>, <-5,0>, // sub-shape #1
  <2,-4>, <2,4>, <-4,0>, <2,-4>, <2,4>, <-4,0>,  // sub-shape #2
  <1,-3>, <1,3>, <-3,0>, <1, -3>, <1,3>, <-3,0>  // sub-shape #3
  
  pigment { Green }
}
