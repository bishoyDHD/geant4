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
//
// $Id$
//
//
//  Last Modification : 02/Feb/96 M.Asai
//


#ifndef G4StackedTrack_h
#define G4StackedTrack_h 1

class G4VTrajectory;
class G4Track;

// class description:
//
// This class is exclusively used by G4StackManager and G4TrackStack
// classes for storing a G4Track object.

class G4StackedTrack
{
public:
  G4StackedTrack() : track(0), trajectory(0) {}
  G4StackedTrack(G4Track* aTrack, G4VTrajectory* aTraj = 0) : track(aTrack), trajectory(aTraj) {}
  ~G4StackedTrack() {}
  
private:
  G4Track* track;
  G4VTrajectory* trajectory;
  
public:
  G4Track* GetTrack() const { return track; }
  G4VTrajectory* GetTrajectory() const { return trajectory; }
};

#endif

