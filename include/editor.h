#ifndef EDITOR_H
#define EDITOR_H

int editor_init(char const *file, char const *name);
void editor_quit(void);
void editor_update(void);
void editor_main_loop(void);

#endif
