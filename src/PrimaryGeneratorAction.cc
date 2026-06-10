#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(const RunConfig& cfg)
: m_cfg(cfg), m_gun(std::make_unique<G4ParticleGun>(1))
{
  auto* table = G4ParticleTable::GetParticleTable();
  auto* def = table->FindParticle(m_cfg.particle);
  if (!def) {
    G4Exception("PrimaryGeneratorAction", "BadParticle", FatalException,
                ("Unknown particle: " + m_cfg.particle).c_str());
  }

  m_gun->SetParticleDefinition(def);
  m_gun->SetParticleEnergy(m_cfg.energy_MeV * MeV);
  
  double m_rand_x = (G4UniformRand() * 4000) - 2000;
  double m_rand_y = (G4UniformRand() * 6000) - 3000;

  double rand_dx = (G4UniformRand() * 4000) - m_rand_x - 2000;
  double rand_dy = (G4UniformRand() * 6000) - m_rand_y - 3000;
	

  double m_rand_x = (G4UniformRand() * 4000) - 2000;
  double m_rand_y = (G4UniformRand() * 6000) - 3000;

  double rand_dx = (G4UniformRand() * 4000) - m_rand_x - 2000;
  double rand_dy = (G4UniformRand() * 6000) - m_rand_y - 3000;


  m_gun->SetParticleMomentumDirection(
    G4ThreeVector(rand_dx, rand_dy, 65530).unit()
  );
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() = default;

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  double x = (m_cfg.position_mm[0] + m_rand_x) * mm;
  double y = (m_cfg.position_mm[1] + m_rand_y) * mm;
  double z = (m_cfg.position_mm[2] - 0.5 * 120827.0) * mm;


  if (m_cfg.sigma_xy_mm > 0) {
    x += G4RandGauss::shoot(0.0, m_cfg.sigma_xy_mm * mm);
    y += G4RandGauss::shoot(0.0, m_cfg.sigma_xy_mm * mm);
  }

  m_gun->SetParticlePosition(G4ThreeVector(x,y,z));
  m_gun->GeneratePrimaryVertex(event);
}
