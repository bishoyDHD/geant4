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
//
// --------------------------------------------------------------
//                 GEANT 4 - ULTRA experiment example
// --------------------------------------------------------------
//
// Code developed by:
// B. Tome, M.C. Espirito-Santo, A. Trindade, P. Rodrigues 
//
//   **********************************************
//   *        UltraDetectorConstruction.hh
//   **********************************************
//
//    Class used in the definition of the Ultra setup consisting of:
//      - the UVscope detector
//      - an optional reflecting surface
//    Optical photons can reach the UVscope either directly or after reflection in the
//    surface, which can be polished or diffusing.
//    The main part of the UVscope definition is the Fresnel lens construction based
//    on the UltraFresnelLens class.
//
#ifndef UltraDetectorConstruction_h
#define UltraDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"


class G4VPhysicalVolume;
class G4LogicalVolume;
class G4SDManager;
class UltraScintSD;
class UltraPMTSD;
class UltraFresnelLens;

class UltraDetectorConstruction : public G4VUserDetectorConstruction
{
 
  friend class UltraPrimaryGeneratorAction ;

  public:
    UltraDetectorConstruction();
    ~UltraDetectorConstruction();

  public:
    G4VPhysicalVolume* Construct();

  private:
    
  // Methods to build ULTRA
    G4VPhysicalVolume  *ConstructUVscope(G4VPhysicalVolume *);
    G4VPhysicalVolume  *ConstructMirror(G4VPhysicalVolume *);
    G4VPhysicalVolume  *ConstructGround(G4VPhysicalVolume *);
    UltraFresnelLens   *FresnelLens ;

    // Material definitions
    void ConstructTableMaterials();


  private:
    UltraPMTSD*   PMTSD  ;          //pointer to the photomultiplier sensitive detector
    G4SDManager*  SDmanager ;       // Sensitive Detector Manager
    
    G4double lambda_min ;
    G4double lambda_max ;
};

#endif 
