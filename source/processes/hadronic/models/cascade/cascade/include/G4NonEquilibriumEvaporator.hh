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
// $Id: G4NonEquilibriumEvaporator.hh,v 1.8 2010-03-19 05:03:23 mkelsey Exp $
// GEANT4 tag: $Name: not supported by cvs2svn $
//
// 20100315  M. Kelsey -- Remove "using" directive and unnecessary #includes.

#ifndef G4NON_EQUILIBRIUM_EVAPORATOR_HH
#define G4NON_EQUILIBRIUM_EVAPORATOR_HH

#include "G4CollisionOutput.hh"

class G4InuclParticle;

class G4NonEquilibriumEvaporator {
public:
  G4NonEquilibriumEvaporator();

  G4CollisionOutput collide(G4InuclParticle* bullet,
			    G4InuclParticle* target);

private: 
  G4int verboseLevel;

  G4double getMatrixElement(G4double A) const;
  G4double getE0(G4double A) const; 
  G4double getParLev(G4double A, G4double Z) const;
};

#endif // G4NON_EQUILIBRIUM_EVAPORATOR_HH 
