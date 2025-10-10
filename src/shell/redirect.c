/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 13:20:27 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/10 16:12:27 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	free_redir_content(t_redir *redir)
{
	if (!redir)
		return ;
	if (redir->target)
		free(redir->target);
	free(redir);
}

t_redir *create_redir(t_redir_type type, char *target)
{
	t_redir *redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->target = target;
	if (!redir->target)
	{
		free(redir);
		return (NULL);
	}
	redir->free_redir = free_redir_content;
	return (redir);
}
