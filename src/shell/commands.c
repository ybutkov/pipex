/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 14:51:24 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/10 16:12:35 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"


static void	free_cmd(t_cmd *cmd);

static void	free_redir_bridge(void *content)
{
	t_redir *redir;

	if (!content)
		return ;
	redir = (t_redir *)content;
	redir->free_redir(redir);
}

t_cmd	*create_cmd(char **argv, char *path)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = argv;
	cmd->path = path;
	cmd->redirs = NULL;
	cmd->free_cmd = free_cmd;
	return (cmd);
}

static void	free_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	free_str_array(cmd->argv);
	if (cmd->path)
		free(cmd->path);
	// if (0)
	// 	free_redir_bridge(NULL);
	if (cmd->redirs)
		ft_lstclear(&cmd->redirs, free_redir_bridge);
	free(cmd);
}

