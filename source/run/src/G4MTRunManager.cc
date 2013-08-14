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

#include "G4MTRunManager.hh"
#include "G4MTRunManagerKernel.hh"
#include "G4Timer.hh"
#include "G4ScoringManager.hh"
#include "G4TransportationManager.hh"
#include "G4VUserActionInitialization.hh"
#include "G4UserWorkerInitialization.hh"
#include "G4WorkerThread.hh"
#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4AutoLock.hh"
#include "G4WorkerRunManager.hh"
#include "G4UserRunAction.hh"
#include "G4ProductionCutsTable.hh"
#include "G4Timer.hh"
#include "G4RNGHelper.hh"

G4ScoringManager* G4MTRunManager::masterScM = 0;
G4MTRunManager::masterWorlds_t G4MTRunManager::masterWorlds = G4MTRunManager::masterWorlds_t();
G4MTRunManager* G4MTRunManager::fMasterRM = 0;

namespace {
 G4Mutex cmdHandlingMutex = G4MUTEX_INITIALIZER;
 G4Mutex scorerMergerMutex = G4MUTEX_INITIALIZER;
 G4Mutex runMergerMutex = G4MUTEX_INITIALIZER;
 G4Mutex setUpEventMutex = G4MUTEX_INITIALIZER;
}

//This is needed to initialize windows conditions
#if defined(WIN32)
namespace {
	void InitializeWindowsConditions();
}
#endif

G4MTRunManager* G4MTRunManager::GetMasterRunManager()
{
////////#ifdef G4MULTITHREADED
    return fMasterRM;
////////#else
////////    return G4RunManager::GetRunManager();
////////#endif
}

G4RunManagerKernel* G4MTRunManager::GetMasterRunManagerKernel()
{
    return fMasterRM->kernel;
}

G4MTRunManagerKernel* G4MTRunManager::GetMTMasterRunManagerKernel()
{
    return fMasterRM->MTkernel;
}

G4MTRunManager::G4MTRunManager() : G4RunManager(masterRM),
    nworkers(2),
    masterRNGEngine(0),
    nextActionRequest(UNDEFINED)
{
    if ( fMasterRM )
    {
        G4Exception("G4MTRunManager::G4MTRunManager", "Run0035",FatalException,
                    "Another instance of a G4MTRunManager already exists.");
    }
    fMasterRM = this;
    MTkernel = static_cast<G4MTRunManagerKernel*>(kernel);
#ifndef G4MULTITHREADED
    G4ExceptionDescription msg;
    msg << "Geant4 code is compiled without multi-threading support"
        << "(-DG4MULTITHREADED is set to off).\n";
    msg << "G4MTRunManager can only be used in multi-threaded applications.";
    G4Exception("G4MTRunManager::G4MTRunManager","Run0035",FatalException,msg);
#endif

    G4int numberOfStaticAllocators = kernel->GetNumberOfStaticAllocators();
    if(numberOfStaticAllocators>0)
    {
      G4ExceptionDescription msg1;
      msg1 << "There are " << numberOfStaticAllocators
           << " static G4Allocator objects detected.\n"
           << "In multi-threaded mode, all G4Allocator objects must be dynamicly instantiated.";
      G4Exception("G4MTRunManager::G4MTRunManager","Run1035",FatalException,msg1);
    }
    G4UImanager::GetUIpointer()->SetMasterUIManager(true);
    masterScM = G4ScoringManager::GetScoringManagerIfExist();

    //Check if a default RandomNumberGenerator has been created by user,
    // if not create default one
    //Note this call forces creation of defaults if not already there
    //G4Random::getTheEngine(); //User did not specify RNG, create defaults
    //Now remember the master instance of the RNG Engine
    masterRNGEngine = G4Random::getTheEngine();
#if defined (WIN32)
    InitializeWindowsConditions();
#endif

    numberOfEventToBeProcessed = 0;
}

G4MTRunManager::~G4MTRunManager()
{
    //TODO: Currently does not work due to concurrent deletion of something
    //      that is shared:
    //G4ProcessTable::DeleteMessenger from ~G4RunManager
    //G4cout<<"Destroy MTRunManager"<<G4endl;//ANDREA
    TerminateWorkers();
}

void G4MTRunManager::StoreRNGStatus(const G4String& fn )
{
    std::ostringstream os;
    os << randomNumberStatusDir << "G4Master_"<<fn <<".rndm";
    G4Random::saveEngineStatus(os.str().c_str());
}

void G4MTRunManager::SetNumberOfThreads(G4int n )
{
    if ( threads.size() != 0 )
    {
      G4ExceptionDescription msg;
      msg << "Limitation: Number of threads cannot be changed at this moment \n"
          << "(old threads are still alive).";
      G4Exception("G4MTRunManager::SetNumberOfThreads(G4int)",
                  "Run0035", JustWarning, msg);
    }
    else
    {
      nworkers = n;
    }
}

void G4MTRunManager::TerminateEventLoop()
{
    //Nothing to do
}
void G4MTRunManager::ProcessOneEvent(G4int)
{
    //Nothing to do
}
void G4MTRunManager::TerminateOneEvent()
{
    //Nothing to do
}

void G4MTRunManager::PrepareCommandsStack() {
    G4AutoLock l(&cmdHandlingMutex);
    uiCmdsForWorkers.clear();
    std::vector<G4String>* cmdCopy = G4UImanager::GetUIpointer()->GetCommandStack();
    for ( std::vector<G4String>::const_iterator it = cmdCopy->begin() ;
         it != cmdCopy->end(); ++it )
        uiCmdsForWorkers.push_back(*it);
    cmdCopy->clear();
    delete cmdCopy;
}

std::vector<G4String> G4MTRunManager::GetCommandStack()
{
    G4AutoLock l(&cmdHandlingMutex);
    return uiCmdsForWorkers;
}

void G4MTRunManager::InitializeEventLoop(G4int n_events, const char* macroFile, G4int n_select)
{
    numberOfEventToBeProcessed = n_events;
    numberOfEventProcessed = 0;

    if(verboseLevel>0)
    { timer->Start(); }
    
    n_select_msg = n_select;
    if(macroFile!=0)
    {
        if(n_select_msg<0) n_select_msg = n_events;
        msgText = "/control/execute ";
        msgText += macroFile;
    }
    else
    { n_select_msg = -1; }

    //initialize seeds
    //If user did not implement InitializeSeeds, use default: 2 seeds per event 
    if ( InitializeSeeds(n_events) == false )
    {
        G4RNGHelper* helper = G4RNGHelper::GetInstance();
        for ( G4int ne = 0 ; ne < n_events*2 ; ++ne)
            helper->AddOneSeed( (long) (100000000L * G4Random::getTheGenerator()->flat()) );
    }
    
    //Now initialize workers. Check if user defined a WorkerInitialization
    if ( userWorkerInitialization == 0 )
    { userWorkerInitialization = new G4UserWorkerInitialization(); }
    
    //Prepare UI commands for threads
    PrepareCommandsStack();
    //Now loop on requested number of workers
    //This will also start the workers
    //Currently we do not allow to change the
    //number of threads: threads area created once
    if ( threads.size() == 0 ) {
      for ( G4int nw = 0 ; nw<nworkers; ++nw) {
        //Create a new worker and remember it
        G4WorkerThread* context = new G4WorkerThread;
        context->SetNumberThreads(nworkers);
        context->SetThreadId(nw);
        G4Thread* thread = userWorkerInitialization->CreateAndStartWorker(context);
        threads.push_back(thread);
      }
    }
    //Signal to threads they can start a new run
    NewActionRequest(NEXTITERATION);
    
    // We need a barrier here. Wait for workers to start event loop.
    //This will return only when all workers have started processing events.
    WaitForReadyWorkers();
}

void G4MTRunManager::RunTermination()
{
    //Wait for all worker threads to have finished the run
    //i.e. wait for them to return from RunTermination()
    //This guarantee that userrunaction for workers has been called

    // Wait now for all threads to finish event-loop
    WaitForEndEventLoopWorkers();
    //Now call base-class methof
    G4RunManager::RunTermination();
}


void G4MTRunManager::ConstructScoringWorlds()
{
    masterScM = G4ScoringManager::GetScoringManagerIfExist();
    //Call base class stuff...
    G4RunManager::ConstructScoringWorlds();
    
    masterWorlds.clear();
    size_t nWorlds = G4TransportationManager::GetTransportationManager()->GetNoWorlds();
    std::vector<G4VPhysicalVolume*>::iterator itrW
                   = G4TransportationManager::GetTransportationManager()->GetWorldsIterator();
    for(size_t iWorld=0;iWorld<nWorlds;iWorld++)
    { addWorld(iWorld,*itrW); itrW++; }

}

void G4MTRunManager::SetUserInitialization(G4UserWorkerInitialization* userInit)
{
  userWorkerInitialization = userInit;
}

void G4MTRunManager::SetUserInitialization(G4VUserActionInitialization* userInit)
{
  userActionInitialization = userInit;
  userActionInitialization->BuildForMaster();
}

void G4MTRunManager::SetUserInitialization(G4VUserPhysicsList *userPL)
{
    G4RunManager::SetUserInitialization(userPL);
    //Needed for MT, to be moved in kernel 
}

void G4MTRunManager::SetUserInitialization(G4VUserDetectorConstruction *userDC)
{
    G4RunManager::SetUserInitialization(userDC);
}

void G4MTRunManager::SetUserAction(G4UserRunAction* userAction)
{
    userRunAction = userAction; 
    userRunAction->SetMaster();
}

void G4MTRunManager::SetUserAction(G4VUserPrimaryGeneratorAction* /*userAction*/)
{
  G4Exception("G4MTRunManager::SetUserAction()", "Run3011", FatalException,
    "For multi-threaded version, define G4VUserPrimaryGeneratorAction in G4VUserActionInitialization.");
}

void G4MTRunManager::SetUserAction(G4UserEventAction* /*userAction*/)
{
  G4Exception("G4MTRunManager::SetUserAction()", "Run3011", FatalException,
    "For multi-threaded version, define G4UserEventAction in G4VUserActionInitialization.");
}

void G4MTRunManager::SetUserAction(G4UserStackingAction* /*userAction*/)
{
  G4Exception("G4MTRunManager::SetUserAction()", "Run3011", FatalException,
    "For multi-threaded version, define G4UserStackingAction in G4VUserActionInitialization.");
}

void G4MTRunManager::SetUserAction(G4UserTrackingAction* /*userAction*/)
{
  G4Exception("G4MTRunManager::SetUserAction()", "Run3011", FatalException,
    "For multi-threaded version, define G4UserTrackingAction in G4VUserActionInitialization.");
}

void G4MTRunManager::SetUserAction(G4UserSteppingAction* /*userAction*/)
{
  G4Exception("G4MTRunManager::SetUserAction()", "Run3011", FatalException,
    "For multi-threaded version, define G4UserSteppingAction in G4VUserActionInitialization.");
}

void G4MTRunManager::MergeScores(const G4ScoringManager* localScoringManager)
{
  G4AutoLock l(&scorerMergerMutex);
  if(masterScM) masterScM->Merge(localScoringManager); 
}

void G4MTRunManager::MergeRun(const G4Run* localRun)
{
  G4AutoLock l(&runMergerMutex);
  if(currentRun) currentRun->Merge(localRun); 
}

G4bool G4MTRunManager::SetUpAnEvent(G4Event* evt, CLHEP::HepRandomEngine * egn)
{
  G4AutoLock l(&setUpEventMutex);
  if( numberOfEventProcessed < numberOfEventToBeProcessed )
  {
    evt->SetEventID(numberOfEventProcessed);
    G4RNGHelper* helper = G4RNGHelper::GetInstance();
    G4int idx_rndm = 2*numberOfEventProcessed;
    G4long seeds[3] = { helper->GetSeed(idx_rndm), helper->GetSeed(idx_rndm+1), 0 };
    egn->setSeeds(seeds,-1);
    numberOfEventProcessed++;
    return true;
  }
  return false;
}

void G4MTRunManager::TerminateWorkers()
{
    NewActionRequest( ENDWORKER );
    //Now join threads.
#ifdef G4MULTITHREADED //protect here to prevent warning in compilation
    while ( ! threads.empty() )
    {
        G4Thread* t = * ( threads.begin() );
        threads.pop_front();
        userWorkerInitialization->JoinWorker(t);
        //G4THREADJOIN(*t);
        delete t;
    }
#endif
    threads.clear();
}

#include "G4IonTable.hh"
#include "G4ParticleTable.hh"
#include "G4CascadeInterface.hh"

void G4MTRunManager::InitializePhysics()
{
    G4RunManager::InitializePhysics();
    //G4ParticleTable::GetParticleTable()->GetIonTable()->CreateAllIon();
    //G4ParticleTable::GetParticleTable()->GetIonTable()->CreateAllIsomer();
    //BERTINI, this is needed to create pseudo-particles, to be removed
    G4CascadeInterface::Initialize();
}

// =====================================
// Barriers mechanism
// =====================================
// We want to implement barriers.
// We define a barrier has a point in which threads synchronize.
// When workers threads reach a barrier they wait for the master thread a
// signal that they can continue. The master thread broadcast this signal
// only when all worker threads have reached this point.
// Currently only three points require this sync in the life-time of a G4 applicattion:
// Just before and just after the for-loop controlling the thread event-loop.
// Between runs.
// TODO: If this mechanism is needed in other parts of the code we can provide
// the barrier mechanism as a utility class/functions to the kernel.
// Note: we need a special treatment for WIN32
//
// The basic algorith of each barrier works like this:
// In the master:
//   WaitWorkers()  {
//    while (true) 
//    {
//     G4AutoLock l(&counterMutex);
//     if ( counter == nActiveThreads ) break;
//     G4CONDITIONWAIT( &conditionOnCounter, &counterMutex);
//    }
//    G4CONDITIONBROADCAST( &doSomethingCanStart );
//   }
// In the workers:
//   WaitSignalFromMaster() {
//    G4AutoLock l(&counterMutex);
//    ++counter;
//    G4CONDITIONBROADCAST(&conditionOnCounter);
//    G4CONDITIONWAIT( &doSomethingCanStart , &counterMutex);
//   }
// Each barriers requires 2 conditions and one mutex, and a counter.

#ifdef WIN32
#include <windows.h> //For CRITICAL_SECTION objects
#endif
namespace {
    //Avoid compilation warning if squenetial for unused variables
#ifdef G4MULTITHREADED
    //Conditions
    // Condition to signal green light for start of event loop
    G4Condition beginEventLoopCondition = G4CONDITION_INITIALIZER;
    // Condition to signal green light to finish event loop
    // (actuallyt exit function performing event loop)
    G4Condition endEventLoopCondition = G4CONDITION_INITIALIZER;
    // Condition to signal the num of workers ready for event loop has changed
    G4Condition numWorkersBeginEventLoopChangeCondition = G4CONDITION_INITIALIZER;
    // Condition to signal the num of workers that terminated event loop
    // has changed
    G4Condition numWorkersEndEventLoopChangedCondition = G4CONDITION_INITIALIZER;
    // This condition is to handle more than one run w/o killing threads
    G4Condition requestChangeActionForWorker = G4CONDITION_INITIALIZER;
    G4Condition numberOfReadyWorkersForNewActionChangedCondition = G4CONDITION_INITIALIZER;
#endif
    // Counter/mutex for workers ready to begin event loop
    G4Mutex numberOfReadyWorkersMutex = G4MUTEX_INITIALIZER;
    G4int numberOfReadyWorkers = 0;
    //Counter/mutex for workers with end of event loop
    G4Mutex numberOfEndOfEventLoopWorkersMutex = G4MUTEX_INITIALIZER;
    G4int numberOfEndOfEventLoopWorkers = 0;
    //
    //Action handling
    G4Mutex nextActionRequestMutex = G4MUTEX_INITIALIZER;
    G4int numberOfReadyWorkersForNewAction = 0;
    G4Mutex numberOfReadyWorkersForNewActionMutex = G4MUTEX_INITIALIZER;
#ifdef WIN32
    CRITICAL_SECTION cs1;
    CRITICAL_SECTION cs2;
    CRITICAL_SECTION cs3;
    //Note we need to use two separate counters because
    //we can get a situation in which a thread is much faster then the others
    //(for example if last thread has less events to process.
    //We have the extreme case of some medical applications (moving setups)
    //in which the number of events of a run is ~ number of threads
	void InitializeWindowsConditions()
	{
	#ifdef G4MULTITHREADED
           InitializeConditionVariable( &beginEventLoopCondition );
           InitializeConditionVariable( &endEventLoopCondition );
           InitializeConditionVariable( &numWorkersBeginEventLoopChangeCondition );
           InitializeConditionVariable( &numWorkersEndEventLoopChangedCondition );
           InitializeConditionVariable( &requestChangeActionForWorker);
	   InitializeConditionVariable( &numberOfReadyWorkersForNewActionChangedCondition );
	#endif
           InitializeCriticalSection( &cs1 );
           InitializeCriticalSection( &cs2 );
           InitializeCriticalSection( &cs3 );
	}
#endif
}

void G4MTRunManager::WaitForReadyWorkers()
{
    while (true) //begin barrier
    {
#ifndef WIN32
        G4AutoLock lockLoop(&numberOfReadyWorkersMutex);
#else
        EnterCriticalSection( &cs1 );
#endif
        //Check number of workers ready to begin
	G4int activethreads = threads.size();
        if (numberOfReadyWorkers == activethreads )
        {
            //Ok, interrupt the loop
            break;
        }
        //Wait for the number of workers to be changed
        G4CONDITIONWAIT(&numWorkersBeginEventLoopChangeCondition,
                        &numberOfReadyWorkersMutex);
#ifdef WIN32
        LeaveCriticalSection( &cs1 );
#endif
    }
    //Now number of workers is as expected.
    static G4bool createIsomerOnlyOnce = false;
    if(!createIsomerOnlyOnce)
    {
      createIsomerOnlyOnce = true;
      G4ParticleTable::GetParticleTable()->GetIonTable()->CreateAllIsomer();
    }

    //Prepare to wait for workers to end eventloop
    //Reset number of workers in "EndOfEventLoop"
    G4AutoLock l(&numberOfEndOfEventLoopWorkersMutex);
    numberOfEndOfEventLoopWorkers = 0;
    
    //signal workers they can start the event-loop
    G4CONDTIONBROADCAST(&beginEventLoopCondition);
}

void G4MTRunManager::ThisWorkerReady()
{
    //Increament number of active worker by 1
#ifndef WIN32
    G4AutoLock lockLoop(&numberOfReadyWorkersMutex);
#else
    EnterCriticalSection( &cs1 );
#endif
    ++numberOfReadyWorkers;
    //Signal the number of workers has changed
    G4CONDTIONBROADCAST(&numWorkersBeginEventLoopChangeCondition);
    //Wait for condition to start eventloop
    G4CONDITIONWAIT(&beginEventLoopCondition,&numberOfReadyWorkersMutex);
#ifdef WIN32
    LeaveCriticalSection( &cs1 );
#endif
    const_cast<G4PDefManager&>(G4ParticleDefinition::GetSubInstanceManager()).NewSubInstances();
    G4ParticleTable::GetParticleTable()->WorkerG4ParticleTable();
}


void G4MTRunManager::WaitForEndEventLoopWorkers()
{
    while (true)
    {
#ifndef WIN32
        G4AutoLock l(&numberOfEndOfEventLoopWorkersMutex);
#else
        EnterCriticalSection( &cs2 );
#endif
	G4int activethreads = threads.size();
        if ( numberOfEndOfEventLoopWorkers == activethreads )
        {
            break;
        }
        G4CONDITIONWAIT(&numWorkersEndEventLoopChangedCondition,
                        &numberOfEndOfEventLoopWorkersMutex);
#ifdef WIN32
        LeaveCriticalSection( &cs2 );
#endif
    }
    //Now number of workers that reached end of event loop is as expected
    //Reset number of workers in ready for work state so a new run can start
    G4AutoLock l(&numberOfReadyWorkersMutex);
    numberOfReadyWorkers = 0;
    //Signal workers they can end event-loop
    G4CONDTIONBROADCAST(&endEventLoopCondition);
}

void G4MTRunManager::ThisWorkerEndEventLoop()
{
    //Increament number of workers in end of evnet loop by 1
#ifndef WIN32
    G4AutoLock l(&numberOfEndOfEventLoopWorkersMutex);
#else
    EnterCriticalSection( &cs2 );
#endif
    ++numberOfEndOfEventLoopWorkers;
    //Signale this number has changed
    G4CONDTIONBROADCAST(&numWorkersEndEventLoopChangedCondition);
    //Wait for condition to exit eventloop
    G4CONDITIONWAIT(&endEventLoopCondition,&numberOfEndOfEventLoopWorkersMutex);
#ifdef WIN32
    LeaveCriticalSection( &cs2 );
#endif
}

void G4MTRunManager::NewActionRequest(G4MTRunManager::WorkerActionRequest newRequest)
{
  //Wait for all workers to be ready to accept a new action request
  while (true)
  {
#ifndef WIN32
    G4AutoLock l(&numberOfReadyWorkersForNewActionMutex);
#else
    EnterCriticalSection( &cs3 );
#endif
    //Check the number of workers that are ready for next action
    G4int activethreads = threads.size();
    if ( numberOfReadyWorkersForNewAction == activethreads )
    {
	//Ok, exit the loop
	break;
    }
    //Wait for the number of workers ready for new action to change
    G4CONDITIONWAIT(&numberOfReadyWorkersForNewActionChangedCondition,
		    &numberOfReadyWorkersForNewActionMutex);
#ifdef WIN32
    LeaveCriticalSection( &cs3 );
#endif 
  }
  //Now set the new action to the shared resource
  G4AutoLock l(&nextActionRequestMutex);
  nextActionRequest = newRequest;
  l.unlock();
  //Reset counter of workers ready-for-new-action in preparation of next call
  G4AutoLock l2(&numberOfReadyWorkersForNewActionMutex);
  numberOfReadyWorkersForNewAction = 0;
  l2.unlock();
  //Now wignal all workers that there is a new action to be performed
  G4CONDTIONBROADCAST(&requestChangeActionForWorker);
}

G4MTRunManager::WorkerActionRequest G4MTRunManager::ThisWorkerWaitForNextAction()
{
  //This worker is ready to receive a new action request, 
  //increment counter by 1
 #ifndef WIN32
  G4AutoLock l(&numberOfReadyWorkersForNewActionMutex);
#else
  EnterCriticalSection( &cs3 );
#endif
  ++numberOfReadyWorkersForNewAction;
  //Singal the sahred resource has changed to the master
  G4CONDTIONBROADCAST(&numberOfReadyWorkersForNewActionChangedCondition);
  //Wait for condition that a new aciton is ready
  G4CONDITIONWAIT(&requestChangeActionForWorker,&numberOfReadyWorkersForNewActionMutex);
#ifdef WIN32
  LeaveCriticalSection( &cs3 );
#endif 
  //Ok, if I am here it means that a new action has been requested by the master
  //reads it value that is now read-only, so no mutex is needed, but you never know...
  G4AutoLock l2(&nextActionRequestMutex);
  WorkerActionRequest result = nextActionRequest;
  return result;
}
