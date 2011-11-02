#include "G4DNAMolecularStepByStepModel.hh"
#include "G4VDNAReactionModel.hh"

G4DNAMolecularStepByStepModel::G4DNAMolecularStepByStepModel() :
    G4VITModel(),
    fMolecularReactionTable(reference_cast<const G4DNAMolecularReactionTable*>(fReactionTable))
{
    fTimeStepper = new G4DNAMoleculeEncounterStepper();
    fReactionProcess = new G4DNAMolecularReaction();

    fType1 = G4Molecule::ITType();
    fType2 = G4Molecule::ITType();
}

G4DNAMolecularStepByStepModel::~G4DNAMolecularStepByStepModel()
{
    if(fReactionModel) delete fReactionModel;
}

G4DNAMolecularStepByStepModel::G4DNAMolecularStepByStepModel(const G4DNAMolecularStepByStepModel& right):
    G4VITModel(right),
    fMolecularReactionTable(reference_cast<const G4DNAMolecularReactionTable*>(fReactionTable))
{
        fReactionTable = right.fReactionTable;
        if(right.fReactionModel)
        {
            fReactionModel = right.fReactionModel->Clone();
            ((G4DNAMolecularReaction*)  fReactionProcess)->SetReactionModel(fReactionModel);
            ((G4DNAMoleculeEncounterStepper*) 	fTimeStepper)->SetReactionModel(fReactionModel);
        }
}

void G4DNAMolecularStepByStepModel::Initialize()
{
    fReactionModel ->SetReactionTable((const G4DNAMolecularReactionTable*)fReactionTable);
    G4VITModel::Initialize();
}