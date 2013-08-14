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

#include "G4WorkerRunManagerKernel.hh"

G4WorkerRunManagerKernel::G4WorkerRunManagerKernel() : G4RunManagerKernel(workerRMK)
{
    //This version of the constructor should never be called in sequential mode!
#ifndef G4MULTITHREADED
    G4ExceptionDescription msg;
    msg<<"Geant4 code is compiled without multi-threading support (-DG4MULTITHREADED is set to off).";
    msg<<" This type of RunManager can only be used in mult-threaded applications.";
    G4Exception("G4RunManagerKernel::G4RunManagerKernel()","Run0035",FatalException,msg);
#endif

}

G4WorkerRunManagerKernel::~G4WorkerRunManagerKernel()
{
}

void G4WorkerRunManagerKernel::SetupShadowProcess() const
{
    //Master thread has created processes and setup a pointer
    //to the master process, get it and copy it in this instance
    G4ParticleTable* theParticleTable = G4ParticleTable::GetParticleTable();
    G4ParticleTable::G4PTblDicIterator* theParticleIterator = theParticleTable->GetIterator();
    theParticleIterator->reset();
    //loop on particles and get process manager from there list of processes
    while((*theParticleIterator)())
    {
        G4ParticleDefinition* pd = theParticleIterator->value();
        G4ProcessManager* pm = pd->GetProcessManager();
        G4ProcessManager* pmM= pd->GetMasterProcessManager();
        if ( !pm || !pmM )
        {
            G4Exception("G4WorkerRunManagerKernel::SetupShadowProcess()","Run0035",FatalException,"Process manager or process manager shadow to master are not set.");
        }
        G4ProcessVector& procs = *(pm->GetProcessList());
        G4ProcessVector& procsM= *(pmM->GetProcessList());
        if( procs.size() != procsM.size() )
        {
          G4ExceptionDescription msg;
          msg<<" Size of G4ProcessVector is inconsistent between master and worker threads ";
          msg<<" for the particle <"<<pd->GetParticleName()<<">. \n";
          msg<<" size of G4ProcessVector for worker thread is "<<procs.size();
          msg<<" while masther thread is "<<procsM.size()<<".";
          G4Exception("G4RunManagerKernel::G4RunManagerKernel()","Run0035",FatalException,msg);
        }
        //assert( procs.size() == procsM.size() );
        //To each process add the reference to the same
        //process from master. Note that we rely on
        //processes being in the correct order!
        // We could use some checking using process name or type
        for ( G4int idx = 0 ; idx < procs.size() ; ++idx )
        {
            procs[idx]->SetMasterProcess(procsM[idx]);
        }
    }
}
