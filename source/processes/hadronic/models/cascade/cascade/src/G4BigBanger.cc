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

#include "G4BigBanger.hh"
#include "G4InuclNuclei.hh"
#include "G4ParticleLargerEkin.hh"
#include "G4LorentzConvertor.hh"
#include "g4std/algorithm"

typedef G4std::vector<G4InuclElementaryParticle>::iterator particleIterator;

G4BigBanger::G4BigBanger()
  : verboseLevel(1) {
  if (verboseLevel > 3) {
    G4cout << " >>> G4BigBanger::G4BigBanger" << G4endl;
  }
}

G4CollisionOutput G4BigBanger::collide(G4InuclParticle* /*bullet*/,
				       G4InuclParticle* target) {

  if (verboseLevel > 3) {
    G4cout << " >>> G4BigBanger::collide" << G4endl;
  }

  // primitive explosion model A -> nucleons to prevent too exotic evaporation

  const G4double small_ekin = 1.0e-6;

  G4CollisionOutput output;
  G4std::vector<G4double> totscm;
  G4std::vector<G4double> totlab;

  if(G4InuclNuclei* nuclei_target = dynamic_cast<G4InuclNuclei*>(target)) {
  
    G4double A = nuclei_target->getA();
    G4double Z = nuclei_target->getZ();
    G4std::vector<G4double> PEX = nuclei_target->getMomentum();
    G4double EEXS = nuclei_target->getExitationEnergy();
    G4InuclElementaryParticle dummy(small_ekin, 1);
    G4LorentzConvertor toTheNucleiSystemRestFrame;

    toTheNucleiSystemRestFrame.setBullet(dummy.getMomentum(), dummy.getMass());
    toTheNucleiSystemRestFrame.setTarget(PEX, nuclei_target->getMass());
    toTheNucleiSystemRestFrame.toTheTargetRestFrame();

    G4double etot = 0.001 * (EEXS - bindingEnergy(A, Z));

    if (verboseLevel > 2) {
      G4cout << " BigBanger: target " << G4endl;
      nuclei_target->printParticle(); 
      G4cout << " BigBanger: a " << A << " z " << Z << " eexs " << EEXS << " etot " <<
	etot << " nm " << nuclei_target->getMass() << G4endl;
    }
  
    G4std::vector<G4InuclElementaryParticle> particles = 	    
      generateBangInSCM(etot, A, Z, dummy.getParticleMass(1), dummy.getParticleMass(2));

    if (verboseLevel > 2) {
      G4cout << " particles " << particles.size() << G4endl;
      for(G4int i = 0; i < G4int(particles.size()); i++) 
	particles[i].printParticle();
    }
    if(!particles.empty()) { // convert back to Lab
      if (verboseLevel > 2) {
	G4std::vector<G4double> totscm(4, 0.0);
	G4std::vector<G4double> totlab(4, 0.0);
      }
      particleIterator ipart;

      for(ipart = particles.begin(); ipart != particles.end(); ipart++) {
	if (verboseLevel > 2) {
	  G4std::vector<G4double> mom_scm = ipart->getMomentum();

	  for(G4int i = 0; i < 4; i++) totscm[i] += mom_scm[i];
	}
	G4std::vector<G4double> mom = 
	  toTheNucleiSystemRestFrame.backToTheLab(ipart->getMomentum());
	ipart->setMomentum(mom); 

	if (verboseLevel > 2) {
	  mom = ipart->getMomentum();
	  for(G4int i = 0; i < 4; i++) totlab[i] += mom[i];
	}
      };
      G4std::sort(particles.begin(), particles.end(), G4ParticleLargerEkin());
      if (verboseLevel > 2) {
	G4cout << " In SCM: total outgoing momentum " << G4endl 
	       << " E " << totscm[0] << " px " << totscm[1]
	       << " py " << totscm[2] << " pz " << totscm[3] << G4endl; 
	G4cout << " In Lab: mom cons " << G4endl 
	       << " E " << PEX[0] + 0.001 * EEXS - totlab[0] 
	       << " px " << PEX[1] - totlab[1]
	       << " py " << PEX[2] - totlab[2] 
	       << " pz " << PEX[3] - totlab[3] << G4endl; 
      }
    };	
    output.addOutgoingParticles(particles);
  }
  else {
    G4cout << " BigBanger -> try to bang not nuclei " << G4endl;
  }; 

  return output;
}		     

G4std::vector<G4InuclElementaryParticle>  	    
G4BigBanger::generateBangInSCM(G4double etot, 
			       G4double a, 
			       G4double z, 
			       G4double mp,
			       G4double mn) const {

  if (verboseLevel > 3) {
    G4cout << " >>> G4BigBanger::generateBangInSCM" << G4endl;
  }

  const G4double ang_cut = 0.9999;
  const G4int itry_max = 1000;
  
  G4int ia = int(a + 0.1);
  G4int iz = int(z + 0.1);

  if (verboseLevel > 2) {
    G4cout << " ia " << ia << " iz " << iz << G4endl;
  }
  G4std::vector<G4InuclElementaryParticle> particles;
  
  if(ia == 1) {
    // abnormal situation
    G4double m = iz > 0 ? mp : mn;
    G4double pmod = sqrt((etot + 2.0 * m) * etot);
    G4std::vector<G4double> mom(4);
    G4std::pair<G4double, G4double> COS_SIN = randomCOS_SIN();
    G4double FI = randomPHI();
    G4double Pt = pmod * COS_SIN.second;

    mom[1] = Pt * cos(FI);
    mom[2] = Pt * sin(FI);
    mom[3] = Pt * COS_SIN.first;    

    G4int knd = iz > 0 ? 1 : 2;

    particles.push_back(G4InuclElementaryParticle(mom, knd));

    return particles;
  };  
     
  G4std::vector<G4double> pmod = generateMomentumModules(etot, a, z, mp, mn);
  G4bool bad = true;
  G4int itry = 0;

  while(bad && itry < itry_max) {
    itry++;
    G4std::vector<G4std::vector<G4double> > scm_momentums;
    G4std::vector<G4double> tot_mom(4);

    if(ia == 2) {
      G4std::vector<G4double> mom(4);
      G4std::pair<G4double, G4double> COS_SIN = randomCOS_SIN();
      double FI = randomPHI();
      double Pt = pmod[0] * COS_SIN.second;

      mom[1] = Pt * cos(FI);
      mom[2] = Pt * sin(FI);
      mom[3] = Pt * COS_SIN.first;    

      for(G4int j = 1; j < 4; j++) tot_mom[j] += mom[j];		 

      scm_momentums.push_back(mom);

      G4std::vector<G4double> mom1(4);

      for(G4int i = 1; i < 4; i++) mom1[i] = - mom[i];

      scm_momentums.push_back(mom1);  
      bad = false;
    }
    else {
      for(G4int i = 0; i < ia - 2; i++) {
	G4std::vector<G4double> mom(4);
	G4std::pair<G4double, G4double> COS_SIN = randomCOS_SIN();
	G4double FI = randomPHI();
	G4double Pt = pmod[i] * COS_SIN.second;

	mom[1] = Pt * cos(FI);
	mom[2] = Pt * sin(FI);
	mom[3] = Pt * COS_SIN.first;    

	for(G4int j = 1; j < 4; j++) tot_mom[j] += mom[j];		 

	scm_momentums.push_back(mom);
      };

      //                handle last two
      G4double tot_mod = sqrt(tot_mom[1] * tot_mom[1] + 
			      tot_mom[2] * tot_mom[2] + 
			      tot_mom[3] * tot_mom[3]); 
      G4double ct = -0.5 * (tot_mod * tot_mod + pmod[ia - 2] * pmod[ia - 2] -
			    pmod[ia - 1] * pmod[ia - 1]) / tot_mod / pmod[ia - 2];

      if (verboseLevel > 2) {
	G4cout << " ct last " << ct << G4endl;
      }
  
      if(fabs(ct) < ang_cut) {
	G4std::vector<G4double> mom2 = generateWithFixedTheta(ct, pmod[ia - 2]);
	//       rotate to the normal system
	G4std::vector<G4double> apr = tot_mom;
	G4int i;
	for(i = 1; i < 4; i++) apr[i] /= tot_mod;
	G4double a_tr = sqrt(apr[1] * apr[1] + apr[2] * apr[2]);
	G4std::vector<G4double> mom(4);
	mom[1] = mom2[3] * apr[1] + ( mom2[1] * apr[2] + mom2[2] * apr[3] * apr[1]) / a_tr; // ::: replace with clhep tools?
	mom[2] = mom2[3] * apr[2] + (-mom2[1] * apr[1] + mom2[2] * apr[3] * apr[2]) / a_tr;      
	mom[3] = mom2[3] * apr[3] - mom2[2] * a_tr;      
	scm_momentums.push_back(mom);
	//               and the last one
	G4std::vector<G4double> mom1(4);
	for(i = 1; i < 4; i++) mom1[i] = - mom[i] - tot_mom[i];
	scm_momentums.push_back(mom1);  
	bad = false;
      };
    };   
    if(!bad) {
      for(G4int i = 0; i < ia; i++) {
	G4int knd = i < iz ? 1 : 2;

	particles.push_back(G4InuclElementaryParticle(scm_momentums[i], knd));
      };
    };
  };  
  if (verboseLevel > 2) {
    if(itry == itry_max) G4cout << " BigBanger -> can not generate bang " << G4endl;
  }

  return particles;
  
}
	   
G4std::vector<G4double> G4BigBanger::generateMomentumModules(G4double etot, 
							     G4double a, 
							     G4double z, 
							     G4double mp, 
							     G4double mn) const {


  if (verboseLevel > 3) {
    G4cout << " >>> G4BigBanger::generateMomentumModules" << G4endl;
  }

  G4int ia = int(a + 0.1);
  G4int iz = int(z + 0.1);
  G4std::vector<G4double> pmod;
  G4double xtot = 0.0;
  G4double promax = maxProbability(a);
  
  G4int i;
  for(i = 0; i < ia; i++) { 
    G4double x = generateX(ia, a, promax);

    if (verboseLevel > 2) {
      G4cout << " i " << i << " x " << x << G4endl;
    }
    pmod.push_back(x);
    xtot += x;
  };
  for(i = 0; i < ia; i++) {
    G4double m = i < iz ? mp : mn;

    pmod[i] = pmod[i] * etot / xtot;
    pmod[i] = sqrt(pmod[i] * (pmod[i] + 2.0 * m));

    if (verboseLevel > 2) {
      G4cout << " i " << i << " pmod " << pmod[i] << G4endl;
    }
  };

  return pmod;  
}

G4double G4BigBanger::xProbability(G4double x, 
				   G4int ia) const {


  if (verboseLevel > 3) {
    G4cout << " >>> G4BigBanger::xProbability" << G4endl;
  }

  G4int ihalf = ia / 2;
  G4double ekpr = 0.0;

  if(x < 1.0 || x > 0.0) {
    ekpr = x * x;

    if(2 * ihalf == ia) { // even A
      ekpr *= sqrt(1.0 - x) * pow((1.0 - x), int((3 * ia - 6) / 2)); 
    }
    else {
      ekpr *= pow((1.0 - x), int((3 * ia - 5) / 2));
    };
  }; 
  
  return ekpr;
}

G4double G4BigBanger::maxProbability(G4double a) const {

  if (verboseLevel > 3) {
    G4cout << " >>> G4BigBanger::maxProbability" << G4endl;
  }

  return xProbability(1.0 / (a - 1.0) / 1.5, int(a + 0.1));
}

G4double G4BigBanger::generateX(G4int ia, 
				G4double a, 
				G4double promax) const {

  if (verboseLevel > 3) {
    G4cout << " >>> G4BigBanger::generateX" << G4endl;
  }

  const G4int itry_max = 1000;
  G4int itry = 0;
  G4double x;
  
  while(itry < itry_max) {
    itry++;
    x = inuclRndm();

    if(xProbability(x, ia) >= promax * inuclRndm()) return x;
  };
  if (verboseLevel > 2) {
    G4cout << " BigBanger -> can not generate x " << G4endl;
  }

  return maxProbability(a);
}
