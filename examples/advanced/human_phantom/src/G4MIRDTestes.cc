//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
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
#include "G4MIRDTestes.hh"

#include "globals.hh"
#include "G4SDManager.hh"
#include "G4VisAttributes.hh"

G4MIRDTestes::G4MIRDTestes()
{
}

G4MIRDTestes::~G4MIRDTestes()
{
}

G4VPhysicalVolume* G4MIRDTestes::ConstructTestes(G4VPhysicalVolume* mother, G4String sex, G4bool sensitivity)
{
  // Initialize GDML Processor
  parser.Read( "gdmlData/"+sex+"/MIRDTestes.gdml" ); 

  G4LogicalVolume* logicTestes = parser.GetVolume("TestesVolume");

  G4ThreeVector position = parser.GetPosition("TestesPos");
  G4ThreeVector rot = parser.GetRotation("TestesRot");
  G4RotationMatrix* rm = new G4RotationMatrix();
  rm->rotateX(rot.x()); rm->rotateY(rot.y()); rm->rotateZ(rot.z()); 
  
  // Define rotation and position here!
  G4VPhysicalVolume* physTestes = new G4PVPlacement(rm,position,
      			       "physicalTestes",
  			       logicTestes,
			       mother,
			       false,
			       0);

  // Sensitive Body Part
  if (sensitivity==true)
  { 
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    logicTestes->SetSensitiveDetector( SDman->FindSensitiveDetector("BodyPartSD") );
  }

  // Visualization Attributes
  G4VisAttributes* TestesVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  TestesVisAtt->SetForceSolid(true);
  logicTestes->SetVisAttributes(TestesVisAtt);

  G4cout << "Testes created !!!!!!" << G4endl;
  
  return physTestes;
}
