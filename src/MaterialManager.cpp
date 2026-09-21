#include "MaterialManager.h"

#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/Units.h"

using namespace GeoModelKernelUnits;

GeoMaterial* MaterialManager::getCached(const std::string& key) const {
  auto it = m_mats.find(key);
  return (it == m_mats.end()) ? nullptr : it->second;
}

void MaterialManager::cache(const std::string& key, GeoMaterial* mat) {
  m_mats[key] = mat;
}

GeoMaterial* MaterialManager::air() {
  if (auto* m = getCached("Air")) return m;

  auto* N  = new GeoElement("AirElement", "Air", 7.0, 14.01 * g/mole);
  auto* m  = new GeoMaterial("Air", 1.2041 * mg/cm3);
  m->add(N, 1.0);
  m->lock();
  cache("Air", m);
  return m;
}


GeoMaterial* MaterialManager::lead()
{
  static GeoMaterial* m = nullptr;
  if (m) return m;

  static GeoElement* Pb = nullptr;
  if (!Pb) Pb = new GeoElement("LeadElement", "Pb", 82.0, 207.2 * g/mole);

  m = new GeoMaterial("Lead", 11.34 * g/cm3);
  m->add(Pb, 1.0);
  m->lock();
  return m;
}

GeoMaterial* MaterialManager::pvt()
{
  static GeoMaterial* m = nullptr;
  if (m) return m;

  static GeoElement* C = nullptr;
  static GeoElement* H = nullptr;
  if (!C) C = new GeoElement("Carbon",   "C", 6.0, 12.011 * g/mole);
  if (!H) H = new GeoElement("Hydrogen", "H", 1.0, 1.008  * g/mole);

  m = new GeoMaterial("PVT", 1.032 * g/cm3);
  m->add(C,  9.0);
  m->add(H, 10.0);
  m->lock();
  return m;
}

GeoMaterial* MaterialManager::polystyrene()
{
  static GeoMaterial* m = nullptr;
  if (m) return m;

  static GeoElement* C = nullptr;
  static GeoElement* H = nullptr;
  if (!C) C = new GeoElement("Carbon",   "C", 6.0, 12.011 * g/mole);
  if (!H) H = new GeoElement("Hydrogen", "H", 1.0, 1.008  * g/mole);

  // Polystyrene repeat unit C8H8; density depends on formulation.
  // Use a reasonable default; you can make this configurable later.
  m = new GeoMaterial("Polystyrene", 1.05 * g/cm3);
  m->add(C, 8.0);
  m->add(H, 8.0);
  m->lock();
  return m;
}

GeoMaterial* MaterialManager::aluminum()
{
  static GeoMaterial* m = nullptr;
  if (m) return m;

  static GeoElement* Al = nullptr;
  if (!Al) Al = new GeoElement("Aluminium", "Al", 13.0, 26.9815 * g/mole);

  m = new GeoMaterial("Aluminium", 2.70 * g/cm3);
  m->add(Al, 1.0);
  m->lock();
  return m;
}

GeoMaterial* MaterialManager::iron()
{
  static GeoMaterial* m = nullptr;
  if (m) return m;

  static GeoElement* Fe = nullptr;
  if (!Fe) Fe = new GeoElement("Iron", "Fe", 26.0, 55.845 * g/mole);

  m = new GeoMaterial("Iron", 7.874 * g/cm3);
  m->add(Fe, 1.0);
  m->lock();
  return m;
}

GeoMaterial* MaterialManager::steel()
{
  // Alias to iron for now (steel ~ iron for geometry purposes)
  return iron();
}

MaterialManager::RGBA MaterialManager::rgbaFor(const std::string& name) const
{
  // Defaults: white opaque
  if (name == "Lead") return {0.80, 0.80, 0.80, 1.0}; // dark gray
  if (name == "Iron") return {0.60, 0.60, 0.60, 1.0}; // light gray
  if (name == "PVT")  return {0.00, 0.65, 0.65, 1.0}; // teal
  if (name == "Polystyrene")  return {0.00, 0.0, 1.0, 1.0}; // teal
  if (name == "Aluminium")  return {0.00, 0.0, 0.0, 1.0}; // white 
  if (name == "Air")  return {1.00, 1.00, 1.00, 0.02}; // almost invisible, optional
  return {1.0, 1.0, 1.0, 1.0};
}
