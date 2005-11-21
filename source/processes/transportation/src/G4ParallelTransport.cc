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
// $Id: G4ParallelTransport.cc,v 1.14 2005-11-21 21:46:53 asaim Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// ----------------------------------------------------------------------
// GEANT 4 class source file
//
// G4ParallelTransport.cc
//
// ----------------------------------------------------------------------

#include "G4Types.hh"
#include "G4ParallelTransport.hh"
#include "G4VPGeoDriver.hh"
#include "G4VParallelStepper.hh"
#include <sstream>

G4ParallelTransport::G4ParallelTransport(G4VPGeoDriver &pgeodriver,
                                         G4VParallelStepper &aStepper,
                                         const G4String &aName)
 : G4VProcess(aName), 
   fParticleChange(new G4ParticleChange),
   fPgeodriver(pgeodriver),
   fPStepper(aStepper),
   fCrossBoundary(false),
   fInitStep(false)
{
  if (!fParticleChange)
  {
    G4Exception("G4ParallelTransport::G4ParallelTransport()",
                "FatalError", FatalException,
                "Failed to allocate G4ParticleChange !");
  }
  G4VProcess::pParticleChange = fParticleChange;
}

G4ParallelTransport::~G4ParallelTransport()
{
  delete fParticleChange;
}

void G4ParallelTransport::StartTracking(G4Track*)
{
  fInitStep = true;
}

void G4ParallelTransport::EndTracking()
{
}

G4double G4ParallelTransport::
PostStepGetPhysicalInteractionLength(const G4Track& aTrack,
                                     G4double ,
                                     G4ForceCondition* condition)
{
  G4double stepLength = 0;

  // if this function is called on a new track let the navigator 
  // know about it
  // G4bool initStep = (aTrack.GetCurrentStepNumber() == 1);

  if (fInitStep)
  {
    fInitStep = false;
    stepLength = fPgeodriver.
      ComputeStepLengthInit(aTrack.GetPosition(),
                            aTrack.GetMomentumDirection());
    fPStepper.Init(fPgeodriver.GetCurrentGeometryCell());
    fCrossBoundary = false;
  }
  else if (fCrossBoundary)
  {
    stepLength = fPgeodriver.
      ComputeStepLengthCrossBoundary(aTrack.GetPosition(),
                                     aTrack.GetMomentumDirection());
    fPStepper.UnSetCrossBoundary();
    fCrossBoundary = false;
  }
  else
  {
    stepLength = fPgeodriver.
      ComputeStepLengthInVolume(aTrack.GetPosition(),
                                aTrack.GetMomentumDirection());
  }
  
  *condition = NotForced;
  return stepLength;
}

G4VParticleChange * 
G4ParallelTransport::PostStepDoIt(const G4Track& aTrack,
                                  const G4Step& aStep)
{
  if (!(aStep.GetStepLength() > 0.))
  {
    std::ostringstream os;
    os << "G4PArallelTransport::InitPostDoIt: StepLength() == 0.\n"
       << "pos: " << aTrack.GetPosition() << ", " 
       << "dir: " << aTrack.GetMomentumDirection() << "\n";
    G4String m = os.str();
    Warning(m);
  }
  fParticleChange->Initialize(aTrack);

  fCrossBoundary = true;
  fPgeodriver.LocateOnBoundary(aTrack.GetPosition(), 
                               aTrack.GetMomentumDirection());
  fPStepper.Update(fPgeodriver.GetCurrentGeometryCell());

  return fParticleChange;
}
    
void G4ParallelTransport::Error(const G4String &m)
{
  G4Exception("G4ParallelTransport::Error()",
              "ProgramError", FatalException, m);
}

void G4ParallelTransport::Warning(const G4String &m)
{
  G4Exception("G4ParallelTransport::Warning()",
              "ProgramWarning", JustWarning, m);
}

G4double G4ParallelTransport::
AlongStepGetPhysicalInteractionLength(const G4Track&,
                                      G4double  ,
                                      G4double  ,
                                      G4double& ,
                                      G4GPILSelection*)
{
  return -1.0;
}

G4double G4ParallelTransport::
AtRestGetPhysicalInteractionLength(const G4Track&, G4ForceCondition*)
{
  return -1.0;
}
  
G4VParticleChange*
G4ParallelTransport::AtRestDoIt(const G4Track&, const G4Step&)
{
  return 0;
}
  
G4VParticleChange* G4ParallelTransport::
AlongStepDoIt(const G4Track&, const G4Step&)
{
  return 0;
}
  
 
