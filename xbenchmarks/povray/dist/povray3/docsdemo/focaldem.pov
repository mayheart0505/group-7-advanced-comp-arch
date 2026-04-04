**************************************************************
// Persistence of Vision(tm) Ray Tracer Scene Description File
// Filename    : Focaldem.pov
// POV Version : POV-Ray for Windows v3.00e
// Description : Focal Blur tutorial
// Date        : 11/06/96
// Constructor : Alan C. Kong


// ==== Standard POV-Ray Includes ====
#include "colors.inc"	// Standard Color definitions
#include "shapes.inc"   // Standard Shape definitions
#include "textures.inc"	// Standard Texture definitions

#version 3.0

global_settings {
  assumed_gamma 2.2 // for most PC monitors
  max_trace_level 5
}

sphere { <1, 0, -6>, 0.5
  finish {
    ambient 0.1 
    diffuse 0.6
  }
  pigment { NeonPink }
}

box { <-1, -1, -1>, < 1,  1,  1>
  rotate <0, -20, 0>   
  finish {
    ambient 0.1 
    diffuse 0.6
  }
  pigment { Green }
}

cylinder { <-6, 6, 30>, <-6, -1, 30>, 3  
  finish {
    ambient 0.1 
    diffuse 0.6
  } 
  pigment {NeonBlue}              
}

plane { y, -1.0
  pigment {
    checker color Gray65 color Gray30
  }
}

light_source { <5, 30, -30> color White } 

light_source { <-5, 30, -30> color White }

camera {
  location <0.0, 1.0, -10.0> // position of camera <1 unit up, 10 back>
  look_at  <0.0, 1.0,  0.0>  // point center of view at this point

//  focal_point <-6, 1, 30>    // blue cylinder is the center of focus
//  focal_point < 0, 1,  0>    // green box is the center of focus
  focal_point < 1, 1, -6>    // pink sphere is the center of focus

  aperture 0.4     // a nice compromise
//  aperture 0.05    // almost everything is in focus
//  aperture 1.5     // much blurring

//  blur_samples 4       // fewer samples, faster to render
  blur_samples 20      // more samples, higher quality image
}
