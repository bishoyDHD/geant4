//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: testSolidComparisons.cc,v 1.2 2005-08-04 11:26:03 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// --------------------------------------------------------------
// 
// Test for comparison of solids of different type but similar topology
// Returns 0 if there no inconsistencies in the answers provided by the
// compared solids.
//
// Author: Dionisyos Anninos
//
// --------------------------------------------------------------
#include <assert.h>
#include <cmath>

#include "globals.hh"
#include "geomdefs.hh"

#include "G4ThreeVector.hh"

#include "G4Tubs.hh"
#include "G4Ellipsoid.hh"
#include "G4EllipticalTube.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Box.hh"
#include "G4Trap.hh"

#include "Randomize.hh"

#include "G4RotationMatrix.hh"
#include "G4AffineTransform.hh"
#include "G4VoxelLimits.hh"


void logErrors(G4double x, G4double y, G4double z, 
		 G4double vx, G4double vy, G4double vz, G4double dist)
{
  G4cout <<"The point ("<<x<<","<<y<<","<<z<<")"<<G4endl
	 <<"In the direction ("<<vx<<","<<vy<<","<<vz<<")"<<G4endl
         <<"Gives a difference of "<<dist<<"cm !"<<G4endl;
}

G4bool compareEllipsoidtoOrb(G4int N)
{
  
  G4bool what = true;  
  G4int i=0, n=0;
  G4ThreeVector pin, pout, dir;
  G4double xin, yin, zin, xout, yout, zout, dist1, dist2, dist;
   
  // construct the ellipsoid and Orb with the same dimensions 
  
  G4Ellipsoid t1("Solid Ellipsoid #1",
		 20*cm,       // xSemiAxis
		 20*cm,       // ySemiAxis
		 20*cm) ;     // zSemiAxis
  
  
  G4Orb t2("Solid Orb #1", 20*cm) ; 
 
  for(i=0; i<N; i++)
  {
    xout = RandFlat::shoot(25.0*cm,300.0*cm);
    yout = RandFlat::shoot(25.0*cm,300.0*cm);
    zout = RandFlat::shoot(25.0*cm,300.0*cm);
    
    xin  = RandFlat::shoot(-10.0*cm,10.0*cm);
    yin  = RandFlat::shoot(-10.0*cm,10.0*cm);
    zin  = RandFlat::shoot(-10.0*cm,10.0*cm);
    
    pin  = (xin,  yin,  zin );
    pout = (xout, yout, zout);
    
    dir  = pin - pout;
    dir /= dir.mag();

    dist1 = t1.DistanceToIn(pout,dir) ;
    dist2 = t2.DistanceToIn(pout,dir) ;
 
    if(dist1 != kInfinity && dist2 !=kInfinity)
    {
      if(std::fabs(dist1 - dist2) >= 5.*kCarTolerance)
      { 
	what = false; 
	dist = std::fabs(dist1 - dist2);
	logErrors( pout.x(),   pout.y(),   pout.z(),
		   dir.x() ,   dir.y() ,   dir.z() , dist);
	n++;			
      } 
    }

    dist1 = t1.DistanceToOut(pin,dir);
    dist2 = t2.DistanceToOut(pin,dir);
 
    if(dist1 != kInfinity && dist2 !=kInfinity)
    {
      if(std::fabs(dist1 - dist2) >= 5.*kCarTolerance)
      {
	what = false; 
	dist = std::fabs(dist1 - dist2);
	logErrors( pin.x(), pin.y(), pin.z(),
		   dir.x(), dir.y(), dir.z(), dist);
	n++;	   
      }
    }
  }
  
  G4cout <<"The number of inconsistencies when comparing Ellipsoid to Orb were: "<<n<<"."<<G4endl;
  return what;
}

G4bool compareEllipsoidtoSphere(G4int N)
{
  
  G4bool what = true;  
  G4int i=0,n=0;
  G4ThreeVector pin, pout, dir;
  G4double xin, yin, zin, xout, yout, zout, dist1, dist2, dist;
    
  // construct the ellipsoid and sphere with the same dimensions 
  
  G4Ellipsoid  t1("Solid Ellipsoid #1",
				   20*cm,       // xSemiAxis
				   20*cm,       // ySemiAxis
				   20*cm) ;     // zSemiAxis
  
  
  G4Sphere  t2("Solid Sphere #1", 
			     0*cm,
			     20*cm,
			     0*rad, 2*pi*rad,
			     0*rad, 2*pi*rad) ;  
 
  for(i=0; i<N; i++)
  {
    xout = RandFlat::shoot(25.0*cm,300.0*cm);
    yout = RandFlat::shoot(25.0*cm,300.0*cm);
    zout = RandFlat::shoot(25.0*cm,300.0*cm);
    
    xin  = RandFlat::shoot(-10.0*cm,10.0*cm);
    yin  = RandFlat::shoot(-10.0*cm,10.0*cm);
    zin  = RandFlat::shoot(-10.0*cm,10.0*cm);
    
    pin  = (xin, yin, zin);
    pout = (xout, yout, zout);
    
    dir  = pin - pout;
    dir /= dir.mag();

    dist1 = t1.DistanceToIn(pout,dir) ;
    dist2 = t2.DistanceToIn(pout,dir) ;
 
    if(dist1 != kInfinity && dist2 != kInfinity)
    {
      if(std::fabs(dist1 - dist2) >= 5.*kCarTolerance)
      { 
	what = false; 
	dist = std::fabs(dist1 - dist2);
	logErrors(pout.x(), pout.y(), pout.z(),
		  dir.x() , dir.y() , dir.z() , dist);
	n++;
      }
    }

    dist1 = t1.DistanceToOut(pin,dir);
    dist2 = t2.DistanceToOut(pin,dir);
 
    if(dist1 != kInfinity && dist2 !=kInfinity)
    {
      if(std::fabs(dist1 - dist2) >= 5.*kCarTolerance)
      {
	what = false; 
	dist = std::fabs(dist1 - dist2);
	logErrors( pin.x(), pin.y(), pin.z(),
		   dir.x(), dir.y(), dir.z(), dist);
	n++;
      }
    }
  }
  
  G4cout <<"The number of inconsistencies when comparing Ellipsoid to Sphere were: "<<n<<"."<<G4endl;
  return what;
}

G4bool compareEllipticalTubetoTubs(G4int N)
{
  
  G4bool what = true;  
  G4int i=0,n=0;
  G4ThreeVector pin, pout, dir;
  G4double xin, yin, zin, xout, yout, zout, dist1, dist2,dist;
    
  // construct the tube and elliptical tube with the same dimensions 
  
  G4EllipticalTube t1("Solid EllipticalTube #1",
		      20*cm,       // xSemiAxis
		      20*cm,       // ySemiAxis
		      20*cm) ;     // zHeight
  
  
  G4Tubs t2("Solid Tubs #1", 
	    0*cm,
	    20*cm,
	    20*cm,
	    0*rad, 2*pi*rad); 
  
  for(i=0; i<N; i++)
  {
    xout = RandFlat::shoot(25.0*cm,300.0*cm);
    yout = RandFlat::shoot(25.0*cm,300.0*cm);
    zout = RandFlat::shoot(25.0*cm,300.0*cm);
    
    xin  = RandFlat::shoot(-19.0*cm,19.0*cm);
    yin  = RandFlat::shoot(-1.0*cm , 1.0*cm)*std::sqrt(361.*cm*cm-sqr(xin));
    zin  = RandFlat::shoot(-19.0*cm,19.0*cm);
    
    pin  = (xin, yin, zin);
    pout = (xout, yout, zout);
    
    dir  = pin - pout;
    dir /= dir.mag();

    dist1 = t1.DistanceToIn(pout,dir) ;
    dist2 = t2.DistanceToIn(pout,dir) ;
 
    if(dist1 != kInfinity && dist2 != kInfinity)
    {
      if(std::fabs(dist1 - dist2) >= 5.*kCarTolerance)
      {  
	what = false; 
	dist = std::fabs(dist1 - dist2);
	logErrors(pout.x(), pout.y(), pout.z(),
		  dir.x() , dir.y() , dir.z() , dist);
	n++;
      }
    }

    dist1 = t1.DistanceToOut(pin,dir);
    dist2 = t2.DistanceToOut(pin,dir);
 
    if(dist1 != kInfinity && dist2 !=kInfinity)
    {
      if(std::fabs(dist1 - dist2) >= 5.*kCarTolerance)
      {
	what = false; 
	dist = std::fabs(dist1 - dist2);
	logErrors(pin.x(), pin.y(), pin.z(),
		  dir.x(), dir.y(), dir.z(), dist);
	n++;
      }
    }
  }
  
  G4cout <<"The number of inconsistencies when comparing EllipticalTube to Tubs were: "<<n<<"."<<G4endl;
  return what;
}

G4bool compareBoxtoTrap(G4int N)
{
  
  G4bool what = true;  
  G4int i=0,n=0;
  G4ThreeVector pin, pout, dir;
  G4double xin, yin, zin, xout, yout, zout, dist1, dist2, dist;
    
  G4ThreeVector pt[8] = { G4ThreeVector(-20*cm,-20*cm,-20*cm ),
			  G4ThreeVector( 20*cm,-20*cm,-20*cm ),
			  G4ThreeVector(-20*cm, 20*cm,-20*cm ),
			  G4ThreeVector( 20*cm, 20*cm,-20*cm ),
			  G4ThreeVector(-20*cm,-20*cm, 20*cm ),
			  G4ThreeVector( 20*cm,-20*cm, 20*cm ),
			  G4ThreeVector(-20*cm, 20*cm, 20*cm ),
			  G4ThreeVector( 20*cm, 20*cm, 20*cm )  };
  
  // construct the Trap  and Box with the same dimensions 
  
  G4Trap  t1("Solid Trap #1",
	     pt) ;     
  
  
  G4Box  t2("Solid Box #1", 
	    20*cm,
	    20*cm,
	    20*cm); 
  
  for(i=0; i<N; i++)
  {
    xout = RandFlat::shoot(25.0*cm,300.0*cm);
    yout = RandFlat::shoot(25.0*cm,300.0*cm);
    zout = RandFlat::shoot(25.0*cm,300.0*cm);
    
    xin  = RandFlat::shoot(-10.0*cm,10.0*cm);
    yin  = RandFlat::shoot(-10.0*cm,10.0*cm);
    zin  = RandFlat::shoot(-10.0*cm,10.0*cm);
    
    pin  = (xin, yin, zin);
    pout = (xout, yout, zout);
    
    dir  = pin - pout;
    dir /= dir.mag();

    dist1 = t1.DistanceToIn(pout,dir) ;
    dist2 = t2.DistanceToIn(pout,dir) ;
 
    if(dist1 != kInfinity && dist2 !=kInfinity)
    {
      if(std::fabs(dist1 - dist2) >= 5.*kCarTolerance)
      { 
	what = false; 
	dist = std::fabs(dist1 - dist2);
	logErrors(pout.x(),pout.y(),pout.z(),
		  dir.x(), dir.y(), dir.z(), dist);
	n++;
      }
    }

    dist1 = t1.DistanceToOut(pin,dir);
    dist2 = t2.DistanceToOut(pin,dir);
 
    if(dist1 != kInfinity && dist2 !=kInfinity)
    {
      if(std::fabs(dist1 - dist2) >= 5.*kCarTolerance)
      {
	what = false; 
	dist = std::fabs(dist1 - dist2);
	logErrors(pin.x(), pin.y(), pin.z(),
		  dir.x(), dir.y(), dir.z(), dist);
	n++;
      }
    }
  }
  
  G4cout <<"The number of inconsistencies when comparing Box to Trap were: "<<n<<"."<<G4endl;
  return what;
}

G4bool compareSpheretoOrb(G4int N)
{
  
  G4bool what = true;  
  G4int i=0,n=0;
  G4ThreeVector pin, pout, dir;
  G4double xin, yin, zin, xout, yout, zout, dist1, dist2,dist;
    
  // construct the ellipsoid and sphere with the same dimensions 
  
  G4Orb  t1("Solid Orb #1",
		 20*cm) ;

  G4Sphere t2("Solid Sphere #1", 
			     0*cm,
			     20*cm,
			     0*deg, 2*pi*rad,
			     0*rad, 2*pi*rad) ; 
  
  for(i=0; i<N; i++)
  {
    xout = RandFlat::shoot(25.0*cm,300.0*cm);
    yout = RandFlat::shoot(25.0*cm,300.0*cm);
    zout = RandFlat::shoot(25.0*cm,300.0*cm);
    
    xin  = RandFlat::shoot(-10.0*cm,10.0*cm);
    yin  = RandFlat::shoot(-10.0*cm,10.0*cm);
    zin  = RandFlat::shoot(-10.0*cm,10.0*cm);
    
    pin  = (xin, yin, zin);
    pout = (xout, yout, zout);
    
    dir  = pin - pout;
    dir /= dir.mag();

    dist1 = t1.DistanceToIn(pout,dir) ;
    dist2 = t2.DistanceToIn(pout,dir) ;
  
    if(dist1 != kInfinity && dist2 != kInfinity)
    {
      if(std::fabs(dist1 - dist2) >= 5.*kCarTolerance)
      { 
	what = false; 
	dist = std::fabs(dist1 - dist2);
	logErrors(pout.x(),pout.y(), pout.z(),
		  dir.x() ,dir.y() , dir.z() ,dist);
	n++;
      }
    }

    dist1 = t1.DistanceToOut(pin,dir);
    dist2 = t2.DistanceToOut(pin,dir);
 
    if(dist1 != kInfinity && dist2 !=kInfinity)
    {
      if(std::fabs(dist1 - dist2) >= 5.*kCarTolerance)
      {
	what = false; 
	dist = std::fabs(dist1 - dist2);
	logErrors(pin.x(), pin.y(), pin.z(),
		  dir.x(), dir.y(), dir.z(), dist);
	n++;
      }
    }
  }
  G4cout <<"The number of inconsistencies when comparing Sphere to Orb were: "<<n<<"."<<G4endl;
  return what;
}


// other comparisons can be, trap with polyhedra, cube with polyhedra, tet with polyhedra

int main()
{

  G4bool what;

  what = compareEllipsoidtoOrb(1000);
  what = compareEllipticalTubetoTubs(1000);
  what = compareEllipsoidtoSphere(1000);
  what = compareBoxtoTrap(1000);
  what = compareSpheretoOrb(1000);
  return 0;
}
