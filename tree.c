#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."


FileTree createFileTree(char* rootFolderName) {
    FileTree tree;
    tree.root = malloc(sizeof(TreeNode));
    DIE(!tree.root, "malloc failed for tree.root\n");

    // root are parinte NULL
    tree.root->parent = NULL;

    // salvam pointer ul la root name (numele fiind deja alocat prin strdup)
    tree.root->name = rootFolderName;

    tree.root->type = FOLDER_NODE;
    tree.root->content = ll_create(sizeof(TreeNode));

    return tree;
}

void freeTree(FileTree fileTree) {
    TreeNode* root = fileTree.root;
    List* ll = (List*)root->content;

    // folosim rmrec ca sa eliberam fiecare director si fisier din ierarhie
    while (ll->size) {
        ListNode* aux = ll->head;
        rmrec(root, aux->info->name);
    }
    free(root->name);
    ll_free(&ll);
    free(root);
}


void ls(TreeNode* currentNode, char* arg) {
    List *ll = (List*)currentNode->content;
    ListNode *aux = ll->head;

    // daca nu exista parametru
    if (arg[0] == 0) {
        while (aux) {
            TreeNode *resource = aux->info;
            printf("%s\n", resource->name);
            aux = aux->next;
        }

    } else {
        while (aux) {
            TreeNode *resource = aux->info;
            if (strcmp(resource->name, arg) == 0) {
                // daca e fisier
                if (resource->type == 0) {
                    printf("%s: %s\n",
                           resource->name, (char*)resource->content);
                    return;
                // daca e director
                } else {
                    List *ll_content = (List *)(resource->content);
                    ListNode *aux_1 = ll_content->head;

                    while (aux_1) {
                        TreeNode *resource_1 = aux_1->info;
                        printf("%s\n", resource_1->name);
                        aux_1 = aux_1->next;
                    }
                    return;
                }
            }
        aux = aux->next;
        }

        // daca iese din while inseamna ca nu am gasit resursa
        printf(FAIL_LS, arg);
    }
}

void pwd(TreeNode* treeNode) {
    char *path = malloc(strlen(treeNode->name) + 2);
    strcpy(path, "/");
    strcat(path, treeNode->name);

    TreeNode *move = treeNode;
    // cat timp nu ajungem la root, ne construim in path ca parametrul
    // al functiei cd
    while (move->parent) {
        move = move->parent;

        char *p_cop = malloc(strlen(path) + 1 + strlen(move->name) + 1);
        strcpy(p_cop, "/");
        strcat(p_cop, move->name);
        strcat(p_cop, path);

        path = realloc(path, strlen(path) + 1 + strlen(move->name) + 1);
        strcpy(path, p_cop);
        free(p_cop);
    }

    printf("%s", path + 1);

    free(path);
}

TreeNode* cd(TreeNode* currentNode, char* path) {
    char* path_cp = malloc(strlen(path) + 1);
    DIE(!path_cp, "malloc failed for path_cp\n");

    strcpy(path_cp, path);
    TreeNode* move = currentNode;

    char *token = strtok(path, "/");
    // token va lua valoarea fiecarui director despartit de / din path pe rand
    // ne folosim de move pentru a accesa path

    while (token) {
        // cd pentru a urca in ierarhie
        if (strcmp(token, "..") == 0) {
            // daca move e root
            if (move->parent == NULL) {
               printf(FAIL_CD, path_cp);
               free(path_cp);
               return currentNode;
            } else {
                move = move->parent;
            }
        // cd pentru a cobori in ierarhie
        } else {
            List *ll = (List*)move->content;
            ListNode *aux = ll->head;

            int ok = 0;
            while (aux) {
                TreeNode *resource = aux->info;
                if (strcmp(resource->name, token) == 0
                    && resource->type == 1) {
                    move = resource;
                    ok = 1;
                    aux = NULL;
                }
                if (aux)
                    aux = aux->next;
            }
            // daca nu gasim directorul catre care indica path
            if (ok == 0) {
                printf(FAIL_CD, path_cp);
                free(path_cp);
                return currentNode;
            }
       }
       token = strtok(NULL, "/");
    }
    free(path_cp);
    return move;
}

void tree(TreeNode* currentNode, char* arg) {
    int nr_dir = 0;
    int nr_files = 0;

    TreeNode* move = currentNode;

    // ne folosim de move pentru a accesa arg, daca exista
    if (arg[0] != 0) {
        char* path_cp = malloc(strlen(arg) + 1);
        DIE(!path_cp, "malloc failed for path_cp\n");

        strcpy(path_cp, arg);
        char *token = strtok(arg, "/");
        while (token) {
            // pentru a urca in ierarhie
            if (strcmp(token, "..") == 0) {
                if (move->parent == NULL) {
                    printf(FAIL_TREE_PATH, path_cp);
                    free(path_cp);
                    return;
                } else {
                    move = move->parent;
                }
            // pentru a cobori in ierarhie
            } else {
                List *ll = (List*)move->content;
                ListNode *aux = ll->head;

                int ok = 0;
                while (aux) {
                    TreeNode *resource = aux->info;
                    if (strcmp(resource->name, token) == 0
                        && resource->type == 1) {
                        move = resource;
                        ok = 1;
                        aux = NULL;
                    }
                    if (aux)
                        aux = aux->next;
                }
                if (ok == 0) {
                    printf(FAIL_TREE_PATH, path_cp);
                    free(path_cp);
                    return;
                }
            }
            token = strtok(NULL, "/");
        }
        free(path_cp);
    }

    // dupa ce am setat nodul de la care afisam (move) incepem si afisam tree
    List *ll = (List*)move->content;
    ListNode *aux = ll->head;

    int tabs = 0;

    int *files = malloc(sizeof(int));
    DIE(!files, "malloc failed for files\n");

    int *folders = malloc(sizeof(int));
    DIE(!folders, "malloc failed for folders\n");

    // int pointer pentru a contoriza apeland recursiv print_tree
    *files = 0;
    *folders = 0;

    while (aux != NULL) {
        TreeNode *resource = aux->info;
        print_tree(resource, tabs, files, folders);
        aux = aux->next;
    }

    printf("%d directories, %d files\n", *folders, *files);
    free(files);
    free(folders);
}

void print_tree(TreeNode *node, int tabs, int *files, int *folders) {
    // afisam recursim in functie de tipul resursei (fisier sau director)

    // ne dolosim de variabila tabs pentru a tine cond de cat de jos
    // suntem in ierarhie
    int cop = tabs;

    // daca e fisier, nu mai apelam recusiv
    if (node->type == 0) {
        (*files)++;
        while (cop) {
            printf("\t");
            cop--;
        }
        printf("%s\n", node->name);
        return;
    // daca e director, apelam recursiv doar daca nu e gol
    } else {
        (*folders)++;
        while (cop) {
            printf("\t");
            cop--;
        }
        printf("%s\n", node->name);

        List *ll = (List*)node->content;
        ListNode *aux = ll->head;

        while (aux != NULL) {
            print_tree(aux->info, tabs + 1, files, folders);
            aux = aux->next;
        }
        return;
    }
}


void mkdir(TreeNode* currentNode, char* folderName) {
    TreeNode *folder = malloc(sizeof(TreeNode));
    DIE(!folder, "malloc failed for folder\n");

    // initializam campurile
    folder->parent = currentNode;
    folder->name = folderName;
    folder->content = ll_create(sizeof(TreeNode));
    folder->type = FOLDER_NODE;

    List *ll = (List*)currentNode->content;
    ListNode *aux = ll->head;

    // ne asiguram ca nu mai exista inca un director sau fisier
    // cu acelasi nume in cadrul directorului parinte
    while (aux) {
        TreeNode *resource = aux->info;
        if (strcmp(resource->name, folderName) == 0) {
            printf(FAIL_MKDIR, folderName);

            List *ll_content = (List*)folder->content;

            ll_free(&ll_content);
            free(folder->name);
            free(folder);
            return;
        }
        aux = aux->next;
    }
    // adaugam noul director pe pozitia 0 in lista directorului parinte
    ll_add_nth_node(ll, 0, folder);
}


void rmrec(TreeNode* currentNode, char* resourceName) {
    List *ll = (List *)(currentNode->content);
    ListNode *aux = ll->head;

    // iteram prinn lista de resurse din folder ul curent
    int i = 0;
    while (aux) {
        TreeNode *resource = aux->info;
        if (strcmp(resource->name, resourceName) == 0) {
            // daca e fisier, il eliberam
            if (resource->type == 0) {
                free(resource->name);
                free(resource->content);

                // stergem nodul removed = aux si restabilim leg
                ListNode *removed = ll_remove_nth_node(ll, i);
                free(removed->info);
                free(removed);
                return;
            } else {
                // daca e director
                List *ll_content = (List *)(resource->content);

                // eliberam recursiv tot din director
                while (ll_content->size) {
                    ListNode *aux_1 = ll_content->head;
                    rmrec(resource, aux_1->info->name);
                }

                // cand list size = NULL, director gol = lista goala,
                // deci il eliberam
                free(resource->name);
                ll_free(&ll_content);

                ListNode *removed = ll_remove_nth_node(ll, i);
                free(removed->info);
                free(removed);
                return;
            }
        }
        i++;
        aux = aux->next;
    }

    printf(FAIL_RMREC, resourceName);
}

void rm(TreeNode* currentNode, char* fileName) {
    List *ll = (List*)currentNode->content;
    ListNode *aux = ll->head;

    int i = 0;
    // cautam fisierul pe care vrem sa il stergem in
    // lista de resurse a directorului parinte
    while (aux) {
        TreeNode *resource = aux->info;
        if (strcmp(resource->name, fileName) == 0) {
            if (resource->type == 0) {
                free(resource->name);
                free(resource->content);

                ListNode *removed = ll_remove_nth_node(ll, i);
                free(removed->info);
                free(removed);

                return;
            } else {
                // daca nu e fisier
                printf(FAIL_RM_TYPE, fileName);
                return;
            }
        }
        i++;
        aux = aux->next;
    }
    // daca fisierul pe care vrem sa il stergem nu exista
    printf(FAIL_RM, fileName);
}

void rmdir(TreeNode* currentNode, char* folderName) {
    List *ll = (List*)currentNode->content;
    ListNode *aux = ll->head;

    int i = 0;
    // cautam directorul pe care vrem sa il stergem in
    // lista de resurse a directorului parinte
    while (aux) {
        TreeNode *resource = aux->info;

        if (strcmp(resource->name, folderName) == 0) {
            if (resource->type == 1) {
                List *ll_content = (List *)(resource->content);

                if (ll_content->size == 0) {
                    free(resource->name);
                    ll_free(&ll_content);

                    ListNode *removed = ll_remove_nth_node(ll, i);
                    free(removed->info);
                    free(removed);
                    return;
                } else {
                    // daca directorul nu e gol
                    printf(FAIL_RMDIR_2, folderName);
                    return;
                }
            } else {
                // daca nu e director
                printf(FAIL_RMDIR_3, folderName);
                return;
            }
        }
        i++;
        aux = aux->next;
    }
    // daca directorul pe care vrem sa il stergem nu exista
    printf(FAIL_RMDIR_1, folderName);
}


void touch(TreeNode* currentNode, char* fileName, char* fileContent) {
    TreeNode *file = malloc(sizeof(TreeNode));
    DIE(!file, "malloc failed for file\n");

    file->parent = currentNode;
    file->name = fileName;
    file->content = fileContent;
    file->type = FILE_NODE;

    List *ll = (List*)currentNode->content;
    ListNode *aux = ll->head;

    // ne asiguram ca nu mai exista inca un director sau fisier
    // cu acelasi nume in cadrul directorului parinte
    while (aux) {
        TreeNode *resource = aux->info;
        if (strcmp(resource->name, fileName) == 0) {
            free(file->content);
            free(file->name);
            free(file);
            return;
        }
        aux = aux->next;
    }
    // adaugam noul fisier pe pozitia 0 in lista directorului parinte
    ll_add_nth_node(ll, 0, file);
}

void cp(TreeNode* currentNode, char* source, char* destination) {
    int nr_path = 0;
    TreeNode* move = currentNode;

    char* path_cp = malloc(strlen(source) + 1);
    DIE(!path_cp, "malloc failed for path_cp\n");

    char* path_cp_2 = malloc(strlen(source) + 1);
    DIE(!path_cp_2, "malloc failed for path_cp_2\n");

    strcpy(path_cp, source);
    strcpy(path_cp_2, source);

    char *token = strtok(source, "/");

    while (token) {
        token = strtok(NULL, "/");
        nr_path++;
    }
    token = strtok(path_cp, "/");

    // token va lua valoarea fiecarui director despartit de / din path-ul sursa
    // nr_path va contine numarul de directoare despartite de / din sursa
    // ne folosim de move pentru a accesa source

    while (token) {
        // pentru a urca in ierarhie
        if (strcmp(token, "..") == 0) {
            if (move->parent == NULL) {
                printf(FAIL_CP_SOURCE_PATH, path_cp_2);
                free(path_cp);
                free(path_cp_2);
                return;
            } else {
                move = move->parent;
            }
        // pentru a cobori in ierarhie
        } else {
            List *ll = (List*)move->content;
            ListNode *aux = ll->head;

            int ok = 0;
            while (aux) {
                // ultima chestie din path trebuie sa fie fisier
                if (nr_path == 1) {
                    TreeNode *resource = aux->info;
                        if (strcmp(resource->name, token) == 0
                            && resource->type == 0) {
                            move = resource;
                            ok = 1;
                            // iesim din while daca am gasit resursa cautata
                            aux = NULL;
                    }
                    if (aux)
                        aux = aux->next;
                } else {
                    TreeNode *resource = aux->info;
                    if (strcmp(resource->name, token) == 0
                        && resource->type == 1) {
                        move = resource;
                        ok = 1;
                        // iesim din while daca am gasit resursa cautata
                        aux = NULL;
                    }
                    if (aux)
                        aux = aux->next;
                }
            }
            if (ok == 0) {
                printf(FAIL_CP_SOURCE_PATH, path_cp_2);
                free(path_cp);
                free(path_cp_2);
                return;
            }
        }
        // actualizam nr_path pentru cate mai avem
        nr_path--;
        token = strtok(NULL, "/");
    }

    free(path_cp);
    free(path_cp_2);

    // stim sigur ca sursa path ului e fisier
    TreeNode *node_source = move;
    nr_path = 0;
    move = currentNode;

    path_cp = malloc(strlen(destination) + 1);
    DIE(!path_cp, "malloc failed for path_cp\n");

    path_cp_2 = malloc(strlen(destination) + 1);
    DIE(!path_cp_2, "malloc failed for path_cp_2\n");

    strcpy(path_cp, destination);
    strcpy(path_cp_2, destination);

    token = strtok(destination, "/");
    while (token) {
        token = strtok(NULL, "/");
        nr_path++;
    }
    token = strtok(path_cp, "/");

    // token va lua valoarea fiecarui director despartit de / din path-ul
    // destinatie, iar la ultimul va iesi din while
    // nr_path va contine numarul de directoare despartite de / din destinatie

    // ne folosim de move pentru a accesa destinatia

    while (nr_path > 1) {
        // ne mutam in functie de destinatie pana la penultima chestie
        if (strcmp(token, "..") == 0) {
            if (move->parent == NULL) {
                printf(FAIL_CP_DEST_PATH, path_cp_2);
                free(path_cp);
                free(path_cp_2);
                return;
            } else {
                move = move->parent;
            }
        } else {
            List *ll = (List*)move->content;
            ListNode *aux = ll->head;

            int ok = 0;
            while (aux) {
                TreeNode *resource = aux->info;
                if (strcmp(resource->name, token) == 0
                    && resource->type == 1) {
                    move = resource;
                    ok = 1;
                    // iesim din while daca am gasit resursa cautata
                    aux = NULL;
                }
                if (aux)
                    aux = aux->next;
            }
            if (ok == 0) {
                printf(FAIL_CP_DEST_PATH, path_cp_2);
                free(path_cp);
                free(path_cp_2);
                return;
            }
        }
        nr_path--;
        token = strtok(NULL, "/");
    }

    // am ajuns la ultima chestie din path si verificam daca este
    // "..", fisier sau director

    if (strcmp(token, "..") == 0) {
        if (move->parent == NULL) {
            printf(FAIL_CP_DEST_PATH, path_cp_2);
            free(path_cp);
            free(path_cp_2);
            return;
        } else {
            // cream in parinte un nou fisier acelasi nume si continut
            // ca cel din sursa
            move = move->parent;
            char* srs_content = (char*)node_source->content;
            touch(move, strdup(node_source->name), strdup(srs_content));
        }

    // daca ultima chestie din path e un nume
    } else {
        List *ll = (List*)move->content;
        ListNode *aux = ll->head;

        int ok = 0;
        while (aux) {
            TreeNode *resource = aux->info;
            if (strcmp(resource->name, token) == 0) {
                move = resource;
                ok = 1;
                // iesim din while daca am gasit resursa cautata
                aux = NULL;
            }
            if (aux)
                aux = aux->next;
        }
        // daca n-am gasit resursa (ultima chestie din path)
        if (!ok) {
            char* srs_content = (char*)node_source->content;
            touch(move, strdup(token), strdup(srs_content));
        } else {
            // daca am gasit-o si e fisier
            if (move->type == 0) {
                char* srs_content = (char*)node_source->content;
                char* dest_content = (char*)move->content;

                dest_content = realloc(dest_content, strlen(srs_content) + 1);
                DIE(!dest_content, "realloc failed for dest_content\n");

                strcpy(dest_content, srs_content);
            } else {
                // daca am gasit-o si e director
                char* srs_content = (char*)node_source->content;
                touch(move, strdup(node_source->name), strdup(srs_content));
            }
        }
    }

    free(path_cp);
    free(path_cp_2);
}


void mv(TreeNode* currentNode, char* source, char* destination) {
    // ne mutam pe source si destinatin exact cum am facut in functia cp
    int nr_path = 0;
    TreeNode* move = currentNode;

    char* path_cp = malloc(strlen(source) + 1);
    DIE(!path_cp, "malloc failed for path_cp\n");
    strcpy(path_cp, source);

    char* path_cp_2 = malloc(strlen(source) + 1);
    DIE(!path_cp_2, "malloc failed for path_cp_2\n");
    strcpy(path_cp_2, source);

    char *token = strtok(source, "/");
    while (token) {
        token = strtok(NULL, "/");
        nr_path++;
    }
    token = strtok(path_cp, "/");

    while (token) {
        // urcam in ierarhie
        if (strcmp(token, "..") == 0) {
            if (move->parent == NULL) {
                printf(FAIL_MV_SOURCE_PATH, path_cp_2);
                free(path_cp);
                free(path_cp_2);
                return;
            } else {
                move = move->parent;
            }
        // coboram in ierarhie
        } else {
            List *ll = (List*)move->content;
            ListNode *aux = ll->head;

            int ok = 0;
            while (aux) {
                if (nr_path == 1) {
                    TreeNode *resource = aux->info;
                        if (strcmp(resource->name, token) == 0) {
                            move = resource;
                            ok = 1;
                            aux = NULL;
                    }
                    if (aux)
                        aux = aux->next;
                } else {
                    TreeNode *resource = aux->info;
                    if (strcmp(resource->name, token) == 0
                        && resource->type == 1) {
                        move = resource;
                        ok = 1;
                        // iesim din while daca am gasit resursa cautata
                        aux = NULL;
                    }
                    if (aux)
                        aux = aux->next;
                }
            }
            if (ok == 0) {
                printf(FAIL_MV_SOURCE_PATH, path_cp_2);
                free(path_cp);
                free(path_cp_2);
                return;
            }
        }
        nr_path--;
        token = strtok(NULL, "/");
    }

    free(path_cp);
    free(path_cp_2);

    TreeNode *node_source = move;
    nr_path = 0;

    move = currentNode;
    path_cp = malloc(strlen(destination) + 1);
    DIE(!path_cp, "malloc failed for path_cp\n");

    path_cp_2 = malloc(strlen(destination) + 1);
    DIE(!path_cp_2, "malloc failed for path_cp_2\n");

    strcpy(path_cp, destination);
    strcpy(path_cp_2, destination);

    token = strtok(destination, "/");
    while (token) {
        token = strtok(NULL, "/");
        nr_path++;
    }
    token = strtok(path_cp, "/");

    // ajungem la ultima chestie din path
    while (nr_path > 1) {
        // urcam in ierarhie
        if (strcmp(token, "..") == 0) {
            if (move->parent == NULL) {
                printf(FAIL_MV_DEST_PATH, path_cp_2);
                free(path_cp);
                free(path_cp_2);
                return;
            } else {
                move = move->parent;
            }
        // daca nu e ".." , ne mutam pe calea dest
        } else {
            List *ll = (List*)move->content;
            ListNode *aux = ll->head;

            int ok = 0;
            while (aux) {
                TreeNode *resource = aux->info;
                if (strcmp(resource->name, token) == 0
                    && resource->type == 1) {
                    move = resource;
                    ok = 1;
                    // iesim din while daca am gasit resursa cautata
                    aux = NULL;
                }
                if (aux)
                    aux = aux->next;
            }
            if (ok == 0) {
                printf(FAIL_MV_DEST_PATH, path_cp_2);
                free(path_cp);
                free(path_cp_2);
                return;
            }
        }
        nr_path--;
        token = strtok(NULL, "/");
    }

    if (strcmp(token, "..") == 0) {
        if (move->parent == NULL) {
            printf(FAIL_MV_DEST_PATH, path_cp_2);
            free(path_cp);
            free(path_cp_2);
            return;
        } else {
            // daca node_source e folder => mutam lista in directorulul parinte
            move = move->parent;

            if (node_source->type == 1) {
                char *name_mv = malloc(strlen(node_source->name) + 1);
                DIE(!name_mv, "malloc failed for name_mv\n");
                strcpy(name_mv, node_source->name);

                // ll_mv contine content ul din resursa cautata
                List *ll_mv = (List*)node_source->content;
                List *buff = ll_create(sizeof(TreeNode));

                node_source->content = buff;
                node_source = node_source->parent;

                rmdir(node_source, name_mv);
                mkdir(move, name_mv);

                // cautam directorul proaspat creat in
                // lista directorului parinte
                List *ll = (List *) move->content;
                ListNode *aux = ll->head;

                while (aux) {
                    TreeNode *resource = aux->info;
                    if (strcmp(resource->name, name_mv) == 0) {
                        resource->content = ll_mv;

                        free(path_cp);
                        free(path_cp_2);
                        return;
                    }
                    aux = aux->next;
                }
            } else {
                // mutam un fisier abia creat
                char* srs_content = (char*)node_source->content;
                touch(move, strdup(node_source->name), strdup(srs_content));

                rm(node_source->parent, node_source->name);
            }
        }
    // daca token ul nostru nu e ":"
    } else {
        List *ll = (List*)move->content;
        ListNode *aux = ll->head;

        int ok = 0;
        while (aux) {
            TreeNode *resource = aux->info;
            if (strcmp(resource->name, token) == 0) {
                move = resource;
                ok = 1;
                // iesim din while daca am gasit resursa cautata
                aux = NULL;
            }
            if (aux)
                aux = aux->next;
        }
        // nu exista ultima chestie din destinatie => o cream in
        // functie de tipul ultimei chestii din sursa

        // token a ramas pe ultima chestie de dupa "/"" din destinatie
        if (!ok) {
            if (node_source->type == 1) {
                char *name_mv = node_source->name;

                // ll_mv contine content ul din resursa cautata
                List *ll_mv = (List*)node_source->content;
                List *buff = ll_create(sizeof(TreeNode));

                node_source->content = buff;
                node_source = node_source->parent;

                rmdir(node_source, name_mv);
                mkdir(move, strdup(token));

                // cautam directorul proaspat creat,
                // in lista directorului parinte
                List *ll = (List *) move->content;
                ListNode *aux = ll->head;

                while (aux) {
                    TreeNode *resource = aux->info;
                    if (strcmp(resource->name, name_mv) == 0) {
                        resource->content = ll_mv;

                        free(path_cp);
                        free(path_cp_2);
                        return;
                    }
                    aux = aux->next;
                }

            } else {
                // mutam fisierul, redenumindu-l
                char* srs_content = (char*)node_source->content;
                touch(move, strdup(token), strdup(srs_content));

                rm(node_source->parent, node_source->name);
            }

        } else {
            // ok = 1, deci am gasit destinatia
            // destinatia e fisier => sursa e neaparat fisier

            if (move->type == 0) {
                char* srs_content = (char*)node_source->content;
                char* dest_content = (char*)move->content;

                dest_content = realloc(dest_content, strlen(srs_content) + 1);
                DIE(!dest_content, "realloc failed for dest_content\n");
                strcpy(dest_content, srs_content);

                rm(node_source->parent, node_source->name);

            } else {
                // destinatia e director
                // sursa e director
                if (node_source->type == 1) {
                    char *name_mv = malloc(strlen(node_source->name) + 1);
                    DIE(!name_mv, "malloc failed for name_mv\n");
                    strcpy(name_mv, node_source->name);

                    // ll_mv contine content ul din resursa cautata
                    List *ll_mv = (List*)node_source->content;
                    List *buff = ll_create(sizeof(TreeNode));

                    node_source->content = buff;
                    node_source = node_source->parent;
                    rmdir(node_source, name_mv);
                    mkdir(move, name_mv);

                    // cautam directorul proaspat creat,
                    // in lista directorului parinte
                    List *ll = (List *) move->content;
                    ListNode *aux = ll->head;

                    while (aux) {
                        TreeNode *resource = aux->info;
                        if (strcmp(resource->name, name_mv) == 0) {
                            resource->content = ll_mv;
                            free(path_cp);
                            free(path_cp_2);
                            return;
                        }
                        aux = aux->next;
                    }
                } else {
                    // sursa e fisier si destinatia e director
                    char* srs_content = (char*)node_source->content;
                    touch(move, strdup(node_source->name), strdup(srs_content));
                    rm(node_source->parent, node_source->name);
                }
            }
        }
    }

    free(path_cp);
    free(path_cp_2);
}

List*
ll_create(int data_size)
{
	List* ll = calloc(1, sizeof(*ll));
    DIE(!ll, "calloc failed for ll\n");

	ll->head = NULL;
	ll->data_size = data_size;
    ll->size = 0;

	return ll;
}

ListNode*
ll_get_nth_node(List* list, int n)
{
	int len = list->size - 1;
	int i;
	ListNode* node = list->head;

	n = MIN(n, len);

	for (i = 0; i < n; ++i)
		node = node->next;

	return node;
}

ListNode*
create_node(void* new_data, int data_size)
{
	ListNode* node = calloc(1, sizeof(*node));
    DIE(!node, "calloc failed for node\n");

    node->info = new_data;

	return node;
}

void
ll_add_nth_node(List* list, int n, void* new_data)
{
	ListNode *new_node, *prev_node;

	if (!list)
		return;

	new_node = create_node(new_data, list->data_size);

	if (!n || !list->size) {
		new_node->next = list->head;
		list->head = new_node;
	} else {
		prev_node = ll_get_nth_node(list, n - 1);
		new_node->next = prev_node->next;
		prev_node->next = new_node;
	}

	++list->size;
}

ListNode*
ll_remove_nth_node(List* list, int n)
{
	ListNode *prev_node, *removed_node;

	if (!list || !list->size)
		return NULL;

	if (!n) {
		removed_node = list->head;
		list->head = removed_node->next;
		removed_node->next = NULL;
	} else {
		prev_node = ll_get_nth_node(list, n - 1);
		removed_node = prev_node->next;
		prev_node->next = removed_node->next;
		removed_node->next = NULL;
	}

	--list->size;

	return removed_node;
}

int
ll_get_size(List* list)
{
	return !list ? 0 : list->size;
}

void
ll_free(List** pp_list)
{
	ListNode* node;

	if (!pp_list || !*pp_list)
		return;

	while ((*pp_list)->size) {
		node = ll_remove_nth_node(*pp_list, 0);
		free(node->info);
		free(node);
	}

	free(*pp_list);
	*pp_list = NULL;
}
