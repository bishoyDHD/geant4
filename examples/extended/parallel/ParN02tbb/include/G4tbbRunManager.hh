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
#ifndef G4TBBRUNMANAGER_HH
#define G4TBBRUNMANAGER_HH

#include "G4RunManager.hh"

class G4tbbTask;
class G4VtbbJob;

//A queue of Seeds. A global concurrent queue
//Ranecu Engine
#include <tbb/task.h>
#include <tbb/concurrent_queue.h>

//Implement TLS for RunManager
#include <tbb/enumerable_thread_specific.h>

class G4tbbRunManager : public G4RunManager {
  friend class G4tbbTask;
public:
  G4tbbRunManager();
  G4tbbRunManager( int isSlaveFlag );
  virtual ~G4tbbRunManager();
  static void AddSeed( const long& seed );
  void SetSeedsSequenceLength( int l ) { seedsSequenceLength = l; }
  void SetTaskList( tbb::task_list* tlist ) { tasklist = tlist; }

  typedef tbb::concurrent_queue<G4tbbRunManager*> G4tbbRunManagerInstancesType;
  static G4tbbRunManagerInstancesType& GetInstancesList(); 
            //This should be "clear" with delete

  void SetJob( G4VtbbJob* j ) { job=j; }
protected:
  //Create tasks (1task = 1event)
  virtual void DoEventLoop(G4int n_event,
                           const char* macroFile = 0,
                           G4int n_select=-1);
private:
  //Process one event. Each task. Returns false if runAborted flag is true
  G4int n_select;
  G4String msg;

  //Returns runAborted status
  G4bool DoOneEvent(G4int i_event);

  //Returns false if seed queue is empty
  static G4bool GetNextSeed( long& seed );

  //One global instance for all threads
  typedef tbb::concurrent_queue< long > G4tbbSeedsQueueType;
  static G4tbbSeedsQueueType seedsQueue;

  //Length, of the seed sequence to be used. Default is 1
  int seedsSequenceLength;

  //G4bool isSlaveInitialized;
  //void InitSlave();

  //Pointer to the list of tasks in which add the new created tasks
  tbb::task_list* tasklist;

  //Global static instance of a queue containing all instances of this objects
  static G4tbbRunManagerInstancesType instancesList;

  //The job configuration object
  G4VtbbJob* job;
public:
  //A global instance of the TLS. Every call to ::local() will create 
  // a TLS instance
  //Of a G4tbbRunManager
  //typedef tbb::enumerable_thread_specific< G4tbbRunManager > 
  //   G4tbbRunManagerTLSType;
  //static G4tbbRunManagerTLSType G4tbbRunManagerTLS;
 };


#endif //G4TBBRUNMANAGER_HH
