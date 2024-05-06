#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
typedef struct users Users;
typedef struct massages massages;
typedef struct group group;
struct users
{
	char *name;
	char *password;
	char  *phone;
	time_t time;
	group **group_name;
	time_t *last_check;
	Users *next;
};
struct massages
{
	char *sender;
	char *receiver;
	int seen;
	char *massage;
	time_t time;
	massages *next;
};
struct group
{
	char *name;
	char *admin;
	char **member;
	massages *head_mass;
	group *next;
};
void setTextColor(int color);
int add_to(Users *new_user, Users *head);
int show_sent(Users *head, Users *new_user, massages*head1);
int check_existgroup(Users *new_user, char *check);
int show_user_sent(Users *head, Users *new_user, massages*head1, char* check);
int creat_gruop(Users *new_user, group* head2, Users* head);
int show_user_sent(Users *head, Users *new_user, massages*head1, char* check);
char* read(FILE* ptr);
int main_mode();
int savepass(Users *newuser);
Users *signup(Users *);
int savename(Users * head, Users *newuser);
int savephone(Users *, Users *newuser);
int command(int mode, Users *new_user, Users* head, massages* head1, group *head2);
Users *login(Users *head);
Users *checkname(Users *head);
int checkpass(Users *user);
int edit_username(Users* user, Users *head, massages*head1, group* head2);
int edit_password(Users* user);
void info(Users *head);
void about(Users *head);
int show_recieved(Users *head, Users *new_user, massages*head1);
void save_file(Users*head, massages *head1, group*head2);
void save_file_users(Users*head);
void save_file_massage(massages*head1, char* file_name, char*mode);
void save_file_group(group *head2);
void read_file(Users*head, massages *head1, group*head2);
void read_file_user(Users*head, group*head2);
void read_file_massage(FILE *ptr, massages*head1);
void read_file_group(group*head2);
void unread_massage(Users*new_user, massages*head1);
int send_mass(Users *head, Users *new_user, massages*head1);

