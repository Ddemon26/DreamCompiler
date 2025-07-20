#ifndef AST_H
#define AST_H
#include "../lexer/lexer.h"
#include <stddef.h>

/**
 * @brief Represents a memory arena for dynamic memory allocation.
 *
 * This structure is used to manage a resizable memory buffer,
 * including its pointer, current length, and capacity.
 */
typedef struct {
  char *ptr;
  size_t len;
  size_t cap;
} Arena;

/**
 * @brief Initializes the memory arena.
 *
 * @param a Pointer to the memory arena to initialize.
 */
void arena_init(Arena *a);

/**
 * @brief Allocates memory from the memory arena.
 *
 * @param a Pointer to the memory arena.
 * @param size Size of the memory block to allocate.
 * @return Pointer to the allocated memory block.
 */
void *arena_alloc(Arena *a, size_t size);

/**
 * @brief Enumerates the different kinds of nodes in the abstract syntax tree
 * (AST).
 */
typedef enum {
  ND_INT,
  ND_FLOAT,
  ND_CHAR,
  ND_STRING,
  ND_BOOL,
  ND_IDENT,
  ND_UNARY,
  ND_POST_UNARY,
  ND_BINOP,
  ND_COND,
  ND_INDEX,
  ND_FIELD,
  ND_BASE,
  ND_VAR_DECL,
  ND_IF,
  ND_WHILE,
  ND_DO_WHILE,
  ND_FOR,
  ND_BREAK,
  ND_CONTINUE,
  ND_RETURN,
  ND_BLOCK,
  ND_EXPR_STMT,
  ND_SWITCH,
  ND_CONSOLE_CALL,
  ND_CALL,
  ND_FUNC,
  ND_NEW,
  ND_STRUCT_DECL,
  ND_CLASS_DECL,
  ND_TRY,
  ND_THROW,
  ND_AWAIT,
  ND_ERROR
} NodeKind;

/**
 * @brief Forward declaration of the Node structure.
 */
typedef struct Node Node;

/**
 * @brief Represents a slice of a string or array.
 *
 * This structure contains a pointer to the start of the slice
 * and its length.
 */
typedef struct {
  const char *start;
  size_t len;
} Slice;

/**
 * @brief Creates a new node in the abstract syntax tree (AST).
 *
 * @param a Pointer to the memory arena for allocation.
 * @param kind The type of node to create.
 * @return Pointer to the newly created node.
 */
Node *node_new(Arena *a, NodeKind kind);

/**
 * @brief Represents a case in a switch statement.
 *
 * This structure includes information about whether the case
 * is the default case, the value associated with the case,
 * and the body of the case.
 *
 * @param is_default Indicates if this is the default case (1 if true, 0
 * otherwise).
 * @param value Pointer to the value of the case (NULL if default).
 * @param body Pointer to the body of the case.
 */
typedef struct {
  int is_default;
  Node *value; // NULL if default
  Node *body;
} SwitchCase;

/**
 * @brief Represents a node in the abstract syntax tree (AST).
 *
 * This structure contains the type of the node and a union
 * of various possible node-specific data, depending on the node kind.
 */
struct Node {
  NodeKind kind; /**< The type of the node. */
  Pos pos;       /**< Source position of this node. */
  union {
    Slice lit;   /**< Literal value for ND_* literal nodes. */
    Slice ident; /**< Identifier for ND_IDENT nodes. */
    struct {
      TokenKind op; /**< Operator for ND_UNARY and ND_POST_UNARY nodes. */
      Node *expr;   /**< Operand expression. */
    } unary;
    struct {
      TokenKind op; /**< Operator for ND_BINOP nodes. */
      Node *lhs;    /**< Left-hand side expression. */
      Node *rhs;    /**< Right-hand side expression. */
    } bin;
    struct {
      Node *cond;      /**< Condition expression for ND_COND nodes. */
      Node *then_expr; /**< Expression if condition is true. */
      Node *else_expr; /**< Expression if condition is false. */
    } cond;
    struct {
      Node *array; /**< Array expression for ND_INDEX nodes. */
      Node *index; /**< Index expression. */
    } index;
    struct {
      Node *object; /**< Object expression for ND_FIELD nodes. */
      Slice name;   /**< Field name. */
    } field;
    struct {
      Slice name;   /**< Base member name for ND_BASE nodes. */
    } base;
    struct {
      TokenKind type;   /**< Variable type for ND_VAR_DECL nodes. */
      Slice type_name;  /**< Identifier for custom types. */
      Slice name;       /**< Variable name. */
      Node *init;       /**< Initializer expression. */
      size_t array_len; /**< Array length (0 if not an array). */
      int is_static;    /**< 1 if this is a static member field. */
      int is_pointer;   /**< 1 if this is a pointer type. */
    } var_decl;
    struct {
      Node *cond;    /**< Condition expression for ND_IF nodes. */
      Node *then_br; /**< Then branch. */
      Node *else_br; /**< Else branch (may be NULL). */
    } if_stmt;
    struct {
      Node *cond; /**< Condition expression for ND_WHILE nodes. */
      Node *body; /**< Loop body. */
    } while_stmt;
    struct {
      Node *body; /**< Loop body for ND_DO_WHILE nodes. */
      Node *cond; /**< Condition expression. */
    } do_while_stmt;
    struct {
      Node *init;   /**< Initialization expression (may be NULL). */
      Node *cond;   /**< Condition expression (may be NULL). */
      Node *update; /**< Update expression (may be NULL). */
      Node *body;   /**< Loop body. */
    } for_stmt;
    struct {
      Node **items; /**< List of items for ND_BLOCK nodes. */
      size_t len;   /**< Number of items. */
    } block;
    struct {
      Node *expr; /**< Return expression for ND_RETURN nodes (may be NULL). */
    } ret;
    struct {
      Node *expr; /**< Expression for ND_EXPR_STMT nodes. */
    } expr_stmt;
    struct {
      Node *arg;   /**< Argument for Write/WriteLine, NULL for ReadLine. */
      int newline; /**< 1 if WriteLine. */
      int read;    /**< 1 if ReadLine. */
    } console;
    struct {
      Node *expr;        /**< Expression for ND_SWITCH nodes. */
      SwitchCase *cases; /**< Array of cases. */
      size_t len;        /**< Number of cases. */
    } switch_stmt;
    struct {
      Node *callee; /**< Function being called for ND_CALL nodes. */
      Node **args;  /**< Array of arguments. */
      size_t len;   /**< Number of arguments. */
    } call;
    struct {
      Slice type_name; /**< Name of the type for ND_NEW nodes. */
      Node **args;     /**< Arguments to the constructor. */
      size_t arg_len;  /**< Number of constructor arguments. */
    } new_expr;
    struct {
      TokenKind ret_type; /**< Return type for ND_FUNC nodes. */
      Slice name;         /**< Function name. */
      Node **params;      /**< Array of parameters. */
      size_t param_len;   /**< Number of parameters. */
      Node *body;         /**< Function body. */
      int is_static;      /**< 1 if this is a static method. */
      int is_async;       /**< 1 if this is an async function. */
    } func;
    struct {
      Slice name;     /**< Name of the struct or class. */
      Slice base_name; /**< Name of the base class (empty if no inheritance). */
      Node **members; /**< Array of member declarations. */
      size_t len;     /**< Number of members. */
    } type_decl;
    struct {
      Node *body;         /**< Body of the try block. */
      Node *catch_body;   /**< Catch block (may be NULL). */
      Node *finally_body; /**< Finally block (may be NULL). */
      Slice catch_param;  /**< Name of catch parameter (e.g., "e" in catch(Exception e)). */
      Slice catch_type;   /**< Type of catch parameter (e.g., "Exception"). */
    } try_stmt;
    struct {
      Node *expr; /**< Expression to throw (may be NULL). */
    } throw_stmt;
    struct {
      Node *expr; /**< Expression to await for ND_AWAIT nodes. */
    } await_expr;
  } as;
};

#endif
