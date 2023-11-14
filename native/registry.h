/** \file */
#ifndef REGISTRY_H
#define REGISTRY_H

#include "io.h"

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
//! Print a single registry value name to standard output.
/*!
 \param top_key Handle to the top key (such as `HKEY_CURRENT_USER`).
 \param reg_path Registry path to a key or a value name.
 \param format Output format.
 \return `true` if successful, `false` otherwise.
 */
bool export_single_value(HKEY top_key, const wchar_t *reg_path, enum OUTPUT_FORMAT format);
#endif // REGISTRY_H
