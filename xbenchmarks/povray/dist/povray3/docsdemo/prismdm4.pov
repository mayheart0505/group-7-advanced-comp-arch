#include "colors.inc"

camera {  
  angle 20
  location <2, 10, -30>
  look_at <0, 1, 0>
}

light_source { <20, 20, -20> color White }

prism { 
  conic_sweep
  linear_spline
  0, // height 1
  1, // height 2
  5, // the number of points making up the shape...

  <4,4>,<-4,4>,<-4,-4>,<4,-4>,<4,4>

  rotate <180, 0, 0>
  translate <0, 1, 0>
  scale <1, 4, 1>
  pigment { gradient y scale .2 }
}
