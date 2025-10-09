/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:16:54 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/08 15:15:17 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_H
# define SHELL_H

# include "ast.h"
# include "shell_node.h"

typedef struct s_shell
{
	t_ast	*ast;
	t_ctx	*ctx;

	void	*(*build)(struct s_shell *shell, char **commands, int argc);
	void	*(*free)(struct s_shell *shell);
	void	(*execute)(struct s_shell *shell);
}			t_shell;

t_shell		*create_shell(char	**envp);

#endif
