/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   do_b.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/22 03:17:58 by nboulaye          #+#    #+#             */
/*   Updated: 2016/11/19 02:38:44 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static long double	chk_b_modifier(va_list *pa, t_flags *flag)
{
	long double nb;

	if (flag->f_char & F_C_L)
		nb = (unsigned long)va_arg(*pa, unsigned long);
	else if (flag->f_char & F_C_LL)
		nb = (unsigned long long)va_arg(*pa, unsigned long long);
	else if (flag->f_char & F_C_H)
		nb = (unsigned short)va_arg(*pa, int);
	else if (flag->f_char & F_C_HH)
		nb = (unsigned char)va_arg(*pa, int);
	else if (flag->f_char & F_C_J)
		nb = (uintmax_t)va_arg(*pa, uintmax_t);
	else if (flag->f_char & F_C_Z)
		nb = (size_t)va_arg(*pa, size_t);
	else
		nb = (unsigned int)va_arg(*pa, unsigned int);
	return (nb);
}

static void			do_b_f_l_moins(int *t, char *buf, t_buf *b, char type)
{
	if (t[2] && (t[0]-- || 1))
	{
		add_in_buf(b, '0');
		add_in_buf(b, type);
	}
	if (b->flag->f_precision > 0)
		while (b->flag->f_precision-- > t[3] && (t[0]-- > 0))
			add_in_buf(b, '0');
	else if (b->flag->f_length & F_L_ZERO && t[0] > t[3])
		while (t[0]-- > t[3])
			add_in_buf(b, '0');
	t[3] = -1;
	while (buf[++t[3]] && (t[0]-- || 1))
		add_in_buf(b, buf[t[3]]);
	while (t[0]-- > t[2])
		add_in_buf(b, ' ');
}

static void			do_b_wthout_f_l_moins(int *t, char *buf, t_buf *b, char c)
{
	if (!(b->flag->f_length & F_L_ZERO))
		while (t[0] > b->flag->f_precision &&
			t[0] > t[3] + 2 * t[2])
		{
			add_in_buf(b, ' ');
			t[0]--;
		}
	if (t[2] && (t[0]-- || 1))
	{
		add_in_buf(b, '0');
		add_in_buf(b, c);
	}
	if (b->flag->f_precision > 0)
		while (b->flag->f_precision-- > t[3] && t[0]-- > 0)
			add_in_buf(b, '0');
	else if (b->flag->f_length & F_L_ZERO && t[0] > t[3])
		while (t[0]-- > t[3] + (b->flag->f_length & F_L_DIEZE))
			add_in_buf(b, '0');
	t[3] = -1;
	while (buf[++t[3]])
		add_in_buf(b, buf[t[3]]);
}

void				do_b(t_buf *b, va_list *pa, char type)
{
	int			t[4];
	char		buf[256];
	long double	nb;

	t[1] = 0;
	t[2] = 0;
	if ((b->flag->f_length & F_L_MOINS && b->flag->f_length & F_L_ZERO))
		b->flag->f_length -= F_L_ZERO;
	if (b->flag->f_precision >= 0 && b->flag->f_length & F_L_ZERO)
		b->flag->f_length -= F_L_ZERO;
	t[0] = (b->flag->f_precision > b->flag->f_lenmin) ? b->flag->f_precision :
		b->flag->f_lenmin;
	nb = chk_b_modifier(pa, b->flag);
	if (nb == 0 && b->flag->f_precision == 0)
	{
		while (b->flag->f_lenmin--)
			add_in_buf(b, ' ');
		return ;
	}
	t[3] = ft_itoa_base_buffer(nb, 2, buf);
	(nb > 0 && b->flag->f_length & F_L_DIEZE) ? (t[2] = 1) : 1;
	if (b->flag->f_length & F_L_MOINS)
		do_b_f_l_moins(t, buf, b, type);
	else
		do_b_wthout_f_l_moins(t, buf, b, type);
}
