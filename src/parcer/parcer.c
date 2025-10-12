/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parcer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 13:07:34 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/12 20:14:53 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "parcer.h"
#include "utils.h"
#include <stdlib.h>

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = value;
	token->next = NULL;
	return (token);
}

char	*get_word(char **str)
{
	char	quote;
	char	*start;

	quote = 0;
	start = *str;
	while (**str)
	{
		if (!quote && in_set(**str, QUOTES))
			quote = **str;
		else if (quote && quote == **str)
			quote = 0;
		else if (!quote
			&& (in_set(**str, SPECIAL_CHARS) || in_set(**str, SPACES)))
			break ;
		(*str)++;
	}
	if (in_set(*start, QUOTES))
		return (ft_substr(start, 1, *str - start - 2));
	return (ft_substr(start, 0, *str - start));
}

void skip_spaces(char **str)
{
	while (**str && in_set(**str, SPACES))
		(*str)++;
}

t_token	*tokenize(char *input)
{
	t_token	*head;
	t_token	*curr_token;
	t_token	*token;
	char	*word;

	head = NULL;
	curr_token = NULL;
	while (*input)
	{
		skip_spaces(&input);
		if (*input == '\0')
			break ;
		if (in_set(*input, SPECIAL_CHARS))
		{
			input++;
			continue ;
		}
		else
		{
			word = get_word(&input);
			token = create_token(TOKEN_WORD, word);
		}
		if (!head)
			head = token;
		else
			curr_token->next = token;
		curr_token = token;
	}
	return (head);
}

char	**parse_command(char *command)
{
	t_token		*token;
	t_token 	*temp;
	int			token_count;
	char		**res;

	token = tokenize(command);
	token_count = 0;
	temp = token;
	while (temp)
	{
		token_count++;
		temp = temp->next;
	}
	res = malloc((token_count + 1) * sizeof(char *));
	res[token_count] = NULL;
	int i = 0;
	while (token)
	{
		res[i++] = token->value;
		temp = token->next;
		free(token);
		token = temp;
	}
	return (res);
}

// char	**parse_command(char *command)
// {
// 	char **cmd_argv;
// 	char *str;
// 	char *comm;

// 	str = ft_strtrim(command, " ");
// 	if (!str)
// 		return (NULL);
// 	if (ft_strlen(str) == 0)
// 		return (free(str), NULL);
// 	comm = ft_strchr(str, ' ');
// 	if (comm)
// 	{
// 		*comm = '\0';
// 		comm = ft_strtrim((const char *)(comm + 1), " ");
// 		if (!comm)
// 			return (free(str), NULL);
// 		cmd_argv = malloc(3 * sizeof(char *));
// 		if (!cmd_argv)
// 			return (free(str), free(comm), NULL);
// 		cmd_argv[2] = NULL;
// 		cmd_argv[1] = comm;
// 	}
// 	else
// 	{
// 		cmd_argv = malloc(2 * sizeof(char *));
// 		if (!cmd_argv)
// 			return (free(str), NULL);
// 		cmd_argv[1] = NULL;
// 	}
// 	cmd_argv[0] = str;
// 	return (cmd_argv);
// }

