#ifndef PASSTHROUGH_H_INC
#define PASSTHROUGH_H_INC

bool writeall(int, const char*, size_t);
bool manual_splice(int, int);
void passthrough(int, int, int, int);


#endif
