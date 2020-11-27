/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   do_p.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/22 03:17:58 by nboulaye          #+#    #+#             */
/*   Updated: 2016/11/19 02:39:09 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static long double	chk_p_modifier(va_list *pa, t_flags *flag)
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
		nb = (unsigned long)va_arg(*pa, unsigned long);
	return (nb);
}

static void			do_p_f_l_moins(int *t, char *buf, t_buf *b)
{
	if (t[2] && (t[0]-- || 1))
	{
		add_in_buf(b, '0');
		add_in_buf(b, 'x');
	}
	if (b->flag->f_precision > 0)
		while (b->flag->f_precision-- > t[3] && (t[0]-- > t[3] - 1))
			add_in_buf(b, '0');
	t[3] = -1;
	while (buf[++t[3]] && (t[0]-- || 1))
		add_in_buf(b, buf[t[3]]);
	while (t[0]-- > t[2])
		add_in_buf(b, ' ');
}

static void			do_p_wthout_f_l_moins(int *t, char *buf, t_buf *b)
{
	if (!(b->flag->f_length & F_L_ZERO))
		while (t[0] - 2 > b->flag->f_precision &&
			t[0] - 2 > t[3])
		{
			add_in_buf(b, ' ');
			t[0]--;
		}
	if (t[2] && (t[0]-- || 1))
	{
		add_in_buf(b, '0');
		add_in_buf(b, 'x');
	}
	if (b->flag->f_precision > 0)
		while (b->flag->f_precision-- > t[3] && t[0]-- > t[3] - 1)
			add_in_buf(b, '0');
	else if (b->flag->f_length & F_L_ZERO && t[0] > t[3])
		while (t[0]-- > t[3] + (b->flag->f_length & F_L_DIEZE))
			add_in_buf(b, '0');
	t[3] = -1;
	while (buf[++t[3]])
		add_in_buf(b, buf[t[3]]);
}

/*
** t[0];//total_lenght
** t[1];//isnegativ == 1
** t[2];//addone char ('+' || '-' || '0')
** t[3];//nb_digit
*/

void				do_p(t_buf *b, va_list *pa)
{
	int			t[4];
	char		buf[64];
	long double	nb;

	t[1] = 0;
	t[2] = 0;
	if (!(b->flag->f_length & F_L_DIEZE))
		b->flag->f_length += F_L_DIEZE;
	t[0] = (b->flag->f_precision > b->flag->f_lenmin) ? b->flag->f_precision :
		b->flag->f_lenmin;
	nb = chk_p_modifier(pa, b->flag);
	if (nb == 0 && b->flag->f_precision == 0)
	{
		while (b->flag->f_lenmin-- > 2)
			add_in_buf(b, ' ');
		add_in_buf(b, '0');
		add_in_buf(b, 'x');
		return ;
	}
	t[3] = ft_itoa_base_buffer(nb, 16, buf);
	t[2] = 1;
	if (b->flag->f_length & F_L_MOINS)
		do_p_f_l_moins(t, buf, b);
	else
		do_p_wthout_f_l_moins(t, buf, b);
}
