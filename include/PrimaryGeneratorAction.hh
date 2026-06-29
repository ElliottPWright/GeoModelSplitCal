#pragma once
#include "G4VUserPrimaryGeneratorAction.hh"
#include <memory>
#include "RunConfig.hh"

class G4ParticleGun;
class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  explicit PrimaryGeneratorAction(const RunConfig& cfg);
  ~PrimaryGeneratorAction() override;

  void GeneratePrimaries(G4Event* event) override;

private:
  RunConfig m_cfg;
  std::unique_ptr<G4ParticleGun> m_gun;
};
