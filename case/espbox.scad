use <ESPModels.scad>

$fn = 40;

espW = 55;
espD = 28.2;
espH = 1.8;
espY = 10;

headerBlockH = 2.6;
headerPinThick = 0.65;

espHeaderH = 6 + headerBlockH;
espHeaderBelow = 2;

espUsbY = 10;
espUsbD = 8;
espUsbDTol = 1;

espUsbH = 3;


wallThick = 2;
raise = 3.6;
tol = .2;

deltaPin = 2.54;
pinX = 4.5;
pinY1 = deltaPin/2;
pinY2 = pinY1 + 25.5; // was 25.4 (should be 25.4)

d = .1;
dd = 2*d;

module headerBlock () {
     half = deltaPin/2;
     intersection () {
          translate([-half,-half,0])
               cube([deltaPin,deltaPin,headerBlockH]);
          rotate ([0,0,45]) scale([1.1,1.1,1]) translate([-half,-half,0]) 
               cube([deltaPin,deltaPin,headerBlockH]);
          
     }
}

module pin() {
     hpt = headerPinThick/2;
     union() {
          headerBlock();
          translate([-hpt,-hpt, -espHeaderBelow])
               cube([headerPinThick,headerPinThick,espHeaderH+espHeaderBelow]);
     }
}

module pinCutout (number, row) {
     cutoutSize = 3.2;
     hcs = cutoutSize/2;
     y = row == 0 ? pinY1 : pinY2;
     translate([pinX + number*deltaPin - hcs, y-hcs, -50])
          cube([cutoutSize,cutoutSize,100]);
}

module pinLabel(number, row, string) {
     dist = 2.4;
     
     /* y = row == 0 ? pinY1 + dist  : pinY2 - dist; */
     /* translate([pinX + number*deltaPin, y, -.4]) */
     /*      linear_extrude(1) rotate([0,0,90]) text(string,size=4,halign="center",valign="center"); */

     if (row == 0) {
          y = pinY1 + dist;
          translate([pinX + number*deltaPin, y, -.4])
               linear_extrude(1) rotate([0,0,90]) text(string,size=4,halign="left",valign="center");
     } else {
          y = pinY2 - dist;
          translate([pinX + number*deltaPin, y, -.4])
               linear_extrude(1) rotate([0,0,90]) text(string,size=4,halign="right",valign="center");
     }          
}

/* headerBlock (); */
/*
module esp32() {
     hhpt = headerPinThick/2;
     
     cube([espW,espD,espH]);    // Board 
     
     for (i = [0:18]) {         // Pins 
          translate([pinX + i*deltaPin, pinY1, espH])
               pin();
          translate([pinX + i*deltaPin, pinY2, espH])
               pin();
     }
     translate([-1, espUsbY, espH])
          cube([10,espUsbD,espUsbH]);
     
}
*/

module esp32() {
     hhpt = headerPinThick/2;
     
     cube([espW,espD,espH]);    
     
     for (i = [0:18]) {         
          translate([pinX + i*deltaPin, pinY1, espH])
               pin();
          translate([pinX + i*deltaPin, pinY2, espH])
               pin();
     }
     translate([-1, espUsbY, -espUsbH])
          cube([10,espUsbD,espUsbH]);
     
}


/* translate([wallThick+tol,wallThick+tol,wallThick+raise]) color([1,0,0],.5) esp32(); */


aboveBoardH = espHeaderH + .4;
belowBoardH = espUsbH;


aboveBoardZ = wallThick+raise+espH;

module rcube(dim, r) {
     hull () {
          translate([r,r,0])
                    cylinder(r=r,h=dim[2]);
          translate([dim[0]-r,r,0])
                    cylinder(r=r,h=dim[2]);
          translate([r,dim[1]-r,0])
                    cylinder(r=r,h=dim[2]);
          translate([dim[0]-r,dim[1]-r,0])
                    cylinder(r=r,h=dim[2]);
     }
}

module bottom(item=0) {
     boxW = espW + 2*wallThick + 2*tol;
     boxD = espD + 2*wallThick + 2*tol;
     boxH = aboveBoardZ + aboveBoardH;

     echo(str("Box dimensions are width ", boxW, ", depth ", boxD, ", height ", boxH+wallThick));
     
     lipSide = 1;
     lipFront = 2;

     lipY = wallThick+tol+2;
     lipD = 2;

     union () {
          difference() {
               rcube([boxW, boxD, boxH],wallThick);
               translate([wallThick, wallThick, wallThick])
                    cube([espW+2*tol, espD+2*tol, 100]);
               /* translate([wallThick+lipFront, wallThick+lipSide, wallThick]) cube([espW+2*tol-2*lipFront, espD+2*tol-2*lipSide, 100]); */

               translate([-d, wallThick+ tol + espUsbY - espUsbDTol, wallThick+raise - espUsbH - tol])
                    cube([wallThick+2*d,espUsbD+2*espUsbDTol, 100]);
               /* translate([boxW/2, .6*boxD, -.1]) */
               /*      linear_extrude(1) rotate([0,180,0]) text("CreaTe",size=7,halign="center",valign="center"); */
               /* translate([boxW/2, boxD/4, -.1]) */
               /*      linear_extrude(1) rotate([0,180,0]) text(str(item),size=8,halign="center",valign="center"); */
               translate([boxW/2, .4, .5*boxH])
                    rotate([90,0,0]) linear_extrude(1) rotate([180,180,180]) text("CreaTe",size=10,halign="center",valign="center");
               translate([boxW/2, boxD+1-0.4, .5*boxH])
                    rotate([90,0,0]) linear_extrude(1) rotate([180,0,180]) text("CreaTe",size=10,halign="center",valign="center");
          }

          
          translate([0, lipY, wallThick])
               cube([boxW, lipD,raise]);
          translate([0, wallThick, wallThick])
               cube([boxW, lipY-wallThick,raise - espHeaderBelow - tol]);

          translate([0, boxD - lipY - lipD, wallThick])
               cube([boxW, lipD,raise]);
          translate([0, boxD - lipY, wallThick])
               cube([boxW, lipY-wallThick,raise - espHeaderBelow - tol]);

          
     }
}

/* color([0,1,0],.5) bottom(); */

module lidReinforce () {
     difference () {
          lidD = espD + 2*wallThick + 2*tol;
          lidH = aboveBoardH + wallThick;
          /* translate([wallThick, 4*wallThick, wallThick])  */
          cube([3*wallThick, lidD - 8*wallThick, lidH-wallThick]);
          translate([wallThick, -d, 0]) rotate([0,45,0])
               cube([5*wallThick, lidD - 8*wallThick+ 2*d, lidH-2*wallThick]);
     }
}


module lid(item=0) {
     lidW = espW + 2*wallThick + 2*tol;
     lidD = espD + 2*wallThick + 2*tol;
     lidH = aboveBoardH + wallThick;

     rw = 4;

     pressThick = 2;
     pressD = 5;

     walltol = 0.1;

     difference() {
          union () {
               translate([0,0,aboveBoardH])
                    rcube([lidW,lidD,wallThick],wallThick);


               // Pressors on ESP32
               translate([wallThick+walltol,wallThick+walltol,0])
                    cube([pressThick,lidD-2*wallThick-2*walltol, lidH-tol]);
               translate([lidW-pressThick-wallThick-walltol,wallThick+walltol,0])
                    cube([pressThick,lidD-2*wallThick-2*walltol, lidH-tol]);

               // reinforcement
               translate([wallThick+walltol, 4*wallThick, wallThick])
                    lidReinforce();
               translate([lidW-wallThick-walltol, lidD - 4*wallThick, wallThick]) rotate([0,0,180])
                    lidReinforce();

               // Key for USB slot:
               translate([0,wallThick+tol+espUsbY-espUsbDTol,0])
                    cube([wallThick+2*walltol,espUsbD+2*espUsbDTol,lidH-tol]);

               // lid rigidification:
               translate ([wallThick+walltol, 4*wallThick,lidH-2*wallThick])
                    cube([lidW-2*wallThick-2*walltol,wallThick,wallThick]);
               translate ([wallThick+walltol, lidD-5*wallThick,lidH-2*wallThick])
                    cube([lidW-2*wallThick-2*walltol,wallThick,wallThick]);

               difference () {
                    translate ([wallThick+walltol, 1*wallThick+walltol,lidH-2*wallThick-d])
                         cube([lidW-2*wallThick-2*walltol,rw,wallThick+d]);
                    translate ([wallThick, 1*wallThick+pinY1-headerPinThick/2,lidH-2*wallThick-dd])
                         cube([lidW-2*wallThick,headerPinThick+2*tol,wallThick+d]);
               }
                    
               difference () {
                    translate ([wallThick+walltol, lidD-wallThick-rw-walltol,lidH-2*wallThick-d])
                         cube([lidW-2*wallThick-2*walltol,rw,wallThick+d]);
                    translate([wallThick,wallThick+pinY2-headerPinThick/2,lidH-2*wallThick-dd])
                         cube([lidW,headerPinThick+2*tol,wallThick+d]);
               }
               
          }

          // cutouts
          translate([wallThick+tol,wallThick+tol,aboveBoardH]) union () {
               pinCutout(7,0);
               pinCutout(18,0);
               pinCutout(13,1);
               pinCutout(16,1);
               pinCutout(18,1);
          }
          translate([wallThick+tol,wallThick+tol,lidH]) union () {
               pinLabel(7,0,"6");
               pinLabel(18,0,"2");
               pinLabel(13,1,"4");
               pinLabel(16,1,"5");
               pinLabel(18,1,"3");

               /* translate([5, espD/2, -.4]) */
               /*      linear_extrude(1) rotate([0,0,90]) text(str(item),size=8,halign="center",valign="center"); */

          }
     }
}


module closed()  {
     union () {
          translate([wallThick+tol,wallThick+tol,wallThick+raise]) color([1,0,0],.5)
               esp32();
          color([0,1,0],.5)
               bottom();          
          translate([0,0,aboveBoardZ])           color([0,0,1],.5)
               lid();
     };
}

module open(item=0)  {
     union () {
          /* translate([wallThick+tol,wallThick+tol,wallThick+raise]) color([1,0,0],.5) */
          /*      esp32(); */
          color([0,1,0],.5)
               bottom(item);
          
          translate([0,-2,aboveBoardH+wallThick])
               rotate([180,0,0])         //color([0,0,1],.5)
               lid(item);
     };
}

module exploded()  {
     delta = 20;
     union () {
          translate([wallThick+tol,wallThick+tol,delta]) color([1,0,0],.5)
               esp32();

          color([0,1,0],.5)
               bottom();
          
          translate([0,0,2*delta])
               /* rotate([180,0,0]) */
               color([0,0,1],.5)
               lid();
     };
}


module slices(sep=10,w=5) {
     union () {
          for ( i = [-5:sep:70]) {
               translate([-20,i,-10]) cube([100,w,30]);
          }
     }
};
module slicesX(sep=10,w=5) {
     union () {
          for ( i = [-5:sep:70]) {
               translate([i, -20,-10]) cube([w,100,30]);
          }
     }
};

module sliced() {
     difference () {
          closed();
          slicesX(20,15);
     }
}

module production (rows=3,cols=2, startat=1) {
     for (i = [0:cols-1])
          for (j = [0:rows-1])
               translate([61*i,69*j,0]) open(startat+i+j*cols);
}

/* module prod2(rows=3,cols=2,startat=1) { */

/*      difference () { */
/*           color("red") translate([-3,-38, 0]) rcube([245,210,0.2],5); */

/*           /\* translate([-1.5,0,0]) production(rows,cols,startat); *\/ */
/*           /\* translate([1.5,0,0]) production(rows,cols,startat); *\/ */
/*           /\* translate([0,-.1,0]) production(rows,cols,startat); *\/ */
/*           /\* translate([0,.1,0]) production(rows,cols,startat); *\/ */
/*      } */
/*      production(rows,cols,startat); */
/* } */

     
          

/* closed(); */
/* sliced(); */
/* exploded(); */
/* open(); */

module anim () {
     intersection () {
          children();

          translate([$t*60,-100,0])
               cube([200,200,200]);
     }
}
/* anim() closed();  */

/* production(rows=1,cols=3,startat=7); */
/* open(); */
module lids() {
     translate([0,-37,aboveBoardH+wallThick]) rotate([180,0,0]) lid();
     translate([61,-37,aboveBoardH+wallThick]) rotate([180,0,0]) lid();
     translate([2*61,-37,aboveBoardH+wallThick]) rotate([180,0,0]) lid();
}

boxD = espD+2*wallThick+2*tol;
boxW = espW+2*wallThick+2*tol;


module supportX(angle=45,width=20, depth=2,delta=2){
     union() {
          intersection () {
               translate([0,-depth/2,0]) cube([width,depth,width*tan(angle)]);
               translate([.2,-width,0]) rotate([0,90-angle,0]) cube([width+dd,2*width, 2*width*tan(angle)]);
          }
          intersection () {
               translate([0,0,0]) cylinder(r=width,h=.2);
               translate([0,-width,0]) cube([width,2*width,1]);
          }
          for (x= [delta:delta:width]) {
               translate([x,0,0]) cylinder(r=.2,h=x*tan(angle)+1);
          }
     }
}

module support(angle=45,width=20, depth=2,delta=2){
     rotate([0,0,90])
          supportX(angle,width,depth,delta);
}


module slantedLid(angle=45) {
     rotate([angle,0,0]) translate([0,boxD,aboveBoardH+wallThick])
               rotate([180,0,0])         //color([0,0,1],.5)
               lid();

     translate([boxW/2,0,0]) support(angle,15,depth=1);

}
//slantedLid();
module slantedBottom(angle) {
     rotate([angle,0,0])// translate([0,boxD,aboveBoardH+wallThick])
               /* rotate([180,0,0])         //color([0,0,1],.5) */
               bottom();

     translate([boxW/2,0,0]) support(angle,15,depth=1);

}
/* slantedBottom(45); */

module slantedBox () {
     slantedBottom(45);
     translate([0,20,0]) slantedLid(45);
}

module slantedProduction (rows=3,cols=2) {
     for (i = [0:cols-1])
          for (j = [0:rows-1])
               translate([61*i,50*j,0]) slantedBox();
}

//slantedProduction(1,1);
production(1,1);

module devel() {
     delta = 80;
     closed();
     translate([delta,0,0]) open();
     translate([2*delta,0,0]) exploded();
     translate([3*delta,0,0]) sliced();
}

/* devel(); */

/* esp32(); */
/* translate([28,15,10]) rotate([0,0,270]) (); */
/* ESPDemo(); */
