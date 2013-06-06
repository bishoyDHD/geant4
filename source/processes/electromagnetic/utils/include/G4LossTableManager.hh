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
// $Id$
//
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4LossTableManager
//
// Author:        Vladimir Ivanchenko on base of G4LossTables class
//                and Maria Grazia Pia ideas
//
// Creation date: 03.01.2002
//
// Modifications:
//
// 20-01-03 Migrade to cut per region (V.Ivanchenko)
// 17-02-03 Fix problem of store/restore tables for ions (V.Ivanchenko)
// 10-03-03 Add Ion registration (V.Ivanchenko)
// 25-03-03 Add deregistration (V.Ivanchenko)
// 26-03-03 Add GetDEDXDispersion (V.Ivanchenko)
// 02-04-03 Change messenger (V.Ivanchenko)
// 23-07-03 Add exchange with G4EnergyLossTables (V.Ivanchenko)
// 05-10-03 Add G4VEmProcesses registration (V.Ivanchenko)
// 17-10-03 Add SetParameters method (V.Ivanchenko)
// 12-11-03 G4EnergyLossSTD -> G4EnergyLossProcess (V.Ivanchenko)
// 14-01-04 Activate precise range calculation (V.Ivanchenko)
// 08-11-04 Migration to new interface of Store/Retrieve tables (V.Ivantchenko)
// 10-01-06 PreciseRange -> CSDARange (V.Ivantchenko)
// 20-01-06 Introduce GetSubDEDX method (VI)
// 26-01-06 Rename GetRange -> GetRangeFromRestricteDEDX (V.Ivanchenko)
// 10-05-06 Add methods  SetMscStepLimitation, FacRange and MscFlag (VI)
// 22-05-06 Add methods  Set/Get bremsTh (VI)
// 12-02-07 Add SetSkin, SetLinearLossLimit (V.Ivanchenko)
// 18-06-07 Move definition of msc parameters to G4EmProcessOptions (V.Ivanchenko)
// 12-04-10 Added PreparePhsyicsTables and BuildPhysicsTables entries (V.Ivanchenko)
// 04-06-13 Adaptation for MT mode, new method LocalPhysicsTables (V.Ivanchenko)  
//
// Class Description:
//
// A utility static class, responsable for the energy loss tables
// for each particle
//
// Energy loss processes have to register their tables with this
// class. The responsibility of creating and deleting the tables
// remains with the energy loss classes.

// -------------------------------------------------------------------
//

#ifndef G4LossTableManager_h
#define G4LossTableManager_h 1

#include <map>
#include <vector>
#include "globals.hh"
#include "G4VEnergyLossProcess.hh"

class G4PhysicsTable;
class G4MaterialCutsCouple;
class G4EnergyLossMessenger;
class G4ParticleDefinition;
class G4VMultipleScattering;
class G4VEmProcess;
class G4EmCorrections;
class G4EmSaturation;
class G4EmConfigurator;
class G4ElectronIonPair;
class G4LossTableBuilder;
class G4VAtomDeexcitation;
class G4Region;

class G4LossTableManager
{

public:

  static G4LossTableManager* Instance();

  ~G4LossTableManager();

  //-------------------------------------------------
  // called from destructor
  //-------------------------------------------------

  void Clear();

  //-------------------------------------------------
  // initialisation before a new run
  //-------------------------------------------------

  void PreparePhysicsTable(const G4ParticleDefinition* aParticle,
			   G4VEnergyLossProcess* p, G4bool theMaster);

  void PreparePhysicsTable(const G4ParticleDefinition* aParticle,
			   G4VEmProcess* p);

  void PreparePhysicsTable(const G4ParticleDefinition* aParticle,
			   G4VMultipleScattering* p, G4bool theMaster);

  void BuildPhysicsTable(const G4ParticleDefinition* aParticle);

  void BuildPhysicsTable(const G4ParticleDefinition* aParticle, 
			 G4VEnergyLossProcess* p);

  void LocalPhysicsTables(const G4ParticleDefinition* aParticle, 
			  G4VEnergyLossProcess* p);
  
  //-------------------------------------------------
  // Run time access to DEDX, range, energy for a given particle, 
  // energy, and G4MaterialCutsCouple
  //-------------------------------------------------

  G4double GetDEDX(
    const G4ParticleDefinition *aParticle,
    G4double kineticEnergy,
    const G4MaterialCutsCouple *couple);

  G4double GetSubDEDX(
    const G4ParticleDefinition *aParticle,
    G4double kineticEnergy,
    const G4MaterialCutsCouple *couple);

  G4double GetRange(
    const G4ParticleDefinition *aParticle,
    G4double kineticEnergy,
    const G4MaterialCutsCouple *couple);

  G4double GetCSDARange(
    const G4ParticleDefinition *aParticle,
    G4double kineticEnergy,
    const G4MaterialCutsCouple *couple);

  G4double GetRangeFromRestricteDEDX(
    const G4ParticleDefinition *aParticle,
    G4double kineticEnergy,
    const G4MaterialCutsCouple *couple);

  G4double GetEnergy(
    const G4ParticleDefinition *aParticle,
    G4double range,
    const G4MaterialCutsCouple *couple);

  G4double GetDEDXDispersion(
    const G4MaterialCutsCouple *couple,
    const G4DynamicParticle* dp,
          G4double& length);

  //-------------------------------------------------
  // Methods to be called only at initialisation
  //-------------------------------------------------

  void Register(G4VEnergyLossProcess* p);

  void DeRegister(G4VEnergyLossProcess* p);

  void Register(G4VMultipleScattering* p);

  void DeRegister(G4VMultipleScattering* p);

  void Register(G4VEmProcess* p);

  void DeRegister(G4VEmProcess* p);

  void Register(G4VEmModel* p);

  void DeRegister(G4VEmModel* p);

  void Register(G4VEmFluctuationModel* p);

  void DeRegister(G4VEmFluctuationModel* p);

  void RegisterExtraParticle(const G4ParticleDefinition* aParticle, 
			     G4VEnergyLossProcess* p);

  void SetLossFluctuations(G4bool val);

  void SetSubCutoff(G4bool val, const G4Region* r=0);

  void SetIntegral(G4bool val);

  void SetRandomStep(G4bool val);

  void SetMinSubRange(G4double val);

  void SetMinEnergy(G4double val);

  void SetMaxEnergy(G4double val);

  void SetMaxEnergyForCSDARange(G4double val);

  void SetMaxEnergyForMuons(G4double val);

  void SetDEDXBinning(G4int val);

  void SetDEDXBinningForCSDARange(G4int val);

  void SetLambdaBinning(G4int val);

  G4int GetNumberOfBinsPerDecade() const;

  void SetStepFunction(G4double v1, G4double v2);

  void SetBuildCSDARange(G4bool val);

  void SetLPMFlag(G4bool val);

  void SetSplineFlag(G4bool val);

  void SetLinearLossLimit(G4double val);

  void SetBremsstrahlungTh(G4double val);

  void SetFactorForAngleLimit(G4double val);

  void SetVerbose(G4int val);

  //-------------------------------------------------
  // Access methods
  //-------------------------------------------------

  G4EnergyLossMessenger* GetMessenger();

  G4bool BuildCSDARange() const;

  G4bool LPMFlag() const;

  G4bool SplineFlag() const;

  G4bool IsMaster() const;

  G4double BremsstrahlungTh() const;

  G4double FactorForAngleLimit() const;

  G4double MinKinEnergy() const;

  G4double MaxKinEnergy() const;

  const std::vector<G4VEnergyLossProcess*>& GetEnergyLossProcessVector();

  const std::vector<G4VEmProcess*>& GetEmProcessVector();

  const std::vector<G4VMultipleScattering*>& GetMultipleScatteringVector();

  G4VEnergyLossProcess* GetEnergyLossProcess(const G4ParticleDefinition*);

  G4EmCorrections* EmCorrections();

  G4EmSaturation* EmSaturation();

  G4EmConfigurator* EmConfigurator();

  G4ElectronIonPair* ElectronIonPair();

  G4VAtomDeexcitation* AtomDeexcitation();

  G4LossTableBuilder* GetTableBuilder();

  void SetAtomDeexcitation(G4VAtomDeexcitation*);

private:

  //-------------------------------------------------
  // Private methods and members
  //-------------------------------------------------

  G4LossTableManager();

  G4VEnergyLossProcess* BuildTables(const G4ParticleDefinition* aParticle);

  void CopyTables(const G4ParticleDefinition* aParticle, 
		  G4VEnergyLossProcess*);

  void ParticleHaveNoLoss(const G4ParticleDefinition* aParticle);

  void SetParameters(const G4ParticleDefinition* aParticle,
		     G4VEnergyLossProcess*);

  void CopyDEDXTables();

  G4LossTableManager(G4LossTableManager &);
  G4LossTableManager & operator=(const G4LossTableManager &right);

  static G4ThreadLocal G4LossTableManager* theInstance;

  typedef const G4ParticleDefinition* PD;

  std::map<PD,G4VEnergyLossProcess*,std::less<PD> > loss_map;

  std::vector<G4VEnergyLossProcess*> loss_vector;
  std::vector<PD> part_vector;
  std::vector<PD> base_part_vector;
  std::vector<G4bool> tables_are_built;
  std::vector<G4bool> isActive;
  std::vector<G4PhysicsTable*> dedx_vector;
  std::vector<G4PhysicsTable*> range_vector;
  std::vector<G4PhysicsTable*> inv_range_vector;
  std::vector<G4VMultipleScattering*> msc_vector;
  std::vector<G4VEmProcess*> emp_vector;
  std::vector<G4VEmModel*> mod_vector;
  std::vector<G4VEmFluctuationModel*> fmod_vector;

  // cash
  G4VEnergyLossProcess* currentLoss;
  PD                    currentParticle;
  PD                    theElectron;
  PD                    theGenericIon;
  PD                    firstParticle;

  G4int n_loss;
  G4int run;

  G4bool all_tables_are_built;
  G4bool startInitialisation;

  G4bool lossFluctuationFlag;
  G4bool subCutoffFlag;
  G4bool rndmStepFlag;
  G4bool integral;
  G4bool integralActive;
  G4bool buildCSDARange;
  G4bool minEnergyActive;
  G4bool maxEnergyActive;
  G4bool maxEnergyForMuonsActive;
  G4bool stepFunctionActive;
  G4bool flagLPM;
  G4bool splineFlag;
  G4bool isMaster;

  G4double minSubRange;
  G4double maxRangeVariation;
  G4double maxFinalStep;
  G4double minKinEnergy;
  G4double maxKinEnergy;
  G4double maxKinEnergyForMuons;
  G4double bremsTh;
  G4double factorForAngleLimit;

  G4LossTableBuilder*         tableBuilder;
  G4EnergyLossMessenger*      theMessenger;
  G4EmCorrections*            emCorrections;
  G4EmSaturation*             emSaturation;
  G4EmConfigurator*           emConfigurator;
  G4ElectronIonPair*          emElectronIonPair;
  G4VAtomDeexcitation*        atomDeexcitation;

  G4int nbinsLambda;
  G4int nbinsPerDecade;
  G4int verbose;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

