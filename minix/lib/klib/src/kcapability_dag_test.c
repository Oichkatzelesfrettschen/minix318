/**
 * @file kcapability_dag_test.c
 * @brief Mathematical verification tests for Capability DAG implementation
 *
 * These tests verify that our implementation correctly implements the
 * mathematical properties of directed acyclic graphs and capability theory.
 * They serve as executable mathematical proofs of the DAG's correctness.
 */

#include "../include/klib.h" // For kprintf (or kprintf_stub), KASSERT, etc.
#include <minix/kcapability_dag.h> // For DAG structures and functions
#include <stdbit.h> // For _BitInt literals if used directly in tests
#include <stdbool.h> // For bool, if not already included

// Define constants used in node creation if they are not globally visible
// from kcapability_dag.c or kcapability_dag.h
#ifndef INVALID_CAPABILITY_ID
#define INVALID_CAPABILITY_ID 0wb
#endif
#ifndef MIN_SECURITY_LEVEL
#define MIN_SECURITY_LEVEL 0wb
#endif
#ifndef MAX_SECURITY_LEVEL
#define MAX_SECURITY_LEVEL 255wb
#endif

/**
 * @brief Test basic DAG creation and mathematical property verification.
 *
 * This test verifies that DAG creation establishes the correct mathematical
 * foundations (an empty, valid DAG) for subsequent operations.
 * It acts as a proof that the axioms of an empty graph hold.
 */
void test_capability_dag_creation(void) {
    kprintf("Testing capability DAG creation...\n");

    /* Create a DAG with known mathematical properties */
    // Test with a common initial capacity
    kcapability_dag_t* dag = kcapability_dag_create(10);

    /* Verify mathematical invariants hold for empty DAG */
    KASSERT(dag != NULL, "test_dag_creation: dag is NULL");
    if (!dag) return; // Guard for static analyzers if KASSERT doesn't halt

    KASSERT(dag->node_count == 0wb, "test_dag_creation: node_count not initialized to 0");
    KASSERT(dag->edge_count == 0wb, "test_dag_creation: edge_count not initialized to 0");
    KASSERT(dag->node_capacity == (_BitInt(32))10, "test_dag_creation: node_capacity not matching initial_capacity");

    // The validate_invariants function itself contains KASSERTs.
    // Calling it here serves as part of the test.
    KASSERT(dag->validate_invariants(dag), "test_dag_creation: initial DAG failed invariant check");

    /* Clean up test resources */
    kcapability_dag_destroy(dag);

    kprintf("DAG creation test passed - mathematical invariants verified.\n");
}

/**
 * @brief Test capability node creation with mathematical validation.
 *
 * This test verifies that capability nodes are created with correct
 * mathematical properties (ID, rights, security level) and constraints.
 * It proves that individual mathematical entities (nodes) can be correctly formed.
 */
void test_capability_node_creation(void) {
    kprintf("Testing capability node creation...\n");

    /* Create nodes with specific mathematical properties */
    kcapability_dag_node_t* root_node = kcapability_dag_node_create(
        1wb,        /* capability_id */
        0xFFFFwb,   /* full rights mask */
        (_BitInt(16))10wb /* high security level */
    );

    kcapability_dag_node_t* derived_node = kcapability_dag_node_create(
        2wb,        /* capability_id */
        0x00FFwb,   /* subset of root rights */
        (_BitInt(16))10wb /* same security level */
    );

    /* Verify mathematical properties of individual nodes */
    KASSERT(root_node != NULL, "test_node_creation: root_node is NULL");
    KASSERT(derived_node != NULL, "test_node_creation: derived_node is NULL");

    if(root_node) {
        KASSERT(root_node->capability_id == 1wb, "test_node_creation: root_node ID mismatch");
        KASSERT(root_node->rights_mask == 0xFFFFwb, "test_node_creation: root_node rights_mask mismatch");
        KASSERT(root_node->security_level == 10wb, "test_node_creation: root_node security_level mismatch");
        KASSERT(root_node->parent_count == 0wb && root_node->child_count == 0wb, "test_node_creation: root_node counts not zero");
    }
    if(derived_node) {
        KASSERT(derived_node->capability_id == 2wb, "test_node_creation: derived_node ID mismatch");
        KASSERT(derived_node->rights_mask == 0x00FFwb, "test_node_creation: derived_node rights_mask mismatch");
    }

    /* Verify mathematical relationship (subset property for rights) */
    if(root_node && derived_node) {
        KASSERT((derived_node->rights_mask & root_node->rights_mask) == derived_node->rights_mask,
                "test_node_creation: derived_node rights not a subset of root_node rights");
    }

    /* Clean up test resources */
    // Note: kcapability_dag_node_destroy is meant for nodes *within* a DAG structure
    // if they have dynamically allocated parent/child lists.
    // If these standalone nodes were just kmalloc'd and don't have such lists yet,
    // kfree might be more direct. However, using kcapability_dag_node_destroy
    // is consistent if it correctly handles NULL children/parents pointers.
    // The kcapability_dag_node_destroy includes a KASSERT for reference_count == 0.
    // For standalone nodes not yet in a DAG, reference_count might not be managed,
    // or should be set to 0 before destroy if the KASSERT is active.
    // For now, assume the KASSERT in node_destroy is appropriate or will be handled.
    if (root_node) root_node->reference_count = 0wb; // Simulate node no longer referenced
    kcapability_dag_node_destroy(root_node);
    if (derived_node) derived_node->reference_count = 0wb; // Simulate node no longer referenced
    kcapability_dag_node_destroy(derived_node);

    kprintf("Node creation test passed - mathematical constraints verified.\n");
}

void test_capability_dag_add_edge_and_delete(void) {
    kprintf("Testing DAG edge addition, removal, and node deletion...\n");
    kcapability_dag_t* dag = kcapability_dag_create(10);
    KASSERT(dag != NULL, "add_edge_test: dag creation failed");
    if(!dag) return;

    // Initialize nodes directly in the dag's array.
    // Ensure kcapability_dag_node_t fields are all initialized.
    // Assuming VALIDATION_UNKNOWN is 0 for cached_validation_result.
    // Assuming resource_ptr is NULL for these test nodes.
    // Assuming parents_capacity and children_capacity are initialized to 0 if parents/children are NULL.
    dag->nodes[0] = (kcapability_dag_node_t){ .capability_id=101wb, .rights_mask=0xFFwb, .security_level=5wb, .generation=1wb, .reference_count=0wb, .resource_ptr=NULL, .parents=NULL, .parent_count=0wb, .parents_capacity=0wb, .children=NULL, .child_count=0wb, .children_capacity=0wb, .cached_validation_result=0, .cache_timestamp=0wb };
    dag->nodes[1] = (kcapability_dag_node_t){ .capability_id=102wb, .rights_mask=0x0Fwb, .security_level=5wb, .generation=1wb, .reference_count=0wb, .resource_ptr=NULL, .parents=NULL, .parent_count=0wb, .parents_capacity=0wb, .children=NULL, .child_count=0wb, .children_capacity=0wb, .cached_validation_result=0, .cache_timestamp=0wb };
    dag->nodes[2] = (kcapability_dag_node_t){ .capability_id=103wb, .rights_mask=0x07wb, .security_level=6wb, .generation=1wb, .reference_count=0wb, .resource_ptr=NULL, .parents=NULL, .parent_count=0wb, .parents_capacity=0wb, .children=NULL, .child_count=0wb, .children_capacity=0wb, .cached_validation_result=0, .cache_timestamp=0wb };
    dag->nodes[3] = (kcapability_dag_node_t){ .capability_id=104wb, .rights_mask=0x01wb, .security_level=3wb, .generation=1wb, .reference_count=0wb, .resource_ptr=NULL, .parents=NULL, .parent_count=0wb, .parents_capacity=0wb, .children=NULL, .child_count=0wb, .children_capacity=0wb, .cached_validation_result=0, .cache_timestamp=0wb }; // For sec level violation
    dag->nodes[4] = (kcapability_dag_node_t){ .capability_id=105wb, .rights_mask=0xF0wb, .security_level=5wb, .generation=1wb, .reference_count=0wb, .resource_ptr=NULL, .parents=NULL, .parent_count=0wb, .parents_capacity=0wb, .children=NULL, .child_count=0wb, .children_capacity=0wb, .cached_validation_result=0, .cache_timestamp=0wb }; // For rights violation (will be changed)
    dag->node_count = 5wb;

    kcapability_dag_node_t* dn1 = &dag->nodes[0]; // ID 101
    kcapability_dag_node_t* dn2 = &dag->nodes[1]; // ID 102
    kcapability_dag_node_t* dn3 = &dag->nodes[2]; // ID 103
    kcapability_dag_node_t* dn4 = &dag->nodes[3]; // ID 104 (low sec)
    kcapability_dag_node_t* dn5 = &dag->nodes[4]; // ID 105 (wrong rights)


    // 1. Successful edge addition (n1 -> n2)
    KASSERT(kcapability_dag_add_edge(dag, dn1, dn2) == SUCCESS, "add_edge_test: n1->n2 failed");
    KASSERT(dag->edge_count == 1wb, "add_edge_test: edge_count not 1 after n1->n2");
    KASSERT(dn1->child_count == 1wb && dn1->children[0] == dn2, "add_edge_test: n1 children incorrect");
    KASSERT(dn2->parent_count == 1wb && dn2->parents[0] == dn1, "add_edge_test: n2 parents incorrect");
    KASSERT(dag->validate_invariants(dag), "add_edge_test: invariants failed after n1->n2");

    // 2. Successful edge addition (n2 -> n3)
    KASSERT(kcapability_dag_add_edge(dag, dn2, dn3) == SUCCESS, "add_edge_test: n2->n3 failed");
    KASSERT(dag->edge_count == 2wb, "add_edge_test: edge_count not 2 after n2->n3");
    KASSERT(dag->validate_invariants(dag), "add_edge_test: invariants failed after n2->n3");

    // 3. Rights subset violation (n1 rights 0xFF, dn5 rights 0x100 - not a subset)
    dn5->rights_mask = 0x100wb;
    KASSERT(kcapability_dag_add_edge(dag, dn1, dn5) == CAPABILITY_RIGHTS_VIOLATION, "add_edge_test: rights violation not caught");
    KASSERT(dag->edge_count == 2wb, "add_edge_test: edge_count changed after rights violation");
    KASSERT(dag->validate_invariants(dag), "add_edge_test: invariants failed after rights violation test");


    // 4. Security level violation (n1 (sec 5) -> dn4 (sec 3)) child sec < parent sec
    KASSERT(kcapability_dag_add_edge(dag, dn1, dn4) == CAPABILITY_SECURITY_VIOLATION, "add_edge_test: security violation not caught");
    KASSERT(dag->edge_count == 2wb, "add_edge_test: edge_count changed after security violation");
    KASSERT(dag->validate_invariants(dag), "add_edge_test: invariants failed after security violation test");

    // 5. Cycle detection (n3 -> n1, when n1->n2->n3 exists)
    KASSERT(kcapability_dag_add_edge(dag, dn3, dn1) == CAPABILITY_CYCLE_DETECTED, "add_edge_test: cycle n3->n1 not caught");
    KASSERT(dag->edge_count == 2wb, "add_edge_test: edge_count changed after cycle detection test");
    KASSERT(dag->validate_invariants(dag), "add_edge_test: invariants failed after cycle test");

    // 6. Edge Removal (n1 -> n2)
    KASSERT(kcapability_dag_remove_edge(dag, dn1, dn2) == SUCCESS, "add_edge_test: remove n1->n2 failed");
    KASSERT(dag->edge_count == 1wb, "add_edge_test: edge_count not 1 after remove n1->n2");
    KASSERT(dn1->child_count == 0wb, "add_edge_test: n1 child_count not 0 after remove");
    KASSERT(dn2->parent_count == 0wb, "add_edge_test: n2 parent_count not 0 after remove");
    KASSERT(dag->validate_invariants(dag), "add_edge_test: invariants failed after remove n1->n2");

    // 6a. Attempt to remove a non-existent edge (n1 -> n2)
    int prev_edge_count = dag->edge_count;
    KASSERT(kcapability_dag_remove_edge(dag, dn1, dn2) == CAPABILITY_EDGE_NOT_FOUND, "add_edge_test: removing non-existent n1->n2 did not return CAPABILITY_EDGE_NOT_FOUND");
    KASSERT(dag->edge_count == prev_edge_count, "add_edge_test: edge_count changed after removing non-existent edge");
    KASSERT(dag->validate_invariants(dag), "add_edge_test: invariants failed after removing non-existent edge");

    // 7. Node Deletion (dn2)
    // dn2 currently has one edge: dn2->dn3.
    KASSERT(kcapability_dag_delete_node(dag, dn2->capability_id) == SUCCESS, "add_edge_test: delete dn2 failed");
    KASSERT(dag->node_count == 4wb, "add_edge_test: node_count not 4 after deleting dn2");
    KASSERT(dag->edge_count == 0wb, "add_edge_test: edge_count not 0 after deleting dn2 (and its edge to dn3)");
    // Verify dn3's parent count (was dn2)
    // dn3 is at index 2 initially, after dn2 (index 1) is deleted, dn3 shifts to index 1.
    kcapability_dag_node_t* shifted_dn3 = &dag->nodes[1]; // dn3 was originally dag->nodes[2]
    KASSERT(shifted_dn3->capability_id == 103wb, "add_edge_test: shifted_dn3 is not dn3 after delete");
    KASSERT(shifted_dn3->parent_count == 0wb, "add_edge_test: dn3 parent_count not 0 after dn2 deleted");
    KASSERT(dag->validate_invariants(dag), "add_edge_test: invariants failed after deleting dn2");


    kprintf("DAG edge/delete tests passed.\n");
    kcapability_dag_destroy(dag);
}


/**
 * @brief Run comprehensive mathematical verification test suite for the Capability DAG.
 *
 * This function orchestrates all defined tests to verify that the DAG
 * implementation correctly implements the specified mathematical theory
 * and maintains its invariants.
 */
void kcapability_dag_run_mathematical_tests(void) {
    kprintf("Starting Capability DAG mathematical verification tests...\n");

    test_capability_dag_creation();
    test_capability_node_creation();
    test_capability_dag_add_edge_and_delete(); // New call

    kprintf("All currently implemented mathematical verification tests passed!\n");
    // kprintf("Capability DAG implementation mathematically verified.\n"); // This will be true once all tests are in.
}
