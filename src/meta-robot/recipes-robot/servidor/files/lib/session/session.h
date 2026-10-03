#ifndef SESSION_H
#define SESSION_H

int session_create(const char *username, char *token_out, int token_out_len);
int session_validate(const char *token);

#endif