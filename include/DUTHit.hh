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
/// \file DUTHit.hh
/// \brief Definition of the DUTHit class

#ifndef DUTHit_h
#define DUTHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"

/// Calorimeter hit class
///
/// It defines data members to store the the energy deposit and track lengths
/// of charged particles in a selected volume:
/// - fEdep, fTrackLength

class DUTHit : public G4VHit
{
  public:
    DUTHit();
    DUTHit(const DUTHit&);
    virtual ~DUTHit();

    // operators
    const DUTHit& operator=(const DUTHit&);
    G4bool operator==(const DUTHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw() {}
    virtual void Print();

    // methods to handle data
    void Add(G4double de);
    void Add(G4double de, G4double dl);
    void Add(G4double de, G4ThreeVector pos);
    
    // get methods
    G4double GetEdep() const;
    G4double GetTrackLength() const;
    G4double GetX() const;
    G4double GetY() const;
    G4double GetZ() const;
    G4ThreeVector GetPosVec() const;
      
  private:
    G4double fEdep;         ///< Energy deposit in the sensitive volume
    G4double fTrackLength;  ///< Track length in the  sensitive volume
    G4ThreeVector fEdepPos; ///< Position of the energy deposit
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using DUTHitsCollection = G4THitsCollection<DUTHit>;

extern G4ThreadLocal G4Allocator<DUTHit>* DUTHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* DUTHit::operator new(size_t)
{
  if (!DUTHitAllocator) {
    DUTHitAllocator = new G4Allocator<DUTHit>;
  }
  void *hit;
  hit = (void *) DUTHitAllocator->MallocSingle();
  return hit;
}

inline void DUTHit::operator delete(void *hit)
{
  if (!DUTHitAllocator) {
    DUTHitAllocator = new G4Allocator<DUTHit>;
  }
  DUTHitAllocator->FreeSingle((DUTHit*) hit);
}

inline void DUTHit::Add(G4double de) {
  fEdep += de; 
}

inline void DUTHit::Add(G4double de, G4double dl) {
  fEdep += de; 
  fTrackLength += dl;
}

inline void DUTHit::Add(G4double de, G4ThreeVector pos) {
  fEdep += de; 
  fEdepPos += pos;
}

inline G4double DUTHit::GetEdep() const { 
  return fEdep; 
}

inline G4double DUTHit::GetTrackLength() const { 
  return fTrackLength; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline G4double DUTHit::GetX() const { 
  return fEdepPos.x(); 
}

inline G4double DUTHit::GetY() const { 
  return fEdepPos.y(); 
}

inline G4double DUTHit::GetZ() const { 
  return fEdepPos.z(); 
}

inline G4ThreeVector DUTHit::GetPosVec() const { 
  return fEdepPos; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
