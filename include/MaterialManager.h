#pragma once

#include <map>
#include <string>
#include <array>

class GeoMaterial;
class GeoElement;

class MaterialManager {
public:
  MaterialManager() = default;

  // Return existing or create+cache
  GeoMaterial* air();   // simple air proxy (N)
  GeoMaterial* lead();  // Pb
  GeoMaterial* iron();  // Fe
  GeoMaterial* steel(); // alias for iron (convenience)
  GeoMaterial* pvt();   // C9H10, density ~1.032 g/cm3
  GeoMaterial* polystyrene(); 
  GeoMaterial* aluminum();
  // RGBA in [0,1]
  using RGBA = std::array<double,4>;
  RGBA rgbaFor(const std::string& materialName) const;

private:
  GeoMaterial* getCached(const std::string& key) const;
  void cache(const std::string& key, GeoMaterial* mat);

  mutable std::map<std::string, GeoMaterial*> m_mats;
};
