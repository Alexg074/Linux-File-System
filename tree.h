#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define FAIL_RMREC "rmrec: failed to remove '%s': No such file or directory"
#define FAIL_MKDIR "mkdir: cannot create directory '%s': File exists"
#define FAIL_CD "cd: no such file or directory: %s"
#define FAIL_LS "ls: cannot access '%s': No such file or directory"
#define FAIL_RM_TYPE "rm: cannot remove '%s': Is a directory"
#define FAIL_RM "rm: failed to remove '%s': No such file or directory"
#define FAIL_RMDIR_1 "rmdir: failed to remove '%s': No such file or directory"
#define FAIL_RMDIR_2 "rmdir: failed to remove '%s': Directory not empty"
#define FAIL_RMDIR_3 "rmdir: failed to remove '%s': Not a directory"
#define FAIL_TREE_PATH "%s [error opening dir]\n\n0 directories, 0 files\n"
#define FAIL_CP_SOURCE_PATH "cp: -r not specified; omitting directory '%s'"
#define FAIL_CP_DEST_PATH "cp: failed to access '%s': Not a directory"
#define FAIL_MV_SOURCE_PATH "mv: failed to access '%s': Not a directory"
#define FAIL_MV_DEST_PATH "mv: failed to access '%s': Not a directory"

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ", __FILE__,		\
				__LINE__);				\
			perror(call_description);			\
			exit(errno);					\
		}							\
	} while (0)

typedef struct FileContent FileContent;
typedef struct FolderContent FolderContent;
typedef struct TreeNode TreeNode;
typedef struct FileTree FileTree;
typedef struct ListNode ListNode;
typedef struct List List;

enum TreeNodeType {
    FILE_NODE,
    FOLDER_NODE
};

struct FileContent {
    char* text;
};

struct FolderContent {
    List* children;
};

struct TreeNode {
    TreeNode* parent;
    char* name;
    enum TreeNodeType type;
    void* content;
};

struct FileTree {
    TreeNode* root;
};

struct ListNode {
    TreeNode* info;
    ListNode* next;
};

struct List {
    ListNode* head;
    int data_size;
    int size;
};

// functii tree
void ls(TreeNode* currentNode, char* arg);
void pwd(TreeNode* treeNode);
TreeNode* cd(TreeNode* currentNode, char* path);
void tree(TreeNode* currentNode, char* arg);
void print_tree(TreeNode *node, int tabs, int *files, int *folders);
void mkdir(TreeNode* currentNode, char* folderName);
void rm(TreeNode* currentNode, char* fileName);
void rmdir(TreeNode* currentNode, char* folderName);
void rmrec(TreeNode* currentNode, char* resourceName);
void touch(TreeNode* currentNode, char* fileName, char* fileContent);
void cp(TreeNode* currentNode, char* source, char* destination);
void mv(TreeNode* currentNode, char* source, char* destination);
FileTree createFileTree();
void freeTree(FileTree fileTree);

// functii standard liste
List* ll_create(int data_size);
ListNode* ll_get_nth_node(List* list, int n);
ListNode* create_node(void* new_data, int data_size);
void ll_add_nth_node(List* list, int n, void* new_data);
ListNode* ll_remove_nth_node(List* list, int n);
int ll_get_size(List* list);
void ll_free(List** pp_list);
