/**
 * @file readable_codegen.h
 * @brief Header file for maintaining readable C code generation.
 *
 * This file declares utilities and passes that ensure the generated C code
 * remains readable and debuggable even after aggressive optimizations.
 */

#ifndef READABLE_CODEGEN_H
#define READABLE_CODEGEN_H

#include "../cfg/cfg.h"
#include "../ir/ir.h"
#include <stdbool.h>

/**
 * @brief Configuration for readable code generation.
 */
typedef struct {
    bool preserve_variable_names;   /**< Keep original variable names when possible. */
    bool add_optimization_comments; /**< Add comments explaining optimizations. */
    bool maintain_structure;        /**< Preserve original control flow structure. */
    bool generate_debug_info;       /**< Include debug information. */
    int indentation_level;          /**< Base indentation level. */
    bool use_goto_sparingly;        /**< Avoid goto unless necessary. */
} ReadableCodegenConfig;

/**
 * @brief Variable naming information for readable output.
 */
typedef struct {
    IRValue original_id;            /**< Original variable ID. */
    char *suggested_name;           /**< Human-readable name suggestion. */
    char *type_hint;               /**< Type information for comments. */
    bool is_temporary;             /**< True if this is a compiler temporary. */
    bool preserve_name;            /**< True if name should be preserved. */
} VariableNaming;

/**
 * @brief Structure annotation for optimized code.
 */
typedef struct {
    BasicBlock *block;             /**< Associated basic block. */
    char *optimization_note;       /**< Description of optimization applied. */
    char *original_source_hint;    /**< Hint about original source structure. */
    bool is_optimization_artifact; /**< True if block was created by optimization. */
} StructureAnnotation;

/**
 * @brief Analyzes the optimized CFG to preserve readability information.
 * @param cfg Pointer to the control flow graph.
 * @param config Pointer to the readable codegen configuration.
 * @return Array of structure annotations.
 */
StructureAnnotation *analyze_for_readability(CFG *cfg, ReadableCodegenConfig *config);

/**
 * @brief Generates meaningful variable names for the optimized IR.
 * @param cfg Pointer to the control flow graph.
 * @param config Pointer to the readable codegen configuration.
 * @return Array of variable naming information.
 */
VariableNaming *generate_variable_names(CFG *cfg, ReadableCodegenConfig *config);

/**
 * @brief Reconstructs high-level control flow from optimized IR.
 * @param cfg Pointer to the control flow graph.
 * @param annotations Array of structure annotations.
 * @return true if high-level structure was recovered, false otherwise.
 */
bool reconstruct_control_flow(CFG *cfg, StructureAnnotation *annotations);

/**
 * @brief Adds optimization comments to explain transformed code.
 * @param cfg Pointer to the control flow graph.
 * @param annotations Array of structure annotations.
 * @return true if comments were added, false otherwise.
 */
bool add_optimization_comments(CFG *cfg, StructureAnnotation *annotations);

/**
 * @brief Minimizes the use of goto statements in generated C code.
 * @param cfg Pointer to the control flow graph.
 * @return true if goto usage was reduced, false otherwise.
 */
bool minimize_goto_usage(CFG *cfg);

/**
 * @brief Ensures the generated C code compiles and behaves correctly.
 * @param cfg Pointer to the control flow graph.
 * @param config Pointer to the readable codegen configuration.
 * @return true if code quality checks passed, false otherwise.
 */
bool validate_generated_code_quality(CFG *cfg, ReadableCodegenConfig *config);

/**
 * @brief Formats the CFG for human-readable C code generation.
 * @param cfg Pointer to the control flow graph.
 * @param config Pointer to the readable codegen configuration.
 * @return true if formatting was successful, false otherwise.
 */
bool format_for_readable_output(CFG *cfg, ReadableCodegenConfig *config);

/**
 * @brief Inserts debug metadata that survives optimization.
 * @param cfg Pointer to the control flow graph.
 * @param source_mapping Mapping from IR to original source lines.
 * @return true if debug info was inserted, false otherwise.
 */
bool preserve_debug_information(CFG *cfg, void *source_mapping);

/**
 * @brief Creates a default readable codegen configuration.
 * @param debug_mode True if compiling in debug mode.
 * @return Default configuration for readable code generation.
 */
ReadableCodegenConfig create_readable_config(bool debug_mode);

/**
 * @brief Estimates the readability impact of optimizations.
 * @param original_cfg Pointer to the original CFG.
 * @param optimized_cfg Pointer to the optimized CFG.
 * @return Readability score (0.0 = unreadable, 1.0 = perfectly readable).
 */
double estimate_readability_score(CFG *original_cfg, CFG *optimized_cfg);

/**
 * @brief Applies final cleanup for readable C code generation.
 * @param cfg Pointer to the control flow graph.
 * @param config Pointer to the readable codegen configuration.
 * @return true if cleanup was successful, false otherwise.
 */
bool apply_readable_cleanup(CFG *cfg, ReadableCodegenConfig *config);

/**
 * @brief Frees structure annotations.
 * @param annotations Pointer to annotations array.
 * @param count Number of annotations.
 */
void structure_annotations_free(StructureAnnotation *annotations, size_t count);

/**
 * @brief Frees variable naming information.
 * @param naming Pointer to naming array.
 * @param count Number of entries.
 */
void variable_naming_free(VariableNaming *naming, size_t count);

#endif