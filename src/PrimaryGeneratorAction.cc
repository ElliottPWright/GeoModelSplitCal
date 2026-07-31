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

if (m_cfg.half_lengths[0] == 0.0 && m_cfg.half_lengths[1] == 0.0 && m_cfg.half_lengths[2] == 0.0) {
   m_gun->SetParticleMomentumDirection(
    G4ThreeVector(m_cfg.direction[0], m_cfg.direction[1], m_cfg.direction[2]).unit()
 );
 }
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() = default;

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{

  // EPWL 27/07: When nothing is specified, use the default position and direction from the config file. 
  // If the user specifies a direction, use that instead.
 double x = m_cfg.position_mm[0];
 double y = m_cfg.position_mm[1];
 double z = (m_cfg.position_mm[2] - 0.5 * 120827.0);

double dx = m_cfg.direction[0];
double dy = m_cfg.direction[1];
double dz = m_cfg.direction[2];

if (dx == 0.0 && dy == 0.0 && dz == 0.0) {
  if (m_cfg.gaussian == false) {
    // EPWL 20/07: Sample a rectangular entrance and exit plane so the
    // resulting trajectories cover a truncated pyramid from 2x3 m^2 to 4x6 m^2
    // over a range of 50 m.

    if (m_cfg.half_lengths[0] > 0.0 || m_cfg.half_lengths[1] > 0.0) {
      const double source_half_x = m_cfg.half_lengths[0];
      const double source_half_y = m_cfg.half_lengths[1];
      const double target_half_x = 2.0 * source_half_x;
      const double target_half_y = 2.0 * source_half_y;
  
      const double source_z = m_cfg.position_mm[2];
      const double target_z = 50000.0; // 50 m in mm


      const double source_x = (G4UniformRand() * 2.0 * source_half_x) - source_half_x;
      const double source_y = (G4UniformRand() * 2.0 * source_half_y) - source_half_y;
      const double target_x = (G4UniformRand() * 2.0 * target_half_x) - target_half_x;
      const double target_y = (G4UniformRand() * 2.0 * target_half_y) - target_half_y;

      x += source_x;
      y += source_y;
      dx = target_x - source_x;
      dy = target_y - source_y;
      dz = target_z - source_z;

      // Keep the z component as the 50 m propagation distance.
      (void)target_z;
    }  
  }

  else if (m_cfg.gaussian == true) {
    // EPWL 27/07: Sample Gaussian starting points and uniform target points.
    const double source_half_x = m_cfg.half_lengths[0];
    const double source_half_y = m_cfg.half_lengths[1];
    const double target_half_x = 2000.0; // 2 m in mm
    const double target_half_y = 3000.0; // 3 m in mm
  
    const double source_z = m_cfg.position_mm[2];
    const double target_z = 97320; // 97.32 m in mm

    const double source_x = (G4RandGauss::shoot(0.0, source_half_x/1.5));
    const double source_y = (G4RandGauss::shoot(0.0, source_half_y/1.5));
    const double target_x = (G4UniformRand() * 2.0 * target_half_x) - target_half_x;
    const double target_y = (G4UniformRand() * 2.0 * target_half_y) - target_half_y;

    x += source_x;
    y += source_y;
    dx = target_x - source_x;
    dy = target_y - source_y;
    dz = target_z - source_z;
  }
}

if (m_cfg.sigma_xy_mm > 0) {
    x += G4RandGauss::shoot(0.0, m_cfg.sigma_xy_mm * mm);
    y += G4RandGauss::shoot(0.0, m_cfg.sigma_xy_mm * mm);
  }

  
  m_gun->SetParticlePosition(G4ThreeVector(x * mm, y * mm, z * mm));

  m_gun->SetParticleMomentumDirection(G4ThreeVector(dx, dy, dz).unit());

  m_gun->GeneratePrimaryVertex(event);
}
