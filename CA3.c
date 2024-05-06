#include "CA3.h"

int main()
{
	Users head;
	massages head1;
	group head2;
	head.next = NULL;
	head1.next = NULL;
	head2.next = NULL;
	read_file(&head, &head1, &head2);
	printf("welcome \n ");
	while (1)
	{
		int mode = main_mode();
		if (mode == 1)
		{
			Users *user = signup(&head);
			if (user == NULL)
			{
				continue;
			}
			while (1)
			{
				int state = command(1, user, &head, &head1, &head2);
				if (state == 2)
				{
					break;
				}
			}
		}
		if (mode == 2)
		{
			Users *user = login(&head);
			if (user == NULL)
			{
				continue;
			}
			unread_massage(user, &head1);
			while (1)
			{
				int state = command(0, user, &head, &head1, &head2);
				if (state == 2)
				{
					break;
				}
			}
		}
		if (mode == 3)
		{
			save_file(&head, &head1, &head2);
			break;
		}

	}
}
void setTextColor(int color)
{
	WORD wColor;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(hStdout, &csbi))
	{
		wColor = (csbi.wAttributes & 0xF0) + (color & 0x0F);
		SetConsoleTextAttribute(hStdout, wColor);
	}
}
char* read(FILE* ptr)
{
	char*get = (char *)malloc(sizeof(char));
	int length = 1;
	while (1)
	{
		get[length - 1] = 0;
		char temp = fgetc(ptr);
		if (temp == 0)
			return NULL;
		if (temp == ' ' || temp == '\n' || temp == '\t')
			break;
		length++;
		get = (char*)realloc(get, sizeof(char)*length);
		get[length - 2] = temp;
	}
	return get;
}
void unread_massage(Users*new_user, massages*head1)
{
	int i = 0, flag = 0;
	printf("your unread massages are : \n");
	while (head1->next != NULL)
	{
		head1 = head1->next;
		if ((strcmp(head1->receiver, new_user->name)) == 0)
		{
			if (head1->seen == 0)
			{
				flag = 1;
				time_t tim = head1->time;
				printf("massage from %s at %s \n", head1->sender, ctime(&tim));
			}
		}
	}
	while (new_user->group_name[i] != NULL)
	{
		massages* temp = new_user->group_name[i]->head_mass;
		while (temp->next != NULL)
		{
			temp = temp->next;
			if (new_user->last_check[i] < temp->time && strcmp(temp->sender, new_user->name))
			{
				flag = 1;
				time_t tim = temp->time;
				printf("massage in group : %s at %s \n", temp->receiver, ctime(&tim));
			}
		}
		i++;
	}
	if (flag == 0)
	{
		printf("you dont have any unread massage \n ");
	}
}
void read_file(Users*head, massages *head1, group*head2)
{
	FILE *ptr;
	while ((ptr = fopen("user_maasage.txt", "a+")) == NULL);
	fseek(ptr, 0, SEEK_END);
	int g = ftell(ptr);
	rewind(ptr);
	if (g != 0)
	{
		read_file_massage(ptr, head1);
	}
	fclose(ptr);
	read_file_group(head2);
	read_file_user(head, head2);
}
void read_file_user(Users*head, group*head2)
{
	FILE *ptr;
	while ((ptr = fopen("user_info.txt", "a+")) == NULL);
	fseek(ptr, 0, SEEK_END);
	int g = ftell(ptr);
	long long temp = 0;
	rewind(ptr);
	if (g != 0)
	{

		while (1)
		{
			int i = 0;
			Users* newuse = (Users*)malloc(sizeof(Users));
			newuse->group_name = (group **)malloc(sizeof(group *));
			newuse->group_name[0] = NULL;
			newuse->last_check = (time_t*)malloc(sizeof(time_t));
			newuse->last_check[0] = -1;
			head->next = newuse;
			head = head->next;
			newuse->next = NULL;
			newuse->name = read(ptr);
			newuse->password = read(ptr);
			newuse->phone = read(ptr);
			fscanf(ptr, "%lld", &(newuse->time));
			fgetc(ptr);
			fscanf(ptr, "%lld", &temp);
			while (temp != -1)
			{
				newuse->last_check[i] = (time_t)temp;
				newuse->last_check = realloc(newuse->last_check, sizeof(time_t)*(i + 2));
				newuse->last_check[i + 1] = -1;
				fgetc(ptr);
				fscanf(ptr, "%lld", &temp);

				i++;
			}
			newuse->group_name = (group**)malloc(sizeof(group*)*(i + 1));
			int num = 0;
			group* temp = head2;
			while (1)
			{
				
				if (temp->next != NULL)
					temp = temp->next;
				else
					break;

				for (int j = 0; temp->member[j] != NULL; j++)
				{
					if ((strcmp(temp->member[j], newuse->name)) == 0)
					{
						newuse->group_name[num] = temp;
						num++;
						break;
					}
				}
			}
			newuse->group_name[i] = NULL;
			if (fgetc(ptr) == EOF)
				break;
		}
	}
	fclose(ptr);
}
void read_file_massage(FILE *ptr, massages*head1)
{
	char *temp = read(ptr);
	while (temp != NULL)
	{
		massages *new_mass = (massages*)malloc(sizeof(massages));
		head1->next = new_mass;
		head1 = head1->next;
		new_mass->next = NULL;
		new_mass->sender = temp;
		new_mass->receiver = read(ptr);
		fscanf(ptr, "%d", &(head1->seen));
		fgetc(ptr);
		new_mass->massage = read(ptr);
		fscanf(ptr, "%lld", &(head1->time));
		int result = fgetc(ptr);
		if (result == -1 || result == 0 || result == '\t')
			break;
		temp = read(ptr);
	}
}
void read_file_group(group*head2)
{
	FILE *ptr;
	while ((ptr = fopen("group.txt", "a+")) == NULL);
	fseek(ptr, 0, SEEK_END);
	int g = ftell(ptr), i = 0;
	rewind(ptr);
	if (g != 0)
	{
		while (1)
		{
			i = 0;
			group* new_gp = (group*)malloc(sizeof(group));
			new_gp->member = (char**)malloc(sizeof(char*));
			new_gp->head_mass = (massages*)malloc(sizeof(massages));
			new_gp->head_mass->next = NULL;
			new_gp->member[0] = NULL;
			head2->next = new_gp;
			head2 = head2->next;
			new_gp->next = NULL;
			new_gp->name = read(ptr);
			new_gp->admin = read(ptr);
			char* temp = read(ptr);
			while (temp != NULL)
			{
				new_gp->member[i] = temp;
				new_gp->member = realloc(new_gp->member, sizeof(char*)*(i + 2));
				new_gp->member[i + 1] = NULL;
				temp = read(ptr);
				i++;
			}
			fgetc(ptr);
			read_file_massage(ptr, new_gp->head_mass);
			if (fgetc(ptr) == EOF)
				break;
		}
	}
	fclose(ptr);
}
void save_file_group(group *head2)
{
	int i = 0;
	FILE *ptr;
	while ((ptr = fopen("group.txt", "w")) == NULL);
	while (head2->next != NULL)
	{
		i = 0;
		head2 = head2->next;
		fputs(head2->name, ptr);
		fputc('\t', ptr);
		fputs(head2->admin, ptr);
		fputc('\n', ptr);
		while (head2->member[i] != NULL)
		{
			fputs(head2->member[i], ptr);
			fputc('\t', ptr);
			i++;
		}
		fputc('\0', ptr);
		fputc('\n', ptr);
		fclose(ptr);
		save_file_massage(head2->head_mass, "group.txt", "a+");
		fopen("group.txt", "a+");
		fputc('\t', ptr);
		if (head2->next != NULL)
			fputc('\n', ptr);
	}
	fclose(ptr);
}
void save_file(Users*head, massages *head1, group*head2)
{
	save_file_users(head);
	save_file_massage(head1, "user_maasage.txt", "w");
	save_file_group(head2);
}
void save_file_users(Users*head)
{
	int i = 0;
	FILE *ptr;
	while ((ptr = fopen("user_info.txt", "w")) == NULL);
	head = head->next;
	while (head != NULL)
	{
		i = 0;
		fputs(head->name, ptr);
		fputc('\t', ptr);
		fputs(head->password, ptr);
		fputc('\t', ptr);
		fputs(head->phone, ptr);
		fputc('\t', ptr);
		fprintf(ptr, "%lld", head->time);
		fputc('\t', ptr);
		while (head->last_check[i] != -1)
		{
			fprintf(ptr, "%lld", (long long)head->last_check[i]);
			fputc('\t', ptr);
			i++;
		}
		fprintf(ptr, "%lld", (long long)head->last_check[i]);
		if (head->next != NULL)
			fputc('\n', ptr);
		head = head->next;
	}
	fclose(ptr);
}
void save_file_massage(massages*head1, char* file_name, char*mode)
{
	FILE *ptr;
	while ((ptr = fopen(file_name, mode)) == NULL);
	while (head1->next != NULL)
	{
		head1 = head1->next;
		fputs(head1->sender, ptr);
		fputc('\t', ptr);
		fputs(head1->receiver, ptr);
		fputc('\t', ptr);
		fprintf(ptr, "%d", head1->seen);
		fputc('\t', ptr);
		fputs(head1->massage, ptr);
		fputc('\t', ptr);
		fprintf(ptr, "%lld", head1->time);
		if (head1->next != NULL)
			fputc('\n', ptr);
	}
	fclose(ptr);
}

int main_mode()
{
	printf("please enter your command \n");
	printf(">>");
	char*check_mode = read(stdin);
	if (!(strncmp(check_mode, "signup", 6)))
	{
		free(check_mode);
		return 1;
	}
	if (!(strncmp(check_mode, "login", 5)))
	{
		free(check_mode);
		return 2;
	}
	if (!(strncmp(check_mode, "close", 5)))
	{
		free(check_mode);
		return 3;
	}
	if (!(strncmp(check_mode, "logout", 6)))
	{
		free(check_mode);
		printf("you are not in your account \n");
		return 0;
	}
	printf("wrong!please try again \n");
	printf("press enter if it was needed for clearing the buffer \n");
	char f;
	while ((f = getchar()) != '\n' && f != EOF);
	free(check_mode);
	return 0;
}
int command(int mode, Users *new_user, Users* head, massages* head1, group *head2)
{
	char f;
	printf(">>");
	char*check_mode = read(stdin);
	if (!(strncmp(check_mode, "edit_username", 13)))
	{
		edit_username(new_user, head, head1,head2);
		free(check_mode);
		return 1;
	}
	if (!(strncmp(check_mode, "edit_password", 13)))
	{
		edit_password(new_user);
		free(check_mode);
		return 1;
	}
	if (!(strncmp(check_mode, "send", 4)))
	{
		send_mass(head, new_user, head1);
		free(check_mode);
		return 1;
	}
	if (!(strncmp(check_mode, "info", 4)))
	{
		info(head);
		free(check_mode);
		return 1;
	}
	if (!(strncmp(check_mode, "about", 5)))
	{
		if (mode)
		{
			printf("you can use about only in login sitution \n");
			printf("press enter if it was needed for clearing the buffer \n");
			while ((f = getchar()) != '\n' && f != EOF);
			return 0;
		}
		about(head);
		free(check_mode);
		return 1;
	}
	if (!(strncmp(check_mode, "show_recieved", 13)))
	{
		show_recieved(head, new_user, head1);
		free(check_mode);
		return 1;
	}
	if (!(strncmp(check_mode, "show_sent", 9)))
	{
		show_sent(head, new_user, head1);
		free(check_mode);
		return 1;
	}
	if (!(strncmp(check_mode, "create_group", 11)))
	{
		creat_gruop(new_user, head2,head);
		free(check_mode);
		return 1;
	}
	if (!(strncmp(check_mode, "add_to", 6)))
	{
		add_to(new_user, head);
		free(check_mode);
		return 1;
	}
	if (!(strncmp(check_mode, "logout", 6)))
	{
		printf("you logged out \n ");
		free(check_mode);
		return 2;
	}
	printf("wrong! invalid command! please try again \n");
	printf("press enter if it was needed for clearing the buffer \n");
	while ((f = getchar()) != '\n' && f != EOF);
	free(check_mode);
	return 0;
}
Users* signup(Users * head)
{
	int flag = 0;
	Users *newuser = (Users *)malloc(sizeof(Users));
	if (savename(head, newuser))
	{
		if (savepass(newuser))
		{
			if (savephone(head, newuser))
			{
				printf("congratulation ! you are logined\n");
				flag = 1;
			}
		}
	}
	if (flag == 0)
	{
		printf("please try again \n");
		char f;
		free(newuser);
		printf("press enter if it was needed for clearing the buffer \n");
		while ((f = getchar()) != '\n' && f != EOF);
		return NULL;
	}
	else
	{
		Users *curr = head;
		while (curr->next != NULL)
		{
			curr = curr->next;
		}
		curr->next = newuser;
		newuser->time = time(NULL);
		newuser->next = NULL;
		newuser->group_name = (group **)malloc(sizeof(group *));
		newuser->group_name[0] = NULL;
		newuser->last_check = (time_t*)malloc(sizeof(time_t));
		newuser->last_check[0] = -1;
		return newuser;
	}
}
int savename(Users *head, Users *newuser)
{
	char *check_name = read(stdin);
	if (checkname == NULL)
	{
		printf(" you should write something for yuor name \n ");
		free(check_name);
		return 0;
	}
	Users *curr = head;
	int compare = 1;
	while (curr->next != NULL)
	{
		curr = curr->next;
		compare *= strcmp(check_name, curr->name);
		if (compare == 0)
		{
			printf("your name is already exist try again \n");
			free(check_name);
			return 0;
		}
	}

	newuser->name = check_name;
	return 1;
}
Users* checkname(Users *head)
{
	char *check_name = read(stdin);
	Users *curr = head;
	int compare = 1;
	while (curr->next != NULL)
	{
		curr = curr->next;
		compare *= strcmp(check_name, curr->name);
		if (compare == 0)
		{
			free(check_name);
			return curr;
		}
	}
	printf("your name doesnt exist \n");
	free(check_name);
	return NULL;
}
int checkpass(Users *user)
{
	char *check_pass = read(stdin);
	int compare = 1;
	compare = strcmp(check_pass, user->password);
	if (compare == 0)
	{
		free(check_pass);
		return 1;
	}

	printf("your password doesnt exist \n");
	free(check_pass);
	return 0;
}
int savepass(Users *newuser)
{
	char *check_password = read(stdin);
	int pass_sieze = strlen(check_password);
	if (pass_sieze < 6)
	{
		printf("wrong ! you must enter at least 6 character for password \n ");
		free(check_password);
		return 0;
	}
	newuser->password = check_password;
	return 1;
}
int savephone(Users *head, Users *newuser)
{
	char *check_phone = read(stdin);
	int lengh = strlen(check_phone);
	if (lengh != 11)
	{
		printf("your phone number must be 11 character \n");
		free(check_phone);
		return 0;
	}
	if (check_phone[0] != '0')
	{
		printf("wrong phone number!! your first character must be 0 \n");
		free(check_phone);
		return 0;
	}
	for (int i = 1; i < 11; i++)
	{
		if (check_phone[i] <= '9' && check_phone[i] >= '0');
		else
		{
			printf("your number is wrong! only use numbers. try again \n ");
			free(check_phone);
			return 0;
		}
	}
	int compare = 1;
	Users *curr = head;
	curr = curr->next;
	while (curr != NULL)
	{
		compare *= strcmp(check_phone, curr->phone);
		if (compare == 0)
		{
			printf("your phone number is already exist  \n");
			free(check_phone);
			return 0;
		}
		curr = curr->next;
	}
	newuser->phone = check_phone;
	return 1;
}
Users *login(Users *head)
{
	char f;
	if (head->next == NULL)
	{
		printf("nobody has signed up yet , you shuold sign up first \n");
		while ((f = getchar()) != '\n' && f != EOF);
		return NULL;
	}
	Users *user = checkname(head);
	if (user != NULL)
	{
		if (checkpass(user))
		{
			printf("congratulation ! you are logined\n");
			return user;
		}
	}
	printf("press enter if it was needed for clearing the buffer \n");
	while ((f = getchar()) != '\n' && f != EOF);
	printf("please try again \n ");
	return NULL;
}
int edit_username(Users* user, Users* head, massages* head1, group* head2)
{
	char *change_name = read(stdin);
	while (head2->next != NULL)
	{
		head2 = head2->next;
		if (!strcmp(head2->name, change_name))
		{
			printf("wrong !! your name exists \n");
			free(change_name);
			return 0;
		}
	}
	while (head->next != NULL)
	{
		head = head->next;
		if (strcmp(head->name, change_name) == 0)
		{
			printf("your username has already been used \n");
			free(change_name);
			return 0;
		}
	}
	char *temp = user->name;
	user->name = change_name;

	printf("your user_name has changed \n ");
	int i = 0, j = 0;
	while (user->group_name[i] != NULL)
	{
		if (strcmp(user->group_name[i]->admin, temp) == 0)
		{
			user->group_name[i]->admin = user->name;
			user->group_name[i]->member[0] = user->name;
		}
		else
		{
			while (user->group_name[i]->member[j] != NULL)
			{
				if (strcmp(user->group_name[i]->member[j], temp) == 0)
				{
					user->group_name[i]->member[j] = user->name;
					break;
				}
				j++;
			}
		}
		massages *temp1 = user->group_name[i]->head_mass;
		while (temp1->next != NULL)
		{
			temp1 = temp1->next;
			if (strcmp(temp1->sender, temp) == 0)
			{
				temp1->sender = user->name;
			}
		}
		i++;
	}
	while (head1->next != NULL)
	{
		head1 = head1->next;
		if (strcmp(head1->receiver, temp) == 0)
		{
			head1->receiver = user->name;
		}
		if (strcmp(head1->sender, temp) == 0)
		{
			head1->sender = user->name;
		}
	}
	free(temp);
	return 1;
}
int edit_password(Users* user)
{
	char *check_password = read(stdin);
	int pass_sieze = strlen(check_password);
	if (pass_sieze < 6)
	{
		printf("wrong ! you must enter at least 6 character \n ");
		free(check_password);
		return 0;
	}
	if (strcmp(check_password, user->password) == 0)
	{
		printf("you can not have your previous \n");
		free(check_password);
		return 0;
	}
	char *temp = user->password;
	user->password = check_password;
	free(temp);
	printf("your password changed \n ");
	return 1;
}

void info(Users *head)
{
	Users *curr = head;
	while (curr->next != NULL)
	{
		curr = curr->next;
		printf("%s \n", curr->name);
	}
}
void about(Users *head)
{
	char *username = read(stdin);
	Users *curr = head;
	int compare = 1;
	while (curr->next != NULL)
	{
		curr = curr->next;
		compare = strcmp(username, curr->name);
		if (compare == 0)
		{
			printf("name is %s \t phone is %s \n", curr->name, curr->phone);
			break;
		}
	}
	if (compare != 0)
	{
		printf("your number doesnt exist");
	}

}

int show_recieved(Users *head, Users *new_user, massages*head1)
{
	char *check = read(stdin);
	int i = 0, compare = 1, j = 0, compare1 = 1, compare2 = 1, flag = 0;
	int result = check_existgroup(new_user, check);
	if (result)
	{
		free(check);
		return 1;
	}
	while (head->next != NULL)
	{
		head = head->next;
		compare = strcmp(check, head->name);
		if (compare == 0)
		{
			while (head1->next != NULL)
			{
				head1 = head1->next;
				compare1 = strcmp(head1->receiver, new_user->name);
				compare2 = strcmp(head1->sender, check);
				if (compare1 == 0 && compare2 == 0)
				{
					head1->seen = 1;
					time_t tim = head1->time;
					printf(" in %s \n from %s \n %s \n", ctime(&tim), head1->sender, head1->massage);
					flag = 1;
				}
			}
			break;
		}
	}
	free(check);
	if (compare != 0)
	{
		printf("your name doesnt exist\n");

		return 0;
	}
	else if (flag == 0)
	{
		printf("there isnt any massage \n");
	}
	return 1;
}
int show_sent(Users *head, Users *new_user, massages*head1)
{
	char *check = read(stdin);
	int i = 0, compare = 1, compare1 = 1, flag = 0;
	massages *head_massage;
	int result = show_user_sent(head, new_user, head1, check);
	if (result)
	{
		free(check);
		return 1;
	}
	while (new_user->group_name[i] != NULL)
	{
		i = 0;
		compare = strcmp(new_user->group_name[i]->name, check);
		if (compare == 0)
		{
			head_massage = new_user->group_name[i]->head_mass;
			break;
		}
		i++;
	}
	free(check);
	if (compare == 0)
	{
		if (head_massage->next == NULL)
		{
			printf("there isnt any massage in group\n");
		}
		while (head_massage->next != NULL)
		{
			head_massage = head_massage->next;
			compare1 = strcmp(head_massage->sender, new_user->name);
			if (compare1 == 0)
			{
				if (head_massage->seen == 0)
					setTextColor(2);
				time_t tim = head_massage->time;
				printf("in %s to %s \n %s \n", ctime(&tim), head_massage->receiver, head_massage->massage);
				flag = 1;
				if (head_massage->seen == 0)
					setTextColor(7);
			}
		}
		if (flag == 1)
			return 1;
	}
	if (compare != 0)
		printf("your name doesnt exist \n");
	else if (flag == 0)
	{
		printf("there isnt any massage \n");
	}
	return 0;
}
int check_existgroup(Users *new_user, char *check)
{
	int i = 0, compare = 1;
	massages *head_massage;
	while (new_user->group_name[i] != NULL)
	{
		compare = strcmp(new_user->group_name[i]->name, check);
		if (compare == 0)
		{
			head_massage = new_user->group_name[i]->head_mass;
			new_user->last_check[i] = time(NULL);
			break;
		}
		i++;
	}

	if (compare == 0)
	{
		if (head_massage->next == NULL)
		{
			printf("there isnt any massage in group\n");
		}
		while (head_massage->next != NULL)
		{
			head_massage = head_massage->next;
			head_massage->seen = 1;
			time_t tim = head_massage->time;
			printf("in %s \n from %s \n %s \n", ctime(&tim), head_massage->sender, head_massage->massage);
		}
		return 1;
	}
	return 0;
}
int show_user_sent(Users *head, Users *new_user, massages*head1, char* check)
{

	int i = 0, compare = 1, j = 0, compare1 = 1, compare2 = 1, flag = 0;

	while (head->next != NULL)
	{
		head = head->next;
		compare = strcmp(check, head->name);
		if (compare == 0)
		{
			while (head1->next != NULL)
			{
				head1 = head1->next;
				compare1 = strcmp(head1->sender, new_user->name);
				compare2 = strcmp(head1->receiver, check);
				if (compare1 == 0 && compare2 == 0)
				{
					if (head1->seen == 0)
						setTextColor(2);
					time_t tim = head1->time;
					printf(" in %s to %s \n %s \n", ctime(&tim), head1->receiver, head1->massage);
					flag = 1;
					if (head1->seen == 0)
						setTextColor(7);
				}
			}
			break;
		}
	}
	if (compare != 0)
	{
		return 0;
	}
	else if (flag == 0)
	{
		printf("there isnt any massage \n");
	}
	return 1;



}
int creat_gruop(Users *new_user, group* head2,Users*head)
{
	char * check = read(stdin);
	int compare = 1, i = 0;
	while (head2->next != NULL)
	{
		head2 = head2->next;
		if (!strcmp(head2->name, check))
		{
			printf("wrong !! your name exists \n");
			free(check);
			return 0;
		}
	}
	while (head->next != NULL)
	{
		head = head->next;
		if (!strcmp(head->name, check))
		{
			printf("wrong !! your name exists \n");
			free(check);
			return 0;
		}
	}
	group* temp = (group*)malloc(sizeof(group));
	head2->next = temp;
	temp->next = NULL;
	temp->name = check;
	temp->admin = new_user->name;
	temp->member = (char**)malloc(2 * sizeof(char*));
	temp->member[0] = temp->admin;
	temp->member[1] = NULL;
	massages* head1 = (massages*)malloc(sizeof(massages));
	temp->head_mass = head1;
	head1->next = NULL;
	while (new_user->group_name[i] != NULL)
	{
		i++;
	}
	new_user->group_name[i] = temp;
	new_user->group_name = realloc(new_user->group_name, sizeof(group*)*(i + 2));
	new_user->group_name[i + 1] = NULL;
	i = 0;
	while (new_user->last_check[i] != -1)
	{
		i++;
	}
	new_user->last_check = realloc(new_user->last_check, sizeof(time_t)*(i + 2));
	new_user->last_check[i] = 0;
	new_user->last_check[i + 1] = -1;
	printf("new group created \n ");
	return 1;
}
int add_to(Users *new_user, Users *head)
{
	char * check = read(stdin), f;
	int i = 0, j = 0, compare = 1, compare1 = 1;
	while (new_user->group_name[i] != NULL)
	{
		compare = strcmp(new_user->group_name[i]->name, check);
		if (compare == 0)
		{
			free(check);
			compare1 = strcmp(new_user->group_name[i]->admin, new_user->name);
			if (compare1 == 0)
			{
				break;
			}
			while ((f = getchar()) != '\n' && f != EOF);
			printf("you are not admin\n");
			return 0;
		}
		i++;
	}
	if (compare != 0)
	{
		free(check);
		printf("the group doesnt exist\n");
		while ((f = getchar()) != '\n' && f != EOF);
		return 0;
	}
	Users *result = checkname(head);
	if (result == NULL)
	{
		return 0;
	}
	while (new_user->group_name[i]->member[j] != NULL)
	{
		compare = strcmp(new_user->group_name[i]->member[j], result->name);
		if (compare == 0)
		{
			printf(" your member already exists \n");
			return 0;
		}
		j++;
	}
	new_user->group_name[i]->member = (char**)realloc(new_user->group_name[i]->member, (j + 2) * sizeof(char*));
	new_user->group_name[i]->member[j] = result->name;
	new_user->group_name[i]->member[j + 1] = NULL;
	j = 0;
	while (result->group_name[j] != NULL)
	{
		j++;
	}
	result->group_name = realloc(result->group_name, sizeof(group*)*(i + 2));
	result->group_name[j] = new_user->group_name[i];
	result->group_name[j + 1] = NULL;
	i = 0;
	while (result->last_check[i] != -1)
	{
		i++;
	}
	result->last_check = realloc(result->last_check, sizeof(time_t)*(i + 2));
	result->last_check[i] = 0;
	result->last_check[i + 1] = -1;
	printf("new member added to group \n ");
	return 1;
}
int send_mass(Users *head, Users *new_user, massages*head1)
{
	char *check = read(stdin), *temp = read(stdin);
	Users *curr = head;
	int i = 0, j = 0, compare = 1;
	massages *head_massage;
	if (temp == NULL)
	{
		printf("you must write some thing \n");
		free(check);
		free(temp);
		return 0;
	}
	while (new_user->group_name[i] != NULL)
	{
		compare = strcmp(new_user->group_name[i]->name, check);
		if (compare == 0)
		{
			while (new_user->group_name[i]->member[j] != NULL)
			{
				compare = strcmp(new_user->group_name[i]->member[j], new_user->name);
				if (compare == 0)
				{
					break;
				}
				j++;
			}
			if (compare != 0)
			{
				free(check);
				printf("you are not a member of this group\n");
				return 0;
			}
			break;
		}
		i++;
	}
	if (compare == 0)
	{
		head_massage = new_user->group_name[i]->head_mass;
		massages* new_massage = (massages*)malloc(sizeof(massages));
		while (head_massage->next != NULL)
		{
			head_massage = head_massage->next;
		}
		head_massage->next = new_massage;
		new_massage->next = NULL;
		new_massage->massage = temp;
		new_massage->sender = new_user->name;
		new_massage->receiver = check;
		new_massage->seen = 0;
		new_massage->time = time(NULL);
		printf("your massage successfully sent \n ");
		return 1;
	}

	compare = 1;
	while (curr->next != NULL)
	{
		curr = curr->next;
		compare = strcmp(check, curr->name);
		if (compare == 0)
		{
			break;
		}
	}

	if (compare == 0)
	{
		massages* new_massage = (massages*)malloc(sizeof(massages));
		while (head1->next != NULL)
		{
			head1 = head1->next;
		}
		head1->next = new_massage;
		new_massage->next = NULL;
		new_massage->massage = temp;
		new_massage->sender = new_user->name;
		new_massage->receiver = check;
		new_massage->seen = 0;
		new_massage->time = time(NULL);
		printf("your massage successfully sent!! \n ");
		return 1;
	}
	else
	{
		printf("your name doesnt exist \n");
		free(check);
		return 0;
	}
}