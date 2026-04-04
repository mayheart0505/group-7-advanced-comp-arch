#include "colors.inc"

camera {
  location <0, 0, -6>
  look_at <0, 0, 0>
}

light_source { <-20, 30, -100> color White }
light_source { <10, 30, -10> color White }
light_source { <0, 30, 10> color White }

#declare PLAIN_TEXTURE =  
  // red/white check
  texture {
    pigment {
      checker
      color rgb<1.000, 0.000, 0.000> 
      color rgb<1.000, 1.000, 1.000>  
      scale <0.2500, 0.2500, 0.2500>
    }
  }

// plain red/white check box

box { <-1, -1, -1>, <1, 1, 1>
  texture {
    PLAIN_TEXTURE
  }
  translate  <-1.5, 1.2, 0>
}

#declare FADED_TEXTURE = 
  // red/white check texture
  texture {
    pigment {
      checker
      color rgb<0.920, 0.000, 0.000>
      color rgb<1.000, 1.000, 1.000>
      scale <0.2500, 0.2500, 0.2500>
    }
  }
  // greys to fade red/white
  texture {
    pigment {
      checker
      color rgbf<0.632, 0.612, 0.688, 0.698>
      color rgbf<0.420, 0.459, 0.520, 0.953>
      turbulence 0.500
      scale <0.2500, 0.2500, 0.2500>
    }
  }

// faded red/white check box

box { <-1, -1, -1>, <1, 1, 1>
  texture {
    FADED_TEXTURE
  }
  translate  <1.5, 1.2, 0>
}

#declare STAINED_TEXTURE = 
  // red/white check
  texture {
    pigment {
      checker
      color rgb<0.920, 0.000, 0.000>
      color rgb<1.000, 1.000, 1.000>
      scale <0.2500, 0.2500, 0.2500>
    }
  }
  // greys to fade check
  texture {
    pigment {
      checker
      color rgbf<0.634, 0.612, 0.688, 0.698>
      color rgbf<0.421, 0.463, 0.518, 0.953>
      turbulence 0.500
      scale <0.2500, 0.2500, 0.2500>
    }
  }
  // wine stain
  texture {
    pigment {
      spotted
      color_map {
        [ 0.000  color rgb<0.483, 0.165, 0.165> ]
        [ 0.329  color rgbf<1.000, 1.000, 1.000, 1.000> ]
        [ 0.734  color rgbf<1.000, 1.000, 1.000, 1.000> ]
        [ 1.000  color rgb<0.483, 0.165, 0.165> ]
      }
      turbulence 0.500
      frequency 1.500
    }
  }

// stained box

box { <-1, -1, -1>, <1, 1, 1>
  texture {
    STAINED_TEXTURE
  }
  translate  <-1.5, -1.2, 0>
}
