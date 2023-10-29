/** \file */
#ifndef GIT_H
#define GIT_H

//! Get the current Git branch in a checked out repository.
/*!
 \param git_dir_arg `--git-dir=...` argument.
 \param git_dir_arg_len `git_dir_arg` length in word size.
 \param work_tree_arg `--work-tree=...` argument.
 \param work_tree_arg_len `work_tree_arg` length in word size.
 \return Pointer to string with branch name. Must be freed on caller side.
 */
wchar_t *get_git_branch(const wchar_t *git_dir_arg,
                        size_t git_dir_arg_len,
                        const wchar_t *work_tree_arg,
                        size_t work_tree_arg_len);
bool git_commit(const wchar_t *output_dir, const wchar_t *deploy_key);

#endif // GIT_H
