$fn=50;

// Delta for making sure differences don't leave a zero-width plane
d=.01;
dd = 2*d;
lots = 100;                    /* Just something big, when the size of the difference doesn't matter */

// Dimensions in mm
boardWidth = 65;
boardDepth = 55;
boardHeight = 1.6;

// Define the board
headerX = 5;
headerY = .5;
headerZ = 2;                    /* Elevation off the board of the bottom of the horizontal pins */

bridgeW = 1;
headerW = 18 + 2*bridgeW;
headerD = 2;                    /* Depth of the on-board connector */
headerH = 3;                    /* Height of the horizontal pins */
headerHeight = headerZ+headerH;
headerDepth = 6;                /* Depth of the folded pins */

usbWidth = 5;
usbDepth = 8;
usbHeight = 3;
usbX = -1;
usbY = 10;

tongueX=21;
tongueY=40;

espW = 55;
espD = 28.2;
espH = 5;
espY = 10;

espUsbY = 8;
espUsbD = 12;
clearance = 5.8; // Clearance between board top and ESP

//////////////////////////////////////////////////////////////////////////////////////////
/// Definition of a mock-up of the board with connectors
//////////////////////////////////////////////////////////////////////////////////////////
module board() {
     union () {
          //Base plate
          difference() {
               cube([boardWidth,boardDepth,boardHeight]);
               translate([tongueX,tongueY,-d])
                    cube([27,16,lots]);
          }
          // Flex connector
          translate([21,33,boardHeight])
               cube([30,5,3]);
          // Header connector
          translate([headerX,headerY,boardHeight])
               cube([headerW,headerD,headerZ+headerH+5]);
          translate([headerX,headerY-headerDepth,boardHeight+2])
               cube([headerW,headerDepth,headerH]);
          // USB connector
          translate([usbX,usbY,boardHeight]) cube([usbWidth,usbDepth,usbHeight]);
     }
        
}



//////////////////////////////////////////////////////////////////////////////////////////
// define the lower half of the box
//////////////////////////////////////////////////////////////////////////////////////////
sw = 2;                                          // Side wall width
bwh = 1.2;                                         /* Bottom wall height */
ledgeHeight = 1.4;                               /* Height of hte ledge the board sits on */
ledgeWidth = 3;
tol=.5;                                          /* Tolerance around the edges of the board */
dtol = 2*tol;                                    /* Twice the tolerance (double tolerance) */
tongueLen = 15;                                  /* Length of the tongue clamping */
tongueWidth = 40;                                /* Width of the tongue, including silicone */
tongueHeight = 6;                                /* Height of the tongue */
tongueMid = sw+tol+21+.5*27;
boxWidth = boardWidth + dtol + 2*sw;             /*  */
boxDepth = boardDepth + tongueLen + dtol + 2*sw; /*  */
boxHeight = bwh + ledgeHeight + boardHeight + headerHeight + 3; /* 2 mm above the header */

// Screw mounts
screwX = 14;
screwY = 65;
screwX2 = 60;
screwH = 3;
screwHead = 3; //M3
screwInner= 1.2;// works for M2 screws! (M3 is diam 2.45 officially)
screwOuter = 1.5; // diam 3.0

module ribs(num, dist, width, height) {
     for (i = [0:num]) {
          translate([0,i*(dist+height),0]) cube([width,height,height]);
     }
}

slitTol = 1;
dSlitTol = 2*slitTol;
lowerTongueDepressorHeight = 1.4;
module lower () {
     difference () {
          cube([boxWidth, boxDepth, boxHeight]);
          union () { // Stuff to remove:
               // bulk centre
               translate([sw,sw,bwh]) difference () {
                    union () { // this is the stuff to remove from the outer box
                         cube([boardWidth+dtol,boxDepth-2*sw,lots]); // board space, inside of box
                         // "tongue" slit
                         translate([tongueMid-sw-.5*tongueWidth, boardDepth, 0]) cube([tongueWidth,tongueLen+10,boxHeight]);
                         // Header slit
                         translate([tol+headerX-slitTol,-2*sw,ledgeHeight+boardHeight+headerZ-slitTol]) cube([headerW+dSlitTol,4*sw,lots]);
                         // USB slit
                         translate([-sw-d,usbY,ledgeHeight+boardHeight-tol]) // Did not subtract tol from y, 
                              cube([sw+dd,usbDepth+dtol,lots]);  // because board can slide by tol in either direction

                         // ESP USB slit
                         translate([boardWidth-sw-d,espY+espUsbY-sw-slitTol,boxHeight-espH-3*slitTol]) 
                              cube([lots,espUsbD+dSlitTol,lots]);
                    }

                    // this is the stuff that's not removed
                    translate([-d,-d,-d]) cube([ledgeWidth,boardDepth+dd,ledgeHeight+d]); // add back ledge 1
                    translate([boardWidth+dtol-ledgeWidth,-d,-d]) cube([ledgeWidth,boardDepth+dd,ledgeHeight+d]); // add back ledge 2
                    translate([headerW+headerX+dSlitTol,0,-d]) cube([boardWidth+dd,ledgeWidth,ledgeHeight+d]); /* Add back ledge at back */
                    translate([-d,boardDepth-ledgeWidth+dtol,-d]) cube([20+dd,ledgeWidth,ledgeHeight+d]); /* Add back ledge under tongue */
                    translate([50,boardDepth-ledgeWidth+dtol,-d]) cube([20+dd,ledgeWidth,ledgeHeight+d]); /* Add back ledge under tongue */
                    translate([-d,boardDepth+dtol,-d]) cube([20+dd,sw,ledgeHeight+boardHeight+d]); /* Fixing wall for board */
                    translate([50,boardDepth+dtol,-d]) cube([20+dd,sw,ledgeHeight+boardHeight+d]); /* Fixing wall for board */
                    // ribs
                    translate([0,boardDepth+dtol,-d]) cube([lots,lots,lowerTongueDepressorHeight+d]);
                    translate([15,boardDepth+2*dtol,lowerTongueDepressorHeight-d]) ribs(6,2,40,1+d);
                    for (x = [screwX,screwX2])
                         translate([x-sw,screwY-sw,0]) 
                              cylinder(h=screwH, r=5);
               }
          }    
          for (x = [screwX,screwX2]) {
               translate([x,screwY,-d]) 
                    cylinder(h=lots, r=screwOuter);
               translate([x,screwY,-d]) 
                    cylinder(h=2, r=screwHead);
          }
     }
}

boardTop = bwh+ledgeHeight+boardHeight; /* Height at which the top of the board sits */

//////////////////////////////////////////////////////////////////////////////////////////
// Define the middle half of the box
//////////////////////////////////////////////////////////////////////////////////////////
lidHeight = 1.6;                  /* Thickness of the lid above the top of the box */
middleHeight = lidHeight + boxHeight - boardTop;

fixD = 2;
lipD = .2;
espX = boxWidth-2*sw-espW;
module middle () {
     union () {
          difference () {
               cube([boxWidth,boxDepth,middleHeight]); /* Cube the size of the box */
               difference () {                      /* Outer walls of the box */
                    translate([-d,-d,-boardTop])
                         cube([boxWidth+dd,boxDepth+dd,boxHeight]); /* Cube the size of the box */
                    translate([sw,sw,-d])
                         cube([boxWidth-2*sw,boxDepth-2*sw,lots]); /* Inside to take away so we're left with the walls */
               // Header slit
                    translate([d+sw+tol+headerX-slitTol,-2*sw,boxHeight-lots])
                         cube([headerW+dSlitTol-dd,5*sw,lots]);
               }
               difference () {
                    /* Hollow out lid, leave the tongue depressor and the cradle for ESP32 */
                    translate([2*sw,2*sw,-d])
                         cube([boxWidth-4*sw,boxDepth-4*sw,middleHeight-lidHeight+dd]); /* Take away the inside of the box */
                    /* translate([tongueMid-.5*tongueWidth,55+sw+tol,tongueHeight-boardHeight-ledgeHeight]) */
                    /*      cube([tongueWidth,lots,lots]); */
                    translate([-d,55+sw+tol,tongueHeight-boardHeight-ledgeHeight])
                         cube([lots,lots,lots]);
                    for (x = [screwX,screwX2])
                         translate([x,screwY,screwH-bwh-ledgeHeight]) 
                              cylinder(h=lots, r=3);

                    /* Cradle for the ESP32 */
                    /* translate([espX-tol,espY-fixD,clearance]) // mount 1 */
                    /*      cube([espW+dtol,fixD,lots]); */
                    /* translate([espX-tol,espY+espD,clearance]) // Mount 2 */
                    /*      cube([espW+dtol,fixD,lots]); */
                    translate([0,espY-fixD,clearance]) // mount 1
                         cube([lots,fixD,lots]);
                    translate([0,espY+espD,clearance]) // Mount 2
                         cube([lots,fixD,lots]);

                    lipL = 10;
                    for (i = [10:3*lipL:espW]) {
                         translate([espX-tol+i,espY,clearance]) // Fixing lip 1
                              cube([lipL,lipD,.5]);
                         translate([espX-tol+i,espY+espD-lipD,clearance]) // Fixing lip 2
                              cube([lipL,lipD,.5]);
                    }
                    translate([espX-fixD-tol,espY-fixD,clearance])
                         cube([fixD,espD+2*fixD,lots]);
                    
               }
               // "tongue" slit
               translate([tongueMid-.5*tongueWidth, boardDepth, tongueHeight-boardHeight-ledgeHeight-lots])
                    cube([tongueWidth,tongueLen+10,lots]);
               // Header slit
               translate([sw+tol+headerX-slitTol,-2*sw,headerHeight+slitTol-lots])
                    cube([headerW+dSlitTol,5*sw,lots]);
               // Header opening in top
               /* translate([sw+tol+headerX-slitTol,headerY+sw+tol-slitTol-d,-d]) */
               /*      cube([headerW+dSlitTol,headerD+dSlitTol+d,lots]); */
               translate([sw+tol+headerX-slitTol+bridgeW,headerY+sw+tol-slitTol-d,-d])
                    cube([headerW+dSlitTol-2*bridgeW,headerD+dSlitTol+d,lots]);
               // USB slit
               translate([-d,usbY+sw,-d]) // Did not subtract tol from y, because board can slide by tol in either direction
                    cube([2*sw+dd,usbDepth+dtol,usbHeight+tol+d]);
               // ESP USB slit in wall
               translate([boxWidth-2*sw-d,espY+espUsbY,-d])
                    cube([lots,espUsbD,middleHeight-lidHeight-boardHeight+d]);
               // ESP USB cutout in lid, for cable connector
               translate([boxWidth-sw,espY+espUsbY,-d])
                    cube([lots,espUsbD,lots]);

               translate([16,boxDepth-15,middleHeight-.8]) linear_extrude(1) text("CreaTe",size=9);
               translate([16,boxDepth-25,middleHeight-.8]) linear_extrude(1) text("Hands-On AI",size=5);

               for (x = [screwX,screwX2]) {
                    translate([x,screwY,-d]) 
                         cylinder(h=middleHeight-lidHeight, r=screwInner);
          }

          }

          translate([tongueMid-.5*tongueWidth,60,tongueHeight-boardHeight-ledgeHeight-.8-d])
               ribs(4,2,tongueWidth,1.0+d); /* Add ribs to the top */
          translate([0,usbY+sw,usbHeight+tol])
               cube([sw+d,usbDepth+dtol,middleHeight-(usbHeight+dtol)]); // Let wall fill up USB slit in lower part
          /* translate([10,10,]) linear_extrude(1) text("Hands-on AI",size=10,halign="center"); */

     } 

}    


module slices(sep=10,w=5) {
     union () {
          for ( i = [-5:sep:70]) {
               translate([-20,i,-10]) cube([100,w,30]);
          }
     }
};
// slices(10);
// Final elements

difference () {
     union () {
         /* color(c=[1,0,0],alpha=.3) translate([sw+tol,sw+tol,bwh+ledgeHeight]) board(); */
         color(c=[.5,.5,.9],alpha=.9) lower();
          color([.3,1,.5,.5]) translate([-2,0,middleHeight]) rotate([0,180,0]) middle();
          color([.3,1,.5,.3]) translate([0,0,boardTop]) middle();
     };
     /* slices(70,30); */
};


/* w = 100; */
/* h = 70; */
/* t = 2; */


/* module txt(y, string, size) { */
/*     translate([0,y,-.5]) linear_extrude(1) text(string,size=size,halign="center"); */
/* } */
    
/* translate([0,-30,1]) difference() { */
/*     cube([w,h,t],center=true); */
/*     translate([0,0,.5*t]) cube([w-2,h-2,t],center=true); */
/*     txt(.25*h,"Bon", 14); */
/* txt(.1*h,"Pour une bobine de filament 3D",5); */
/* txt(-.15*h,"Joyeux Noël",12); */
/* txt(-.35*h,"Gros Bisous, Franklin!",size=5); */
/* } */
