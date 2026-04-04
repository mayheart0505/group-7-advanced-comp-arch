//
// POV-Ray(tm) 3.0 tutorial example scene.
// Copyright 1996 by the POV-Ray Team
//

#include "colors.inc"

camera { 
  angle 15
  location <0,2,-10>
  look_at <0,0,0>
}

light_source { <10, 20, -10> color White }

blob {
  threshold .65
  sphere { <.5,0,0>, .8, 1 pigment {Blue} }
  sphere { <-.5,0,0>,.8, 1 pigment {Pink} }
  
  finish { phong 1 }
}

