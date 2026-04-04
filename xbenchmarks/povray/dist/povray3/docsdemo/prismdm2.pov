#include "colors.inc"

camera {  
  angle 20
  location <2, 10, -30>
  look_at <0, 1, 0>
}

light_source { <20, 20, -20> color White }

prism { 
  cubic_spline 
  0, // sweep the following shape from here ...
  1, // ... up through here
  6, // the number of points making up the shape ...

  < 3, -5>, // point#1 (control point... not on curve)
  < 3,  5>, // point#2  ... THIS POINT ...
  <-5,  0>, // point#3
  < 3, -5>, // point#4
  < 3,  5>, // point#5 ... MUST MATCH THIS POINT
  <-5,  0>  // point#6 (control point... not on curve)

  pigment { Green }
}
