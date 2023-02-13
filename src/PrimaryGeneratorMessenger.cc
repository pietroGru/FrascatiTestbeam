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
/// \file electromagnetic/TestEm4/src/PrimaryGeneratorMessenger.cc
/// \brief Implementation of the PrimaryGeneratorMessenger class
//
//
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"



PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* Gun)
 :G4UImessenger(),
  fAction(Gun),
  fGunDir(0),
  fBeamMultiplicity(0)
{
  fGunDir = new G4UIdirectory("/btf/gun/");
  fGunDir->SetGuidance("BTF beam control");

  fBeamMultiplicity = new G4UIcmdWithAnInteger("/btf/gun/setMultiplicity",this);
  fBeamMultiplicity->SetGuidance("set the e- beam multiplicity");
  fBeamMultiplicity->SetParameterName("multiplicity", false);
  fBeamMultiplicity->SetDefaultValue(1);
  fBeamMultiplicity->SetRange("multiplicity > 0");
  fBeamMultiplicity->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
}



PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fGunDir;
  delete fBeamMultiplicity;
}



void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fBeamMultiplicity) fAction->SetBeamMultiplicity(fBeamMultiplicity->GetNewIntValue(newValue));
}



