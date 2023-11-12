/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ylamsiah <ylamsiah@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 20:48:23 by ylamsiah          #+#    #+#             */
/*   Updated: 2023/11/11 21:33:28 by ylamsiah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

void ft_putchar(char c, int fd)
{
    write(fd, &c, 1);
}

void ft_putstr(char *s, int fd)
{
    if (!s)
        write(fd, "(null)", 6);
    else
        while (*s)
            ft_putchar(*s++, fd);
}

int ft_cd(char **s, int i)
{
    if (i != 2)
    {
        ft_putstr("error: cd: bad arguments", 2);
    }
    else if (chdir(s[1]) == -1)
    {
        ft_putstr("error: cd: cannot change directory to ", 2);
        ft_putstr(s[1], 2);
    }
	ft_putchar('\n', 2);
    return EXIT_FAILURE;
}

int execut_cmd(char **s, char **envv, int tmp, int i)
{
    s[i] = NULL;
    dup2(tmp, 0);
    close(tmp);
    execve(s[0], s, envv);
    ft_putstr("error: cannot execute ", 2);
    ft_putstr(s[0], 2);
    ft_putchar('\n', 2);
    return EXIT_FAILURE;
}

int main(int ac, char **str, char **envv)
{
    int i, tmp, fd[2];

	(void)ac;
    i = 0;
    tmp = dup(0);
    while (str[i] && str[i + 1])
    {
        str = &str[i + 1];
        i = 0;
        while (str[i] && (strcmp(str[i], "|") && strcmp(str[i], ";")))
            i++;
        if (!strcmp(str[0], "cd"))
            ft_cd(str, i);
        else if (i && (!str[i] || !strcmp(";", str[i])))
        {
            if (!fork())
            {
                if (execut_cmd(str, envv, tmp, i))
                    return (1);
            }
            else
            {
                close(tmp);
                while (waitpid(-1, NULL, 0) != -1)
                    ;
            }
            tmp = dup(0);
        }
        else if (i && !strcmp(str[i], "|"))
        {
            pipe(fd);
            if (!fork())
            {
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
                if (execut_cmd(str, envv, tmp, i))
                    return 1;
            }
            else
            {
                close(fd[1]);
                close(tmp);
                tmp = fd[0];
            }
        }
    }
	return (close(tmp), 0);
}