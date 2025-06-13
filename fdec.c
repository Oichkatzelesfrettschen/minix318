/* Copyright (c) 1979 Regents of the University of California */
/* Modernized for C23 standards */

/*
 * pi - Pascal interpreter code translator
 * Function declaration and body handling
 *
 * Charles Haley, Bill Joy UCB
 * Version 2.0 - Modernized C23 implementation
 */

#include "pi_interpreter.h"
#include "symbol_table.h"
#include "error_handling.h"
#include "type_system.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Forward declarations
static struct nl_entry *process_function_parameters(ast_node_t *params, 
												   struct nl_entry *parent);
static bool validate_function_return_type(type_t *return_type);
static void enter_function_variable(struct nl_entry *func_entry, 
								  const char *name, type_t *type);
static void cleanup_symbol_table_level(int block_level);
static void report_unresolved_symbols(int block_level);

// Global state (minimized and encapsulated)
static struct {
	int procedure_nesting_count;
	struct nl_entry *current_function;
	bool in_error_recovery;
} function_state = {0};

/*
 * Process function/procedure/program header declaration
 * Returns the newly created symbol table entry
 */
struct nl_entry *process_function_header(ast_node_t *declaration) {
	if (!declaration) {
		return error_handler_report(ERROR_NULL_POINTER, 
								  "Null declaration in function header");
	}

	// Send reverse compilation info
	reverse_compiler_send(REVFHDR, declaration);
	
	// Handle input file list processing
	if (is_in_input_file_list(ast_get_name(declaration))) {
		operation_stack_push('l');
		parser_retrieve_state(); // Legacy parser state recovery
	}

	// Update current line number
	source_line_set(ast_get_line_number(declaration));
	
	// Check for existing declaration in current scope
	struct nl_entry *existing_entry = symbol_table_lookup_current_scope(
		ast_get_name(declaration));
	
	if (existing_entry && is_forward_declaration_resolution(existing_entry, 
														   declaration)) {
		return resolve_forward_declaration(existing_entry, declaration);
	}

	// Create new symbol table entry based on declaration type
	struct nl_entry *new_entry = create_function_entry(declaration);
	if (!new_entry) {
		return NULL;
	}

	// Process parameters and function body setup
	if (ast_get_type(declaration) != AST_PROGRAM) {
		setup_function_scope(new_entry, declaration);
	} else {
		setup_program_scope(new_entry, declaration);
	}

	return new_entry;
}

/*
 * Create appropriate symbol table entry for function/procedure/program
 */
static struct nl_entry *create_function_entry(ast_node_t *declaration) {
	const char *name = ast_get_name(declaration);
	ast_node_type_t decl_type = ast_get_type(declaration);
	
	switch (decl_type) {
		case AST_PROGRAM: {
			struct nl_entry *prog_entry = symbol_table_define(name, 
															 SYMBOL_PROGRAM, 
															 NULL, 0);
			program_entry_set(prog_entry);
			return prog_entry;
		}
		
		case AST_PROCEDURE: {
			ast_node_t *return_type = ast_get_return_type(declaration);
			if (return_type) {
				error_handler_report(ERROR_SEMANTIC, 
					"Procedures cannot have return types, only functions can");
				return NULL;
			}
			
			struct nl_entry *proc_entry = symbol_table_define(name, 
															 SYMBOL_PROCEDURE, 
															 NULL, 0);
			return symbol_table_enter(proc_entry);
		}
		
		case AST_FUNCTION: {
			type_t *return_type = process_function_return_type(
				ast_get_return_type(declaration));
			
			if (!return_type) {
				error_handler_report(ERROR_SEMANTIC,
					"Function return type must be specified");
				return NULL;
			}
			
			if (!validate_function_return_type(return_type)) {
				return NULL;
			}
			
			struct nl_entry *func_entry = symbol_table_define(name, 
															 SYMBOL_FUNCTION, 
															 return_type, NULL);
			return symbol_table_enter(func_entry);
		}
		
		default:
			error_handler_report(ERROR_INTERNAL, 
				"Unknown declaration type in function header");
			return NULL;
	}
}

/*
 * Process and validate function return type
 */
static type_t *process_function_return_type(ast_node_t *type_node) {
	if (!type_node) {
		return NULL;
	}
	
	if (ast_get_type(type_node) != AST_TYPE_ID) {
		error_handler_report(ERROR_SEMANTIC,
			"Function return type must be specified using a type identifier");
		return NULL;
	}
	
	return type_system_resolve(type_node);
}

/*
 * Validate that function return type is allowed
 */
static bool validate_function_return_type(type_t *return_type) {
	if (!return_type) return false;
	
	type_class_t type_class = type_get_class(return_type);
	
	// Check for restricted return types
	switch (type_class) {
		case TYPE_FILE:
		case TYPE_ARRAY:
		case TYPE_RECORD:
		case TYPE_SET:
		case TYPE_STRING:
			warning_handler_report("Non-portable function return type");
			
			if (compiler_options_get_strict_mode()) {
				error_handler_report(ERROR_SEMANTIC,
					"Functions should not return %s types", 
					type_get_name(type_class));
				return false;
			}
			break;
			
		default:
			break;
	}
	
	return true;
}

/*
 * Setup function/procedure scope and parameters
 */
static void setup_function_scope(struct nl_entry *entry, ast_node_t *declaration) {
	// Mark as forward declaration initially
	symbol_entry_set_flags(entry, SYMBOL_FLAG_FORWARD);
	
	// Enter new scope level
	if (symbol_table_enter_scope() >= MAX_NESTING_DEPTH) {
		error_handler_report(ERROR_FATAL,
			"Procedure/function nesting too deep (max %d levels)", 
			MAX_NESTING_DEPTH);
		exit(EXIT_FAILURE);
	}
	
	// For functions, create the function result variable
	if (symbol_entry_get_class(entry) == SYMBOL_FUNCTION) {
		enter_function_variable(entry, ast_get_name(declaration), 
							  symbol_entry_get_type(entry));
	}
	
	// Process parameters
	ast_node_t *params = ast_get_parameters(declaration);
	if (params) {
		process_function_parameters(params, entry);
	}
	
	symbol_table_exit_scope();
}

/*
 * Setup program scope and command line arguments
 */
static void setup_program_scope(struct nl_entry *entry, ast_node_t *declaration) {
	ast_node_t *params = ast_get_parameters(declaration);
	struct nl_entry *current = entry;
	
	// Program parameters are treated as file variables
	for (ast_node_t *param = params; param; param = ast_get_next(param)) {
		if (ast_get_name(param)) {
			struct nl_entry *param_entry = symbol_table_define(
				ast_get_name(param), SYMBOL_VARIABLE, 
				type_system_get_file_type(), 0);
			
			symbol_entry_chain(current, param_entry);
			current = param_entry;
		}
	}
}

/*
 * Process function/procedure parameters
 */
static struct nl_entry *process_function_parameters(ast_node_t *params, 
												   struct nl_entry *parent) {
	struct nl_entry *current = parent;
	
	for (ast_node_t *param_group = params; param_group; 
		 param_group = ast_get_next(param_group)) {
		
		ast_node_t *param_list = ast_get_parameter_list(param_group);
		if (!param_list) continue;
		
		// Get parameter type (if not procedure/function parameter)
		type_t *param_type = NULL;
		ast_node_type_t param_kind = ast_get_type(param_group);
		
		if (param_kind != AST_PROC_PARAM) {
			ast_node_t *type_node = ast_get_parameter_type(param_group);
			if (!type_node || ast_get_type(type_node) != AST_TYPE_ID) {
				error_handler_report(ERROR_SEMANTIC,
					"Parameter types must be specified using type identifiers");
				continue;
			}
			param_type = type_system_resolve(type_node);
		}
		
		// Process each parameter in the group
		for (ast_node_t *param = param_list; param; param = ast_get_next(param)) {
			struct nl_entry *param_entry = create_parameter_entry(
				param, param_kind, param_type);
			
			if (param_entry) {
				symbol_entry_chain(current, param_entry);
				current = param_entry;
			}
		}
	}
	
	return current;
}

/*
 * Create individual parameter entry
 */
static struct nl_entry *create_parameter_entry(ast_node_t *param,
											  ast_node_type_t param_kind,
											  type_t *param_type) {
	const char *param_name = ast_get_name(param);
	
	switch (param_kind) {
		case AST_VALUE_PARAM:
			if (param_type && !validate_value_parameter_type(param_type)) {
				return NULL;
			}
			return symbol_table_define(param_name, SYMBOL_VARIABLE, 
									 param_type, 0);
			
		case AST_VAR_PARAM:
			return symbol_table_define(param_name, SYMBOL_REFERENCE, 
									 param_type, 0);
			
		case AST_FUNC_PARAM:
		case AST_PROC_PARAM:
			error_handler_report(ERROR_NOT_IMPLEMENTED,
				"Procedure/function parameters not yet implemented");
			return NULL;
			
		default:
			error_handler_report(ERROR_INTERNAL, 
				"Unknown parameter type in function declaration");
			return NULL;
	}
}

/*
 * Validate value parameter type restrictions
 */
static bool validate_value_parameter_type(type_t *param_type) {
	if (type_get_class(param_type) == TYPE_FILE) {
		error_handler_report(ERROR_SEMANTIC,
			"File types cannot be passed by value");
		return false;
	}
	
	if (type_has_file_components(param_type)) {
		error_handler_report(ERROR_SEMANTIC,
			"Types containing file components cannot be passed by value: %s",
			type_get_description(param_type));
		return false;
	}
	
	return true;
}

/*
 * Enter function result variable into symbol table
 */
static void enter_function_variable(struct nl_entry *func_entry, 
								  const char *name, type_t *type) {
	struct nl_entry *fvar = symbol_table_define(name, SYMBOL_FUNCTION_VAR, 
											   type, 0);
	symbol_entry_chain(fvar, func_entry);
	symbol_entry_set_function_var(func_entry, fvar);
}

/*
 * Initialize function body processing
 * Called when entering the actual function implementation
 */
struct nl_entry *initialize_function_body(struct nl_entry *func_entry) {
	if (!func_entry) {
		return NULL;
	}
	
	// Enter new scope level
	if (symbol_table_enter_scope() >= MAX_NESTING_DEPTH) {
		error_handler_report(ERROR_FATAL,
			"Too many levels of function/procedure nesting (max %d)",
			MAX_NESTING_DEPTH);
		exit(EXIT_FAILURE);
	}
	
	reverse_compiler_send(REVFBDY);
	error_handler_set_scope_base(symbol_table_get_current_level());
	
	// Save symbol table state for cleanup
	symbol_entry_set_name_list_ptr(func_entry, symbol_table_get_name_ptr());
	
	// Enter parameters into new scope
	if (symbol_entry_get_class(func_entry) != SYMBOL_PROGRAM) {
		enter_parameters_in_scope(func_entry);
	}
	
	// Enter function variable for functions
	if (symbol_entry_get_class(func_entry) == SYMBOL_FUNCTION) {
		struct nl_entry *fvar = symbol_entry_get_function_var(func_entry);
		if (fvar) {
			symbol_table_enter(fvar);
		}
	}
	
	return func_entry;
}

/*
 * Enter function parameters into current scope
 */
static void enter_parameters_in_scope(struct nl_entry *func_entry) {
	for (struct nl_entry *param = symbol_entry_get_chain(func_entry);
		 param; param = symbol_entry_get_chain(param)) {
		symbol_table_enter(param);
	}
}

/*
 * Finalize function processing
 * Handles cleanup, error checking, and symbol resolution
 */
void finalize_function(struct nl_entry *func_entry, ast_node_t *body, 
					  int end_line) {
	if (!func_entry) {
		symbol_table_exit_scope();
		return;
	}
	
	ast_node_t *block = ast_get_block(body);
	
	reverse_compiler_send(REVFEND, body, end_line, 
						 error_handler_scope_clean());
	
	if (function_state.current_function) {
		function_state.current_function = func_entry;
	}
	
	// Check for unresolved symbols and clean up
	source_line_set(end_line);
	cleanup_function_scope(func_entry);
	
	// Restore symbol table state
	symbol_table_restore_name_ptr(symbol_entry_get_name_list_ptr(func_entry));
	
	// Mark function as resolved
	symbol_entry_clear_flags(func_entry, SYMBOL_FLAG_FORWARD);
	
	error_handler_enable_line_numbers();
	symbol_table_exit_scope();
	
	// Handle input file list cleanup
	if (is_in_input_file_list(symbol_entry_get_name(func_entry))) {
		operation_stack_pop('l');
	}
}

/*
 * Clean up function scope and report unresolved symbols
 */
static void cleanup_function_scope(struct nl_entry *func_entry) {
	int current_level = symbol_table_get_current_level();
	
	report_unresolved_symbols(current_level);
	cleanup_symbol_table_level(current_level);
	
#ifdef DEBUG
	symbol_table_dump(symbol_entry_get_name_list_ptr(func_entry),
					 symbol_entry_get_name(func_entry));
#endif
}

/*
 * Report unresolved symbols in current scope
 */
static void report_unresolved_symbols(int block_level) {
	for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
		struct nl_entry *entry = symbol_table_get_bucket(i);
		
		while (entry && symbol_entry_get_block_level(entry) == block_level) {
			if (symbol_entry_get_class(entry) == SYMBOL_BAD_USE) {
				report_symbol_usage_error(entry);
			}
			entry = symbol_entry_get_next(entry);
		}
	}
}

/*
 * Report specific symbol usage errors
 */
static void report_symbol_usage_error(struct nl_entry *entry) {
	struct usage_info *usage = symbol_entry_get_usage_info(entry);
	const char *symbol_name = symbol_entry_get_name(entry);
	
	bool multiple_uses = (usage && usage->next != NULL);
	const char *plural = multiple_uses ? "s" : "";
	
	error_handler_hold_newline();
	
	if (symbol_entry_has_flag(entry, SYMBOL_FLAG_UNDEFINED)) {
		error_handler_report_detailed("%s undefined on line%s", 
									symbol_name, plural);
	} else {
		error_handler_report_detailed("%s improperly used on line%s", 
									symbol_name, plural);
	}
	
	// Print line numbers where symbol was used
	function_state.procedure_nesting_count = 10;
	print_usage_line_numbers(usage);
	putchar('\n');
}

/*
 * Print line numbers where symbol was used incorrectly
 */
static void print_usage_line_numbers(struct usage_info *usage) {
	if (!usage) return;
	
	if (usage->next) {
		print_usage_line_numbers(usage->next);
	}
	
	if (function_state.procedure_nesting_count == 0) {
		printf("\n\t");
		function_state.procedure_nesting_count = 20;
	}
	
	function_state.procedure_nesting_count--;
	printf(" %d", usage->line_number);
}

/*
 * Clean up symbol table level
 */
static void cleanup_symbol_table_level(int block_level) {
	for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
		struct nl_entry *entry = symbol_table_get_bucket(i);
		
		// Skip entries from deeper scopes and pop this level
		while (entry && symbol_entry_get_block_level(entry) == block_level) {
			entry = symbol_entry_get_next(entry);
		}
		
		symbol_table_set_bucket(i, entry);
	}
}

/*
 * Enhanced error reporting for function context
 */
void report_function_error(const char *format, ...) {
	if (function_state.current_function) {
		parser_synchronize();
		printf("In %s %s:\n", 
			   symbol_class_name(symbol_entry_get_class(function_state.current_function)),
			   symbol_entry_get_name(function_state.current_function));
		function_state.current_function = NULL;
	}
	
	error_handler_disable_line_numbers();
	
	va_list args;
	va_start(args, format);
	error_handler_report_va(ERROR_SEMANTIC, format, args);
	va_end(args);
}

/*
 * Check if this is a forward declaration resolution
 */
static bool is_forward_declaration_resolution(struct nl_entry *existing,
											ast_node_t *declaration) {
	if (!existing || !declaration) return false;
	
	symbol_class_t existing_class = symbol_entry_get_class(existing);
	ast_node_type_t decl_type = ast_get_type(declaration);
	
	// Must be function or procedure
	if (existing_class != SYMBOL_FUNCTION && existing_class != SYMBOL_PROCEDURE) {
		return false;
	}
	
	// Must be marked as forward declaration
	if (!symbol_entry_has_flag(existing, SYMBOL_FLAG_FORWARD)) {
		return false;
	}
	
	// Must be in same block level
	return symbol_entry_get_block_level(existing) == 
		   symbol_table_get_current_level();
}

/*
 * Resolve a forward declaration
 */
static struct nl_entry *resolve_forward_declaration(struct nl_entry *existing,
												   ast_node_t *declaration) {
	// For functions, verify that type isn't redeclared
	if (symbol_entry_get_class(existing) == SYMBOL_FUNCTION) {
		ast_node_t *return_type = ast_get_return_type(declaration);
		if (return_type) {
			error_handler_report(ERROR_SEMANTIC,
				"Function type should be given only in forward declaration");
		}
	}
	
	return existing;
}

/* 
 * Module cleanup function
 */
void function_handler_cleanup(void) {
	function_state.procedure_nesting_count = 0;
	function_state.current_function = NULL;
	function_state.in_error_recovery = false;
}
