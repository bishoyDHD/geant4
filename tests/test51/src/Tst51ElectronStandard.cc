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
// $Id: Tst51ElectronStandard.cc,v 1.1 2005-07-05 11:06:27 guatelli Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Author: Maria.Grazia.Pia@cern.ch
//
// History:
// -----------
// 22 Feb 2003 MGP          Designed for modular Physics List
//
// -------------------------------------------------------------------

#include "Tst51ElectronStandard.hh"

#include "G4ProcessManager.hh"
#include "G4Gamma.hh"
#include "G4ParticleDefinition.hh"
#include "G4MultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4StepLimiter.hh"
#include "G4eBremsstrahlung52.hh"
Tst51ElectronStandard::Tst51ElectronStandard(const G4String& name): G4VPhysicsConstructor(name)
{ }

Tst51ElectronStandard::~Tst51ElectronStandard()
{ }

void Tst51ElectronStandard::ConstructProcess()
{
  // Add standard processes for electrons
  
  theParticleIterator->reset();

  while( (*theParticleIterator)() )
    {
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* manager = particle->GetProcessManager();
      G4String particleName = particle->GetParticleName();
     
      if (particleName == "e-") 
	{
          // Physics list for preliminary study
	  manager->AddProcess(new G4MultipleScattering, -1, 1,1);
	  manager->AddProcess(new G4eIonisation,        -1, 2,2);
	   manager->AddProcess(new G4eBremsstrahlung,    -1,-1,3); 
          //manager->AddProcess(new G4StepLimiter(),      -1,-1, 3);
	  /*
          manager->AddProcess(new G4MultipleScattering, -1, 1,1);
	  manager->AddProcess(new G4eIonisation,        -1, 2,2);
	  manager->AddProcess(new G4eBremsstrahlung52,    -1,-1,3); 
	  */
	}   
    }
}
