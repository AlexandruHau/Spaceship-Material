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

#include "BasicDetectorConstruction.hh"
#include "BasicPETSD.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4AutoDelete.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSDoseDeposit.hh"

#include "G4SDManager.hh"
#include "G4GenericMessenger.hh"
#include "G4RunManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//

//G4ThreadLocal

//

BasicDetectorConstruction::BasicDetectorConstruction()
 : G4VUserDetectorConstruction(),
   fCheckOverlaps(true)
{
}

//

BasicDetectorConstruction::~BasicDetectorConstruction()
{
}

//

G4VPhysicalVolume* BasicDetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//

void BasicDetectorConstruction::DefineMaterials()
{
  G4NistManager* man = G4NistManager::Instance();

  G4bool isotopes = false;

  G4Element*  O = man->FindOrBuildElement("O" , isotopes);
  G4Element* Si = man->FindOrBuildElement("Si", isotopes);
  G4Element* Lu = man->FindOrBuildElement("Lu", isotopes);

  G4Material* LSO = new G4Material("Lu2SiO5", 7.4*g/cm3, 3);
  LSO->AddElement(Lu, 2);
  LSO->AddElement(Si, 1);
  LSO->AddElement(O , 5);
}

//

G4VPhysicalVolume* BasicDetectorConstruction::DefineVolumes()
{
  // Initialize the pointer which directs the user to the selection of the materials.
  // Do this by invoking the G4NistManager pointer
  G4NistManager* nist = G4NistManager::Instance();
  
  // Choose now material for the overall physical world. This material should
  // correspond to the overall intergalactic gas of very low pressure and the
  // temperature of 2.73K
  G4Material* space_mat = nist->FindOrBuildMaterial("G4_Galactic");
  
  // Select now the material for the space shield. The purpose of these 
  // Monte Carlo Simulations is to choose which material, or which combination
  // of materials is best suited
  G4Material* shield_mat   = nist->FindOrBuildMaterial("G4_Pb");
  
  // Now select the material for the inner chamber inside the space shield.
  // This should correspond to the air at normal pressure and temperature
  // breathed by the human population on Earth. Hence choose the values of
  // 101 325 Pa and 300K
  G4Material* chamber_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  // Lastly, select the required material for the phantom (human body). The
  // choice can be of either bone or water structure, let us proceed only 
  // with one of them for the moment.
  G4Material* phantom_mat = nist->FindOrBuildMaterial("G4_MUSCLE_STRIATED_ICRU");

  // World size dimension is declared: Recreate the physical world as a cube
  // of size world_dim. For the example below, the world stretches from -5 to
  // 5 meters.
  G4double world_dim = 10*m;
  G4Box* solidWorld = new G4Box("World", 0.5*world_dim, 0.5*world_dim, 0.5*world_dim);

  // Fill the physical world with the intergalactic gas
  G4LogicalVolume* logicWorld =
  new G4LogicalVolume(solidWorld,
                      space_mat,
                      "World");

  // Place the physical world at the center of the world
  G4VPhysicalVolume* physWorld =
  new G4PVPlacement(0,                    // no rotation
                      G4ThreeVector(),    // set origin
                      logicWorld,         // logical volume
                      "World",            // name
                      0,                  // mother volume
                      false,              // boolean
                      0,                  // copy number
                      fCheckOverlaps);    // checks for volume overlaps

  // Now create the shield for the spacecraft. This should be built using one
  // or more of the given materials in the world. The purpose of the simulation
  // is to inspect which material represents the best choice for protection 
  // against the outer Galactic Cosmic Rays (GCR radiation). Represent the shield
  // through a cube of smaller dimensions - choose 4 meters as a start
                                          
  // Now create the shield for the spacecraft. This should be built using one
  // or more of the given materials in the world. The purpose of the simulation
  // is to inspect which material represents the best choice for protection 
  // against the outer Galactic Cosmic Rays (GCR radiation). Represent the shield
  // through a cube of smaller dimensions - choose 4 meters as a start
  G4double shield_dim = 4*m;
  G4Box* solidShield =
  new G4Box("Shield", shield_dim, shield_dim, shield_dim);

  // Fill it with the required material
  G4LogicalVolume* logicShield =
  new G4LogicalVolume(solidShield,
                      shield_mat,
                      "Shield");

  // Place the shield in the world
  new G4PVPlacement(0,  // no rotation
                      G4ThreeVector(),  // set origin
                      logicShield,  // logical volume
                      "Shield",    // name
                      logicWorld,    // mother volume
                      false,  // boolean
                      0,    // copy number
                      fCheckOverlaps);    // checks for volume overlaps
                      
  // Now create the chamber with air inside the spacecraft. This should be filled with
  // the nominal parameters of 101,325Pa and 300K. Make the air chamber smaller than 
  // the spacecraft shield, say at 3m
  G4double airchamber_dim = 3*m;
  G4Box* solidChamber = new G4Box("AirChamber", airchamber_dim, airchamber_dim, airchamber_dim);
  
  // Fill the air chamber with the proper material
  G4LogicalVolume* logicChamber = 
  new G4LogicalVolume(solidChamber, chamber_mat, "AirChamber");
  
  // Place the chamber in the physical world
  new G4PVPlacement(0,                       // no rotation
                    G4ThreeVector(),         // set origin
                    logicChamber,            // logical volume
                    "AirChamber",            // name
                    logicWorld,              // mother  volume
                    false,                   // boolean operation
                    0,                       // number
                    fCheckOverlaps);         // checking overlaps  
  
  // Finally, create the patient which can be made of either bones or water materials - up
  // to the user's choice. Declare the patient as a cyllinder.
  G4double patient_radius = 2.75*m;
  G4double patient_dZ = 2.75*m;  
    
  // Declare the solid object for the patient  
  G4Tubs* solidPatient = new G4Tubs("Patient", 0., patient_radius, 0.5*patient_dZ, 0., twopi);
  
  // Now create the logical volume by implementing the proper material for the phantom
  G4LogicalVolume* logicPatient =                         
    new G4LogicalVolume(solidPatient,        //its solid
                        phantom_mat,         //its material
                        "Patient");          //its name
               
  // Place now the patient in world
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(),         //at (0,0,0)
                    logicPatient,            //its logical volume
                    "Patient",               //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    fCheckOverlaps);         // checking overlaps 
  
  
  return physWorld;
}

//

void BasicDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // Sensitive detector
  auto detectorSD
    = new BasicPETSD("detectorSD", "DetectorHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(detectorSD);
  SetSensitiveDetector("Shield",detectorSD);

  // Make phantom a sensitive detector 

  
  G4MultiFunctionalDetector* patient = new G4MultiFunctionalDetector("patient");
  // the next line was missing which was causing a segmentation fault
  G4SDManager::GetSDMpointer()->AddNewDetector(patient);
  //
  G4VPrimitiveScorer* primitiv2 = new G4PSEnergyDeposit("edep");
  patient->RegisterPrimitive(primitiv2);
  SetSensitiveDetector("Patient",patient);
  
  
  /*
  auto phantomSD
    = new BasicPETSD("phantomSD", "PatientHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(phantomSD);
  SetSensitiveDetector("Patient",phantomSD);
  */
 
}

//
