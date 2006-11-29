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
// $Id: test203.cc,v 1.1 2006-11-29 18:04:30 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

// Usage: test202 [dumb|tcsh|Xm|Xaw|GAG] [vis-verbosity]
// Typically: test202 tcsh 4
// See help /vis/verbose.

#include "globals.hh"

#include "GmGeometryFromText.hh"
#include "QGSP.hh"
//#include "ExN04RunAction.hh"
#include "ExN04PrimaryGeneratorAction.hh"
//#include "ExN04EventAction.hh"
//#include "ExN04SteppingAction.hh"
#include "G4UIsession.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#ifdef G4UI_USE_GAG
  #include "G4UIGAG.hh"
#endif
#ifdef G4UI_USE_XM
  #include "G4UIXm.hh"
#endif
#ifdef G4UI_USE_XAW
  #include "G4UIXaw.hh"
#endif
#ifdef G4UI_USE_WIN32
  #include "G4UIWin32.hh"
#endif

#include "G4RunManager.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE_WIN32
#include <windows.h>
int WINAPI WinMain (
HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int nCmdShow) {
#else
int main (int argc, char** argv) {
#endif

  G4int Verbose = 0;
#ifndef G4UI_USE_WIN32
  if ((argc >= 3)) Verbose = atoi (argv[2]);
#endif

  // Choose (G)UI.
  G4UIsession* session;
#ifdef G4UI_USE_WIN32
  session = new G4UIWin32 (hInstance,hPrevInstance,lpszCmdLine,nCmdShow);
#else
  if (argc >= 2) {
    if (strcmp (argv[1], "dumb")==0)     session = new G4UIterminal;
    else if (strcmp (argv[1], "tcsh")==0)
                            session = new G4UIterminal(new G4UItcsh);
#ifdef G4UI_USE_XM
    else if (strcmp (argv[1], "Xm")==0)  session = new G4UIXm (argc, argv);
#endif
#ifdef G4UI_USE_XAW
    else if (strcmp (argv[1], "Xaw")==0) session = new G4UIXaw (argc, argv);
#endif
#ifdef G4UI_USE_GAG
    else if (strcmp (argv[1], "gag")==0) session = new G4UIGAG ;
#endif
#ifdef G4UI_USE_GAG
    else                                 session = new G4UIGAG;
#else
    else                                 session = new G4UIterminal;
#endif
  } else {
#ifdef G4UI_USE_GAG
    session = new G4UIGAG;
#else
    session = new G4UIterminal;
#endif
  }
#endif
  G4UImanager::GetUIpointer()->SetSession(session);  //So that Pause works..

  // Run manager
  G4cout << "RunManager is constructing...." << G4endl;
  G4RunManager * runManager = new G4RunManager;

  // mandatory initialization classes
  runManager -> SetUserInitialization (new GmGeometryFromText);
  runManager -> SetUserInitialization (new QGSP);

  // User Action classes.
  runManager -> SetUserAction (new ExN04PrimaryGeneratorAction);
  //runManager -> SetUserAction (new ExN04RunAction);
  //runManager -> SetUserAction (new ExN04EventAction);
  //runManager -> SetUserAction (new ExN04StackingAction);
  //runManager -> SetUserAction (new ExN04TrackingAction);
  //runManager -> SetUserAction (new ExN04SteppingAction);

  // Construct geometry
  G4cout << "Constructing geometry.  Please be patient..." << G4endl;
  runManager -> Initialize();

#ifdef G4VIS_USE
  G4VisManager* visManager = new G4VisExecutive;
  visManager -> SetVerboseLevel (Verbose);
  visManager -> Initialize ();
#endif

  if(argc>1)
  // execute an argument macro file if exist
  {
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else
  // start interactive session
  {
#ifdef G4UI_USE_TCSH
    G4UIsession* session = new G4UIterminal(new G4UItcsh);
#else
    G4UIsession* session = new G4UIterminal();
#endif
    session->SessionStart();
    delete session;
  }

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete session;
  delete runManager; // Should be last.

  return 0;
}
