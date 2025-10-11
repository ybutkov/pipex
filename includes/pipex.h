/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 13:11:49 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/11 13:53:33 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# define WHICH_CMD "/usr/bin/which"
# define HERE_DOC "here_doc"
# define PIPEX "pipex: "
# define COLON ": "
# define EXIT_CMD_NOT_FOUND 127

char	*get_cmd_path(char *cmd, char **envp);
void	free_str_array(char **arr);
int		exit_from_child(int status_code);

#endif
