#include "colors.inc"
#include "textures.inc"

camera { 
  location <0, 5, -30> 
  look_at <0, 0, 0> 
}

light_source { <-20, 30, -50> color White }

plane { y, 0 pigment { checker color Green color Yellow  } }

background { rgb <.7, .7, 1> }

box { <-10, 0, -10>, <10, 10, 10>
  texture { 
    Silver_Metal // a metal object ...
    normal {     // ... which has suffered a beating
      dents 2 
      scale 1.5 
    }
  } // (end of base texture)

  texture { // ... has some flecks of rust ...
    pigment {  
      granite
      color_map {
        [0.0 rgb <.2, 0, 0> ]
        [0.2 color Brown ]
        [0.2 rgbt <1, 1, 1, 1> ]
        [1.0 rgbt <1, 1, 1, 1> ]
      } 
      frequency 16
    }
  } // (end rust fleck texture)

  texture { // ... and some sooty black marks
    pigment {
      bozo                
      color_map {
        [0.0 color Black ]
        [0.2 color rgbt <0, 0, 0, .5> ]
        [0.4 color rgbt <.5, .5, .5, .5> ]
        [0.5 color rgbt <1, 1, 1, 1> ]
        [1.0 color rgbt <1, 1, 1, 1> ]
      } 
      scale 3
    }
  } // (end of sooty mark texture)

} // (end of box declaration)
