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
// INCL++ intra-nuclear cascade model
// Pekka Kaitaniemi, CEA and Helsinki Institute of Physics
// Davide Mancusi, CEA
// Alain Boudard, CEA
// Sylvie Leray, CEA
// Joseph Cugnon, University of Liege
//
// INCL++ revision: v5.1_rc1
//
#define INCLXX_IN_GEANT4_MODE 1

#include "globals.hh"

/*
 * G4INCLReflectionAvatar.cc
 *
 *  Created on: Jun 8, 2009
 *      Author: Pekka Kaitaniemi
 */

#include "G4INCLSurfaceAvatar.hh"
#include "G4INCLRandom.hh"
#include "G4INCLReflectionChannel.hh"
#include "G4INCLTransmissionChannel.hh"
#include "G4INCLClustering.hh"
#include <sstream>
#include <string>

namespace G4INCL {

  SurfaceAvatar::SurfaceAvatar(G4INCL::Particle *aParticle, G4double time, G4INCL::Nucleus *n)
    :IAvatar(time), theParticle(aParticle), theNucleus(n)
  {
    setType(SurfaceAvatarType);
    // TODO Auto-generated constructor stub
  }

  SurfaceAvatar::~SurfaceAvatar() {

  }

  G4INCL::IChannel* SurfaceAvatar::getChannel() const
  {
    if(!theParticle->isParticipant()) {
      DEBUG("Particle " << theParticle->getID() << " is a spectator, reflection" << std::endl);
      return new ReflectionChannel(theNucleus, theParticle);
    }

    // Don't try to make a cluster if the leading particle is too slow
    const G4double transmissionProbability = getTransmissionProbability(theParticle);

    DEBUG("Transmission probability for particle " << theParticle->getID() << " = " << transmissionProbability << std::endl);
    if(theParticle->isNucleon() && transmissionProbability>1.E-4) {
      Cluster *candidateCluster = 0;

      candidateCluster = Clustering::getCluster(theNucleus, theParticle);
      if(candidateCluster != 0 &&
          Clustering::clusterCanEscape(candidateCluster)) {

        DEBUG("Cluster algorithm succeded. Candidate cluster:" << std::endl << candidateCluster->print() << std::endl);

        // Check if the cluster can penetrate the Coulomb barrier
        const G4double clusterTransmissionProbability = getTransmissionProbability(candidateCluster);
        const G4double x = Random::shoot();

        DEBUG("Transmission probability for cluster " << candidateCluster->getID() << " = " << clusterTransmissionProbability << std::endl);

        if (x <= clusterTransmissionProbability) {
          DEBUG("Cluster " << candidateCluster->getID() << " passes the Coulomb barrier, transmitting." << std::endl);
          return new TransmissionChannel(theNucleus, candidateCluster);
        } else {
          DEBUG("Cluster " << candidateCluster->getID() << " does not pass the Coulomb barrier. Falling back to transmission of the leading particle." << std::endl);
          delete candidateCluster;
        }
      } else {
        delete candidateCluster;
      }
    }

    // If we haven't transmitted a cluster (maybe cluster feature was
    // disabled or maybe we just can't produce an acceptable cluster):
    const G4double x = Random::shoot();

    if(x <= transmissionProbability) { // Transmission
      DEBUG("Particle " << theParticle->getID() << " passes the Coulomb barrier, transmitting." << std::endl);
      return new TransmissionChannel(theNucleus, theParticle);
    } else { // Reflection
      DEBUG("Particle " << theParticle->getID() << " does not pass the Coulomb barrier, reflection." << std::endl);
      return new ReflectionChannel(theNucleus, theParticle);
    }
  }

  G4INCL::FinalState* SurfaceAvatar::getFinalState() const
  {
    return getChannel()->getFinalState();
  }

  void SurfaceAvatar::preInteraction() {}
  FinalState *SurfaceAvatar::postInteraction(FinalState *fs) {
    ParticleList outgoing = fs->getOutgoingParticles();
    if(!outgoing.empty()) { // Transmission
// assert(outgoing.size()==1);
      Particle *out = outgoing.front();
      if(out->isCluster()) {
	Cluster *clusterOut = dynamic_cast<Cluster*>(out);
        ParticleList const *components = clusterOut->getParticles();
        for(ParticleIter i=components->begin(); i!=components->end(); ++i) {
          if((*i)->isParticipant())
            theNucleus->getStore()->getBook()->decrementParticipants();
        }
      } else if(theParticle->isParticipant()) {
// assert(out==theParticle);
        theNucleus->getStore()->getBook()->decrementParticipants();
      }
    }
    return fs;
  }

  std::string SurfaceAvatar::dump() const {
    std::stringstream ss;
    ss << "(avatar " << theTime << " 'reflection" << std::endl
      << "(list " << std::endl 
      << theParticle->dump()
      << "))" << std::endl;
    return ss.str();
  }

  G4double SurfaceAvatar::getTransmissionProbability(Particle const * const particle) const {

    const G4double E = particle->getKineticEnergy();
    const G4double V = particle->getPotentialEnergy();

    if (E <= V) // No transmission if total energy < 0
      return 0.0;

    const G4double m = particle->getMass();
    const G4double EMinusV = E-V;
    const G4double EMinusV2 = EMinusV*EMinusV;

    // Intermediate variable for calculation
    const G4double x=std::sqrt((2.*m*E+E*E)*(2.*m*EMinusV+EMinusV2));

    // The transmission probability for a potential step
    G4double theTransmissionProbability =
      4.*x/(2.*m*(E+EMinusV)+E*E+(EMinusV2)+2.*x);

    // For neutral and negative particles, no Coulomb transmission
    // Also, no Coulomb if the particle takes away all of the nuclear charge
    const G4int theZ = theNucleus->getZ();
    const G4int theParticleZ = particle->getZ();
    if (theParticleZ <= 0 || theParticleZ >= theZ)
      return theTransmissionProbability;

    // Nominal Coulomb barrier
    const G4double theTransmissionBarrier = theNucleus->getTransmissionBarrier(particle);
    if (EMinusV >= theTransmissionBarrier) // Above the Coulomb barrier
      return theTransmissionProbability;

    // Coulomb-penetration factor
    const G4double px = std::sqrt(EMinusV/theTransmissionBarrier);
    const G4double logCoulombTransmission =
      theParticleZ*(theZ-theParticleZ)/137.03*std::sqrt(2.*m/EMinusV/(1.+EMinusV/2./m))
      *(std::acos(px)-px*std::sqrt(1.-px*px));
    if (logCoulombTransmission > 35.) // Transmission is forbidden by Coulomb
      return 0.;
    theTransmissionProbability *= std::exp(-2.*logCoulombTransmission);

    return theTransmissionProbability;
  }

}
