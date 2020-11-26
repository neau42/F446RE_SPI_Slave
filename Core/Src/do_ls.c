/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   do_ls.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/22 03:17:58 by nboulaye          #+#    #+#             */
/*   Updated: 2016/08/30 20:16:34 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int		ft_putwchar_2(int c, t_buf *b, int precision)
{
	if (c >= 0x10000)
	{
		if (precision && (b->flag->f_precision - 4) < 0)
			return (4);
		add_in_buf(b, ((c >> 18) & 0xF) + 0xF0);
		add_in_buf(b, ((c >> 12) & 0x3F) + 0x80);
		add_in_buf(b, ((c >> 6) & 0x3F) + 0x80);
		add_in_buf(b, (c & 0X3F) + 0x80);
		return (4);
	}
	else
	{
		if (precision && (b->flag->f_precision - 3) < 0)
			return (3);
		add_in_buf(b, ((c >> 12) & 0xF) + 0xE0);
		add_in_buf(b, ((c >> 6) & 0x3F) + 0x80);
		add_in_buf(b, (c & 0X3F) + 0x80);
		return (3);
	}
}

static int		ft_putwchar(int c, t_buf *b, int precision)
{
	if (c >= 0x800)
		return (ft_putwchar_2(c, b, precision));
	else if (c >= 0x80)
	{
		if (precision && (b->flag->f_precision - 2) < 0)
			return (2);
		add_in_buf(b, ((c >> 6) & 0x1F) + 0xc0);
		add_in_buf(b, (c & 0X3F) + 0x80);
		return (2);
	}
	if (precision && (b->flag->f_precision - 1) < 0)
		return (1);
	add_in_buf(b, (c));
	return (1);
}

static size_t	ft_longstrlen(int *str, int max, int precision)
{
	size_t	len;
	int		tmp;

	tmp = 0;
	len = 0;
	if (!str)
		return (0);
	while (*str)
	{
		if (*str >= 0x10000)
			tmp = 4;
		else if (*str >= 0x800)
			tmp = 3;
		else if (*str >= 0x80)
			tmp = 2;
		else
			len++;
		str++;
		max -= tmp;
		if (max < 0 && precision)
			break ;
		len += tmp;
	}
	return (len);
}

static void		add_l_str_in_buf(t_buf *b, char *buf, int *s, int precision)
{
	int		len_char;

	len_char = 0;
	if (s == NULL)
		while (*buf && b->flag->f_precision)
		{
			add_in_buf(b, *buf);
			buf++;
			b->flag->f_lenmin--;
			b->flag->f_precision--;
		}
	else
		while (*s && ((b->flag->f_precision) > 0 || !precision))
		{
			len_char = ft_putwchar(*s, b, precision);
			b->flag->f_lenmin -= len_char;
			b->flag->f_precision -= len_char;
			s += 1;
		}
}

void			do_long_s(t_buf *b, va_list *pa, char *buf)
{
	int		*s;
	int		precision;
	int		len;

	s = va_arg(*pa, int *);
	precision = (b->flag->f_precision >= 0) ? 1 : 0;
	len = ft_longstrlen(s, b->flag->f_precision, precision);
	if (precision && b->flag->f_precision < len)
		len = b->flag->f_precision;
	if (b->flag->f_lenmin > 0 && !(b->flag->f_length & F_L_MOINS))
		while (b->flag->f_lenmin-- > len)
			(b->flag->f_length & F_L_ZERO) ? add_in_buf(b, '0') :
			add_in_buf(b, ' ');
	add_l_str_in_buf(b, buf, s, precision);
	if (b->flag->f_lenmin > 0)
		while (b->flag->f_lenmin-- > 0)
			(b->flag->f_length & F_L_ZERO) ? add_in_buf(b, '0') :
			add_in_buf(b, ' ');
}
