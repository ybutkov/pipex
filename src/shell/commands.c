/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 14:51:24 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/09 14:54:21 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"


void	free_cmd(t_cmd *cmd);

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
	int	i;

	if (!cmd)
		return ;
	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
		{
			free(cmd->argv[i]);
			i++;
		}
		free(cmd->argv);
	}
	if (cmd->path)
		free(cmd->path);
	if (cmd->redirs)
	{
		ft_lstclear(&cmd->redirs, free);
	}
	free(cmd);
}
