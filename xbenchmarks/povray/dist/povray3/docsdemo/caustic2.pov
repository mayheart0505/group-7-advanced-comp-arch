#include "colors.inc"

// Our camera is underwater, looking at the bottom of 
// the pool for the best view of the caustics produced 

camera { 
  location <0, -5, 0>
  look_at  <0, -10, -5>
}

light_source { <0, 100, 49.5> color White }

// the bottom of the pool...

plane { y, -10
  texture {
    pigment { color rgb <0.6, 0.7, 0.7> }
    finish { ambient 0.1 diffuse 0.7 }
    scale 0.01
  }
}

// and the surface of the water

plane { y, 0
  texture { 
    pigment { rgbf <0.6, 0.67, 0.72, 0.9> }
    normal {
      bumps .6
      scale <.75, .25, .25>
      rotate <0, 45, 0>
    }
    finish { caustics .9 }
  }
}
