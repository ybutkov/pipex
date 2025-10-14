/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_internal.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:32:22 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/14 13:24:12 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_INTERNAL_H
# define SHELL_INTERNAL_H

# include "shell.h"

int		execute_shell_node(t_ast_node *node, t_shell *shell, int in_fd,
			int out_fd);
int		execute_redir_heredoc(t_ast_node *ast_node, t_shell *shell, int in_fd,
			int old_fd_out);
void	build_shell(t_shell *shell, char **commands, int argc);
int		execute_cmd(t_cmd *cmd, t_shell *shell, int input_fd, int output_fd);
void	close_fds(int fds[2]);

#endif
