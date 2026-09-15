/**
 * @file        :   tuple.h
 * @author      :   Braiden Gole
 * @brief       :   Header definition for a pharmaceutical data tuple.
 * @details     :   Provides structures and function prototypes for managing
 * a collection of pharmaceutical records. Supports brand-based retrieval
 * and structured storage of pharmacokinetics and regulatory data.
 * @version     :   1.0
 * @date        :   2026-04-19
 */

#ifndef TUPLE_H
#define TUPLE_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief       :   Enum defining the FDA/Regulatory standing of a drug.
 */
typedef enum
{
    STATUS_FDA_APPROVED,
    STATUS_PENDING,
    STATUS_WITHDRAWN,
    STATUS_INVESTIGATIONAL
} RegulatoryStatus;

/**
 * @brief       :   Enum defining the DEA Controlled Substance Schedule.
 */
typedef enum
{
    SCHEDULE_NONE,
    SCHEDULE_I,
    SCHEDULE_II,
    SCHEDULE_III,
    SCHEDULE_IV,
    SCHEDULE_V
} DEASchedule;

/**
 * @brief       :   Enum defining the primary therapeutic drug class.
 */
typedef enum
{
    CLASS_SMS,
    CLASS_SSRI,
    CLASS_SNRI,
    CLASS_MAOI,
    CLASS_TCA,
    CLASS_BETA_BLOCKER
} DrugClass;

/**
 * @brief       :   Structure containing ADME (Absorption, Distribution, 
 *                  Metabolism, Excretion) properties.
 */
typedef struct
{
    const char* absorption;
    const char* distribution;
    const char* metabolism;
    const char* elimination;
    const char* halfLife;
} Pharmacokinetics;

/**
 * @brief       :   Structure representing comprehensive pharmaceutical data.
 * @details     :   Contains chemical identifiers, pharmacologic profiles, 
 *                  and clinical guidelines.
 */
typedef struct 
{
    const char* brandName;
    const char* genericName;

    DrugClass drugClass;
    
    const char* manufacturer;
    const char* chemicalStructure;
    const char* mechanismOfAction;

    Pharmacokinetics pharmacokinetics;

    const char** pharmacodynamics;
    size_t number_of_pharmacodynamics;

    const char** indications;
    size_t number_of_indications;

    const char** contraindications;
    size_t number_of_contraindications;

    const char** adverseEffects;
    size_t number_of_adverse_effects;

    const char** drugInteractions;
    size_t number_of_interations;

    const char** dosageForms;
    size_t number_of_dosage_forms;

    bool is_controlled_substance;

    DEASchedule deaSchedule;
    RegulatoryStatus regulatoryStatus;
} Pharmaceutical;

/**
 * @brief       :   A dynamic container (Tuple) for Pharmaceutical pointers.
 */
typedef struct
{
    Pharmaceutical** items;
    size_t size;
    size_t capacity;
} Tuple;

// Function prototypes.
Tuple* create(size_t capacity);
bool add(Tuple* tuple, const Pharmaceutical* item);
Pharmaceutical* at(const Tuple* tuple, size_t index);
Pharmaceutical* by_brand_name(const Tuple* tuple, const char* name);
void destroy(Tuple* tuple);

#endif