#include "G4RunManager.hh" 
#include "G4UImanager.hh" 
#include "RandomCaloDetectorConstruction.hh" 
#include "LHEP.hh" 
//#include "LHEP_HP.hh" 
#include "QGSP.hh" 
#include "QGSP_HP.hh" 
#include "QGSP_BERT.hh" 
#include "QGSP_BIC.hh" 
#include "QGSC.hh" 
#include "QGSP_BERT_HP.hh" 
#include "QGSP_EMV.hh"
// #include "QGSP_EMX.hh"
#include "RandomCaloPrimaryGeneratorAction.hh" 
#include "RandomCaloEventAction.hh" 
#include "RandomCaloRunAction.hh" 
#include "RandomCaloStackingAction.hh" 
#include "G4UIterminal.hh" 
#ifdef G4UI_USE_TCSH 
  #include "G4UItcsh.hh" 
#endif 
#ifdef G4VIS_USE 
  #include "RandomCaloVisManager.hh" 
#endif 
#ifdef G4UI_USE_XM 
#include "G4UIXm.hh" 
#endif 
#include "CLHEP/Random/RanluxEngine.h" 
int main(int argc,char** argv) { 
  CLHEP::RanluxEngine defaultEngine( 1234567, 4 ); 
  CLHEP::HepRandom::setTheEngine( &defaultEngine ); 
  G4int seed = time( NULL ); 
  CLHEP::HepRandom::setTheSeed( seed ); 
  G4cout << G4endl 
         << " ===================================================== " << G4endl 
         << " Initial seed = " << seed << G4endl 
	 << " ===================================================== " << G4endl 
	 << G4endl; 
  G4RunManager* runManager = new G4RunManager; 
  runManager->SetUserInitialization( new RandomCaloDetectorConstruction ); 
  QGSP *thePL = new QGSP;    //***LOOKHERE*** 
  //thePL->SetDefaultCutValue( 0.020 *mm ); // 20 microns 
  runManager->SetUserInitialization( thePL ); 
  runManager->SetUserAction( new RandomCaloPrimaryGeneratorAction ); 
  runManager->SetUserAction( new RandomCaloRunAction ); 
  runManager->SetUserAction( new RandomCaloEventAction ); 
  //runManager->SetUserAction( new RandomCaloStackingAction ); 
#ifdef G4VIS_USE 
  RandomCaloVisManager *visManager = new RandomCaloVisManager; 
  visManager->Initialize(); 
#endif 
  runManager->Initialize(); 
  G4UImanager* UI = G4UImanager::GetUIpointer(); 
  if ( argc==1 ) {   // Define UI session for interactive mode. 
    G4UIsession* session = 0; 
#ifdef G4UI_USE_XM 
    session = new G4UIXm(argc,argv); 
#else 
#ifdef G4UI_USE_TCSH 
    session = new G4UIterminal(new G4UItcsh); 
#else 
    session = new G4UIterminal(); 
#endif 
#endif 
#ifdef G4VIS_USE 
    // Create empty scene 
    G4String visCommand = "/vis/scene/create"; 
    UI->ApplyCommand(visCommand); 
    // Choose one default viewer (you can always change it later on) 
#ifdef WIN32 
    visCommand = "/vis/open VRML2FILE"; 
#else 
    // visCommand = "/vis/open VRML2"; 
    visCommand = "/vis/open OGLIX"; 
#endif 
    UI->ApplyCommand(visCommand); 
    visCommand = "/vis/viewer/flush"; 
    UI->ApplyCommand(visCommand); 
    visCommand = "/tracking/storeTrajectory 1"; 
    UI->ApplyCommand(visCommand); 
#endif 
#ifdef G4UI_USE_XM 
    // Customize the G4UIXm menubar with a macro file : 
    UI->ApplyCommand("/control/execute gui.g4"); 
#else 
    G4cout << "Now, please, apply beamOn command..." << G4endl; 
#endif 
    session->SessionStart(); 
    delete session; 
  } else {   // Batch mode 
    G4String command = "/control/execute "; 
    G4String fileName = argv[1]; 
    UI->ApplyCommand(command+fileName); 
  } 
  G4cout << G4endl 
	 << " ===================================================== " << G4endl 
         << " Final random number = "          << CLHEP::HepRandom::getTheEngine()->flat() << G4endl 
	 << " ===================================================== " << G4endl 
         << G4endl; 
  // job termination 
#ifdef G4VIS_USE 
  delete visManager; 
#endif 
  delete runManager; 
  return 0; 
} 
