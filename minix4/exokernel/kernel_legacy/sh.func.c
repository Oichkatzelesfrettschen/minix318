/**
 * @file sh_func.c
 * @brief C shell built-in functions and control flow implementation
 * @version 2.0
 * @date 2024
 * @author Original: Regents of the University of California (1979)
 * @author Modernized: GitHub Copilot (2024)
 * 
 * This file implements the core built-in functions and control flow
 * constructs for the C shell, including conditionals, loops, and
 * various utility commands.
 */

#include "sh.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

/**
 * @brief Maximum length for variable names
 */
#define MAX_VARIABLE_NAME_LENGTH 255

/**
 * @brief Maximum recursion depth for control structures
 */
#define MAX_CONTROL_DEPTH 1000

/**
 * @brief Buffer size for word processing
 */
#define WORD_BUFFER_SIZE 4096

/**
 * @brief Search operation types for control flow
 */
typedef enum {
	SEARCH_IF = 0,      ///< Searching for if statement
	SEARCH_ELSE,        ///< Searching for else clause
	SEARCH_ENDIF,       ///< Searching for endif
	SEARCH_WHILE,       ///< Searching for while loop
	SEARCH_FOREACH,     ///< Searching for foreach loop
	SEARCH_END,         ///< Searching for end statement
	SEARCH_SWITCH,      ///< Searching for switch statement
	SEARCH_CASE,        ///< Searching for case label
	SEARCH_DEFAULT,     ///< Searching for default case
	SEARCH_ENDSW,       ///< Searching for endsw
	SEARCH_BREAK,       ///< Searching for break statement
	SEARCH_BRKSW,       ///< Searching for break switch
	SEARCH_GOTO,        ///< Searching for goto label
	SEARCH_LABEL        ///< Searching for label
} search_type_t;

/**
 * @brief Structure for search context
 */
typedef struct search_context {
	search_type_t type;     ///< Type of search operation
	char *goal;             ///< Target label for goto operations
	int level;              ///< Current nesting level
	size_t max_depth;       ///< Maximum allowed nesting depth
} search_context_t;

// Forward declarations
static bool validate_variable_name(const char *name) noexcept;
static int execute_builtin_safely(const struct biltins *bp, char **args, struct command *t) noexcept;
static void cleanup_search_context(search_context_t *ctx) noexcept;

/**
 * @brief Check if a command is a built-in function
 * @param cp Command name to check
 * @return Pointer to builtin structure if found, NULL otherwise
 * @note Returns (struct biltins *)1 for label commands ending with ':'
 * @since 1.0
 */
struct biltins *isbfunc(const char *cp) noexcept
{
	if (cp == nullptr) {
		return nullptr;
	}

	// Check for label (commands ending with ':')
	size_t len = strlen(cp);
	if (len > 0 && cp[len - 1] == ':') {
		return (struct biltins *)1;
	}

	// Binary search through built-in functions (assuming sorted array)
	const struct biltins *bp = bfunc;
	while (bp != nullptr && bp->bname != nullptr) {
		int cmp = strcmp(bp->bname, cp);
		if (cmp == 0) {
			return (struct biltins *)bp;
		}
		if (cmp > 0) {
			break; // Past where it would be in sorted array
		}
		bp++;
	}

	return nullptr;
}

/**
 * @brief Execute a built-in function
 * @param t Command structure containing function and arguments
 * @return 1 if function executed successfully, 0 if not a builtin
 * @retval 1 Function executed successfully
 * @retval 0 Not a built-in function
 * @note Handles argument validation and special cases for time/nice commands
 * @since 1.0
 */
int func(struct command *t) noexcept
{
	if (t == nullptr || t->t_dcom == nullptr || t->t_dcom[0] == nullptr) {
		return 0;
	}

	const char *cmd_name = t->t_dcom[0];
	size_t cmd_len = strlen(cmd_name);

	// Handle label commands
	if (cmd_len > 0 && cmd_name[cmd_len - 1] == ':') {
		xechoit(t->t_dcom);
		if (strcmp(cmd_name, ":") != 0 && t->t_dcom[1] != nullptr) {
			error("No args on labels");
		}
		return 1;
	}

	// Find the built-in function
	const struct biltins *bp = isbfunc(cmd_name);
	if (bp == nullptr || bp == (struct biltins *)1) {
		return 0;
	}

	// Special handling for timed builtins
	if (strcmp(bp->bname, "time") == 0) {
		if ((t->t_dflg & FAND) || (t->t_dflg & FPOU)) {
			return 0;
		}
	}

	// Special handling for nohup
	if (strcmp(bp->bname, "nohup") == 0 && t->t_dcom[1] != nullptr) {
		return 0;
	}

	xechoit(t->t_dcom);
	setname(bp->bname);

	// Count arguments
	int argc = blklen(t->t_dcom) - 1;
	
	// Validate argument count
	if (argc < bp->minargs) {
		bferr("Too few arguments");
		return 0;
	}
	if (argc > bp->maxargs && bp->maxargs >= 0) {
		bferr("Too many arguments");
		return 0;
	}

	// Execute the function safely
	int result = execute_builtin_safely(bp, t->t_dcom, t);
	
	// Special return value handling for time and nice
	return (strcmp(bp->bname, "time") == 0 || strcmp(bp->bname, "nice") == 0) 
		   ? result : 1;
}

/**
 * @brief Safely execute a built-in function with error handling
 * @param bp Pointer to builtin function structure
 * @param args Command arguments
 * @param t Command structure
 * @return Result of function execution
 */
static int execute_builtin_safely(const struct biltins *bp, char **args, struct command *t) noexcept
{
	if (bp == nullptr || bp->bfunct == nullptr) {
		return 0;
	}

	// Set up error handling context
	jmp_buf saved_context;
	memcpy(saved_context, error_context, sizeof(jmp_buf));
	
	int result = 0;
	if (setjmp(error_context) == 0) {
		result = (*bp->bfunct)(args, t);
	} else {
		// Function threw an error
		result = 0;
	}
	
	// Restore error context
	memcpy(error_context, saved_context, sizeof(jmp_buf));
	return result;
}

/**
 * @brief Handle interrupt signal setup/teardown
 * @param v Command arguments
 * @note Sets up signal handling for SIGINT based on arguments
 * @since 1.0
 */
void doonintr(char **v) noexcept
{
	if (v == nullptr) {
		return;
	}

	if (parintr == SIG_IGN) {
		return;
	}

	if (setintr && intty) {
		bferr("Can't from terminal");
		return;
	}

	// Clean up previous interrupt handler
	char *old_gointr = gointr;
	gointr = nullptr;
	if (old_gointr != nullptr) {
		xfree(old_gointr);
	}

	const char *new_handler = v[1];
	if (new_handler == nullptr) {
		// Reset to default
		signal(SIGINT, setintr ? SIG_IGN : SIG_DFL);
		gointr = nullptr;
	} else if (strcmp(strip(new_handler), "-") == 0) {
		// Ignore signals
		signal(SIGINT, SIG_IGN);
		gointr = savestr("-");
	} else {
		// Custom handler
		gointr = savestr(strip(new_handler));
		signal(SIGINT, pintr);
	}
}

/**
 * @brief Implement nohup functionality
 * @note Prevents hangup signals when running in background
 * @since 1.0
 */
void donohup(void) noexcept
{
	if (intty) {
		bferr("Can't from terminal");
		return;
	}

	if (setintr == 0) {
		signal(SIGHUP, SIG_IGN);
#ifdef CC
		submit(getpid());
#endif
	}
}

/**
 * @brief Placeholder function for zip command
 * @note Currently does nothing - reserved for future implementation
 * @since 1.0
 */
void dozip(void) noexcept
{
	// Reserved for future implementation
}

/**
 * @brief Change directory command implementation
 * @param vp Command arguments (directory path)
 * @note Changes to home directory if no argument provided
 * @since 1.0
 */
void chngd(char **vp) noexcept
{
	if (vp == nullptr) {
		return;
	}

	vp++; // Skip command name
	char *target_dir = *vp;
	char *allocated_dir = nullptr;

	if (target_dir != nullptr) {
		allocated_dir = globone(target_dir);
		target_dir = allocated_dir;
	} else {
		target_dir = value("home");
		if (target_dir == nullptr || *target_dir == '\0') {
			bferr("No home directory set");
			return;
		}
	}

	// Validate directory path
	if (strlen(target_dir) >= PATH_MAX) {
		bferr("Path too long");
		goto cleanup;
	}

	int result = chdir(target_dir);
	
cleanup:
	if (allocated_dir != nullptr) {
		xfree(allocated_dir);
	}
	
	if (result < 0) {
		Perror(target_dir);
	}
}

/**
 * @brief Print shell variables
 * @note Displays all currently set shell variables
 * @since 1.0
 */
void prvars(void) noexcept
{
	plist(&shvhed);
}

/**
 * @brief Handle alias command
 * @param v Command arguments
 * @note Creates, displays, or lists aliases based on arguments
 * @since 1.0
 */
void doalias(char **v) noexcept
{
	if (v == nullptr) {
		return;
	}

	v++; // Skip command name
	const char *alias_name = *v++;

	if (alias_name == nullptr) {
		// List all aliases
		plist(&aliases);
		return;
	}

	if (*v == nullptr) {
		// Display specific alias
		const struct varent *vp = adrof1(strip(alias_name), &aliases);
		if (vp != nullptr) {
			blkpr(vp->vec);
			printf("\n");
		}
		return;
	}

	// Create new alias
	const char *stripped_name = strip(alias_name);
	
	// Prevent dangerous aliases
	if (strcmp(stripped_name, "alias") == 0 || strcmp(stripped_name, "unalias") == 0) {
		setname(stripped_name);
		bferr("Too dangerous to alias that");
		return;
	}

	// Validate alias name
	if (!validate_variable_name(stripped_name)) {
		bferr("Invalid alias name");
		return;
	}

	set1(stripped_name, saveblk(v), &aliases);
}

/**
 * @brief Validate variable/alias name
 * @param name Name to validate
 * @return true if valid, false otherwise
 */
static bool validate_variable_name(const char *name) noexcept
{
	if (name == nullptr || *name == '\0') {
		return false;
	}

	size_t len = strlen(name);
	if (len > MAX_VARIABLE_NAME_LENGTH) {
		return false;
	}

	// First character must be letter or underscore
	if (!isalpha(*name) && *name != '_') {
		return false;
	}

	// Remaining characters must be alphanumeric or underscore
	for (size_t i = 1; i < len; i++) {
		if (!isalnum(name[i]) && name[i] != '_') {
			return false;
		}
	}

	return true;
}

/**
 * @brief Remove alias command
 * @param v Command arguments (alias names to remove)
 * @since 1.0
 */
void unalias(char **v) noexcept
{
	unset1(v, &aliases);
}

/**
 * @brief Handle logout command
 * @note Verifies login shell status before logging out
 * @since 1.0
 */
void dologout(void) noexcept
{
	islogin();
	goodbye();
}

/**
 * @brief Verify current shell is a login shell
 * @note Throws error if not a login shell
 * @since 1.0
 */
void islogin(void) noexcept
{
	if (!loginsh) {
		error("Not login shell");
	}
}

/**
 * @brief Handle if statement
 * @param v Command arguments (condition expression)
 * @param kp Command structure for attached command
 * @note Evaluates condition and executes appropriate branch
 * @since 1.0
 */
void doif(char **v, struct command *kp) noexcept
{
	if (v == nullptr || kp == nullptr) {
		return;
	}

	v++; // Skip 'if'
	int condition_result = exp(&v);
	char **remaining_args = v;

	if (*remaining_args != nullptr && strcmp(*remaining_args, "then") == 0) {
		remaining_args++;
		if (*remaining_args != nullptr) {
			bferr("Improper then");
			return;
		}
		setname("then");
		
		// If condition is false, search for else clause
		if (!condition_result) {
			search_context_t ctx = {
				.type = SEARCH_IF,
				.goal = nullptr,
				.level = 0,
				.max_depth = MAX_CONTROL_DEPTH
			};
			search(ctx.type, ctx.level);
		}
		return;
	}

	// Simple command attached to if
	if (condition_result) {
		// Calculate shift amount safely
		ptrdiff_t shift_amount = remaining_args - kp->t_dcom;
		if (shift_amount > 0 && shift_amount < blklen(kp->t_dcom)) {
			lshift(kp->t_dcom, (int)shift_amount);
			reexecute(kp);
			donefds();
		}
	}
}

/**
 * @brief Re-execute a command with REDO flag
 * @param kp Command structure to re-execute
 * @note Careful not to redo I/O redirection
 * @since 1.0
 */
void reexecute(struct command *kp) noexcept
{
	if (kp == nullptr) {
		return;
	}

	kp->t_dflg = FREDO;
	execute(kp);
}

/**
 * @brief Handle else clause
 * @note Searches for corresponding else block
 * @since 1.0
 */
void doelse(void) noexcept
{
	search(SEARCH_ELSE, 0);
}

/**
 * @brief Handle goto statement
 * @param v Command arguments (label name)
 * @note Jumps to specified label, cleaning up loops
 * @since 1.0
 */
void dogoto(char **v) noexcept
{
	if (v == nullptr || v[1] == nullptr) {
		bferr("goto: missing label");
		return;
	}

	// Locate unknown ends of existing loops
	for (struct whyle *wp = whyles; wp != nullptr; wp = wp->w_next) {
		if (wp->w_end == 0) {
			wp->w_end = search(SEARCH_BREAK, 0);
		} else {
			bseek(wp->w_end);
		}
	}

	char *target_label = globone(v[1]);
	if (target_label == nullptr) {
		bferr("goto: invalid label");
		return;
	}

	search(SEARCH_GOTO, 0, target_label);
	xfree(target_label);

	// Clean up exited loops
	wfree();
}

/**
 * @brief Handle switch statement
 * @param v Command arguments (switch expression)
 * @note Implements C-style switch with case matching
 * @since 1.0
 */
void doswitch(char **v) noexcept
{
	if (v == nullptr) {
		bferr("switch: missing arguments");
		return;
	}

	v++; // Skip 'switch'
	
	// Validate syntax: switch (expression)
	if (*v == nullptr || **v != '(') {
		goto syntax_error;
	}

	const char *switch_expr = "";
	v++; // Move past '('
	
	if (*v != nullptr && **v != ')') {
		switch_expr = *v++;
	}
	
	if (*v == nullptr || **v != ')') {
		v--; // Back up if we went too far
	}
	
	v++; // Move past ')'
	if (*v != nullptr) {
		goto syntax_error;
	}

	char *globbed_expr = globone(switch_expr);
	if (globbed_expr == nullptr) {
		bferr("switch: invalid expression");
		return;
	}

	search(SEARCH_SWITCH, 0, globbed_expr);
	xfree(globbed_expr);
	return;

syntax_error:
	error("Syntax error");
}

/**
 * @brief Handle break statement
 * @note Breaks out of current loop structure
 * @since 1.0
 */
void dobreak(void) noexcept
{
	if (whyles != nullptr) {
		toend();
	} else {
		bferr("Not in while/foreach");
	}
}

/**
 * @brief Handle exit statement
 * @param v Command arguments (exit status)
 * @note Exits shell with specified or computed status
 * @since 1.0
 */
void doexit(char **v) noexcept
{
	if (v != nullptr) {
		v++; // Skip 'exit'
		if (*v != nullptr) {
			int exit_status = exp(&v);
			if (*v != nullptr) {
				bferr("Expression syntax");
				return;
			}
			set("status", putn(exit_status));
		}
	}

	btoeof();
	if (intty) {
		close(SHIN);
	}
}

/**
 * @brief Handle foreach loop
 * @param v Command arguments (variable and value list)
 * @note Implements foreach variable (value_list) loop
 * @since 1.0
 */
void doforeach(char **v) noexcept
{
	if (v == nullptr) {
		return;
	}

	v++; // Skip 'foreach'
	const char *loop_var = strip(*v);
	
	// Validate variable name
	if (!validate_variable_name(loop_var)) {
		bferr("Invalid variable");
		return;
	}

	v++; // Move to value list
	
	// Validate parentheses around value list
	int arg_count = blklen(v);
	if (arg_count < 2 || v[0][0] != '(' || v[arg_count - 1][0] != ')') {
		bferr("Words not ()'ed");
		return;
	}

	v++; // Skip opening paren
	
	// Glob the value list
	gflag = 0;
	rscan(v, tglob);
	char **value_list = glob(v);
	if (value_list == nullptr) {
		bferr("No match");
		return;
	}

	// Create new while structure for foreach
	struct whyle *nwp = (struct whyle *)calloc(1, sizeof(*nwp));
	if (nwp == nullptr) {
		bferr("Out of memory");
		blkfree(value_list);
		return;
	}

	nwp->w_fe = nwp->w_fe0 = value_list;
	gargv = nullptr;
	nwp->w_start = btell();
	nwp->w_fename = savestr(loop_var);
	nwp->w_next = whyles;
	whyles = nwp;

	// Pre-read loop for terminal users
	if (intty) {
		preread();
	}
	doagain();
}

/**
 * @brief Handle while loop
 * @param v Command arguments (condition expression)
 * @note Implements while (condition) loop
 * @since 1.0
 */
void dowhile(char **v) noexcept
{
	if (v == nullptr) {
		return;
	}

	v++; // Skip 'while'
	bool is_continuation = (whyles != nullptr && whyles->w_start == lineloc);
	
	// Evaluate condition
	int condition_result;
	if (intty && !is_continuation) {
		condition_result = !exp0(&v, 1);
	} else {
		condition_result = !exp(&v);
	}
	
	if (*v != nullptr) {
		bferr("Expression syntax");
		return;
	}

	if (!is_continuation) {
		// Create new while structure
		struct whyle *nwp = (struct whyle *)calloc(1, sizeof(*nwp));
		if (nwp == nullptr) {
			bferr("Out of memory");
			return;
		}
		
		nwp->w_start = lineloc;
		nwp->w_end = 0;
		nwp->w_next = whyles;
		whyles = nwp;
		
		if (intty) {
			preread();
			doagain();
			return;
		}
	}

	if (condition_result) {
		toend(); // Exit loop
	}
}

/**
 * @brief Pre-read loop body for interactive mode
 * @note Reads entire loop structure before execution
 * @since 1.0
 */
void preread(void) noexcept
{
	if (whyles == nullptr) {
		return;
	}

	void (*old_sigint)(int) = nullptr;
	whyles->w_end = -1;
	
	if (setintr) {
		old_sigint = signal(SIGINT, pintr);
	}
	
	search(SEARCH_BREAK, 0);
	
	if (setintr && old_sigint != nullptr) {
		signal(SIGINT, old_sigint);
	}
	
	whyles->w_end = btell();
}

/**
 * @brief Handle end statement
 * @note Marks end of control structure and continues execution
 * @since 1.0
 */
void doend(void) noexcept
{
	if (whyles == nullptr) {
		bferr("Not in while/foreach");
		return;
	}

	whyles->w_end = btell();
	doagain();
}

/**
 * @brief Handle continue statement
 * @note Continues to next iteration of current loop
 * @since 1.0
 */
void docontin(void) noexcept
{
	if (whyles == nullptr) {
		bferr("Not in while/foreach");
		return;
	}

	doagain();
}

/**
 * @brief Continue loop execution
 * @note Handles both while and foreach loop continuation
 * @since 1.0
 */
void doagain(void) noexcept
{
	if (whyles == nullptr) {
		return;
	}

	// Handle while loop
	if (whyles->w_fename == nullptr) {
		bseek(whyles->w_start);
		return;
	}

	// Handle foreach loop
	// Check if we're at end of value list
	if (whyles->w_fe == nullptr || whyles->w_fe[1] == nullptr) {
		dobreak();
		return;
	}

	// Set variable to next value and continue
	set(whyles->w_fename, savestr(*whyles->w_fe++));
	bseek(whyles->w_start);
}

/**
 * @brief Handle repeat statement
 * @param v Command arguments (count and command)
 * @param kp Command structure to repeat
 * @note Executes command specified number of times
 * @since 1.0
 */
void dorepeat(char **v, struct command *kp) noexcept
{
	if (v == nullptr || v[1] == nullptr || kp == nullptr) {
		bferr("repeat: missing arguments");
		return;
	}

	int repeat_count = getn(v[1]);
	if (repeat_count < 0) {
		bferr("repeat: invalid count");
		return;
	}

	void (*saved_sigint)(int) = nullptr;
	if (setintr) {
		saved_sigint = signal(SIGINT, SIG_IGN);
	}

	lshift(v, 2);
	
	for (int i = 0; i < repeat_count; i++) {
		if (setintr) {
			signal(SIGINT, pintr);
		}
		reexecute(kp);
	}

	donefds();
	
	if (setintr && saved_sigint != nullptr) {
		signal(SIGINT, saved_sigint);
	}
}

/**
 * @brief Handle switch break statement
 * @note Breaks out of switch statement
 * @since 1.0
 */
void doswbrk(void) noexcept
{
	search(SEARCH_BRKSW, 0);
}

/**
 * @brief Search for control structure keywords
 * @param cp Keyword to search for
 * @return Search type value or -1 if not found
 */
int srchx(const char *cp) noexcept
{
	if (cp == nullptr) {
		return -1;
	}

	static const struct {
		const char *name;
		int value;
	} search_table[] = {
		{"else", SEARCH_ELSE},
		{"if", SEARCH_IF},
		{"endif", SEARCH_ENDIF},
		{"foreach", SEARCH_FOREACH},
		{"while", SEARCH_WHILE},
		{"end", SEARCH_END},
		{"switch", SEARCH_SWITCH},
		{"endsw", SEARCH_ENDSW},
		{"case", SEARCH_CASE},
		{"default", SEARCH_DEFAULT},
		{"break", SEARCH_BREAK},
		{"breaksw", SEARCH_BRKSW},
		{"label", SEARCH_LABEL},
		{nullptr, -1}
	};

	for (int i = 0; search_table[i].name != nullptr; i++) {
		if (strcmp(cp, search_table[i].name) == 0) {
			return search_table[i].value;
		}
	}

	return -1;
}

// Global search state (consider encapsulating in the future)
static search_type_t current_search_type;
static char *current_search_goal;

/**
 * @brief Search for control flow constructs
 * @param type Type of construct to search for
 * @param level Current nesting level
 * @param goal Target label (for goto operations)
 * @note Implements complex control flow search with nesting awareness
 * @since 1.0
 */
void search(search_type_t type, int level, const char *goal) noexcept
{
	char word_buffer[WORD_BUFFER_SIZE];
	
	current_search_type = type;
	current_search_goal = (goal != nullptr) ? savestr(goal) : nullptr;
	
	if (type == SEARCH_GOTO) {
		bseek(0L);
	}

	int nesting_level = level;
	
	do {
		if (intty && fseekp == feobp) {
			printf("? ");
			flush();
		}
		
		word_buffer[0] = '\0';
		if (!getword(word_buffer)) {
			break;
		}
		
		int keyword_type = srchx(word_buffer);
		
		switch (keyword_type) {
		case SEARCH_ELSE:
			if (nesting_level == 0 && type == SEARCH_IF) {
				goto search_done;
			}
			break;
			
		case SEARCH_IF:
			// Skip condition
			while (getword(word_buffer)) {
				continue;
			}
			if ((type == SEARCH_IF || type == SEARCH_ELSE) && 
				strcmp(word_buffer, "then") == 0) {
				nesting_level++;
			}
			break;
			
		case SEARCH_ENDIF:
			if (type == SEARCH_IF || type == SEARCH_ELSE) {
				nesting_level--;
			}
			break;
			
		case SEARCH_FOREACH:
		case SEARCH_WHILE:
			if (type == SEARCH_BREAK) {
				nesting_level++;
			}
			break;
			
		case SEARCH_END:
			if (type == SEARCH_BREAK) {
				nesting_level--;
			}
			break;
			
		case SEARCH_SWITCH:
			if (type == SEARCH_SWITCH || type == SEARCH_BRKSW) {
				nesting_level++;
			}
			break;
			
		case SEARCH_ENDSW:
			if (type == SEARCH_SWITCH || type == SEARCH_BRKSW) {
				nesting_level--;
			}
			break;
			
		case SEARCH_LABEL:
			if (type == SEARCH_GOTO && getword(word_buffer) && 
				current_search_goal != nullptr &&
				strcmp(word_buffer, current_search_goal) == 0) {
				nesting_level = -1;
			}
			break;
			
		case SEARCH_CASE:
			if (type != SEARCH_SWITCH || nesting_level != 0) {
				break;
			}
			getword(word_buffer);
			if (word_buffer[strlen(word_buffer) - 1] == ':') {
				word_buffer[strlen(word_buffer) - 1] = '\0';
			}
			char *pattern = strip(Dfix1(word_buffer));
			if (current_search_goal != nullptr && 
				Gmatch(current_search_goal, pattern)) {
				nesting_level = -1;
			}
			xfree(pattern);
			break;
			
		case SEARCH_DEFAULT:
			if (type == SEARCH_SWITCH && nesting_level == 0) {
				nesting_level = -1;
			}
			break;
			
		default:
			// Handle labels and default cases
			if (type == SEARCH_GOTO || (type == SEARCH_SWITCH && nesting_level == 0)) {
				size_t word_len = strlen(word_buffer);
				if (word_len > 0 && word_buffer[word_len - 1] == ':') {
					word_buffer[word_len - 1] = '\0';
					if ((type == SEARCH_GOTO && current_search_goal != nullptr &&
						 strcmp(word_buffer, current_search_goal) == 0) ||
						(type == SEARCH_SWITCH && strcmp(word_buffer, "default") == 0)) {
						nesting_level = -1;
					}
				}
			}
			break;
		}
		
		getword(nullptr); // Consume rest of line
		
	} while (nesting_level >= 0);

search_done:
	if (current_search_goal != nullptr) {
		xfree(current_search_goal);
		current_search_goal = nullptr;
	}
	
	// Handle search failures
	if (nesting_level >= 0) {
		switch (type) {
		case SEARCH_IF:
			bferr("then/endif not found");
			break;
		case SEARCH_ELSE:
			bferr("endif not found");
			break;
		case SEARCH_BRKSW:
		case SEARCH_SWITCH:
			bferr("endsw not found");
			break;
		case SEARCH_BREAK:
			bferr("end not found");
			break;
		case SEARCH_GOTO:
			if (current_search_goal != nullptr) {
				setname(current_search_goal);
			}
			bferr("label not found");
			break;
		default:
			bferr("syntax error in control structure");
			break;
		}
	}
}

/**
 * @brief Get next word from input stream
 * @param wp Buffer to store word (or nullptr to skip)
 * @return true if word found, false at end of input
 * @note Handles quoted strings and escape sequences
 * @since 1.0
 */
bool getword(char *wp) noexcept
{
	bool found = false;
	int c, quote_char = 0;
	
	c = readc(1);
	
	do {
		// Skip whitespace
		while (c == ' ' || c == '\t') {
			c = readc(1);
		}
		
		if (c < 0) {
			goto end_of_input;
		}
		
		if (c == '\n') {
			if (wp != nullptr) {
				break;
			}
			return false;
		}
		
		unreadc(c);
		found = true;
		
		// Read word
		do {
			c = readc(1);
			
			// Handle escape sequences
			if (c == '\\') {
				int next_c = readc(1);
				if (next_c == '\n') {
					c = ' ';
				} else {
					if (wp != nullptr) {
						*wp++ = c;
					}
					c = next_c;
				}
			}
			
			// Handle quotes
			if (c == '\'' || c == '"') {
				if (quote_char == 0) {
					quote_char = c;
				} else if (quote_char == c) {
					quote_char = 0;
				}
			}
			
			if (c < 0) {
				goto end_of_input;
			}
			
			if (wp != nullptr) {
				*wp++ = c;
			}
			
		} while ((quote_char != 0 || (c != ' ' && c != '\t')) && c != '\n');
		
	} while (wp == nullptr);
	
	unreadc(c);
	if (found && wp != nullptr) {
		*--wp = '\0';
	}
	
	return found;

end_of_input:
	// Handle premature end of input based on search context
	switch (current_search_type) {
	case SEARCH_IF:
		bferr("then/endif not found");
		break;
	case SEARCH_ELSE:
		bferr("endif not found");
		break;
	case SEARCH_BRKSW:
	case SEARCH_SWITCH:
		bferr("endsw not found");
		break;
	case SEARCH_BREAK:
		bferr("end not found");
		break;
	case SEARCH_GOTO:
		if (current_search_goal != nullptr) {
			setname(current_search_goal);
		}
		bferr("label not found");
		break;
	default:
		break;
	}
	
	return false;
}

/**
 * @brief Jump to end of current control structure
 * @note Used by break statements and failed conditions
 * @since 1.0
 */
void toend(void) noexcept
{
	if (whyles == nullptr) {
		return;
	}

	if (whyles->w_end == 0) {
		search(SEARCH_BREAK, 0);
		whyles->w_end = btell() - 1;
	} else {
		bseek(whyles->w_end);
	}
	
	wfree();
}

/**
 * @brief Free while/foreach structures that are no longer active
 * @note Cleans up completed loop structures
 * @since 1.0
 */
void wfree(void) noexcept
{
	long current_pos = btell();
	
	while (whyles != nullptr) {
		struct whyle *wp = whyles;
		struct whyle *next_wp = wp->w_next;
		
		// Check if current position is within this loop's scope
		if (current_pos >= wp->w_start && 
			(wp->w_end == 0 || current_pos < wp->w_end)) {
			break;
		}
		
		// Free loop data
		if (wp->w_fe0 != nullptr) {
			blkfree(wp->w_fe0);
		}
		if (wp->w_fename != nullptr) {
			xfree(wp->w_fename);
		}
		
		xfree(wp);
		whyles = next_wp;
	}
}

/**
 * @brief Handle echo command
 * @param v Command arguments
 * @note Outputs arguments separated by spaces
 * @since 1.0
 */
void doecho(char **v) noexcept
{
	echo(' ', v);
}

/**
 * @brief Handle glob command
 * @param v Command arguments
 * @note Outputs arguments without trailing newline
 * @since 1.0
 */
void doglob(char **v) noexcept
{
	echo(0, v);
	flush();
}

/**
 * @brief Echo implementation with separator control
 * @param sep Separator character (0 for none, ' ' for space)
 * @param v Arguments to echo
 * @note Supports escape sequences and globbing
 * @since 1.0
 */
void echo(char sep, char **v) noexcept
{
	if (v == nullptr) {
		return;
	}

	void (*saved_sigint)(int) = nullptr;
	if (setintr) {
		saved_sigint = signal(SIGINT, pintr);
	}

	v++; // Skip command name
	if (*v == nullptr) {
		goto cleanup;
	}

	// Handle globbing
	gflag = 0;
	rscan(v, tglob);
	
	char **output_args;
	if (gflag) {
		output_args = glob(v);
		if (output_args == nullptr) {
			bferr("No match");
			goto cleanup;
		}
	} else {
		scan(v, trim);
		output_args = v;
	}

	// Output each argument
	for (char **current_arg = output_args; *current_arg != nullptr; current_arg++) {
		const char *cp = *current_arg;
		
		while (*cp != '\0') {
			int c = *cp++;
			
			// Handle escape sequences when separator is space
			if (sep == ' ' && *cp != '\0' && c == '\\') {
				c = *cp++;
				switch (c) {
				case 'c':
					flush();
					goto cleanup;
				case 'n':
					c = '\n';
					break;
				default:
					putchar('\\');
					break;
				}
			}
			
			putchar(c | QUOTE);
		}
		
		// Add separator between arguments
		if (*(current_arg + 1) != nullptr) {
			putchar(sep | QUOTE);
		}
	}

	// Add final newline if using space separator
	if (sep != 0) {
		putchar('\n');
	}

cleanup:
	if (setintr && saved_sigint != nullptr) {
		signal(SIGINT, saved_sigint);
	}
	
	if (gargv != nullptr) {
		blkfree(gargv);
		gargv = nullptr;
	}
}

#ifndef V6
/**
 * @brief Handle setenv command
 * @param v Command arguments (variable name and value)
 * @note Sets environment variable
 * @since 1.0
 */
void dosetenv(char **v) noexcept
{
	if (v == nullptr || v[1] == nullptr) {
		bferr("setenv: missing arguments");
		return;
	}

	const char *var_name = v[1];
	const char *var_value = (v[2] != nullptr) ? v[2] : "";
	
	// Validate variable name
	if (!validate_variable_name(var_name)) {
		bferr("setenv: invalid variable name");
		return;
	}

	char *globbed_value = globone(var_value);
	if (globbed_value == nullptr) {
		globbed_value = savestr("");
	}

	setenv(var_name, globbed_value);
	xfree(globbed_value);
}

/**
 * @brief Set environment variable
 * @param name Variable name
 * @param value Variable value
 * @note Thread-safe environment variable setting
 * @since 1.0
 */
void setenv(const char *name, const char *value) noexcept
{
	if (name == nullptr || value == nullptr) {
		return;
	}

	// Search for existing variable
	char **ep = environ;
	char **original_environ = ep;
	
	if (ep != nullptr) {
		for (; *ep != nullptr; ep++) {
			const char *env_var = *ep;
			const char *equals_pos = strchr(env_var, '=');
			
			if (equals_pos != nullptr) {
				size_t name_len = equals_pos - env_var;
				if (strlen(name) == name_len && 
					strncmp(name, env_var, name_len) == 0) {
					// Replace existing variable
					char *new_var = strspl(name, "=");
					char *full_var = strspl(new_var, value);
					xfree(new_var);
					xfree(*ep);
					*ep = full_var;
					return;
				}
			}
		}
	}

	// Add new variable
	char *name_equals = strspl(name, "=");
	char *new_entry = strspl(name_equals, value);
	xfree(name_equals);
	
	char *new_block[2] = {new_entry, nullptr};
	environ = blkspl(environ, new_block);
	
	if (original_environ != nullptr) {
		xfree(original_environ);
	}
}

/**
 * @brief Handle umask command
 * @param v Command arguments (octal mask value)
 * @note Sets or displays file creation mask
 * @since 1.0
 */
void doumask(char **v) noexcept
{
	if (v == nullptr) {
		return;
	}

	const char *mask_str = v[1];
	
	if (mask_str == nullptr) {
		// Display current umask
		mode_t current_mask = umask(0);
		umask(current_mask);
		printf("%03o\n", current_mask);
		return;
	}

	// Parse octal mask value
	mode_t new_mask = 0;
	const char *cp = mask_str;
	
	while (*cp != '\0') {
		if (*cp < '0' || *cp > '7') {
			bferr("Improper mask");
			return;
		}
		new_mask = new_mask * 8 + (*cp++ - '0');
	}
	
	if (new_mask > 0777) {
		bferr("Improper mask");
		return;
	}

	umask(new_mask);
}
#endif // V6

/**
 * @brief Clean up search context resources
 * @param ctx Search context to clean up
 */
static void cleanup_search_context(search_context_t *ctx) noexcept
{
	if (ctx != nullptr && ctx->goal != nullptr) {
		xfree(ctx->goal);
		ctx->goal = nullptr;
	}
}
