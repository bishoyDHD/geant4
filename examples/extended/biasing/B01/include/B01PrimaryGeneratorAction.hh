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
// $Id: B01PrimaryGeneratorAction.hh,v 1.3 2002-10-22 14:09:03 dressel Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

#ifndef B01PrimaryGeneratorAction_hh
#define B01PrimaryGeneratorAction_hh B01PrimaryGeneratorAction_hh 

#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;
class G4Event;

class B01PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  B01PrimaryGeneratorAction();
  virtual ~B01PrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event* anEvent);

private:
  B01PrimaryGeneratorAction(const B01PrimaryGeneratorAction &);
  B01PrimaryGeneratorAction &operator=(const B01PrimaryGeneratorAction &);
  G4ParticleGun* fParticleGun;
};

#endif
