#ifndef AUDIO_CGI_H
#define AUDIO_CGI_H

const char *audio_cgi_query_value(const char *name);
int audio_cgi_call(const char *command, const char *argument);

#endif
