/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 13:11:49 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/04 20:36:57 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# define WHICH_CMD "/usr/bin/which"

typedef enum e_node_type
{
	CMD,
	PIPE
}					e_node_type;

typedef enum e_redir_type
{
	INPUT, OUTPUT
}					e_redir_type;

typedef struct s_redir
{
	e_redir_type	type;
	char			*file;
}					t_redir;

typedef struct s_cmd
{
	char			**argv;
	char			*path;
	t_redir			*redir_in;
	t_redir			*redir_out;
}					t_cmd;

typedef struct s_ast
{
	e_node_type		type;
	struct s_ast	*left;
	struct s_ast	*right;
	// If type == CMD
	t_cmd			*cmd;
}					t_ast;

char				*get_cmd_path(char *cmd, char **envp);

#endif
