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

#include "BasicRunAction.hh"
#include "BasicAnalysis.hh"
#include "BasicDetectorConstruction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//


BasicRunAction::BasicRunAction()
 : G4UserRunAction()

{
  // set printing run number only
  G4RunManager::GetRunManager()->SetPrintProgress(0);

  // Create analysis manager
  // The choice of analysis technology is done via selection of a namespace
  // in BasicAnalysis.hh
}

//

BasicRunAction::~BasicRunAction()
{
  delete G4AnalysisManager::Instance();
}

void BasicRunAction::BeginOfRunAction(const G4Run* run)
{

  auto analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  // Create directories
  analysisManager->SetHistoDirectoryName("histograms");
  analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);
  // Note: merging ntuples is available only with Root output
  
  // Creating histograms for the spatial coordinates and energies of the
  // first two hits indexed 1 and 2, as well as for the energy deposited
  // inside the detector and phantom. An additional histogram is created
  // for the square root of y**2 + z**2 to check the distribution of the
  // hits for Case 1 PrimaryGeneratorAction.cc file
  
  analysisManager->CreateH1("E_detector","Energy Deposited", 50, 0.,1.25*MeV);
  analysisManager->CreateH1("E_Phantom", "Energy in phantom", 50, 0., 1.25*MeV);
  analysisManager->CreateH1("x1","x-position in Detector hit 1", 50,0.,10.0*cm);
  analysisManager->CreateH1("x2","x-position in Detector hit 2", 50,0.,10.0*cm);
  analysisManager->CreateH1("y1","y-position in Detector hit 1", 50, 0., 10.0*cm);
  analysisManager->CreateH1("y2","y-position in Detector hit 2", 50, 0., 10.0*cm);
  analysisManager->CreateH1("z1","z-position in Detector hit 1", 50, 0., 10.0*cm);
  analysisManager->CreateH1("z2","z-position in Detector hit 2", 50, 0., 10.0*cm);
  analysisManager->CreateH1("E1", "Energy of hit 1", 50, 0., 1.25*MeV);
  analysisManager->CreateH1("E2", "Energy of hit 2", 50, 0., 1.25*MeV);

  // Creating ntuple for the same parameters
  analysisManager->CreateNtuple("BasicWithGauss", "Edep spacial distribution");
  analysisManager->CreateNtupleDColumn("E_detector");
  analysisManager->CreateNtupleDColumn("E_Phantom");  
  analysisManager->CreateNtupleDColumn("x21");
  analysisManager->CreateNtupleDColumn("x22");
  analysisManager->CreateNtupleDColumn("y21");
  analysisManager->CreateNtupleDColumn("y22");
  analysisManager->CreateNtupleDColumn("z21");
  analysisManager->CreateNtupleDColumn("z22");
  analysisManager->CreateNtupleDColumn("E1");
  analysisManager->CreateNtupleDColumn("E2");
  analysisManager->FinishNtuple();

  // Reset the GoodEvent counter
  Reset();

  // Open an output file featuring the runID
  G4int runid = run->GetRunID();
  G4String fileName = "BasicWithGauss" + std::to_string(runid);
  analysisManager->OpenFile(fileName);

}

//

void BasicRunAction::EndOfRunAction(const G4Run* run)
{

  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  // print histogram statistics

  auto analysisManager = G4AnalysisManager::Instance();
  if ( analysisManager->GetH1(1) ) {

    G4int goodEvents = GoodEventCount;
    G4double sensitivity = (G4double(goodEvents)/nofEvents) * 100;

    G4cout << " Detector length: " << DetLength << " m " << G4endl;
    G4cout << " Crystal length: " << CrystLength << " cm " << G4endl;
    G4cout << " Good events: " << goodEvents << G4endl;
    G4cout << " Crude sensitivity: " << std::setprecision(5) << sensitivity << " per cent " << G4endl;


    std::ofstream myfile;
    myfile.open("Data.csv", std::ofstream::app);
    //myfile << std::to_string(CrystLength)+", "+std::to_string(sensitivity) +"\n";
    myfile << std::to_string(DetLength)+", "+std::to_string(CrystLength)+", "+std::to_string(sensitivity) +"\n";
    myfile.close();

  }


  // save histograms & ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();
}

//
