//
// POV-Ray(tm) 3.0 tutorial example scene.
// Copyright 1996 by the POV-Ray Team
//

#include "colors.inc"

camera { 
  angle 20
  location<0,2,-10>
  look_at<0,0,0>
}

light_source { <10, 20, -10> color White }

blob {
  threshold .65

  sphere { <-.23,-.32,0>,.43, 1 scale <1.95,1.05,.8> }   //palm
  sphere { <+.12,-.41,0>,.43, 1 scale <1.95,1.075,.8> }  //palm
  sphere { <-.23,-.63,0>, .45, .75 scale <1.78, 1.3,1> } //midhand
  sphere { <+.19,-.63,0>, .45, .75 scale <1.78, 1.3,1> } //midhand
  sphere { <-.22,-.73,0>, .45, .85 scale <1.4, 1.25,1> } //heel
  sphere { <+.19,-.73,0>, .45, .85 scale <1.4, 1.25,1> } //heel
                                                             
  cylinder { <-.65,-.28,0>, <-.65,.28,-.05>, .26, 1 }    //lower pinky
  cylinder { <-.65,.28,-.05>, <-.65, .68,-.2>, .26, 1 }  //upper pinky
  
  cylinder { <-.3,-.28,0>, <-.3,.44,-.05>, .26, 1 }      //lower ring
  cylinder { <-.3,.44,-.05>, <-.3, .9,-.2>, .26, 1 }     //upper ring
                    
  cylinder { <.05,-.28,0>, <.05, .49,-.05>, .26, 1 }     //lower middle
  cylinder { <.05,.49,-.05>, <.05, .95,-.2>, .26, 1 }    //upper middle

  cylinder { <.4,-.4,0>, <.4, .512, -.05>, .26, 1 }      //lower index
  cylinder { <.4,.512,-.05>, <.4, .85, -.2>, .26, 1 }    //upper index

  cylinder { <.41, -.95,0>, <.85, -.68, -.05>, .25, 1 }  //lower thumb
  cylinder { <.85,-.68,-.05>, <1.2, -.4, -.2>, .25, 1 }  //upper thumb

  pigment { Flesh }
}
