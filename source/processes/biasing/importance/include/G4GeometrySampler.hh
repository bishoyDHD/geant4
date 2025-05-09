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
// Class G4GeometrySampler
//
// Class description:
//
// This class inherits from G4VSampler. It is used for scoring and 
// importance sampling in the tracking (mass) geometry.
// See also the description in G4VSampler.hh.
//
// Author: Michael Dressel, CERN
// --------------------------------------------------------------------
#ifndef G4GeometrySampler_hh
#define G4GeometrySampler_hh 1

#include "G4Types.hh"
#include "G4String.hh"
#include "G4VSampler.hh"
#include "G4VSamplerConfigurator.hh"

class G4ImportanceConfigurator;
class G4WeightWindowConfigurator;
class G4WeightCutOffConfigurator;

class G4GeometrySampler : public G4VSampler
{

 public:
 
  explicit G4GeometrySampler(G4VPhysicalVolume *worldvolume, const G4String &particlename);
  explicit G4GeometrySampler(const G4String& worldvolumeName, const G4String &particlename);
  virtual ~G4GeometrySampler();

  G4GeometrySampler(const G4GeometrySampler &) = delete;
  G4GeometrySampler& operator=(const G4GeometrySampler &) = delete;

  //  virtual void PrepareScoring(G4VScorer *Scorer);
  virtual void PrepareImportanceSampling(G4VIStore* istore,
                                         const G4VImportanceAlgorithm 
                                         *ialg);
  virtual void PrepareWeightRoulett(G4double wsurvive, 
                                    G4double wlimit,
                                    G4double isource);
  
  virtual void PrepareWeightWindow(G4VWeightWindowStore *wwstore,
                                   G4VWeightWindowAlgorithm *wwAlg,
                                   G4PlaceOfAction placeOfAction);

  virtual void Configure();
  virtual void AddProcess();

  virtual void ClearSampling();
  virtual G4bool IsConfigured() const;

  void SetParallel(G4bool paraflag);
  void SetWorld(const G4VPhysicalVolume* world);
  void SetParticle(const G4String &particlename);

  inline const G4String& GetParticleName() { return fParticleName; }
  
 private:

  G4String fParticleName;
  const G4VPhysicalVolume* fWorld = nullptr;
  G4String fWorldName;
  G4ImportanceConfigurator* fImportanceConfigurator = nullptr;
  G4WeightCutOffConfigurator* fWeightCutOffConfigurator = nullptr;
  G4VIStore* fIStore = nullptr;
  G4WeightWindowConfigurator* fWeightWindowConfigurator = nullptr;
  G4VWeightWindowStore* fWWStore = nullptr;
  G4bool fIsConfigured = false;
  G4Configurators fConfigurators;
  G4bool paraflag = false;
};
  
#endif
