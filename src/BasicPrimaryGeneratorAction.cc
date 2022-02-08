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

#include "BasicPrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4RandomDirection.hh"
#include "G4PhysicalConstants.hh"
#include <CLHEP/Random/RandGaussQ.h>

// PGA
// based on Tangle2's back2back photons

BasicPrimaryGeneratorAction::BasicPrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(),
   fParticleGun(0)
{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);
}

//

BasicPrimaryGeneratorAction::~BasicPrimaryGeneratorAction()
{
  delete fParticleGun;
}

//

void BasicPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  
  // Case 2: now design a uniform radiation spread throughout the human phantom. The patient can be commented
  // out or not. The patient and detector are modelled as cyllinders, so cyllindrical polar coordinates will 
  // be used. Use the maximum allowed values.
  G4double x0 = 5*m;
  G4double y0 = 5*m;
  G4double z0 = 5*m;
  
  // Now introduce uniform randomness for getting the gamma-radiation inside the human phantom
  G4double x = 2 * x0 * G4UniformRand() - x0;
  G4double y = -4.5*m;
  G4double z = 2 * z0 * G4UniformRand() - z0;
  
  // Apply the spherical polar coordinates
  
  G4ThreeVector radiationOrigin = G4ThreeVector(x,y,z);
  
  G4ThreeVector photonDir = G4ThreeVector(0,1,0);
  // Set the outcoming photon
  
  fParticleGun->SetParticleDefinition(particleTable->FindParticle(particleName="gamma"));
  
  // Photon 1  
  fParticleGun->SetParticlePosition(radiationOrigin);
  fParticleGun->SetParticleEnergy(10*GeV);
  fParticleGun->SetParticleMomentumDirection(photonDir);
  fParticleGun->GeneratePrimaryVertex(anEvent);
}
