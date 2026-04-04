#include "colors.inc"

camera {  
  orthographic
  up <0, 5, 0>
  right <5, 0, 0>
  location <2.5, 2.5, -100>
  look_at <2.5, 2.5, 0>
}

/* set the control points to be used */

#declare Red_Point    = <2.00, 0.00, 0>
#declare Orange_Point = <1.75, 1.00, 0>
#declare Yellow_Point = <2.50, 2.00, 0>
#declare Green_Point  = <2.00, 3.00, 0>
#declare Blue_Point   = <0.00, 4.00, 0>
                            
/* make the control points visible */

cylinder { Red_Point, Red_Point - 20*z, .1 
  pigment { Red }
  finish { ambient 1 }
}

cylinder { Orange_Point, Orange_Point - 20*z, .1
  pigment { Orange }
  finish { ambient 1 }
}

cylinder { Yellow_Point, Yellow_Point - 20*z, .1
  pigment { Yellow }
  finish { ambient 1 }
}

cylinder { Green_Point, Green_Point - 20*z, .1
  pigment { Green }
  finish { ambient 1 }
}
           
cylinder { Blue_Point, Blue_Point- 20*z, .1
  pigment { Blue }
  finish { ambient 1 }
}
           
/* something to make the curve show up */

lathe { 
  linear_spline
  5,
  Red_Point,
  Orange_Point,
  Yellow_Point,
  Green_Point,
  Blue_Point
  
  pigment { White }
  finish { ambient 1 }
}
