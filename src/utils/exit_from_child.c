/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_from_child.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 19:24:14 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/10 19:54:18 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	exit_from_child(int status_code)
{
	char *cmd[3];
	char *code;

	code = ft_itoa(status_code);
	if (!code)
		return (1);
	cmd[0] = "exit_from_child";
	cmd[1] = code;
	cmd[2]  = NULL;
	execve(cmd[0], cmd, NULL);
	return (0);
}

