#include "G4StringChipsInterface.hh"
#include "globals.hh"
#include "G4Pair.hh"
#include "g4std/list"
#include "G4KineticTrackVector.hh"
#include "G4Nucleon.hh"

G4StringChipsInterface::G4StringChipsInterface()
{
  theEnergyLossPerFermi = 1.*GeV;
}

G4VParticleChange* G4StringChipsInterface::
ApplyYourself(const G4Track& aTrack, G4Nucleus& theNucleus)
{
  return theModel.ApplyYourself(aTrack, theNucleus);
}

G4ReactionProductVector* G4StringChipsInterface::
Propagate(G4KineticTrackVector* theSecondaries, G4V3DNucleus* theNucleus)
{
  // Protection for non physical conditions
  
  if(theSecondaries->length() == 1) 
    G4Exception("G4StringChipsInterface: Only one particle from String models!");
  
  // Calculate the mean energy lost
  G4Pair<G4double, G4double> theImpact = theNucleus->RefetchImpactXandY();
  G4double impactX = theImpact.first;
  G4double impactY = theImpact.second;
  G4double inpactPar2 = impactX*impactX + impactY*impactY;
  
  G4double radius2 = theNucleus->GetNuclearRadius(5*perCent);
  radius2 *= radius2;
  G4double pathlength = 2.*sqrt(radius2 - inpactPar2);
  G4double theEnergyLostInFragmentation = theEnergyLossPerFermi*pathlength/fermi;
  
  // now select all particles in range
  G4std::list<G4Pair<G4double, G4KineticTrack *> > theSorted;
  G4std::list<G4Pair<G4double, G4KineticTrack *> >::iterator current;
  for(G4int secondary = 0; secondary<theSecondaries->length(); secondary++)
  {
    G4LorentzVector a4Mom = theSecondaries->at(secondary)->Get4Momentum();
    G4double kinetic = a4Mom.t()-a4Mom.m();
    G4Pair<G4double, G4KineticTrack *> it;
    it.first = kinetic;
    it.second = theSecondaries->at(secondary);
    G4bool inserted = false;
    for(current = theSorted.begin(); current!=theSorted.end(); current++)
    {
      if(current->first > kinetic)
      {
	theSorted.insert(current, it);
	inserted = true;
	break;
      }
    }
    if(!inserted)
    {
      theSorted.push_front(it);
    }
  }
  
  G4LorentzVector proj4Mom = (0,0,0,0);
  G4int nD  = 0;
  G4int nU  = 0;
  G4int nS  = 0;
  G4int nAD = 0;
  G4int nAU = 0;
  G4int nAS = 0;
  G4std::list<G4Pair<G4double, G4KineticTrack *> >::iterator firstEscaping = 0;
  G4double runningEnergy = 0;
  for(current = theSorted.begin(); current!=theSorted.end(); current++)
  {
    firstEscaping = current;
    runningEnergy += current->second->Get4Momentum().t();
    if(runningEnergy > theEnergyLostInFragmentation) break;
    
     // projectile 4-momentum needed in constructor of quasmon
    proj4Mom += current->second->Get4Momentum().t();
    
     // projectile quark contects needed for G4QContent construction
    nD += current->second->GetDefinition()->GetQuarkContent(1);
    nU += current->second->GetDefinition()->GetQuarkContent(2);
    nS += current->second->GetDefinition()->GetQuarkContent(3);
    nAD += current->second->GetDefinition()->GetAntiQuarkContent(1);
    nAU += current->second->GetDefinition()->GetAntiQuarkContent(2);
    nAS += current->second->GetDefinition()->GetAntiQuarkContent(3);
  }
  // construct G4QContent
  G4QContent theProjectiles(nD, nU, nS, nAD, nAU, nAS);
  
  // target properties needed in constructor of quasmon
  // remove all hit nucleons to get Target code
  theNucleus->StartLoop();
  G4Nucleon * aNucleon;
  G4int resA = 0;
  G4int resZ = 0;
  G4ThreeVector hitMomentum(0,0,0);
  G4double hitMass = 0;
  while(aNucleon = theNucleus->GetNextNucleon())
  {
    if(!aNucleon->AreYouHit())
    {
      resA++;
      resZ+=aNucleon->GetDefinition()->GetPDGCharge();
    }
    else
    {
      hitMomentum += aNucleon->GetMomentum().vect();
      hitMass += aNucleon->GetMomentum().m();
    }
  }
  G4int targetPDGCode = 90000000 + 1000*resZ + (resA-resZ);
  G4double targetMass = theNucleus->GetMass();
  targetMass -= hitMass;
  G4double targetEnergy = sqrt(hitMomentum.mag2()+targetMass*targetMass);
  G4LorentzVector targ4Mom(-1.*hitMomentum, targetEnergy);
  
  // construct the quasmon
  G4int nop = 223; // ??????
  G4double fractionOfSingleQuasiFreeNucleons = 0.15;
  G4double fractionOfPairedQuasiFreeNucleons = 0.01;
  G4double clusteringCoefficient = 5.;
  G4double temperature = 180.;
  G4double halfTheStrangenessOfSee = 0.1; // = s/d = s/u
  G4double etaToEtaPrime = 0.3;

  G4QNucleus::SetParameters(fractionOfSingleQuasiFreeNucleons,
                            fractionOfPairedQuasiFreeNucleons,
			    clusteringCoefficient);
  G4Quasmon::SetParameters(temperature,
                           halfTheStrangenessOfSee,
			   etaToEtaPrime);
  G4Quasmon* pan= new G4Quasmon(theProjectiles, targetPDGCode, 1./MeV*proj4Mom, 1./MeV*targ4Mom, nop);

  // now call chips with this info in place
  G4QHadronVector output = pan->HadronizeQuasmon();
  delete pan;
   
  // Fill the result.
  G4ReactionProductVector * theResult = new G4ReactionProductVector;
  G4ReactionProduct * theSec;
  G4cout << "NEXT EVENT"<<endl;
  
  // first add all escaping particles.
  for(current = firstEscaping; current!=theSorted.end(); current++)
  {
    theSec = new G4ReactionProduct(current->second->GetDefinition());
    G4LorentzVector current4Mom = current->second->Get4Momentum();
    theSec->SetTotalEnergy(current4Mom.t());
    theSec->SetMomentum(current4Mom.vect());
    theResult->insert(theSec);
  }
  // now the quasmon output
  for(G4int particle = 0; particle < output.length(); particle++)
  {
    if(output[particle]->GetNFragments() != 0) 
    {
      delete output[particle];
      continue;
    }
    theSec = new G4ReactionProduct;  
    G4int pdgCode = output[particle]->GetPDGCode();
    G4ParticleDefinition * theDefinition;
    // Note that I still have to take care of strange nuclei
    // For this I need the mass calculation, and a changed interface
    // for ion-table ==> work for Hisaya @@@@@@@
    // Then I can sort out the pdgCode. I also need a decau process 
    // for strange nuclei; may be another chips interface
    if(pdgCode>90000000) 
    {
      G4int aZ = (pdgCode-90000000)/1000;
      G4int anN = pdgCode-90000000-1000*aZ;
      theDefinition = G4ParticleTable::GetParticleTable()->FindIon(aZ,anN+aZ,0,aZ);
      if(aZ == 0 && anN == 1) theDefinition = G4Neutron::Neutron();
    }
    else
    {
      theDefinition = G4ParticleTable::GetParticleTable()->FindParticle(output[particle]->GetPDGCode());
    }
    G4cout << "Particle code produced = "<< pdgCode <<endl;
    theSec = new G4ReactionProduct(current->second->GetDefinition());
    theSec->SetTotalEnergy(output[particle]->Get4Momentum().t());
    theSec->SetMomentum(output[particle]->Get4Momentum().vect());
    theResult->insert(theSec);
    delete output[particle];
  }
  return theResult;
} 
