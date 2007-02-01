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
#include "G4MIRDLeftOvary.hh"
#include "G4SDManager.hh"

#include "globals.hh"

#include "G4VisAttributes.hh"
#include "G4Ellipsoid.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4HumanPhantomMaterial.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UnionSolid.hh"
#include "G4HumanPhantomColour.hh"
G4MIRDLeftOvary::G4MIRDLeftOvary()
{
}

G4MIRDLeftOvary::~G4MIRDLeftOvary()
{

}

G4VPhysicalVolume* G4MIRDLeftOvary::Construct(const G4String& volumeName,G4VPhysicalVolume* mother, 
						   const G4String& colourName, G4bool wireFrame, G4bool sensitivity)
{ 
  G4cout << "Construct "<< volumeName  << G4endl;
 
 G4HumanPhantomMaterial* material = new G4HumanPhantomMaterial();
 G4Material* soft = material -> GetMaterial("soft_tissue");
 delete material;
 
 G4double ax= 1. *cm;
 G4double by= 0.5*cm;
 G4double cz= 2.*cm;

 G4Ellipsoid* OneOvary = new G4Ellipsoid("OneOvary",
					 ax, by, cz);

 G4UnionSolid* Ovary = new G4UnionSolid("Ovary",  OneOvary,
					OneOvary,0,
					G4ThreeVector(12.*cm, 
						      0.0*cm,
						      0.0*cm));


  G4LogicalVolume* logicLeftOvary = new G4LogicalVolume(Ovary,
						    soft,
						    "logical" + volumeName,
						    0, 0, 0);
  
  // Define rotation and position here!
  G4VPhysicalVolume* physLeftOvary = new G4PVPlacement(0,
			       G4ThreeVector(-6. *cm,0.0*cm, -20*cm),
      			       "physicalLeftOvary",
  			       logicLeftOvary,
			       mother,
			       false,
			       0, true);

  // Sensitive Body Part
  if (sensitivity==true)
  { 
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    logicLeftOvary->SetSensitiveDetector( SDman->FindSensitiveDetector("BodyPartSD") );
  }

  // Visualization Attributes
  //G4VisAttributes* LeftOvaryVisAtt = new G4VisAttributes(G4Colour(0.85,0.44,0.84));
  G4HumanPhantomColour* colourPointer = new G4HumanPhantomColour();
  G4Colour colour = colourPointer -> GetColour(colourName);
  G4VisAttributes* LeftOvaryVisAtt = new G4VisAttributes(colour);
  LeftOvaryVisAtt->SetForceSolid(wireFrame);
  logicLeftOvary->SetVisAttributes(LeftOvaryVisAtt);

  G4cout << "LeftOvary created !!!!!!" << G4endl;

  // Testing LeftOvary Volume
  G4double LeftOvaryVol = logicLeftOvary->GetSolid()->GetCubicVolume();
  G4cout << "Volume of LeftOvary = " << LeftOvaryVol/cm3 << " cm^3" << G4endl;
  
  // Testing LeftOvary Material
  G4String LeftOvaryMat = logicLeftOvary->GetMaterial()->GetName();
  G4cout << "Material of LeftOvary = " << LeftOvaryMat << G4endl;
  
  // Testing Density
  G4double LeftOvaryDensity = logicLeftOvary->GetMaterial()->GetDensity();
  G4cout << "Density of Material = " << LeftOvaryDensity*cm3/g << " g/cm^3" << G4endl;

  // Testing Mass
  G4double LeftOvaryMass = (LeftOvaryVol)*LeftOvaryDensity;
  G4cout << "Mass of LeftOvary = " << LeftOvaryMass/gram << " g" << G4endl;
  
  return physLeftOvary;
}
