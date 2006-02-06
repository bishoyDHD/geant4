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
// ---------------------------------------------------------------------
// Short description of the G4 Class made by M.K. (asFarAsHeNnderstands)
// =====================================================================
// >>> StandardG4Process wrepper for the Hadronic Model Collection <<<
// This class is necessary only for the navigation through all models
// of the Hadronic collection AND for conversion of the special classes
// of the Hadronic Result (list of the secondary particles) to the
// standard G4VParticleChange form. One should not use this class
// for the simple ElectroWeak Processes, which are using the only
// event Generator CHIPS and produce the Result in the
// standard G4VParticleChange form. -- 29.01.2004. -- M.K.
//
// ***Important*** It is working only for the G4VDiscreteProcess
//                 (not for G4VRestProcess - @@ Create similar class)
//
// ======================================================================================
//
//      ---------- Test39HadronProduction -------
//    Converted from Test39 to Test39 by Mikhail Kossov, 29 Jan 2005 
//
//---------------------------------------------------------------------------------------

#ifndef Test39HadronProduction_Test39HadronProduction_h
#define Test39HadronProduction_Test39HadronProduction_h 1

#include "Test39VSecondaryGenerator.hh"

#include "G4ios.hh"
#include "globals.hh"
#include "G4VDiscreteProcess.hh"
#include "G4HadFinalState.hh"

class G4Step;
class G4Track;
class G4VParticleChange;

class Test39HadronProduction : public G4VDiscreteProcess
{
public:

  Test39HadronProduction(const G4String& processName = "HadronProduction" );
  ~Test39HadronProduction();

  void SetSecondaryGenerator(Test39VSecondaryGenerator*);
  G4double PostStepGetPhysicalInteractionLength(const             G4Track& track,
                                                G4double          previousStepSize,
                                                G4ForceCondition* condition);
  G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);
  // @@This is a bad stile to declare and to implement in one line -> make inline
  G4bool IsApplicable(const G4ParticleDefinition&) {return true;};
  G4double GetMass() {return theGenerator->GetMass();};

//protected:
  G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*) {return DBL_MAX;};

private:
  // hide assignment operator as private
  Test39HadronProduction(const Test39HadronProduction&);
  Test39HadronProduction& operator = (const Test39HadronProduction &right);

  // Navigation tool for the Model Collection of the Hadronic package
  void InitializeMe();

  // Body
  Test39VSecondaryGenerator* theGenerator; // The selected model from the Collection
  G4VParticleChange          theChange;    // Standard G4VProcess output Prototype
};

#endif  // Test39HadronProduction_Test39HadronProduction_h

