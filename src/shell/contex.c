/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   contex.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 15:52:10 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/12 20:58:06 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	free_ctx_content(t_ctx *ctx)
{
	if (!ctx)
		return ;
	if (ctx->envp)
		free_str_array(ctx->envp);
	free(ctx);
}

static char	**apply_func_to_array(char **arr, char *(*func)(const char *))
{
	int		i;
	char	**new_arr;

	if (!arr || !func)
		return (NULL);
	i = 0;
	while (arr[i])
		i++;
	new_arr = malloc((i + 1) * sizeof(char *));
	if (!new_arr)
		return (NULL);
	i = 0;
	while (arr[i])
	{
		new_arr[i] = func(arr[i]);
		i++;
	}
	new_arr[i] = NULL;
	return (new_arr);
}

t_ctx	*create_ctx(char **envp)
{
	t_ctx	*ctx;

	ctx = malloc(sizeof(t_ctx));
	if (!ctx)
		return (NULL);

	ctx->envp = apply_func_to_array(envp, ft_strdup);
	ctx->last_exit_status = 0;
	ctx->free_ctx = free_ctx_content;
	return (ctx);
}
