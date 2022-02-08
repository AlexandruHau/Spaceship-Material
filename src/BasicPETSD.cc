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

#include "BasicPETSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

//

BasicPETSD::BasicPETSD(
                            const G4String& name,
                            const G4String& hitsCollectionName)
 : G4VSensitiveDetector(name),
   fHitsCollection(nullptr)
{
  collectionName.insert(hitsCollectionName);
}

//

BasicPETSD::~BasicPETSD()
{
}

//

void BasicPETSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection
  fHitsCollection
    = new BasicPETHitsCollection(SensitiveDetectorName, collectionName[0]);

  // Add this collection in hce
  auto hcID
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection );

  // Create hits
  for (G4int i=0; i<2; i++ ) {
    fHitsCollection->insert(new BasicPETHit());
  }
}

//

G4bool BasicPETSD::ProcessHits(G4Step* step,
                                     G4TouchableHistory*)
{
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();
  
  // Invoke analysisManager instance
  auto analysisManager = G4AnalysisManager::Instance();

  if(0.0 < edep) {

    // Get spatial coordinates of the hits. The method originates
    // from G4Step() class.
    G4ThreeVector p1 = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector p2 = step->GetPostStepPoint()->GetPosition();
    
    // Check if these are primary particles through GetTrackID()
    // method. This method belongs to G4Track() class
    if(step->GetTrack()->GetTrackID() == 1){
      
      // Fill histograms for particle 1 parameters. Both histograms
      // and Ntuples are filled, in fact
      analysisManager->FillH1(2, p1.getX());
      analysisManager->FillH1(4, p1.getY());
      analysisManager->FillH1(6, p1.getZ());
      
      analysisManager->FillNtupleDColumn(2, p1.getX());
      analysisManager->FillNtupleDColumn(4, p1.getY());
      analysisManager->FillNtupleDColumn(6, p1.getZ());
      
      // Print statement check method
      G4cout << "Hit 1 - x: " << G4BestUnit(p1.getX(), "Length") << G4endl;
      
      }
     
    // Same procedures are carried out for the second primary particle 
    if(step->GetTrack()->GetTrackID() == 2 && p1.getR() > 0){
    
      analysisManager->FillH1(3, p1.getX());
      analysisManager->FillH1(5, p1.getY());
      analysisManager->FillH1(7, p1.getZ());
      
      analysisManager->FillNtupleDColumn(3, p1.getX());
      analysisManager->FillNtupleDColumn(5, p1.getY());
      analysisManager->FillNtupleDColumn(7, p1.getZ());
      }
  }
  // step length
  G4double stepLength = 0.;
  if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
    stepLength = step->GetStepLength();
  }

  if ( edep==0. && stepLength == 0. ) return false;

  auto touchable = (step->GetPreStepPoint()->GetTouchable());

  auto layerNumber = touchable->GetReplicaNumber(1);

  // Get hit accounting data
  auto hit = (*fHitsCollection)[layerNumber];
  if ( ! hit ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hit " << layerNumber;
    G4Exception("BasicPETSD::ProcessHits()",
      "MyCode0004", FatalException, msg);
  }

  // Get hit for total accounting
  auto hitTotal
    = (*fHitsCollection)[fHitsCollection->entries()-1];

  // Add values
  hit->Add(edep, stepLength);
  hitTotal->Add(edep, stepLength);
  
  // analysisManager->AddNtupleRow();

  return true;
}

//

void BasicPETSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) {
     auto nofHits = fHitsCollection->entries();
     G4cout
       << G4endl
       << "-------->Hits Collection: in this event there are " << nofHits
       << " hits in the detector: " << G4endl;
     for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
  }
}

//

