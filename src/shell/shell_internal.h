/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_internal.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:32:22 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/13 18:01:03 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_INTERNAL_H
# define SHELL_INTERNAL_H

# include "shell.h"

int		execute_shell_node(t_ast_node *node, t_shell *shell, int in_fd,
			int out_fd);
void	build_shell(t_shell *shell, char **commands, int argc);

#endif
