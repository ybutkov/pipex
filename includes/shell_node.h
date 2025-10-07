/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_node.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 20:03:14 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/07 13:48:32 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NODE_H
# define NODE_H

# include "libft.h"

typedef struct s_ctx
{
	char	**envp;
}				t_ctx;

typedef enum e_node_type
{
	NODE_CMD,
	NODE_PIPE,
	NODE_AND,
	NODE_OR,
	NODE_REDIR_IN,
	NODE_REDIR_OUT,
	NODE_REDIR_APPEND,
	NODE_REDIR_HEREDOC,
	NODE_SUBSHELL
}					t_node_type;

typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}					t_redir_type;

typedef struct s_redir
{
	t_redir_type	type;
	char			*target;
}					t_redir;

typedef struct s_cmd
{
	char			**argv;
	char			*path;
	t_list			*redirs;
}					t_cmd;

typedef struct s_shell_node
{
	t_node_type		type;
	union u_data
	{
		t_cmd		*cmd;
		t_redir		*redir;
	} data;
}					t_shell_node;

#endif
