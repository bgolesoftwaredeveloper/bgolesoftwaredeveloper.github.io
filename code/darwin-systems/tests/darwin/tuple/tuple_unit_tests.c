/**
 * @file        :   tuple_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Comprehensive unit test suite for the Pharmaceutical Tuple.
 * @details     :   This module uses the Unity Test Framework to validate 
 * memory allocation, record insertion, search logic, 
 * and data integrity for the Pharmaceutical Tuple container.
 * Compile:
 * clang ./tests/darwin/tuple/tuple_unit_tests.c ./src/darwin/tuple/tuple.c ./src/unity.c -Iinclude -o ./bin/tuple_unit_test
 * @version     :   1.0
 * @date        :   2026-04-21
 */
#include <stdio.h>
#include <string.h>

// Include header files.
#include "unity.h"
#include "darwin/tuple/tuple.h"

/**
 * @brief Sets up resources before each test.
 */
void setUp(void)
{
    ;;
}

/**
 * @brief Cleans up resources after each test.
 */
void tearDown(void)
{
    ;;
}

/**
 * @brief Static arrays for Valium complex data.
 */
static const char* valium_indications[] = {"Anxiety", "Alcohol Withdrawal", "Muscle Spasms"};
static const char* valium_adverse_effects[] = {"Drowsiness", "Ataxia", "Fatigue"};

/**
 * @brief Static arrays for Propranolol complex data.
 */
static const char* propranolol_indications[] = {
    "Hypertension", 
    "Angina Pectoris", 
    "Cardiac Arrhythmias", 
    "Performance Anxiety (Off-label)",
    "Essential Tremor"
};

static const char* propranolol_adverse_effects[] = {
    "Fatigue", 
    "Bradycardia", 
    "Bronchospasm", 
    "Cold extremities", 
    "Sleep disturbances/Vivid dreams"
};

static const char* propranolol_interactions[] = {
    "Verapamil/Diltiazem: Additive AV block risk",
    "NSAIDs: Reduced antihypertensive effect",
    "Rizatriptan: Propranolol increases rizatriptan concentrations",
    "Amiodarone: Increased cardiotoxic effects"
};

/**
 * @brief Helper function to create a dummy pharmaceutical record for testing.
 */
static Pharmaceutical create_diazepam(void)
{
    Pharmaceutical drug = {
        .brandName = "Valium",
        .genericName = "Diazepam",
        .drugClass = CLASS_SMS,
        .manufacturer = "Roche",
        .chemicalStructure = "C16H13ClN2O",
        .mechanismOfAction = "GABA-A receptor modulation.",
        .pharmacokinetics = {
            .absorption = "Rapid (>90%)",
            .distribution = "Highly lipophilic",
            .metabolism = "Hepatic (CYP3A4/2C19)",
            .elimination = "Renal",
            .halfLife = "20-50 hours"
        },
        .indications = valium_indications,
        .number_of_indications = 3,
        .adverseEffects = valium_adverse_effects,
        .number_of_adverse_effects = 3,
        .is_controlled_substance = true,
        .deaSchedule = SCHEDULE_IV,
        .regulatoryStatus = STATUS_FDA_APPROVED
    };

    return drug;
}

static Pharmaceutical create_propranolol(void)
{
    Pharmaceutical drug = {
        .brandName = "Teva-Propranolol",
        .genericName = "Propranolol",
        .drugClass = CLASS_BETA_BLOCKER,
        .manufacturer = "Teva Pharmaceutical Industries",
        .chemicalStructure = "C16H21NO2",
        .mechanismOfAction = "Non-selective beta-adrenergic receptor antagonist; blocks both beta-1 and beta-2 receptors.",
        
        .pharmacokinetics = {
            .absorption = "Highly lipophilic; almost completely absorbed, but has a significant first-pass effect (~25% bioavailability).",
            .distribution = "Readily crosses the blood-brain barrier due to high lipid solubility.",
            .metabolism = "Extensively hepatic via CYP2D6, CYP1A2, and CYP3A4.",
            .elimination = "Excreted as metabolites in the urine.",
            .halfLife = "3–6 hours"
        },

        .indications = propranolol_indications,
        .number_of_indications = 5,

        .adverseEffects = propranolol_adverse_effects,
        .number_of_adverse_effects = 5,

        .drugInteractions = propranolol_interactions,
        .number_of_interations = 4,

        .is_controlled_substance = false,
        .deaSchedule = SCHEDULE_NONE,
        .regulatoryStatus = STATUS_FDA_APPROVED
    };

    return drug;
}

/**
 * @brief Tests basic initialization of the pharmaceutical tuple.
 */
void test_tuple_initialization(void)
{
    // Arrange & Act.
    Tuple* tuple = create(10);

    // Assert.
    TEST_ASSERT_NOT_NULL(tuple);
    TEST_ASSERT_EQUAL_UINT(0, tuple->size);
    TEST_ASSERT_EQUAL_UINT(10, tuple->capacity);
    
    destroy(tuple);
}

/**
 * @brief Tests adding Diazepam and verifying its specific brand name.
 */
void test_tuple_add_diazepam(void)
{
    // Arrange.
    Tuple* tuple = create(5);

    Pharmaceutical valium = create_diazepam();

    // Act.
    bool result = add(tuple, &valium);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT(1, tuple->size);
    TEST_ASSERT_EQUAL_STRING("Valium", tuple->items[0]->brandName);
    
    destroy(tuple);
}

/**
 * @brief Tests adding Propranolol and verifying its specific brand name.
 */
void test_tuple_add_propranolol(void)
{
    // Arrange.
    Tuple* tuple = create(5);

    Pharmaceutical propranolol = create_propranolol();

    // Act.
    add(tuple, &propranolol);

    // Assert.
    TEST_ASSERT_EQUAL_STRING("Teva-Propranolol", tuple->items[0]->brandName);
    TEST_ASSERT_EQUAL_INT(CLASS_BETA_BLOCKER, tuple->items[0]->drugClass);
    
    destroy(tuple);
}

/**
 * @brief Tests searching for Propranolol by brand name.
 */
void test_tuple_search_propranolol(void)
{
    // Arrange.
    Tuple* tuple = create(5);

    Pharmaceutical valium = create_diazepam();
    Pharmaceutical propranolol = create_propranolol();

    add(tuple, &valium);
    add(tuple, &propranolol);

    // Act.
    Pharmaceutical* found = by_brand_name(tuple, "Teva-Propranolol");

    // Assert.
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("Propranolol", found->genericName);
    
    destroy(tuple);
}

/**
 * @brief Tests retrieving Diazepam via index.
 */
void test_tuple_at_index_diazepam(void)
{
    // Arrange.
    Tuple* tuple = create(5);

    Pharmaceutical valium = create_diazepam();

    add(tuple, &valium);

    // Act.
    Pharmaceutical* retrieved = at(tuple, 0);

    // Assert.
    TEST_ASSERT_NOT_NULL(retrieved);
    TEST_ASSERT_EQUAL_STRING("Diazepam", retrieved->genericName);
    
    destroy(tuple);
}

/**
 * @brief Tests that the tuple prevents overflow after adding both drugs.
 */
void test_tuple_overflow_prevention(void)
{
    // Arrange.
    Tuple* tuple = create(2);

    Pharmaceutical valium = create_diazepam();
    Pharmaceutical propranolol = create_propranolol();
    Pharmaceutical overflow = create_diazepam();

    add(tuple, &valium);
    add(tuple, &propranolol);

    // Act.
    bool result = add(tuple, &overflow);

    // Assert.
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL_UINT(2, tuple->size);
    
    destroy(tuple);
}

/**
 * @brief Tests deep copy integrity for Propranolol.
 */
void test_tuple_deep_copy_integrity(void)
{
    // Arrange.
    Tuple* tuple = create(5);

    Pharmaceutical propranolol = create_propranolol();

    add(tuple, &propranolol);

    // Act.
    propranolol.brandName = "Modified Name";

    // Assert.
    TEST_ASSERT_EQUAL_STRING("Teva-Propranolol", tuple->items[0]->brandName);
    
    destroy(tuple);
}

/**
 * @brief Tests retrieval with an out-of-bounds index.
 */
void test_tuple_at_out_of_bounds(void)
{
    // Arrange.
    Tuple* tuple = create(5);

    Pharmaceutical valium = create_diazepam();

    add(tuple, &valium);

    // Act.
    Pharmaceutical* result = at(tuple, 10);

    // Assert.
    TEST_ASSERT_NULL(result);
    
    destroy(tuple);
}

/**
 * @brief Tests search behavior for a brand not in the tuple.
 */
void test_tuple_search_not_found(void)
{
    // Arrange.
    Tuple* tuple = create(5);

    Pharmaceutical propranolol = create_propranolol();

    add(tuple, &propranolol);

    // Act.
    Pharmaceutical* result = by_brand_name(tuple, "Advil");

    // Assert.
    TEST_ASSERT_NULL(result);
    
    destroy(tuple);
}

/**
 * @brief Tests initialization with zero capacity.
 */
void test_tuple_init_zero_capacity(void)
{
    // Arrange & Act.
    Tuple* tuple = create(0);

    // Assert.
    TEST_ASSERT_NULL(tuple);
}

/**
 * @brief Tests search logic with NULL parameters.
 */
void test_tuple_search_invalid_params(void)
{
    // Arrange.
    Tuple* tuple = create(5);

    // Act.
    Pharmaceutical* valium = by_brand_name(NULL, "Valium");
    Pharmaceutical* null = by_brand_name(tuple, NULL);

    // Assert.
    TEST_ASSERT_NULL(valium);
    TEST_ASSERT_NULL(null);
    
    destroy(tuple);
}

/**
 * @brief Tests adding a NULL item.
 */
void test_tuple_add_null_item(void)
{
    // Arrange.
    Tuple* tuple = create(5);

    // Act.
    bool result = add(tuple, NULL);

    // Assert.
    TEST_ASSERT_FALSE(result);
    
    destroy(tuple);
}

/**
 * @brief Tests brand search case sensitivity ("valium" vs "Valium").
 */
void test_tuple_brand_case_sensitivity(void)
{
    // Arrange.
    Tuple* tuple = create(5);

    Pharmaceutical p1 = create_diazepam();

    add(tuple, &p1);

    // Act.
    Pharmaceutical* result = by_brand_name(tuple, "valium");

    // Assert.
    TEST_ASSERT_NULL(result);
    
    destroy(tuple);
}

/**
 * @brief Tests controlled substance status of stored drugs.
 */
void test_tuple_controlled_status(void)
{
    // Arrange.
    Tuple* tuple = create(5);

    Pharmaceutical valium = create_diazepam();
    Pharmaceutical propranolol = create_propranolol();

    add(tuple, &valium);
    add(tuple, &propranolol);

    // Act.
    bool diazepam_status = at(tuple, 0)->is_controlled_substance;
    bool propranolol_status = at(tuple, 1)->is_controlled_substance;

    // Assert.
    TEST_ASSERT_TRUE(diazepam_status);
    TEST_ASSERT_FALSE(propranolol_status);
    
    destroy(tuple);
}

/**
 * @brief Tests that multiple additions maintain the correct sequence.
 */
void test_tuple_sequence_integrity(void)
{
    // Arrange.
    Tuple* tuple = create(10);

    Pharmaceutical valim = create_diazepam();
    Pharmaceutical propranolol = create_propranolol();

    // Act.
    add(tuple, &valim);
    add(tuple, &propranolol);

    // Assert.
    TEST_ASSERT_EQUAL_STRING("Valium", at(tuple, 0)->brandName);
    TEST_ASSERT_EQUAL_STRING("Teva-Propranolol", at(tuple, 1)->brandName);
    
    destroy(tuple);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_tuple_initialization);
    RUN_TEST(test_tuple_add_diazepam);
    RUN_TEST(test_tuple_add_propranolol);
    RUN_TEST(test_tuple_search_propranolol);
    RUN_TEST(test_tuple_at_index_diazepam);
    RUN_TEST(test_tuple_overflow_prevention);
    RUN_TEST(test_tuple_deep_copy_integrity);
    RUN_TEST(test_tuple_at_out_of_bounds);
    RUN_TEST(test_tuple_search_not_found);
    RUN_TEST(test_tuple_init_zero_capacity);
    RUN_TEST(test_tuple_search_invalid_params);
    RUN_TEST(test_tuple_add_null_item);
    RUN_TEST(test_tuple_brand_case_sensitivity);
    RUN_TEST(test_tuple_controlled_status);
    RUN_TEST(test_tuple_sequence_integrity);

    // Destroy the session.
    return UNITY_END();
}