//////////////////////////////////////////////////////////////////////////////////////////
//     Material and box properties
//////////////////////////////////////////////////////////////////////////////////////////

thickness = 4;                  /* Thickness of the material */
tol = 3;                        /* Space on each side of the patch */
boxH = 26;                      /* inside height of the box */
corner = 45;                    /* How much space to give for the corner reinforcement */

// contents
tspW = 190;
tspD = 190;
tspH = 8;
flapW = 40;
flapDX = 30;
flapD = 60;

abW = 90;
abD = 80 + 10;                  /* 10 mm for cable */
abH = 10;

angleR = 70;

// resulting dimensions
boxW = tspW + flapDX + 2*tol;
boxD = tspD + flapD + abD + 2*tol;




//////////////////////////////////////////////////////////////////////////////////////////
//  Helper model of the box contents
//////////////////////////////////////////////////////////////////////////////////////////

module contents () {
     color([1,0,.7], .5) union () {
          cube([abW,abD,abH]);
          translate([flapDX,abD,0]) cube([flapW, flapD, tspH]);
          translate([flapDX, abD+flapD, 0]) cube([tspW,tspD,tspH]);
          translate([flapDX+flapW,abD+flapD-angleR,0]) difference () {
               cube([angleR,angleR,tspH]);
               translate([angleR,0,-.1])cylinder(r = angleR, h = 2*tspH);
          }
     }
}



//////////////////////////////////////////////////////////////////////////////////////////
// Define the box
//////////////////////////////////////////////////////////////////////////////////////////

include <lasercut.scad>

/* module cutoutFingersUD(x,start,n,width,height,thickness) { */
/*      union () { */
/*           children(); */
/*           fingers(UP,start,n,thickness,0,height,width,height); */
/*      } */
/* } */


module base () {
     lasercutoutSquare(
          thickness=thickness, x=boxW,y=boxD,
          finger_joints = [
               [UP,0,4],
               [DOWN,1,4],
               [LEFT,0,6],
               [RIGHT,0,6],               
               ]
          /* simple_tab_holes = [ */
          /*      [RIGHT, 0,10] */
          /*      ] */
          );
     
}


module longWall () {
     union () {
          lasercutoutSquare(
               thickness=thickness, x=boxH+thickness,y=boxD,
               finger_joints = [
                    [LEFT,1,6],
                    [UP,0,2],
                    [DOWN,1,2]
                    ]
               );
          translate([-thickness,-thickness,0]) cube([thickness,thickness,thickness]);
     }
}

module shortWall () {
     lasercutoutSquare(
          thickness=thickness, x=boxW, y=boxH+thickness,
          finger_joints = [
               [DOWN,0,4],
               [LEFT,1,2],
               [RIGHT,0,2]               
               ]
          );
}


module sep1 () {
     lasercutoutSquare(
          thickness=thickness, x=boxW - abW -corner - thickness - 2*tol, y = boxH,
          finger_joints = [
               [DOWN,0,4],
               [LEFT,0,2],
               [RIGHT,0,2]
               ]
          );
}

module sep1_ip () {
     translate([abW + corner + 2*tol + thickness,abD + flapD,thickness]) rotate([90,0,0]) sep1();
}


module sep1a () {
     lasercutoutSquare(
          thickness=thickness, x=boxW - abW - thickness - 2*tol, y = boxH,
          finger_joints = [
               [DOWN,0,5],
               [LEFT,0,2],
               [RIGHT,0,2]
               ]
          );
     
}

module sep1a_ip () {
     translate([abW + 2*tol + thickness, abD + flapD - corner,thickness]) rotate([90,0,0]) sep1a();
}


module sep2 () {     
     lasercutoutSquare(
          /* thickness=thickness, x=boxH, y=boxD - 2*tol - tspD - thickness, */
          thickness=thickness, x=boxH, y=abD+flapD-corner - thickness,
          finger_joints = [
               [DOWN,0,2],
               [UP,1,2],
               [LEFT,1,3]
               ],
          cutouts = [
               [ boxH-10, 10, 12, 40]
               ]
          );
}

module sep2_ip() {
     translate([abW + 2*tol + thickness, 0, thickness])
          rotate([0,270,0]) sep2 ();
}

module sep2a () {     
     lasercutoutSquare(
          /* thickness=thickness, x=boxH, y=boxD - 2*tol - tspD - thickness, */
          thickness=thickness, x=boxH, y=corner - thickness,
          finger_joints = [
               [DOWN,0,2],
               [UP,1,2],
               [LEFT,1,2]
               ]
          );
}

module sep2a_ip() {
     translate([abW + 2*tol + corner+ thickness, abD + flapD-corner, thickness])
          rotate([0,270,0]) sep2a ();
}

module sep1a_final_ip () {
     difference () {
          sep1a_ip();
          sep2a_ip();
     }
}

module sep3 () {
     lasercutoutSquare(
          thickness=thickness, x=boxH, y=tspD+flapD-thickness,
          finger_joints = [
               [UP,0,2],
               [DOWN,0,2],
               [LEFT,1,6]
               ]
          );
}

module sep3_ip () {
     translate([flapDX,abD+thickness+2*tol,thickness]) rotate([0,270,0]) sep3 ();
}

module sep4 () {
     lasercutoutSquare(
          thickness=thickness, x=flapDX-thickness, y=boxH,
          finger_joints = [
               [DOWN,0,2],
               [LEFT,1,2],
               [RIGHT,1,2]
               ]
          );
}

module sep4_ip() {
     translate([0,abD+2*tol+thickness,thickness]) rotate([90,0,0]) sep4 ();
}

module lid () {
     tabW = .6* boxW;
     lasercutoutSquare(
          thickness=thickness, x=boxW, y = boxD,
          simple_tabs = [
               [ UP, boxW/2, boxD, tabW ],
               [ DOWN, boxW/2, 0, tabW ],
               [ LEFT, 0,boxD/2, tabW ],
               [ RIGHT, boxW, boxD/2, tabW ]
               ]
          );
}

module base_final () {     
     difference () {
          base();
          sep1_ip();
          sep1a_ip();
          sep2_ip();
          sep2a_ip();
          sep3_ip();
          sep4_ip();
     }
}

module lid_ip () { translate([0,0,boxH+thickness]) lid(); }
module longWall1_ip () { translate([0,0,thickness]) rotate([0,270,0]) longWall(); }
module longWall2_ip () { translate([boxW,boxD,thickness]) rotate([0,270,180]) longWall(); }
module shortWall1_ip () { translate([0,0,thickness]) rotate([90,0,0]) union () { shortWall(); translate([boxW,-thickness]) cube([thickness,thickness,thickness]); } }
module shortWall2_ip () { translate([0,boxD+thickness,thickness]) rotate([90,0,0]) union () { shortWall(); translate([-thickness,-thickness]) cube([thickness,thickness,thickness]); } }

module longWall1_final_ip () {
     difference () {
          longWall1_ip ();
          sep4_ip();
          lid_ip();
     }
}

module longWall2_final_ip () {
     difference () {
          longWall2_ip ();
          sep1_ip();
          sep1a_final_ip();
          lid_ip();
     }
}

module shortWall1_final_ip () {
     difference () {
          shortWall1_ip();
          sep2_ip();
          lid_ip();
     }
}

module shortWall2_final_ip () {
     difference () {
          shortWall2_ip();
          sep3_ip();
          lid_ip();
     }
}

module built () {
     base_final();
     longWall1_final_ip();
     longWall2_final_ip();
     shortWall1_final_ip();
     shortWall2_final_ip();
     sep1_ip();
     sep1a_final_ip();
     sep2_ip ();
     sep2a_ip();
     sep3_ip();
     sep4_ip();
}

module filled () {
     translate([tol,tol,thickness]) contents();
     built ();
}


module exploded () {
     color([0,0,1], .5) translate([0,0,-50]) base_final();
     color([0,0,1], .5) translate([0,0,150]) lid_ip();
     color([1,0,0]) translate([-30,0,0]) longWall1_final_ip();
     color([1,0,0]) translate([30,0,0]) longWall2_final_ip();
     color([0,1,0]) translate([0,-30,0]) shortWall1_final_ip();
     color([0,1,0]) translate([0,30,0]) shortWall2_final_ip();

     translate([0,0,50]) sep1_ip();
     translate([0,0,100]) sep1a_final_ip();
     translate([0,0,100]) sep2_ip ();
     translate([0,0,50]) sep2a_ip ();
     translate([0,0,50]) sep3_ip();
     translate([0,0,50]) sep4_ip();

}

module shift(x,y=0) { translate([x,y,0]) children(); }
module rot(a) { rotate([0,0,a]) children(); }
module proj () { color([0,0,1], .5) projection () children(); }

module cutmodel (element) {
     color([0,0,1], .5) projection ()  {
          if (element == undef || element == 0) { shift(0,0) base_final();}
          if (element == undef || element == 1) { shift(0,0) lid();}
          if (element == undef || element == 2) { shift(0,0) rotate([0,90,0]) longWall1_final_ip(); }
          if (element == undef || element == 3) { shift(0,0) rotate([0,90,0]) longWall2_final_ip(); }
          if (element == undef || element == 4) { shift(0,0) rotate([90,0,90]) shortWall1_final_ip(); }
          if (element == undef || element == 5) { shift(0,0) rotate([90,0,90]) shortWall2_final_ip(); }
          if (element == undef || element == 6) { shift(0,0) rot(90) sep1();}
          if (element == undef || element == 7) { shift(0,0) sep2();}
          if (element == undef || element == 8) { shift(0,0) sep3();}
          if (element == undef || element == 9) { shift(0,0) sep4();}
          if (element == undef || element == 10) { shift(0,0) rotate([90,0,0]) sep1a_final_ip();}
          if (element == undef || element == 11) { shift(0,0) sep2a ();}
     }

}


shift(-300) cutmodel(0); 
shift(100) exploded ();
shift(500) color(alpha=.4) filled ();
