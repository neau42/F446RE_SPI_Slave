/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   do_d.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/22 03:17:58 by nboulaye          #+#    #+#             */
/*   Updated: 2016/11/19 02:38:53 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static long double	chk_d_modifier(va_list *pa, t_flags *flag, char type)
{
	long double nb;

	if (type == 'D' || flag->f_char & F_C_L)
		nb = (long)va_arg(*pa, long);
	else if (flag->f_char & F_C_LL)
		nb = (long long)va_arg(*pa, long long);
	else if (flag->f_char & F_C_Z)
		nb = (long)va_arg(*pa, size_t);
	else if (flag->f_char & F_C_J)
		nb = (intmax_t)va_arg(*pa, intmax_t);
	else if (flag->f_char & F_C_HH)
		nb = (char)va_arg(*pa, int);
	else if (flag->f_char & F_C_H)
		nb = (short)va_arg(*pa, int);
	else
		nb = (int)va_arg(*pa, int);
	return (nb);
}

static void			do_d_f_l_moins(int *t, char *buf, t_buf *b)
{
	if (t[1] && t[0]--)
		add_in_buf(b, '-');
	else if (t[2] && b->flag->f_length & F_L_PLUS && t[0]--)
		add_in_buf(b, '+');
	else if (t[2] && t[0]--)
		add_in_buf(b, ' ');
	if (b->flag->f_precision > 0)
		while (b->flag->f_precision-- > t[3] && t[0]--)
			add_in_buf(b, '0');
	else if (b->flag->f_length & F_L_ZERO && t[0] > t[3])
		while (t[0]-- > t[3])
			add_in_buf(b, '0');
	t[3] = -1;
	while (buf[++t[3]] && (t[0]-- || 1))
		add_in_buf(b, buf[t[3]]);
	while (t[0]-- > 0)
		add_in_buf(b, ' ');
}

static void			do_d_wthout_f_l_moins(int *t, char *buf, t_buf *b)
{
	if (!(b->flag->f_length & F_L_ZERO))
		while (t[0] > b->flag->f_precision + t[2] && t[0] > (t[3] + t[2]))
		{
			add_in_buf(b, ' ');
			t[0]--;
		}
	if (t[1] && t[0]--)
		add_in_buf(b, '-');
	else if (t[2] && b->flag->f_length & F_L_PLUS && t[0]--)
		add_in_buf(b, '+');
	else if (t[2] && t[0]--)
		add_in_buf(b, ' ');
	if (b->flag->f_precision > 0)
		while (b->flag->f_precision-- > t[3] && t[0]--)
			add_in_buf(b, '0');
	else if (b->flag->f_length & F_L_ZERO && t[0] > t[3])
		while (t[0]-- > t[3])
			add_in_buf(b, '0');
	t[3] = -1;
	while (buf[++t[3]])
		add_in_buf(b, buf[t[3]]);
}

static void			do_d_next(int *t, t_buf *b, char *buf, long double nb)
{
	if ((t[1] || b->flag->f_length & F_L_SPACE || b->flag->f_length & F_L_PLUS))
		t[2]++;
	if ((t[3] = ft_itoa_base_buffer(nb, 10, buf)) > t[0])
		t[0] = t[3] + t[2];
	if (b->flag->f_length & F_L_MOINS)
		do_d_f_l_moins(t, buf, b);
	else
		do_d_wthout_f_l_moins(t, buf, b);
}

/*
** t[0];//total_lenght
** t[1];//isnegativ == 1
** t[2];//addone char ('+' || '-' || '0')
** t[3];//nb_digit
*/

void				do_d(t_buf *b, va_list *pa, char type)
{
	int			t[4];
	char		buf[32];
	long double	nb;

	t[1] = 0;
	t[2] = 0;
	if ((b->flag->f_length & F_L_MOINS && b->flag->f_length & F_L_ZERO))
		b->flag->f_length -= F_L_ZERO;
	if (b->flag->f_precision >= 0 && b->flag->f_length & F_L_ZERO)
		b->flag->f_length -= F_L_ZERO;
	t[0] = (b->flag->f_precision > b->flag->f_lenmin) ? b->flag->f_precision :
		b->flag->f_lenmin;
	if ((nb = chk_d_modifier(pa, b->flag, type)) < 0 && ++t[1])
		nb = -nb;
	if (nb == 0 && b->flag->f_precision == 0)
	{
		while (b->flag->f_lenmin--)
			add_in_buf(b, ' ');
		return ;
	}
	else
		do_d_next(t, b, buf, nb);
}
