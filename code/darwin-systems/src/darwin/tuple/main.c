/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Main entry point for testing the pharmaceutical tuple system.
 * @details     :   Demonstrates the initialization of the tuple collection, 
 * filling a Pharmaceutical record with complex desvenlafaxine data, 
 * and performing a brand-name search.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/tuple/main.c ./src/darwin/tuple/tuple.c -I./include -o ./bin/tuple_test
 * @version     :   1.0
 * @date        :   2026-04-20
 */
#include <stdio.h>

// Include header files.
#include "darwin/tuple/tuple.h"

int main(void)
{
    // Create a pharmacy tuple.
    Tuple* pharmacy = create(10);

    // Memory check.
    if (pharmacy == NULL)
    {
        fprintf(stderr, "Failed to create a tuple.\n");

        return 1;
    }

    // Configure JSON attributes.
    const char* pristiq_pharmacokinetics[] = {
        "Inhibits SERT (Serotonin Transporter) and NET (Norepinephrine Transporter).",
        "Higher affinity for SERT than NET (approximately 10:1 ratio)."
    };

    const char* pristiq_indications[] = {
        "Major Depressive Disorder (MDD)"
    };

    const char* pristiq_contraindications[] = {
        "Hypersensitivity to desvenlafaxine or venlafaxine.",
        "MAOI use within the last 14 days."
    };

    const char* pristiq_adverse_effects[] = {
        "Nausea (most common)", 
        "Dry mouth", 
        "Hyperhidrosis (sweating)", 
        "Dizziness", 
        "Increased blood pressure"
    };

    const char* pristiq_interactions[] = {
        "MAOIs (Serotonin Syndrome)", 
        "Other SNRIs/SSRIs", 
        "Drugs that increase blood pressure",
        "Alcohol (may increase sedation)"
    };

    const char* pristiq_dosage_forms[] = {
        "Oral extended-release tablets (25mg, 50mg, 100mg)"
    };

    // Create a pharmaceutical.
    Pharmaceutical pristiq = {
        .brandName = "Pristiq",
        .genericName = "Desvenlafaxine",
        .drugClass = CLASS_SNRI,
        .manufacturer = "Wyeth (Pfizer)",
        .chemicalStructure = "C16H25NO2",
        .mechanismOfAction = "Potent inhibition of the neuronal reuptake of both serotonin and norepinephrine (SNRI).",
        
        .pharmacokinetics = {
            .absorption = "Bioavailability is ~80% and is not significantly affected by food. Extended-release delivery ensures steady plasma levels.",
            .distribution = "30% protein bound; moderate volume of distribution.",
            .metabolism = "Primarily metabolized via conjugation (glucuronidation) and minimally through CYP3A4. Unlike Venlafaxine, it does not rely on CYP2D6.",
            .elimination = "Approximately 45% excreted unchanged in urine; terminal half-life is ~11 hours.",
            .halfLife = "11 hours"
        },

        .pharmacodynamics = pristiq_pharmacokinetics,
        .number_of_pharmacodynamics = 2,

        .indications = pristiq_indications,
        .number_of_indications = 1,

        .contraindications = pristiq_contraindications,
        .number_of_contraindications = 2,

        .adverseEffects = pristiq_adverse_effects,
        .number_of_adverse_effects = 5,

        .drugInteractions = pristiq_interactions,
        .number_of_interations = 4,

        .dosageForms = pristiq_dosage_forms,
        .number_of_dosage_forms = 1,

        .is_controlled_substance = false,
        .deaSchedule = SCHEDULE_NONE,
        .regulatoryStatus = STATUS_FDA_APPROVED
    };

    // Add "Pristiq" to the tuple.
    if (add(pharmacy, &pristiq))
    {
        printf("Successfully added: %s\n", pristiq.brandName);
    }

    // Search for "Pristiq" by brand name.
    Pharmaceutical* desvenlafaxine = by_brand_name(pharmacy, "Pristiq");
    
    if (desvenlafaxine != NULL)
    {
        // Summary...
        printf("\n--- Drug Information ---\n");
        printf("Brand        : %s\n", desvenlafaxine->brandName);
        printf("Generic      : %s\n", desvenlafaxine->genericName);
        printf("Structure    : %s\n", desvenlafaxine->chemicalStructure);
        printf("Mechanism    : %s\n", desvenlafaxine->mechanismOfAction);
        printf("Metabolism   : %s\n", desvenlafaxine->pharmacokinetics.metabolism);
        
        printf("%s", "\nSide Effects :\n");

        for (size_t index = 0; index < desvenlafaxine->number_of_adverse_effects; index++)
        {
            printf("  - %s\n", desvenlafaxine->adverseEffects[index]);
        }
    }
    else
    {
        printf("%s", "Drug not found.\n");
    }

    printf("%c", '\n');

    // Clear memory.
    destroy(pharmacy);

    printf("%s", "Tuple destroyed. Memory freed.\n");

    return 0;
}