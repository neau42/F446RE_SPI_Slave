/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   do_not_specifier.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/22 03:17:58 by nboulaye          #+#    #+#             */
/*   Updated: 2016/08/30 17:54:04 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

void		do_not_specifier(t_buf *b, char type)
{
	if (!type)
		return ;
	if (b->flag->f_lenmin > 0 && !(b->flag->f_length & F_L_MOINS))
		while (b->flag->f_lenmin-- > 1)
			(b->flag->f_length & F_L_ZERO) ? add_in_buf(b, '0') :
			add_in_buf(b, ' ');
	add_in_buf(b, type);
	if ((b->flag->f_length & F_L_MOINS))
		while (b->flag->f_lenmin-- > 1)
			add_in_buf(b, ' ');
}
