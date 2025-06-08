/**
 * @file kcapability_dag_test.c
 * @brief Unit tests for the Capability Directed Acyclic Graph (DAG).
 */

#include <kcapability_dag.h>
#include <klib.h>         // For kprintf_stub, KASSERT (indirectly)
#include <sys/kassert.h>  // For KASSERT

// --- Test Helper Macros/Functions ---
#define TEST_ASSERT(condition, message)                                     \
  do {                                                                      \
    if (!(condition)) {                                                     \
      kprintf_stub("TEST FAIL (%s:%d): %s\n", __func__, __LINE__, message); \
      KASSERT(condition); /* This will panic if KASSERT is enabled */       \
      return false;       /* Indicate test failure */                       \
    } else {                                                                \
      kprintf_stub("TEST PASS (%s:%d): %s\n", __func__, __LINE__, message); \
    }                                                                       \
  } while (0)

// --- Test Case Implementations ---

static kbool test_dag_creation_destruction(void) {
  kprintf_stub("--- Running Test: %s ---\n", __func__);
  kcapability_dag_t* dag = kcapability_dag_create();
  TEST_ASSERT(dag != NULL, "DAG creation should succeed.");
  TEST_ASSERT(dag->num_nodes == 0, "New DAG should have 0 nodes.");

  kcapability_dag_destroy(dag);  // Resets the static instance
  // TEST_ASSERT(dag->num_nodes == 0, "DAG should have 0 nodes after
  // destruction."); // dag pointer might be invalid if heap allocated / or
  // reset
  kprintf_stub("DAG creation/destruction test finished.\n");
  return true;
}

static kbool test_dag_node_creation_lookup(void) {
  kprintf_stub("--- Running Test: %s ---\n", __func__);
  kcapability_dag_t* dag = kcapability_dag_create();  // Gets static instance
  kcapability_dag_destroy(dag);                       // Ensure clean state
  dag = kcapability_dag_create();

  kcapability_dag_node_t* node1 =
      kcapability_dag_node_create(dag, 1001, 0xFF, 10);
  TEST_ASSERT(node1 != NULL, "Node 1 creation.");
  TEST_ASSERT(dag->num_nodes == 1, "DAG node count should be 1.");
  TEST_ASSERT(node1->id == 1001, "Node 1 ID check.");
  TEST_ASSERT(node1->rights_mask == 0xFF, "Node 1 rights check.");
  TEST_ASSERT(node1->security_level == 10, "Node 1 security level check.");

  kcapability_dag_node_t* node2 =
      kcapability_dag_node_create(dag, 1002, 0x0F, 12);
  TEST_ASSERT(node2 != NULL, "Node 2 creation.");
  TEST_ASSERT(dag->num_nodes == 2, "DAG node count should be 2.");

  kcapability_dag_node_t* found_node1 = kcapability_dag_lookup_node(dag, 1001);
  TEST_ASSERT(found_node1 == node1, "Lookup node 1.");

  kcapability_dag_node_t* found_node_non_existent =
      kcapability_dag_lookup_node(dag, 9999);
  TEST_ASSERT(found_node_non_existent == NULL, "Lookup non-existent node.");

  // Test max nodes
  for (int i = dag->num_nodes; i < KCAPABILITY_MAX_NODES; ++i) {
    kcapability_dag_node_create(dag, 2000 + i, 0x1, 1);
  }
  TEST_ASSERT(dag->num_nodes == KCAPABILITY_MAX_NODES, "DAG should be full.");
  kcapability_dag_node_t* node_over_max =
      kcapability_dag_node_create(dag, 3000, 0x1, 1);
  TEST_ASSERT(node_over_max == NULL,
              "Node creation should fail when DAG is full.");

  kprintf_stub("Node creation/lookup test finished.\n");
  return true;
}

static kbool test_dag_add_edge_valid(void) {
  kprintf_stub("--- Running Test: %s ---\n", __func__);
  kcapability_dag_t* dag = kcapability_dag_create();
  kcapability_dag_destroy(dag);
  dag = kcapability_dag_create();

  kcapability_dag_node_t* parent = kcapability_dag_node_create(dag, 1, 0xFF, 5);
  kcapability_dag_node_t* child1 = kcapability_dag_node_create(
      dag, 2, 0x0F, 5);  // Valid: subset rights, same level
  kcapability_dag_node_t* child2 = kcapability_dag_node_create(
      dag, 3, 0xFF, 6);  // Valid: same rights, higher level

  TEST_ASSERT(parent != NULL && child1 != NULL && child2 != NULL,
              "Node creation for edge test.");

  kcapability_dag_status_t status =
      kcapability_dag_add_edge(dag, parent, child1);
  TEST_ASSERT(status == KCAP_SUCCESS,
              "Add valid edge (parent->child1) - subset rights.");
  TEST_ASSERT(parent->num_adjacent_nodes == 1,
              "Parent adj count after 1st edge.");
  TEST_ASSERT(parent->adjacent_nodes[0].target_node == child1,
              "Parent adj node check.");

  status = kcapability_dag_add_edge(dag, parent, child2);
  TEST_ASSERT(status == KCAP_SUCCESS,
              "Add valid edge (parent->child2) - higher sec level.");
  TEST_ASSERT(parent->num_adjacent_nodes == 2,
              "Parent adj count after 2nd edge.");

  kprintf_stub("Valid edge addition test finished.\n");
  return true;
}

static kbool test_dag_add_edge_cycle_detection(void) {
  kprintf_stub("--- Running Test: %s ---\n", __func__);
  kcapability_dag_t* dag = kcapability_dag_create();
  kcapability_dag_destroy(dag);
  dag = kcapability_dag_create();

  kcapability_dag_node_t* n1 = kcapability_dag_node_create(dag, 1, 0xFF, 1);
  kcapability_dag_node_t* n2 = kcapability_dag_node_create(dag, 2, 0xFF, 1);
  kcapability_dag_node_t* n3 = kcapability_dag_node_create(dag, 3, 0xFF, 1);
  TEST_ASSERT(n1 && n2 && n3, "Node creation for cycle test.");

  kcapability_dag_add_edge(dag, n1, n2);  // n1 -> n2
  kcapability_dag_add_edge(dag, n2, n3);  // n2 -> n3

  kcapability_dag_status_t status = kcapability_dag_add_edge(
      dag, n3, n1);  // Attempt n3 -> n1 (creates n1->n2->n3->n1 cycle)
  TEST_ASSERT(status == KCAP_CYCLE_DETECTED, "Cycle detection (n3->n1).");

  status = kcapability_dag_add_edge(dag, n1, n1);  // Self-cycle
  TEST_ASSERT(status == KCAP_SELF_REFERENCE, "Self-cycle detection (n1->n1).");

  kprintf_stub("Cycle detection test finished.\n");
  return true;
}

static kbool test_dag_add_edge_rights_violation(void) {
  kprintf_stub("--- Running Test: %s ---\n", __func__);
  kcapability_dag_t* dag = kcapability_dag_create();
  kcapability_dag_destroy(dag);
  dag = kcapability_dag_create();

  kcapability_dag_node_t* parent =
      kcapability_dag_node_create(dag, 1, 0x0F, 5);  // Parent has fewer rights
  kcapability_dag_node_t* child =
      kcapability_dag_node_create(dag, 2, 0xFF, 5);  // Child has more rights
  TEST_ASSERT(parent && child, "Node creation for rights violation test.");

  kcapability_dag_status_t status =
      kcapability_dag_add_edge(dag, parent, child);
  TEST_ASSERT(status == KCAP_RIGHTS_VIOLATION,
              "Rights violation (child rights not subset of parent).");

  kprintf_stub("Rights violation test finished.\n");
  return true;
}

static kbool test_dag_add_edge_security_violation(void) {
  kprintf_stub("--- Running Test: %s ---\n", __func__);
  kcapability_dag_t* dag = kcapability_dag_create();
  kcapability_dag_destroy(dag);
  dag = kcapability_dag_create();

  kcapability_dag_node_t* parent =
      kcapability_dag_node_create(dag, 1, 0xFF, 5);  // Parent higher security
  kcapability_dag_node_t* child =
      kcapability_dag_node_create(dag, 2, 0xFF, 4);  // Child lower security
  TEST_ASSERT(parent && child, "Node creation for security violation test.");

  kcapability_dag_status_t status =
      kcapability_dag_add_edge(dag, parent, child);
  TEST_ASSERT(status == KCAP_SECURITY_VIOLATION,
              "Security violation (child level < parent level).");

  kprintf_stub("Security violation test finished.\n");
  return true;
}

static kbool test_dag_validate_derivation_valid(void) {
  kprintf_stub("--- Running Test: %s ---\n", __func__);
  kcapability_dag_t* dag = kcapability_dag_create();
  kcapability_dag_destroy(dag);
  dag = kcapability_dag_create();

  kcapability_dag_node_t* n1 =
      kcapability_dag_node_create(dag, 1, 0xFF, 1);  // All rights
  kcapability_dag_node_t* n2 =
      kcapability_dag_node_create(dag, 2, 0x0F, 1);  // Subset of n1
  kcapability_dag_node_t* n3 =
      kcapability_dag_node_create(dag, 3, 0x03, 1);  // Subset of n2
  TEST_ASSERT(n1 && n2 && n3, "Node creation for valid derivation test.");

  kcapability_dag_add_edge(dag, n1, n2);
  kcapability_dag_add_edge(dag, n2, n3);

  kbool is_valid_derivation = kcapability_dag_validate_derivation(dag, n1, n3);
  TEST_ASSERT(is_valid_derivation == true, "Valid derivation n1 -> n2 -> n3.");

  is_valid_derivation = kcapability_dag_validate_derivation(dag, n1, n1);
  TEST_ASSERT(is_valid_derivation == true, "Valid derivation n1 -> n1 (self).");

  kprintf_stub("Valid derivation test finished.\n");
  return true;
}

static kbool test_dag_validate_derivation_invalid_rights(void) {
  kprintf_stub("--- Running Test: %s ---\n", __func__);
  kcapability_dag_t* dag = kcapability_dag_create();
  kcapability_dag_destroy(dag);
  dag = kcapability_dag_create();

  kcapability_dag_node_t* n1 = kcapability_dag_node_create(dag, 1, 0xFF, 1);
  kcapability_dag_node_t* n2_amplified =
      kcapability_dag_node_create(dag, 2, 0xFF, 1);  // n2 has full rights
  kcapability_dag_node_t* n1_subset = kcapability_dag_node_create(
      dag, 11, 0x0F, 1);  // n1_subset has subset of n1

  TEST_ASSERT(n1 && n2_amplified && n1_subset,
              "Node creation for invalid rights derivation test.");

  // Manually set n2_amplified rights to be more than n1_subset, but path is
  // n1_subset -> n2_amplified This means kcapability_dag_add_edge would prevent
  // this if n1_subset was parent. Here we construct a scenario where path
  // validation is key. n1 (0xFF) -> n1_subset (0x0F) -> n2_amplified (0xFF, but
  // add_edge from n1_subset would fail)

  kcapability_dag_add_edge(dag, n1,
                           n1_subset);  // Valid: n1(0xFF) -> n1_subset(0x0F)

  // Now try to add an edge from n1_subset to n2_amplified.
  // This edge itself is invalid by rights if n2_amplified.rights >
  // n1_subset.rights n1_subset.rights_mask = 0x0F; n2_amplified.rights_mask =
  // 0xFF; (0x0F & 0xFF) != 0xFF. So this edge is KCAP_RIGHTS_VIOLATION by
  // add_edge.
  kcapability_dag_status_t edge_status =
      kcapability_dag_add_edge(dag, n1_subset, n2_amplified);
  TEST_ASSERT(edge_status == KCAP_RIGHTS_VIOLATION,
              "Edge n1_subset->n2_amplified should violate rights.");

  // If we used add_edge_unsafe, then validate_derivation would catch it.
  // For this test, we need a path where rights are amplified *along the path*
  // but individual edges might seem fine if checked locally without path
  // context, or if an intermediate node has restricted rights but a child tries
  // to use more. The current kcapability_dag_dfs_validate_path checks:
  // (path_rights_mask & current->rights_mask) == current->rights_mask
  // This ensures current node's rights are subset of what path allows.
  // Then it passes current->rights_mask to children. This is correct.

  // Let's test the scenario: nA (0xFF) -> nB (0x0F) -> nC (0x03)
  // Derivation nA -> nC should be true.
  // If nB had rights 0x01, and nC had 0x03, but edge nB->nC was made with
  // nB.rights=0x01 then nA -> nC should be false if nC needs 0x03. The
  // kcapability_dag_add_edge already ensures child.rights is subset of
  // parent.rights. So, if a path exists, rights are already transitively
  // subsetted. The test for kcapability_dag_validate_derivation for *invalid
  // rights* means that the target_capability itself requests more rights than
  // the source_capability can provide *even if a path exists and all
  // intermediate nodes are fine according to their parents*. No, the function
  // validates the path based on rights *along* the path. If source_cap has
  // 0xFF, and target_cap has 0x0F, derivation is fine IF path exists AND all
  // intermediate steps respect rights. The only way validate_derivation fails
  // for rights is if an intermediate node in the path has rights that are NOT a
  // subset of the rights passed down by its parent in *that specific DFS
  // traversal*. This is already caught by the (path_rights_mask &
  // current->rights_mask) != current->rights_mask check.

  // Consider A(0xFF) -> B(0x0F). C(0x01) -> B(0x0F) (this edge is invalid by
  // add_edge) Let's assume graph: A(0xFF) -> B(0x0F) and C(0x01) exists. Path A
  // to B: validate_derivation(A,B) -> true. Path C to B: add_edge(C,B) fails.
  // The test should be: A(0xFF) -> B(0x0F). Target is B.
  // Create B_prime(0xFF) that is same ID as B, but we want to validate
  // derivation from A to B_prime. This is not how it works. Nodes are fixed.

  // The scenario for invalid rights in validate_derivation:
  // A (0xFF) -> B (0x0F)
  // A (0xFF) -> X (0x01) -> Y (0x01)
  // Validate A -> B (true)
  // If B's actual rights_mask was changed (not by add_edge) to 0xFF after edge
  // A->B was added. This means an invariant is broken. validate_derivation
  // should catch this. Let's create such a scenario:
  kcapability_dag_destroy(dag);
  dag = kcapability_dag_create();
  kcapability_dag_node_t* node_a =
      kcapability_dag_node_create(dag, 701, 0xFF, 1);
  kcapability_dag_node_t* node_b =
      kcapability_dag_node_create(dag, 702, 0x0F, 1);
  TEST_ASSERT(node_a && node_b, "Node creation for rights path test.");
  kcapability_dag_add_edge(dag, node_a, node_b);  // A(0xFF) -> B(0x0F), valid.

  node_b->rights_mask =
      0F0;  // Manually break invariant: B now has rights not grantable by A's
            // path context (though B's own rights_mask is what it is, the path
            // from A to B is evaluated based on A's mask initially, then B's
            // actual mask for next step) The check is (path_mask &
            // current_node_mask) == current_node_mask For A->B:
            // path_mask=A.rights=0xFF. current_node=B. B.rights=0xF0. (0xFF &
            // 0xF0) == 0xF0. This is true. B is valid under A. Then for B's
            // children, path_mask = B.rights = 0xF0.

  // This test needs a multi-step path where an intermediate node violates the
  // rights propagation. A(0xFF) -> B(0x0F) -> C(0x03). This is valid. If B's
  // rights were changed to 0x01 AFTER edge B->C was made (which required B to
  // have at least 0x03):
  kcapability_dag_destroy(dag);
  dag = kcapability_dag_create();
  node_a = kcapability_dag_node_create(dag, 801, 0xFF, 1);
  node_b = kcapability_dag_node_create(dag, 802, 0x0F, 1);
  kcapability_dag_node_t* node_c =
      kcapability_dag_node_create(dag, 803, 0x03, 1);
  TEST_ASSERT(node_a && node_b && node_c,
              "Node creation for rights path test 2.");
  kcapability_dag_add_edge(dag, node_a, node_b);  // A(0xFF) -> B(0x0F)
  kcapability_dag_add_edge(dag, node_b, node_c);  // B(0x0F) -> C(0x03)

  TEST_ASSERT(kcapability_dag_validate_derivation(dag, node_a, node_c) == true,
              "Baseline A->B->C valid.");

  node_b->rights_mask =
      0x01;  // B's rights are now less than what C needs from B.
             // Path from A: A gives 0xFF to B. B has 0x01. (0xFF & 0x01) ==
             // 0x01. B is fine. Path from B: B gives 0x01 to C. C has 0x03.
             // (0x01 & 0x03) != 0x03. C is NOT fine.
  TEST_ASSERT(kcapability_dag_validate_derivation(dag, node_a, node_c) == false,
              "Invalid derivation A->B->C after B's rights reduced.");

  kprintf_stub("Invalid rights derivation test finished.\n");
  return true;
}

static kbool test_dag_validate_derivation_no_path(void) {
  kprintf_stub("--- Running Test: %s ---\n", __func__);
  kcapability_dag_t* dag = kcapability_dag_create();
  kcapability_dag_destroy(dag);
  dag = kcapability_dag_create();

  kcapability_dag_node_t* n1 = kcapability_dag_node_create(dag, 1, 0xFF, 1);
  kcapability_dag_node_t* n2 = kcapability_dag_node_create(dag, 2, 0xFF, 1);
  kcapability_dag_node_t* n3 =
      kcapability_dag_node_create(dag, 3, 0xFF, 1);  // Unconnected
  TEST_ASSERT(n1 && n2 && n3, "Node creation for no path test.");

  kcapability_dag_add_edge(dag, n1, n2);  // n1 -> n2

  kbool is_valid_derivation = kcapability_dag_validate_derivation(dag, n1, n3);
  TEST_ASSERT(is_valid_derivation == false, "No derivation path n1 -> n3.");

  kprintf_stub("No path derivation test finished.\n");
  return true;
}

// --- Test Runner ---
void kcapability_dag_run_mathematical_tests(void) {
  kprintf_stub("=== Starting Capability DAG Mathematical Tests ===\n");
  kbool all_passed = true;

  all_passed &= test_dag_creation_destruction();
  all_passed &=
      test_dag_node_creation_lookup();  // Includes test_dag_lookup_node
                                        // implicitly
  all_passed &= test_dag_add_edge_valid();
  all_passed &= test_dag_add_edge_cycle_detection();
  all_passed &= test_dag_add_edge_rights_violation();
  all_passed &= test_dag_add_edge_security_violation();
  all_passed &= test_dag_validate_derivation_valid();
  all_passed &= test_dag_validate_derivation_invalid_rights();
  all_passed &= test_dag_validate_derivation_no_path();

  if (all_passed) {
    kprintf_stub("=== All Capability DAG Tests Passed ===\n");
  } else {
    kprintf_stub("=== SOME Capability DAG Tests FAILED ===\n");
  }
}
