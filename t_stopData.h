#include <stdint.h>

#pragma once


/**** MUST DEFINE NORTHBOUND AND SOUTHBOUND BEFORE INCLUDING THIS HEADER ****/

#if NORTHBOUND == -1

static const uint8_t tStops_pos[16] = {
    // Single Lane Stops //
    4,    // Walthamstow
    12,   // Blackhorse road
    20,   // Tottenham Hale
    28,   // Seven Sisters
    36,   // Finsbury Park
    44,   // Highbury & Islington
    52,   // Kings Cross st. Pancras
    60,   // Euston
    68,   // Warren Street
    76,   // Oxford Circus
    84,   // Green Park
    92,   // Victoria
    100,  // Pimlico
    108,  // Vauxhall
    116,  // Stockwell
    124,  // Brixton
};

static const uint8_t tStops_times[2][16] = {{
                                                // Northbound //
                                                105,  // Walthamstow
                                                110,  // Blackhorse road
                                                100,  // Tottenham Hale
                                                205,  // Seven Sisters
                                                145,  // Finsbury Park
                                                165,  // Highbury & Islington
                                                90,   // Kings Cross st. Pancras
                                                90,   // Euston
                                                100,  // Warren Street
                                                120,  // Oxford Circus
                                                85,   // Green Park
                                                120,  // Victoria
                                                85,   // Pimlico
                                                130,  // Vauxhall
                                                120,  // Stockwell
                                                0,    // Brixton
                                            },
                                            {
                                                // Southbound //
                                                0,    // Walthamstow
                                                125,  // Blackhorse road
                                                115,  // Tottenham Hale
                                                110,  // Seven Sisters
                                                200,  // Finsbury Park
                                                140,  // Highbury & Islington
                                                175,  // Kings Cross st. Pancras
                                                90,   // Euston
                                                90,   // Warren Street
                                                95,   // Oxford Circus
                                                115,  // Green Park
                                                115,  // Victoria
                                                110,  // Pimlico
                                                90,   // Vauxhall
                                                135,  // Stockwell
                                                105,  // Brixton
                                            }};
#else

static const uint8_t tStops_pos[16] = {
    // Single Lane Stops //
    123,    // Walthamstow
    115,   // Blackhorse road
    107,   // Tottenham Hale
    99,   // Seven Sisters
    91,   // Finsbury Park
    83,   // Highbury & Islington
    75,   // Kings Cross st. Pancras
    67,   // Euston
    59,   // Warren Street
    51,   // Oxford Circus
    43,   // Green Park
    35,   // Victoria
    27,  // Pimlico
    19,  // Vauxhall
    11,  // Stockwell
    3,  // Brixton
};

static const uint8_t tStops_times[2][16] = {{
                                                // Southbound //
                                                0,    // Walthamstow
                                                125,  // Blackhorse road
                                                115,  // Tottenham Hale
                                                110,  // Seven Sisters
                                                200,  // Finsbury Park
                                                140,  // Highbury & Islington
                                                175,  // Kings Cross st. Pancras
                                                90,   // Euston
                                                90,   // Warren Street
                                                95,   // Oxford Circus
                                                115,  // Green Park
                                                115,  // Victoria
                                                110,  // Pimlico
                                                90,   // Vauxhall
                                                135,  // Stockwell
                                                105,  // Brixton
                                            },
                                            {
                                                // Northbound //
                                                105,  // Walthamstow
                                                110,  // Blackhorse road
                                                100,  // Tottenham Hale
                                                205,  // Seven Sisters
                                                145,  // Finsbury Park
                                                165,  // Highbury & Islington
                                                90,   // Kings Cross st. Pancras
                                                90,   // Euston
                                                100,  // Warren Street
                                                120,  // Oxford Circus
                                                85,   // Green Park
                                                120,  // Victoria
                                                85,   // Pimlico
                                                130,  // Vauxhall
                                                120,  // Stockwell
                                                0,    // Brixton
                                            }};
#endif