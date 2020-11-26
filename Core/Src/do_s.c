/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   do_s.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/22 03:17:58 by nboulaye          #+#    #+#             */
/*   Updated: 2016/08/30 17:48:55 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

void			do_s(t_buf *b, va_list *pa, char type)
{
	char		*s;
	int			s_len;
	static char	buf[7] = "(null)";

	if (type == 'S' || b->flag->f_char & (F_C_L))
		return (do_long_s(b, pa, buf));
	s = va_arg(*pa, char *);
	if (s == NULL)
		s = buf;
	s_len = ft_strlen(s);
	if (b->flag->f_precision >= 0 && b->flag->f_precision < s_len)
		s_len = b->flag->f_precision;
	if (!(b->flag->f_length & F_L_MOINS))
		while (b->flag->f_lenmin-- > s_len)
			(b->flag->f_length & F_L_ZERO) ? add_in_buf(b, '0') :
			add_in_buf(b, ' ');
	while (*s && b->flag->f_precision--)
	{
		add_in_buf(b, *s);
		s++;
		b->flag->f_lenmin--;
	}
	if (b->flag->f_lenmin > 0)
		while (b->flag->f_lenmin--)
			add_in_buf(b, ' ');
}
