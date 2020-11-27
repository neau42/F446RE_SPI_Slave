/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flags.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/04/27 16:25:34 by nboulaye          #+#    #+#             */
/*   Updated: 2016/11/19 02:40:52 by nboulaye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int		chk_flag_char(char **format, t_flags *flag)
{
	int chk;

	chk = 0;
	if (**format == 'h' && ++chk && (*format)++)
	{
		if (**format == 'h' && (*format)++ && !(flag->f_char & F_C_HH))
			flag->f_char += F_C_HH;
		else if (!(flag->f_char & F_C_H))
			flag->f_char += F_C_H;
	}
	else if (**format == 'l' && ++chk && (*format)++)
	{
		if (**format == 'l' && (*format)++ && !(flag->f_char & F_C_LL))
			flag->f_char += F_C_LL;
		else if (!(flag->f_char & F_C_L))
			flag->f_char += F_C_L;
	}
	else if (**format == 'j' && ++chk && (*format)++ && !(flag->f_char & F_C_J))
		flag->f_char += F_C_J;
	else if (**format == 'z' && ++chk && (*format)++ && !(flag->f_char & F_C_Z))
		flag->f_char += F_C_Z;
	return (chk);
}

static int		chk_flag_length_modifier(char **format, t_flags *flag)
{
	int		chk;

	chk = 0;
	if (**format == '#' && (chk = 1) && !(flag->f_length & F_L_DIEZE))
		flag->f_length += F_L_DIEZE;
	else if (**format == '0' && (chk = 1) && !(flag->f_length & F_L_ZERO))
		flag->f_length += F_L_ZERO;
	else if (**format == '+' && (chk = 1) && !(flag->f_length & F_L_PLUS))
		flag->f_length += F_L_PLUS;
	else if (**format == '-' && (chk = 1) && !(flag->f_length & F_L_MOINS))
		flag->f_length += F_L_MOINS;
	else if (**format == ' ' && (chk = 1) && !(flag->f_length & F_L_SPACE))
		flag->f_length += F_L_SPACE;
	return (chk);
}

static int		chk_flag_precision(char **format, t_flags *flag, va_list *pa)
{
	int chk;

	chk = 0;
	if (**format == '.' && ((*format)++))
	{
		++chk;
		if (**format == '*' && ++chk && (*format)++)
			flag->f_precision = va_arg(*pa, int);
		else if (ft_isdigit(**format))
			flag->f_precision = ft_atoi(*format);
		else
			flag->f_precision = 0;
		if (chk == 1)
			while (ft_isdigit(**format))
				(*format)++;
	}
	return (chk);
}

static int		chk_flag_lenmin(char **format, t_flags *flag, va_list *pa)
{
	int chk;

	chk = 0;
	if (ft_isdigit(**format) && ++chk)
		flag->f_lenmin = ft_atoi(*format);
	else if (**format == '*')
	{
		flag->f_lenmin = va_arg(*pa, int);
		if (flag->f_lenmin < 0)
		{
			flag->f_lenmin = -flag->f_lenmin;
			if (!(flag->f_length & F_L_MOINS))
				flag->f_length += F_L_MOINS;
		}
		(*format)++;
		return (1);
	}
	if (chk)
		while (ft_isdigit(**format))
			(*format)++;
	return (chk);
}

int				chk_flags(char **format, t_flags *flag, va_list *pa)
{
	int chk;
	int chk_len;

	chk = 1;
	chk_len = 1;
	while (chk)
	{
		chk = chk_flag_char(format, flag);
		while ((chk_len = chk_flag_length_modifier(format, flag)))
		{
			chk += chk_len;
			(*format)++;
		}
		chk += chk_flag_precision(format, flag, pa);
		chk += chk_flag_lenmin(format, flag, pa);
	}
	return (0);
}
