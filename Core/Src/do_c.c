/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   do_c.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/22 03:17:58 by nboulaye          #+#    #+#             */
/*   Updated: 2016/08/31 15:42:42 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void		ft_add_space_n_zero(t_buf *b, int char_len, char a)
{
	if (b->flag->f_length & F_L_ZERO && a)
		while (b->flag->f_lenmin > char_len++)
			add_in_buf(b, '0');
	else
		while (b->flag->f_lenmin > char_len++)
			add_in_buf(b, ' ');
}

static int		ft_putwchar_2(int c, t_buf *b)
{
	if (c >= 0x10000)
	{
		if (!(b->flag->f_length & F_L_MOINS))
			ft_add_space_n_zero(b, 4, 1);
		add_in_buf(b, ((c >> 18) & 0xF) + 0xF0);
		add_in_buf(b, ((c >> 12) & 0x3F) + 0x80);
		add_in_buf(b, ((c >> 6) & 0x3F) + 0x80);
		add_in_buf(b, (c & 0X3F) + 0x80);
		if ((b->flag->f_length & F_L_MOINS))
			ft_add_space_n_zero(b, 4, 0);
		return (4);
	}
	else
	{
		if (!(b->flag->f_length & F_L_MOINS))
			ft_add_space_n_zero(b, 3, 1);
		add_in_buf(b, ((c >> 12) & 0xF) + 0xE0);
		add_in_buf(b, ((c >> 6) & 0x3F) + 0x80);
		add_in_buf(b, (c & 0X3F) + 0x80);
		if ((b->flag->f_length & F_L_MOINS))
			ft_add_space_n_zero(b, 3, 0);
		return (3);
	}
}

static int		ft_putwchar(int c, t_buf *b)
{
	if ((c >= 0xD800 && c <= 0xDFFF) || c > 0x10FFFF)
	{
		add_in_buf(b, -3);
		return (0);
	}
	else if (c >= 0x800)
		return (ft_putwchar_2(c, b));
	else if (c >= 0x80)
	{
		if (!(b->flag->f_length & F_L_MOINS))
			ft_add_space_n_zero(b, 2, 1);
		add_in_buf(b, ((c >> 6) & 0x1F) + 0xc0);
		add_in_buf(b, (c & 0X3F) + 0x80);
		if ((b->flag->f_length & F_L_MOINS))
			ft_add_space_n_zero(b, 2, 0);
		return (2);
	}
	if (!(b->flag->f_length & F_L_MOINS))
		ft_add_space_n_zero(b, 1, 1);
	add_in_buf(b, (c));
	if ((b->flag->f_length & F_L_MOINS))
		ft_add_space_n_zero(b, 1, 0);
	return (1);
}

void			do_c(t_buf *b, va_list *pa, char type)
{
	int		c;

	if (type == 'c' && !(b->flag->f_char & F_C_L)
		&& !(b->flag->f_char & F_C_LL))
	{
		if (!(b->flag->f_length & F_L_MOINS))
			ft_add_space_n_zero(b, 1, 1);
		c = (unsigned char)va_arg(*pa, int);
		add_in_buf(b, c);
		if ((b->flag->f_length & F_L_MOINS))
			ft_add_space_n_zero(b, 1, 0);
		return ;
	}
	c = (wchar_t)va_arg(*pa, wint_t);
	ft_putwchar(c, b);
}
