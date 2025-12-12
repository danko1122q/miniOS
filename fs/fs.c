#include "fs.h"
#include "../lib/string.h"

static inode_t inodes[MAX_FILES];
static inode_t* root = 0;
static inode_t* cwd = 0;

void fs_init(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        inodes[i].used = 0;
    }
    
    root = &inodes[0];
    root->used = 1;
    root->type = INODE_DIR;
    strcpy(root->name, "/");
    root->size = 0;
    root->parent = 0;
    root->child_count = 0;
    
    cwd = root;
}

inode_t* fs_get_root(void) {
    return root;
}

inode_t* fs_get_cwd(void) {
    return cwd;
}

void fs_set_cwd(inode_t* dir) {
    if (dir && dir->type == INODE_DIR) {
        cwd = dir;
    }
}

static inode_t* alloc_inode(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!inodes[i].used) {
            inodes[i].used = 1;
            inodes[i].child_count = 0;
            inodes[i].size = 0;
            memset(inodes[i].data, 0, MAX_FILE_SIZE);
            return &inodes[i];
        }
    }
    return 0;
}

inode_t* fs_create_file(inode_t* parent, const char* name) {
    if (!parent || parent->type != INODE_DIR) return 0;
    if (parent->child_count >= MAX_FILES) return 0;
    if (fs_find_child(parent, name)) return 0;
    
    inode_t* file = alloc_inode();
    if (!file) return 0;
    
    strncpy(file->name, name, MAX_FILENAME - 1);
    file->type = INODE_FILE;
    file->parent = parent;
    
    parent->children[parent->child_count++] = file;
    return file;
}

inode_t* fs_create_dir(inode_t* parent, const char* name) {
    if (!parent || parent->type != INODE_DIR) return 0;
    if (parent->child_count >= MAX_FILES) return 0;
    if (fs_find_child(parent, name)) return 0;
    
    inode_t* dir = alloc_inode();
    if (!dir) return 0;
    
    strncpy(dir->name, name, MAX_FILENAME - 1);
    dir->type = INODE_DIR;
    dir->parent = parent;
    
    parent->children[parent->child_count++] = dir;
    return dir;
}

inode_t* fs_find_child(inode_t* parent, const char* name) {
    if (!parent || parent->type != INODE_DIR) return 0;
    
    for (int i = 0; i < parent->child_count; i++) {
        if (strcmp(parent->children[i]->name, name) == 0) {
            return parent->children[i];
        }
    }
    return 0;
}

int fs_write_file(inode_t* file, const char* data, uint32_t size) {
    if (!file || file->type != INODE_FILE) return -1;
    if (size > MAX_FILE_SIZE) size = MAX_FILE_SIZE;
    
    memcpy(file->data, data, size);
    file->size = size;
    return size;
}

int fs_read_file(inode_t* file, char* buffer, uint32_t size) {
    if (!file || file->type != INODE_FILE) return -1;
    
    uint32_t read_size = (size < file->size) ? size : file->size;
    memcpy(buffer, file->data, read_size);
    return read_size;
}

int fs_delete(inode_t* parent, const char* name) {
    if (!parent || parent->type != INODE_DIR) return -1;
    
    for (int i = 0; i < parent->child_count; i++) {
        if (strcmp(parent->children[i]->name, name) == 0) {
            inode_t* node = parent->children[i];
            
            if (node->type == INODE_DIR && node->child_count > 0) {
                return -1;
            }
            
            node->used = 0;
            
            for (int j = i; j < parent->child_count - 1; j++) {
                parent->children[j] = parent->children[j + 1];
            }
            parent->child_count--;
            return 0;
        }
    }
    return -1;
}

void fs_get_path(inode_t* node, char* buffer) {
    if (!node) {
        buffer[0] = '\0';
        return;
    }
    
    if (node == root) {
        strcpy(buffer, "/");
        return;
    }
    
    char temp[MAX_PATH];
    temp[0] = '\0';
    
    inode_t* current = node;
    while (current && current != root) {
        char new_temp[MAX_PATH];
        strcpy(new_temp, "/");
        strcat(new_temp, current->name);
        strcat(new_temp, temp);
        strcpy(temp, new_temp);
        current = current->parent;
    }
    
    strcpy(buffer, temp);
}
