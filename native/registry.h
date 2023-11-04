/** \file */
#ifndef REGISTRY_H
#define REGISTRY_H

#include "io.h"

//! Gets the `HKEY` pointer for the first part of a registry path.
/*!
 \param reg_path Full registry path.
 \return Pointer to root key handle.
 */
HKEY get_top_key(wchar_t *reg_path);
//! Starts the registry exporting process.
/*!
 \param commit If the changes should be commited with Git. Does not apply if output directory is `-`
   meaning standard output.
 \param deploy_key Relative path to SSH deploy key.
 \param output_dir Output directory.
 \param output_file Output filename.
 \param max_depth Maximum depth to traverse.
 \param hk Starting registry key.
 \param specified_path A direct path to a key (and not a value name).
 \param format Output format.
 \return Pointer to string with branch name. Must be freed on caller side.
 */
bool save_preferences(bool commit,
                      const wchar_t *deploy_key,
                      const wchar_t *output_dir,
                      const wchar_t *output_file,
                      int max_depth,
                      HKEY hk,
                      const wchar_t *specified_path,
                      enum OUTPUT_FORMAT format);
//! Exports a single registry key to a `reg add` command.
/*!
 \param reg_path Registry path to a key or a value name.
 \param top_key Handle to the top key (such as `HKEY_CURRENT_USER`).
 \param format Output format.
 \return `true` if successful, `false` otherwise.
 */
bool export_single_value(const wchar_t *reg_path, HKEY top_key, enum OUTPUT_FORMAT format);
#endif // REGISTRY_H
