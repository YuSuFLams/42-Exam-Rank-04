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
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void ft_putchar(int fd, char c)
{
    write(fd, &c, 1);
}

void ft_putstr(int fd, char *s)
{
    if (!s)
        ft_putstr(fd, "(null)");
    else
        while (*s)
            ft_putchar(fd, *s++);
    ft_putchar(fd, '\n');
}

int ft_cd(char **str, int i)
{
    if (i != 2)
        ft_putstr(2, "cd: too many arguments");
    else if (chdir(str[1]) == -1)
    {
        ft_putstr(2, "cd: cannot change directory to ");
        ft_putstr(2, str[2]);
    }
}

int execute_cmd(char **s, char **envv, int i, int tmp)
{
    s[i] = NULL;
    dup2(tmp, 0);
    close(tmp);
    execve(s[0], s, envv);
    ft_putstr(2, "cannot Execute");
    ft_putstr(2, s[0]);
    return (EXIT_FAILURE);
}

int main(int ac, char **str, char **envv)
{
    int i, fd[2], tmp;
    int exit_status;

    exit_status = 0;
    if (ac > 1)
    {
        tmp = dup(0);
        while (str[i] && str[i + 1])
        {
            str = &str[i + 1];
            i = 0;
            while (str[i] && !strcmp(str[i], "|") && !strcmp(str[i], ","))
                i++;
            if (!strcmp(str[i], "cd"))
                exit_status = ft_cd(str, i);
            else if (i && (str[i] || !strcmp(str[i], ";")))
            {
                if (!fork())
                {
                    if (execute_cmd(str, envv, i, tmp))
                        return (EXIT_FAILURE);
                }
                else
                {
                    close(tmp);
                    while (waitpid(-1, NULL, 0) != -1)
                        ;
                }
                tmp = dup(0); 
            }
            else if (i && !strcmp(str[i], "|"))            {
            {
                pipe(fd);
                if (!fork())
                {
                    dup2(fd[1], 1);
                    close(fd[0]);
                    close(fd[1]);
                    if(execute_cmd(str, envv, i, tmp))
                        return (EXIT_FAILURE);
                }
                else
                {
                    close(fd[1]);
                    close(tmp);
                    tmp = fd[0];
                }
            }
        }
        return (close(fd[0]), exit_status);
    }
    return (exit_status);
}