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
// $Id: G4eIonisation.hh,v 1.12 2001-08-14 10:01:25 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//      ---------- G4eIonisation physics process -----------
//                by Laszlo Urban, 20 March 1997 
//
// 10-02-00 modifications , new e.m. structure, L.Urban
// 03-08-01 new methods Store/Retrieve PhysicsTable (mma)
// 13-08-01 new function ComputeRestrictedMeandEdx() (mma)
//
// ------------------------------------------------------------

// Class description
//
// This class manages the ionisation process for e-/e+
// it inherites from G4VContinuousDiscreteProcess via G4VeEnergyLoss.
//
// Class description - end

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
 
#ifndef G4eIonisation_h
#define G4eIonisation_h 1
 
#include "G4VeEnergyLoss.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
 
class G4eIonisation : public G4VeEnergyLoss 
 
{
  public:   // with description
 
    G4eIonisation(const G4String& processName = "eIonisa"); 

   ~G4eIonisation();

    G4bool IsApplicable(const G4ParticleDefinition&);
      // return true for e+/e-, false otherwise 
    
    void BuildPhysicsTable(const G4ParticleDefinition& aParticleType);
      // this function overloads a virtual function of the base class.
      // It is invoked by the G4ParticleWithCuts::SetCut() method.
      // It invokes BuildLambdaTable(), BuildLossTable(), BuildDEDXTable()
        
    void BuildLossTable(const G4ParticleDefinition& aParticleType);
      // build the dE/dx tables due to the ionisation, for every materials.
      // (restricted stopping power, Berger-Seltzer formula)

    void BuildLambdaTable(const G4ParticleDefinition& aParticleType);
      // build mean free path tables for the delta rays production.
      // the tables are built for every materials.
      
    G4bool StorePhysicsTable(G4ParticleDefinition* ,
		             const G4String& directory, G4bool);
      // store eLoss and MeanFreePath tables into an external file
      // specified by 'directory' (must exist before invokation)
      
    G4bool RetrievePhysicsTable(G4ParticleDefinition* ,   
			        const G4String& directory, G4bool);
      // retrieve eLoss and MeanFreePath tables from an external file
      // specified by 'directory'
            
    void PrintInfoDefinition();
      // Print few lines of informations about the process: validity range,
      // origine ..etc..
      // Invoked by BuildPhysicsTable().
          
    G4double GetMeanFreePath(const G4Track& track,
                             G4double previousStepSize,
                             G4ForceCondition* condition );
      // It returns the MeanFreePath of the process for the current track :
      // (energy, material)
      // The previousStepSize and G4ForceCondition* are not used.
      // This function overloads a virtual function of the base class.
      // It is invoked by the ProcessManager of the Particle.
        
    G4VParticleChange *PostStepDoIt(const G4Track& track,         
                                    const G4Step& Step );                 
      // It computes the final state of the process (at end of step),
      // returned as a ParticleChange object.			    
      // This function overloads a virtual function of the base class.
      // It is invoked by the ProcessManager of the Particle.
       
    G4double GetLambda(G4double KineticEnergy, G4Material* material);
      // It returns the MeanFreePath of the process for a (energy, material)

  protected:   // with description
  
    virtual G4double ComputeRestrictedMeandEdx(
                            const G4ParticleDefinition& aParticleType,
                            G4double KineticEnergy,
                            const G4Material* material,
                            G4double DeltaThreshold);
      // computes restricted mean dE/dx in Geant4 internal units. 
            
    virtual G4double ComputeCrossSectionPerAtom(
                            const G4ParticleDefinition& aParticleType,
                            G4double KineticEnergy,
                            G4double AtomicNumber,
                            G4double DeltaThreshold);
      // computes total cross section per atom in Geant4 internal units.      
     			    
  protected:
                                
    G4PhysicsTable* theMeanFreePathTable;

  private:

  // hide assignment operator 
  G4eIonisation & operator=(const G4eIonisation &right);
  G4eIonisation(const G4eIonisation&);

  private:

    static G4double LowerBoundLambda;    // binning for mean free path table
    static G4double UpperBoundLambda;
    static G4int    NbinLambda;
    
    G4double LowestKineticEnergy;        // binning for dE/dx table
    G4double HighestKineticEnergy;
    G4int    TotBin;

  public:  // with description

    static void SetLowerBoundLambda(G4double val) {LowerBoundLambda = val;};
    static void SetUpperBoundLambda(G4double val) {UpperBoundLambda = val;};
    static void SetNbinLambda(G4int n) {NbinLambda = n;};
        // set the parameters of the mean free path table.
	
    static G4double GetLowerBoundLambda() { return LowerBoundLambda;}; 
    static G4double GetUpperBoundLambda() { return UpperBoundLambda;}; 
    static G4int GetNbinLambda() {return NbinLambda;};
        // get the parameters of the mean free path table.
	
};
 
#include "G4eIonisation.icc"
 
#endif
 
