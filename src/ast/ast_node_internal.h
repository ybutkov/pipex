/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_node_internal.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybutkov <ybutkov@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 18:04:54 by ybutkov           #+#    #+#             */
/*   Updated: 2025/10/06 18:07:16 by ybutkov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AST_NODE_INTERNAL_H
# define AST_NODE_INTERNAL_H

# include "ast.h"

void	*get_content(t_ast_node *node);
void	*set_content(t_ast_node *node, void *content);

#endif
