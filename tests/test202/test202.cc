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
// $Id$
//

// Usage: test202 [macro-file]
// Typically: test202  (executes vis.mac and enters interactive session)
//        or: test202 run0.mac (executes run0.mac, enters interactive session)

#include "globals.hh"

#include "QGSP_BERT.hh"
#include "Tst202DetectorConstruction.hh"
#include "Tst202PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

int main (int argc, char** argv) {

  // Run manager
  G4cout << "RunManager is constructing...." << G4endl;
  G4RunManager * runManager = new G4RunManager;

  // mandatory initialization classes
  runManager -> SetUserInitialization (new Tst202DetectorConstruction);
  runManager -> SetUserInitialization (new QGSP_BERT);

  // User Action classes.
  runManager -> SetUserAction (new Tst202PrimaryGeneratorAction);

  // Construct geometry
  G4cout << "Constructing geometry..." << G4endl;
  runManager -> Initialize();

#ifdef G4VIS_USE
  G4VisManager* visManager = new G4VisExecutive;
  //G4VisManager* visManager = new G4VisExecutive("quiet");
  visManager -> Initialize ();
#endif

  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  G4String controlExecute = "/control/execute ";
  if(argc>1){
    // execute an argument macro file if exist
    G4String fileName = argv[1];
    UImanager->ApplyCommand(controlExecute + fileName);
  } else {
    // Start UI session
#ifdef G4UI_USE
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
    UImanager->ApplyCommand("/control/execute vis.mac");     
#endif
    ui->SessionStart();
    delete ui;
#endif
  }

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager; // Should be last.

  return 0;
}
