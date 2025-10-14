/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_cmd_path.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 16:25:19 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/14 12:57:54 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex.h"

static void	ft_free_split(char **arr);
static char	*check_full_path(char *dir, char *cmd);

static char	*default_getenv(void)
{
	return (ft_strdup("PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:\
		/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:"));
}

static char	*get_full_path(char *cmd, char *env_paths)
{
	char	*full_path;
	char	**paths;
	char	**all_paths;

	full_path = check_full_path("", cmd);
	if (full_path)
		return (full_path);
	all_paths = ft_split(env_paths, ':');
	if (!all_paths)
		return (NULL);
	paths = all_paths;
	while (*paths)
	{
		full_path = check_full_path(*paths, cmd);
		if (full_path)
			return (ft_free_split(all_paths), full_path);
		paths++;
	}
	ft_free_split(all_paths);
	return (NULL);
}

char	*get_cmd_path(char *cmd, char **envp)
{
	while (*envp)
	{
		if (ft_strncmp(*envp, "PATH=", 5) == 0)
			return (get_full_path(cmd, (*envp + 5)));
		envp++;
	}
	return (get_full_path(cmd, default_getenv()));
}

static void	ft_free_split(char **paths)
{
	char	**temp;

	if (!paths)
		return ;
	temp = paths;
	while (*temp)
	{
		free(*temp);
		temp++;
	}
	free(paths);
}

static char	*check_full_path(char *dir, char *cmd)
{
	char	*full_path;
	int		dir_len;
	int		cmd_len;

	dir_len = ft_strlen(dir);
	cmd_len = ft_strlen(cmd);
	full_path = malloc(dir_len + cmd_len + 2);
	if (!full_path)
		return (NULL);
	ft_memcpy(full_path, dir, dir_len);
	full_path[dir_len] = '/';
	ft_memcpy(full_path + dir_len + 1, cmd, cmd_len);
	full_path[dir_len + 1 + cmd_len] = '\0';
	if (access(full_path, X_OK) == 0)
		return (full_path);
	free(full_path);
	return (NULL);
}
