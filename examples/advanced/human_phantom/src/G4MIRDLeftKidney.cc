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
// Authors: S. Guatelli and M. G. Pia, INFN Genova, Italy
// 
// Based on code developed by the undergraduate student G. Guerrieri 
// Note: this is a preliminary beta-version of the code; an improved 
// version will be distributed in the next Geant4 public release, compliant
// with the design in a forthcoming publication, and subject to a 
// design and code review.
//

#include "G4MIRDLeftKidney.hh"

#include "globals.hh"
#include "G4SDManager.hh"
#include "G4VisAttributes.hh"
#include "G4HumanPhantomMaterial.hh"
#include "G4SDManager.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4Ellipsoid.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4Material.hh"
#include "G4EllipticalTube.hh"
#include "G4Box.hh"
#include "G4UnionSolid.hh"
#include "G4HumanPhantomColour.hh"
G4MIRDLeftKidney::G4MIRDLeftKidney()
{
}

G4MIRDLeftKidney::~G4MIRDLeftKidney()
{
}

G4VPhysicalVolume* G4MIRDLeftKidney::Construct(const G4String& volumeName,G4VPhysicalVolume* mother,
						    const G4String& colourName, G4bool wireFrame, G4bool sensitivity)
{
  G4cout << "Construct "<< volumeName << G4endl;
 
 G4HumanPhantomMaterial* material = new G4HumanPhantomMaterial();
 G4Material* soft = material -> GetMaterial("soft_tissue");
 delete material;
 
 G4double ax= 4.5 *cm; //a
 G4double by= 1.5 *cm; //b
 G4double cz= 5.5 *cm; //c
 
 G4VSolid* oneLeftKidney = new G4Ellipsoid("OneLeftKidney",ax, by, cz); 
 
 G4double xx = 6. * cm; 
 G4double yy = 12.00*cm; 
 G4double zz = 12.00*cm;
 G4VSolid* subtrLeftKidney = new G4Box("SubtrLeftKidney",xx/2., yy/2., zz/2.);
 
 G4SubtractionSolid* leftKidney = new G4SubtractionSolid("LeftKidney",
						     oneLeftKidney,
						     subtrLeftKidney,
						     0, 
						     G4ThreeVector(-6. *cm, // x0
								   0.0 *cm,
								   0.0 * cm));

  G4LogicalVolume* logicLeftKidney = new G4LogicalVolume(leftKidney,
						     soft,
						     "logical" + volumeName,
						     0, 0, 0);

  G4VPhysicalVolume* physLeftKidney = new G4PVPlacement(0 ,G4ThreeVector(6.*cm,  // xo
								     6. *cm, //yo
								     -2.50 *cm),//zo
  			       "physicalLeftKidney", logicLeftKidney,
			       mother,
			       false,
			       0, true);

  // Sensitive Body Part
  if (sensitivity==true)
  { 
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    logicLeftKidney->SetSensitiveDetector( SDman->FindSensitiveDetector("BodyPartSD") );
  }

  // Visualization Attributes
  //  G4VisAttributes* LeftKidneyVisAtt = new G4VisAttributes(G4Colour(0.72,0.52,0.04));
  G4HumanPhantomColour* colourPointer = new G4HumanPhantomColour();
  G4Colour colour = colourPointer -> GetColour(colourName);
  G4VisAttributes* LeftKidneyVisAtt = new G4VisAttributes(colour);
  LeftKidneyVisAtt->SetForceSolid(wireFrame);
  logicLeftKidney->SetVisAttributes(LeftKidneyVisAtt);

  G4cout << "Left LeftKidney created !!!!!!" << G4endl;

  // Testing LeftKidney Volume
  G4double LeftKidneyVol = logicLeftKidney->GetSolid()->GetCubicVolume();
  G4cout << "Volume of LeftKidney = " << LeftKidneyVol/cm3 << " cm^3" << G4endl;
  
  // Testing LeftKidney Material
  G4String LeftKidneyMat = logicLeftKidney->GetMaterial()->GetName();
  G4cout << "Material of LeftKidney = " << LeftKidneyMat << G4endl;
  
  // Testing Density
  G4double LeftKidneyDensity = logicLeftKidney->GetMaterial()->GetDensity();
  G4cout << "Density of Material = " << LeftKidneyDensity*cm3/g << " g/cm^3" << G4endl;

  // Testing Mass
  G4double LeftKidneyMass = (LeftKidneyVol)*LeftKidneyDensity;
  G4cout << "Mass of LeftKidney = " << LeftKidneyMass/gram << " g" << G4endl;

  
  return physLeftKidney;
}
