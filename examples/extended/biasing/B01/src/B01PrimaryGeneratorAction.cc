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
// $Id: B01PrimaryGeneratorAction.cc,v 1.6 2002-10-22 14:09:05 dressel Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

#include "globals.hh"

#include "B01PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4Neutron.hh"
#include "G4Gamma.hh"
#include "G4Proton.hh"
#include "G4ThreeVector.hh"

B01PrimaryGeneratorAction::B01PrimaryGeneratorAction()
  :
  fParticleGun(new G4ParticleGun(1))
{
  if (!fParticleGun) {
    G4std::G4Exception("B01PrimaryGeneratorAction::B01PrimaryGeneratorAction: new failed to create G4ParticleGun!");
  }
  fParticleGun->SetParticleDefinition(G4Neutron::NeutronDefinition());
  //  fParticleGun->SetParticleDefinition(G4Gamma::GammaDefinition());
  //  fParticleGun->SetParticleDefinition(G4Proton::ProtonDefinition());
  fParticleGun->SetParticleEnergy(10.0*MeV);
  fParticleGun->SetParticlePosition(G4ThreeVector(0.0, 0.0, -90.0005*cm));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
}

B01PrimaryGeneratorAction::~B01PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void B01PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  fParticleGun->GeneratePrimaryVertex(anEvent);
}
