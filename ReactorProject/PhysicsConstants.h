#ifndef PHYSICSCONSTANTS_H
#define PHYSICSCONSTANTS_H

// All constants were from ENGR1700 Homework 4 Question 3: 

// Microscopic Fission Cross Section = 585 Barns = 585*10^-24 cm2
const float SIGMA_MICRO = 585e-24; 

// Enrichment %
const float ENRICHMENT = 0.05f;   // Fraction of U-235

// mass of UO2 = 269.88 g/mol --> need this
const float MASS_UO2 = 269.88f;

// Energy per fission = 200 MeV/Fission = 3.204*10^-11 J
const float ENERGY_FISSION = 3.204e-11f;

// Number density U-235 undergoing Fission = 1.17*10^21 atoms/cm
const float N = 1.17e21f;

// Neutron Flux = 1.0*10^13 neutrons/cm^2-second
const float NEUTRON_FLUX = 1.0e13f; 

// Avogadros Number [atoms/mol]
const float N_A = 6.022e23f;

const float ROD_TOLERANCE = 5.0f;
const float K_TOLERANCE = 0.09f;
const unsigned long TASK_TIME_MS = 7000; // this is 7 seconds



#endif