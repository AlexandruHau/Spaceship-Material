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
#ifndef BasicPETHit_h
#define BasicPETHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"


class BasicPETHit : public G4VHit
{
  public:
    BasicPETHit();
    BasicPETHit(const BasicPETHit&);
    virtual ~BasicPETHit();

    // operators
    const BasicPETHit& operator=(const BasicPETHit&);
    G4int operator==(const BasicPETHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw() {}
    virtual void Print();

    // methods to handle data
    void Add(G4double de, G4double dl);

    // get methods
    G4double GetEdep() const;
    G4double GetTrackLength() const;
    G4double GetXpos() const;
    G4double GetYpos() const;
    G4double GetZpos() const;

  private:
    G4double fEdep;        ///< Energy deposit in the sensitive volume
    G4double fTrackLength; ///< Track length in the  sensitive volume
    G4double fXpos;
    G4double fYpos;
    G4double fZpos;
};

//

using BasicPETHitsCollection = G4THitsCollection<BasicPETHit>;

extern G4ThreadLocal G4Allocator<BasicPETHit>* BasicPETHitAllocator;

//

inline void* BasicPETHit::operator new(size_t)
{
  if (!BasicPETHitAllocator) {
    BasicPETHitAllocator = new G4Allocator<BasicPETHit>;
  }
  void *hit;
  hit = (void *) BasicPETHitAllocator->MallocSingle();
  return hit;
}

inline void BasicPETHit::operator delete(void *hit)
{
  if (!BasicPETHitAllocator) {
    BasicPETHitAllocator = new G4Allocator<BasicPETHit>;
  }
  BasicPETHitAllocator->FreeSingle((BasicPETHit*) hit);
}

inline void BasicPETHit::Add(G4double de, G4double dl ) {
  fEdep += de;
  fTrackLength += dl;
}

inline G4double BasicPETHit::GetEdep() const {
  return fEdep;
}

inline G4double BasicPETHit::GetTrackLength() const {
  return fTrackLength;
}

//

#endif
