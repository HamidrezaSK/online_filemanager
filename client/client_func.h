void  send_register_req(char*,int,struct sockaddr_in);
int   send_search_req(char*,int,struct sockaddr_in);
void  send_delete_req(char*,int,struct sockaddr_in);
void  send_getall_req(int,struct sockaddr_in);
void  print_all_serverfiles(void);
char* get_my_ip(void);
void add_to_myfiles(char*);
void delete_from_myfiles(char*);




