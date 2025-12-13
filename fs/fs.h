#ifndef FS_H
#define FS_H

#include <stdint.h>

#define MAX_FILES 64
#define MAX_FILENAME 32
#define MAX_FILE_SIZE 4096
#define MAX_PATH 256

typedef enum { INODE_FILE, INODE_DIR } inode_type_t;

typedef struct inode {
	char name[MAX_FILENAME];
	inode_type_t type;
	uint32_t size;
	char data[MAX_FILE_SIZE];
	struct inode *parent;
	struct inode *children[MAX_FILES];
	int child_count;
	uint8_t used;
} inode_t;

void fs_init(void);
inode_t *fs_get_root(void);
inode_t *fs_get_cwd(void);
void fs_set_cwd(inode_t *dir);

inode_t *fs_create_file(inode_t *parent, const char *name);
inode_t *fs_create_dir(inode_t *parent, const char *name);
inode_t *fs_find_child(inode_t *parent, const char *name);
int fs_write_file(inode_t *file, const char *data, uint32_t size);
int fs_read_file(inode_t *file, char *buffer, uint32_t size);
int fs_delete(inode_t *parent, const char *name);
void fs_get_path(inode_t *node, char *buffer);

#endif
