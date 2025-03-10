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
// G4TrackingMessenger class implementation
//
// Contact:
//   Questions and comments to this code should be sent to
//     Katsuya Amako  (e-mail: Katsuya.Amako@kek.jp)
//     Makoto  Asai   (e-mail: asai@slac.stanford.edu)
//     Takashi Sasaki (e-mail: Takashi.Sasaki@kek.jp)
// --------------------------------------------------------------------

#include "G4TrackingMessenger.hh"

#include "G4IdentityTrajectoryFilter.hh"
#include "G4PropagatorInField.hh"
#include "G4SteppingManager.hh"
#include "G4TrackStatus.hh"
#include "G4TrackingManager.hh"
#include "G4TransportationManager.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIdirectory.hh"
#include "G4UImanager.hh"
#include "G4Threading.hh"
#include "G4ClonedTrajectory.hh"
#include "G4ClonedTrajectoryPoint.hh"
#include "G4ClonedRichTrajectory.hh"
#include "G4ClonedRichTrajectoryPoint.hh"
#include "G4ClonedSmoothTrajectory.hh"
#include "G4ClonedSmoothTrajectoryPoint.hh"
#include "G4ios.hh"
#include "globals.hh"

///////////////////////////////////////////////////////////////////
G4TrackingMessenger::G4TrackingMessenger(G4TrackingManager* trMan)
  ///////////////////////////////////////////////////////////////////
  : trackingManager(trMan)
{
  steppingManager = trackingManager->GetSteppingManager();

  TrackingDirectory = new G4UIdirectory("/tracking/");
  TrackingDirectory->SetGuidance("TrackingManager and SteppingManager control commands.");

  AbortCmd = new G4UIcmdWithoutParameter("/tracking/abort", this);
  AbortCmd->SetGuidance("Abort current G4Track processing.");

  ResumeCmd = new G4UIcmdWithoutParameter("/tracking/resume", this);
  ResumeCmd->SetGuidance("Resume current G4Track processing.");

  StoreTrajectoryCmd = new G4UIcmdWithAnInteger("/tracking/storeTrajectory", this);
  StoreTrajectoryCmd->SetGuidance("Store trajectories or not.");
  StoreTrajectoryCmd->SetGuidance(" 0 : Don't Store trajectories.");
  StoreTrajectoryCmd->SetGuidance(" !=0 : Store trajectories.");
  StoreTrajectoryCmd->SetGuidance(" 1 : Choose G4Trajectory as default.");
  StoreTrajectoryCmd->SetGuidance(" 2 : Choose G4SmoothTrajectory as default.");
  StoreTrajectoryCmd->SetGuidance(" 3 : Choose G4RichTrajectory as default.");
  StoreTrajectoryCmd->SetGuidance(" 4 : Choose G4RichTrajectory with auxiliary points as default.");
  StoreTrajectoryCmd->SetParameterName("Store", true);
  StoreTrajectoryCmd->SetDefaultValue(0);
  StoreTrajectoryCmd->SetRange("Store >=0 && Store <= 4");

  VerboseCmd = new G4UIcmdWithAnInteger("/tracking/verbose", this);
#ifdef G4VERBOSE
  VerboseCmd->SetGuidance("Set Verbose level of tracking category.");
  VerboseCmd->SetGuidance(" -1 : Silent.");
  VerboseCmd->SetGuidance(" 0 : Silent.");
  VerboseCmd->SetGuidance(" 1 : Minimum information of each Step.");
  VerboseCmd->SetGuidance(" 2 : Addition to Level=1, info of secondary particles.");
  VerboseCmd->SetGuidance(" 3 : Addition to Level=1, pre/postStepoint information");
  VerboseCmd->SetGuidance("     after all AlongStep/PostStep process executions.");
  VerboseCmd->SetGuidance(" 4 : Addition to Level=3, pre/postStepoint information");
  VerboseCmd->SetGuidance("     at each AlongStepPostStep process execution.");
  VerboseCmd->SetGuidance(" 5 : Addition to Level=4, proposed Step length information");
  VerboseCmd->SetGuidance("     from each AlongStepPostStep process.");
  VerboseCmd->SetParameterName("verbose_level", true);
  VerboseCmd->SetDefaultValue(0);
  VerboseCmd->SetRange("verbose_level >=-1  ");
#else
  VerboseCmd->SetGuidance("You need to recompile the tracking category defining G4VERBOSE ");
#endif
}

////////////////////////////////////////////
G4TrackingMessenger::~G4TrackingMessenger()
////////////////////////////////////////////
{
  delete TrackingDirectory;
  delete AbortCmd;
  delete ResumeCmd;
  delete StoreTrajectoryCmd;
  delete VerboseCmd;
  delete auxiliaryPointsFilter;
}

///////////////////////////////////////////////////////////////////////////////
void G4TrackingMessenger::SetNewValue(G4UIcommand* command, G4String newValues)
///////////////////////////////////////////////////////////////////////////////
{
  if (command == VerboseCmd) {
    trackingManager->SetVerboseLevel(VerboseCmd->ConvertToInt(newValues));
  }

  if (command == AbortCmd) {
    steppingManager->GetTrack()->SetTrackStatus(fStopAndKill);
    G4UImanager::GetUIpointer()->ApplyCommand("/control/exit");
  }

  if (command == ResumeCmd) {
    G4UImanager::GetUIpointer()->ApplyCommand("/control/exit");
  }

  if (command == StoreTrajectoryCmd) {
    G4int trajType = StoreTrajectoryCmd->ConvertToInt(newValues);
    if (trajType == 2 || trajType == 4) {

      if (nullptr == auxiliaryPointsFilter) {
	auxiliaryPointsFilter = new G4IdentityTrajectoryFilter;
      }
      G4TransportationManager::GetTransportationManager()
        ->GetPropagatorInField()
        ->SetTrajectoryFilter(auxiliaryPointsFilter);
    }
    else {
      G4TransportationManager::GetTransportationManager()
        ->GetPropagatorInField()
        ->SetTrajectoryFilter(nullptr);
    }
    trackingManager->SetStoreTrajectory(trajType);

    // Make sure cloning works for sub-event parallel mode
    if(G4Threading::IsMasterThread() && trajType>0) {
      static G4bool traj_1 = false, traj_2 = false, traj_3 = false;
      G4VTrajectory* traj = nullptr;
      G4VTrajectoryPoint* trajp = nullptr;
      switch (trajType) {
       case 1:
        if(!traj_1) {
          traj = new G4ClonedTrajectory();
          trajp = new G4ClonedTrajectoryPoint();
          traj_1 = true;
        }
        break;
       case 2:
        if(!traj_2) {
          traj = new G4ClonedSmoothTrajectory();
          trajp = new G4ClonedSmoothTrajectoryPoint();
          traj_2 = true;
        }
        break;
       case 3:
       case 4:
        if(!traj_3) {
          traj = new G4ClonedRichTrajectory();
          trajp = new G4ClonedRichTrajectoryPoint();
          traj_3 = true;
        }
        break;
       default:
        break;
      }
      if(traj!=nullptr) delete traj;
      if(trajp!=nullptr) delete trajp;
    }
  }

}

////////////////////////////////////////////////////////////////////
G4String G4TrackingMessenger::GetCurrentValue(G4UIcommand* command)
////////////////////////////////////////////////////////////////////
{
  if (command == VerboseCmd) {
    return VerboseCmd->ConvertToString(trackingManager->GetVerboseLevel());
  }
  if (command == StoreTrajectoryCmd) {
    return StoreTrajectoryCmd->ConvertToString(trackingManager->GetStoreTrajectory());
  }
  return G4String(1, '\0');
}
