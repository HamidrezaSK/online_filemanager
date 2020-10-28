int process_register_req(char*);
int process_delete_req(char*);
char* create_ack_response(void);
char* create_error_response(int);
void send_error_response(int,int,struct sockaddr_in);
void send_ack_response(int,struct sockaddr_in);
void send_allfiles_response(int,struct sockaddr_in);
void send_search_response(char*,int,struct sockaddr_in);



