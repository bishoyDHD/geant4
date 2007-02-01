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
// Authors: S. Guatelli and M. G. Pia, INFN Genova, Italy
// 
// Based on code developed by the undergraduate student G. Guerrieri 
// Note: this is a preliminary beta-version of the code; an improved 
// version will be distributed in the next Geant4 public release, compliant
// with the design in a forthcoming publication, and subject to a 
// design and code review.
//
#ifndef G4MIRDRightLung_h
#define G4MIRDRightLung_h 1

#include "G4VPhysicalVolume.hh"
#include "G4VOrgan.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class G4MIRDRightLung: public G4VOrgan
{
public:

  G4MIRDRightLung();
  ~G4MIRDRightLung();
  G4VPhysicalVolume* Construct(const G4String&,G4VPhysicalVolume*, 
				    const G4String&, G4bool, G4bool);

};
#endif
