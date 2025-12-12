#include "shell.h"
#include "../drivers/keyboard.h"
#include "../drivers/vga.h"
#include "../fs/fs.h"
#include "../lib/string.h"

#define CMD_BUFFER_SIZE 256

static char cmd_buffer[CMD_BUFFER_SIZE];

static inline uint8_t inb(uint16_t port)
{
	uint8_t value;
	__asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

static inline void outb(uint16_t port, uint8_t value)
{
	__asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static void show_welcome(void)
{
	vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
	vga_puts("================================\n");
	vga_puts("     Welcome to MiniOS v1.0     \n");
	vga_puts("================================\n\n");
	vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	vga_puts("Type 'help' for available commands\n\n");
}

static void print_prompt(void)
{
	char path[MAX_PATH];
	fs_get_path(fs_get_cwd(), path);

	vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
	vga_puts("minios");
	vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
	vga_puts(":");
	vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
	vga_puts(path);
	vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
	vga_puts("$ ");
	vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

static void print_help(void)
{
	vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
	vga_puts("\nMiniOS Shell Commands:\n\n");
	vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	vga_puts("  help          - Show this help\n");
	vga_puts("  clear         - Clear screen\n");
	vga_puts("  ls            - List files\n");
	vga_puts("  pwd           - Print working directory\n");
	vga_puts("  cd <path>     - Change directory\n");
	vga_puts("  mkdir <name>  - Create directory\n");
	vga_puts("  touch <file>  - Create file\n");
	vga_puts("  cat <file>    - Display file (press 'q' to exit)\n");
	vga_puts("  echo <text> > <file> - Write to file\n");
	vga_puts("  write <file>  - Edit file (Ctrl+S save, Ctrl+Q exit)\n");
	vga_puts("  rm <name>     - Remove file/dir\n");
	vga_puts("  tree          - Show directory tree\n");
	vga_puts("  info          - System information\n");
	vga_puts("  reboot        - Reboot system\n\n");
}

static void cmd_ls(void)
{
	inode_t *dir = fs_get_cwd();

	if (dir->child_count == 0) {
		vga_puts("(empty)\n");
		return;
	}

	for (int i = 0; i < dir->child_count; i++) {
		inode_t *child = dir->children[i];

		if (child->type == INODE_DIR) {
			vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
			vga_puts(child->name);
			vga_puts("/\n");
		} else {
			vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
			vga_puts(child->name);
			vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
			vga_puts(" (");
			vga_print_int(child->size);
			vga_puts(" bytes)\n");
		}
	}
	vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

static void cmd_pwd(void)
{
	char path[MAX_PATH];
	fs_get_path(fs_get_cwd(), path);
	vga_puts(path);
	vga_putch('\n');
}

static void cmd_cd(const char *path)
{
	if (strcmp(path, "..") == 0) {
		inode_t *parent = fs_get_cwd()->parent;
		if (parent)
			fs_set_cwd(parent);
		return;
	}

	if (strcmp(path, "/") == 0) {
		fs_set_cwd(fs_get_root());
		return;
	}

	inode_t *dir = fs_find_child(fs_get_cwd(), path);
	if (!dir) {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("cd: no such directory\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		return;
	}

	if (dir->type != INODE_DIR) {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("cd: not a directory\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		return;
	}

	fs_set_cwd(dir);
}

static void cmd_mkdir(const char *name)
{
	if (!name || name[0] == '\0') {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("mkdir: missing name\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		return;
	}

	inode_t *dir = fs_create_dir(fs_get_cwd(), name);
	if (!dir) {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("mkdir: cannot create directory\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	}
}

static void cmd_touch(const char *name)
{
	if (!name || name[0] == '\0') {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("touch: missing name\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		return;
	}

	inode_t *file = fs_create_file(fs_get_cwd(), name);
	if (!file) {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("touch: cannot create file\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	}
}

static void cmd_cat(const char *name)
{
	if (!name || name[0] == '\0') {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("cat: missing filename\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		return;
	}

	inode_t *file = fs_find_child(fs_get_cwd(), name);
	if (!file) {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("cat: file not found\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		return;
	}

	if (file->type != INODE_FILE) {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("cat: is a directory\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		return;
	}

	vga_putch('\n');
	vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
	for (uint32_t i = 0; i < file->size; i++) {
		vga_putch(file->data[i]);
	}
	if (file->size > 0 && file->data[file->size - 1] != '\n') {
		vga_putch('\n');
	}
	vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
	vga_puts("\n[Press 'q' to exit]\n");
	vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

	while (1) {
		char c = keyboard_getchar();
		if (c == 'q' || c == 'Q')
			break;
	}
}

static void cmd_echo(const char *args)
{
	char text[256];
	char filename[MAX_FILENAME];
	int i = 0, j = 0;

	while (args[i] && args[i] != '>') {
		if (args[i] != ' ' || j > 0) {
			text[j++] = args[i];
		}
		i++;
	}
	while (j > 0 && text[j - 1] == ' ')
		j--;
	text[j] = '\0';

	if (args[i] != '>') {
		vga_puts(text);
		vga_putch('\n');
		return;
	}

	i++;
	while (args[i] == ' ')
		i++;
	j = 0;
	while (args[i] && j < MAX_FILENAME - 1) {
		filename[j++] = args[i++];
	}
	filename[j] = '\0';

	if (filename[0] == '\0') {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("echo: missing filename\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		return;
	}

	inode_t *file = fs_find_child(fs_get_cwd(), filename);
	if (!file) {
		file = fs_create_file(fs_get_cwd(), filename);
	}

	if (!file) {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("echo: cannot write\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		return;
	}

	fs_write_file(file, text, strlen(text));
}

static void cmd_write(const char *name)
{
	if (!name || name[0] == '\0') {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("write: missing filename\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		return;
	}

	inode_t *file = fs_find_child(fs_get_cwd(), name);
	if (!file) {
		file = fs_create_file(fs_get_cwd(), name);
	}

	if (!file || file->type != INODE_FILE) {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("write: cannot create file\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		return;
	}

	vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
	vga_puts("\n======== Vim-like Editor ========\n");
	vga_puts("File: ");
	vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
	vga_puts(name);
	vga_putch('\n');
	vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
	vga_puts("Commands:\n");
	vga_puts("  Ctrl+S  - Save file\n");
	vga_puts("  Ctrl+Q  - Quit without saving\n");
	vga_puts("=================================\n\n");
	vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);

	char buffer[MAX_FILE_SIZE];
	int pos = 0;

	// Load existing content if any
	if (file->size > 0) {
		for (uint32_t i = 0; i < file->size && i < MAX_FILE_SIZE - 1;
		     i++) {
			buffer[pos++] = file->data[i];
			vga_putch(file->data[i]);
		}
	}

	while (pos < MAX_FILE_SIZE - 1) {
		char c = keyboard_getchar();

		// Ctrl+S - Save
		if (c == 19) { // Ctrl+S
			buffer[pos] = '\0';
			fs_write_file(file, buffer, pos);
			vga_putch('\n');
			vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
			vga_puts("\n[File saved! ");
			vga_print_int(pos);
			vga_puts(" bytes]\n\n");
			vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
			return;
		}

		// Ctrl+Q - Quit without saving
		if (c == 17) { // Ctrl+Q
			vga_putch('\n');
			vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
			vga_puts("\n[Quit without saving]\n\n");
			vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
			return;
		}

		if (c == '\n') {
			buffer[pos++] = '\n';
			vga_putch('\n');
			continue;
		}

		if (c == '\b') {
			if (pos > 0) {
				pos--;
				if (buffer[pos] == '\n') {
					int row = vga_get_cursor_row();
					int col = 0;
					int temp = pos - 1;
					while (temp >= 0 &&
					       buffer[temp] != '\n') {
						col++;
						temp--;
					}
					if (row > 0) {
						vga_set_cursor(row - 1, col);
						vga_clear_eol();
					}
				} else {
					vga_putch('\b');
				}
			}
			continue;
		}

		if (c >= 32 && c <= 126) {
			buffer[pos++] = c;
			vga_putch(c);
		}
	}

	vga_putch('\n');
	vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
	vga_puts("\n[Buffer full!]\n\n");
	vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

static void cmd_rm(const char *name)
{
	if (!name || name[0] == '\0') {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("rm: missing name\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		return;
	}

	if (fs_delete(fs_get_cwd(), name) != 0) {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts("rm: cannot remove\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	}
}

static void tree_recursive(inode_t *node, int depth)
{
	for (int i = 0; i < depth; i++) {
		vga_puts("  ");
	}

	if (node->type == INODE_DIR) {
		vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
		vga_puts(node->name);
		vga_puts("/\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

		for (int i = 0; i < node->child_count; i++) {
			tree_recursive(node->children[i], depth + 1);
		}
	} else {
		vga_puts(node->name);
		vga_putch('\n');
	}
}

static void cmd_tree(void)
{
	tree_recursive(fs_get_root(), 0);
}

static void cmd_info(void)
{
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("\n=== MiniOS System Information ===\n\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts("OS Name:      MiniOS\n");
    vga_puts("Version:      1.0\n");
    vga_puts("Architecture: x86 (32-bit)\n");
    vga_puts("Memory:       64 MB\n");
    vga_puts("Filesystem:   In-memory\n");
    vga_puts("Display:      VGA Text Mode (80x25)\n");
    vga_puts("Author:       Davanico (GitHub: danko1122)\n\n");
}

static void cmd_reboot(void)
{
	vga_puts("Rebooting...\n");
	uint8_t temp;
	__asm__ volatile("cli");
	do {
		temp = inb(0x64);
		if (temp & 1)
			inb(0x60);
	} while (temp & 2);
	outb(0x64, 0xFE);
	for (;;)
		__asm__ volatile("hlt");
}

static void parse_and_execute(const char *cmd)
{
	if (cmd[0] == '\0')
		return;

	char command[32];
	char args[CMD_BUFFER_SIZE];
	int i = 0, j = 0;

	while (cmd[i] == ' ')
		i++;

	while (cmd[i] && cmd[i] != ' ' && j < 31) {
		command[j++] = cmd[i++];
	}
	command[j] = '\0';

	while (cmd[i] == ' ')
		i++;
	j = 0;
	while (cmd[i]) {
		args[j++] = cmd[i++];
	}
	args[j] = '\0';

	if (strcmp(command, "help") == 0) {
		print_help();
	} else if (strcmp(command, "clear") == 0) {
		vga_clear();
		show_welcome();
	} else if (strcmp(command, "ls") == 0) {
		cmd_ls();
	} else if (strcmp(command, "pwd") == 0) {
		cmd_pwd();
	} else if (strcmp(command, "cd") == 0) {
		cmd_cd(args);
	} else if (strcmp(command, "mkdir") == 0) {
		cmd_mkdir(args);
	} else if (strcmp(command, "touch") == 0) {
		cmd_touch(args);
	} else if (strcmp(command, "cat") == 0) {
		cmd_cat(args);
	} else if (strcmp(command, "echo") == 0) {
		cmd_echo(args);
	} else if (strcmp(command, "write") == 0) {
		cmd_write(args);
	} else if (strcmp(command, "rm") == 0) {
		cmd_rm(args);
	} else if (strcmp(command, "tree") == 0) {
		cmd_tree();
	} else if (strcmp(command, "info") == 0) {
		cmd_info();
	} else if (strcmp(command, "reboot") == 0) {
		cmd_reboot();
	} else {
		vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
		vga_puts(command);
		vga_puts(": command not found\n");
		vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	}
}

void shell_init(void)
{
	show_welcome();
}

void shell_run(void)
{
	while (1) {
		print_prompt();
		keyboard_readline(cmd_buffer, CMD_BUFFER_SIZE);
		parse_and_execute(cmd_buffer);
	}
}
