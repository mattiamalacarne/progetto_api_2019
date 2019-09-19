#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_NAME_SIZE 32
#define REL_SIZE 256

#define FIRST_TRY "hdfsksbcffs"
#define NO_REL_DEFINED "none\n"
#define ARRAY_TERM_CHAR "terminated_array"
#define SPACE " "

// check rel exist
#define NOT_FOUND 0
#define FOUND 1

// Dati utili

typedef enum {false, true} bool;

// Strutture dati ----------------------------------------------

// Albero binario entità (<= a sinistra, > a destra)
typedef struct entity_node
{
    char name[MAX_NAME_SIZE];
    struct entity_node *left;
    struct entity_node *right;
    struct entity_node *parent;
} entity_node_t;

// Lista per le relazioni
// Costituita dal una lista che contiene tutti i nomi delle relazioni
// Una lista che contiene i from e to



// Lista ordinata relazioni (Ordine alfabetico tramite strcmp)
typedef struct rel_element
{
    struct rel_element *next; // Prossima relazione di questo tipo (con lo stesso nome)
    struct rel_element *prev; // Precedente relazione di questo tipo (NULL se è la prima)
    entity_node_t *from_entity;
} rel_element_t;

typedef struct rel_to
{
    int counter;
    struct rel_to *next;
    struct rel_to *prev;
    rel_element_t *from_list;
    entity_node_t *to_entity;
} rel_to_t;

typedef struct rel_name
{
    char name[REL_SIZE];
    struct rel_name *prev_rel;
    struct rel_name *next_rel;
    rel_to_t *rel_list;
} rel_name_t;

// Lista di entità in una relazione per controllare il massimo
typedef struct to_counter
{
    char name[MAX_NAME_SIZE];
    int counter;
    struct to_counter *next;
} entity_counter_t;

// Prototipi ---------------------------------------------------
//rel_element_t *find_exis_rel(char name[REL_SIZE], rel_element_t *list);
rel_name_t *find_exis_rel(char name[REL_SIZE], rel_name_t *list);

rel_element_t *destroy_element_rel(rel_element_t *to_destroy, rel_name_t *p_pointer, rel_name_t *head);

rel_name_t *del_entity_occ(entity_node_t *entity, rel_name_t *rel_list);


// Algoritmo di ordinamento per le liste di elementi

void *bubblesort_rel_name(rel_name_t *rel_name_list)
{

    rel_name_t *tmp = malloc(sizeof(rel_name_t));
    bool isOrder = true;

    while (isOrder)
    {
        isOrder = false;
        while (rel_name_list->next_rel != NULL)
        {
            rel_name_t *next = rel_name_list->next_rel;
            if (strcmp(rel_name_list->name, next->name) > 0)
            {
                // Scambio
                // Copio l'elemento puntato
                strcpy(tmp->name, rel_name_list->name);
                tmp->rel_list = rel_name_list->rel_list;

                strcpy(rel_name_list->name, next->name);
                rel_name_list->rel_list = next->rel_list;

                strcpy(next->name, tmp->name);
                next->rel_list = tmp->rel_list;

                isOrder = true;
            }
            rel_name_list = rel_name_list->next_rel;
        }
    }

}

void bubblesort_to_list(rel_to_t *to_list)
{
    rel_to_t *tmp = malloc(sizeof(rel_to_t));
    bool isOrder = true;
    while (isOrder)
    {
        isOrder = false;
        while (to_list->next != NULL)
        {
            rel_to_t *next = to_list->next;
            if (strcmp(to_list->to_entity->name, next->to_entity->name) > 0)
            {
                // Scambio come nel precedente algoritmo
                tmp->to_entity = to_list->to_entity;
                tmp->counter = to_list->counter;
                tmp->from_list = to_list->from_list;
                // Scambio con il prossimo
                to_list->to_entity = next->to_entity;
                to_list->counter = next->counter;
                to_list->from_list = next->from_list;
                // Metto tmp nel nuovo
                next->from_list = tmp->from_list;
                next->to_entity = tmp->to_entity;
                next->counter = tmp->counter;
                isOrder = true;
            }
            to_list = to_list->next;
        }
    }
}

// Entity ------------------------------------------------------

entity_node_t *create_new_node(char name[MAX_NAME_SIZE], entity_node_t *parent)
{
    entity_node_t *new_node = malloc(sizeof(entity_node_t));
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = parent;
    strcpy(new_node->name, name);

    return new_node;
}

/*entity_node_t *find_entity_in_tree(char name[MAX_NAME_SIZE], entity_node_t *tree_head)
{
    if (!strcmp(tree_head->name, FIRST_TRY)) return NULL;

    // Mi muovo nell'albero
    while (1)
    {
        if (!strcmp(tree_head->name, name)) return tree_head;

        if (strcmp(tree_head->name, name) < 0)
        {
            if (tree_head->left == NULL) return NULL;
            else (tree_head = tree_head->left);
        }

        if (strcmp(tree_head->name, name) > 0)
        {
            if (tree_head->right == NULL) return NULL;
            else (tree_head = tree_head->right);
        }
    }
}*/

entity_node_t *find_entity_in_tree(char name[MAX_NAME_SIZE], entity_node_t *tree_head)
{
    if ((tree_head == NULL) || !strcmp(tree_head->name, name)) return tree_head;
    if (!strcmp(tree_head->name, FIRST_TRY)) return NULL;
    if (strcmp(tree_head->name, name) < 0) return find_entity_in_tree(name, tree_head->left);
    else return find_entity_in_tree(name, tree_head->right);
}

/*
void addent(char name[MAX_NAME_SIZE], entity_node_t *T)
{
    entity_node_t *y = NULL;
    entity_node_t *x = T;
    entity_node_t *z = create_new_node(name, NULL);

    while (x != NULL)
    {
        y = x;
        if (!strcmp(z->name, x->name)) return;
        if (strcmp(z->name, x->name) < 0)
        {
            x = x->left;
        } else
        {
            x = x->right;
        }
    }

    z->parent = y;
    if (y == NULL)
    {
        T = z; // L'albero è vuoto;
    } else if (strcmp(z->name, y->name) < 0)
    {
        y->left = z;
    } else
    {
        y->right = z;
    }
}
 */

void addent(char name[MAX_NAME_SIZE], entity_node_t *tree_head)
{
    if (!strcmp(tree_head->name, FIRST_TRY))
    {
        // L'albero è vuoto quindi mi limito ad aggiungere la testa
        strcpy(tree_head->name, name);
        return;
    }
    while (1)
    {
        if (!strcmp(tree_head->name, name)) return; // L'entità è gia presente nell'albero

        // Vado a sinistra
        if (strcmp(tree_head->name, name) < 0)
        {
            if (tree_head->left == NULL)
            {
                tree_head->left = create_new_node(name, tree_head);
                return;
            }
            else
            {
                tree_head = tree_head->left;
            }
        }
        // Vado a destra
        if (strcmp(tree_head->name, name) > 0)
        {
            if (tree_head->right == NULL)
            {
                tree_head->right = create_new_node(name, tree_head);
                return;
            }
            else
            {
                tree_head = tree_head->right;
            }
        }


    }
}

// Relazioni --------------------------------------------------
rel_element_t *create_new_rel_element(entity_node_t *from, rel_element_t *prev, rel_element_t *next)
{
    rel_element_t *new = malloc(sizeof(rel_element_t));
    new->next = next;
    new->prev = prev;
    new->from_entity = from;

    return new;
}
rel_name_t *create_new_rel_name(char name[MAX_NAME_SIZE], rel_name_t *prev, rel_name_t *next)
{
    rel_name_t *new = malloc(sizeof(rel_name_t));
    new->next_rel = next;
    new->prev_rel = prev;
    new->rel_list = NULL;
    strcpy(new->name, name);

    return new;
}

rel_to_t *create_new_to_element(entity_node_t *to, rel_to_t *prev, rel_to_t *next)
{
    rel_to_t *new = malloc(sizeof(rel_to_t));
    new->counter = 0;
    new->from_list = NULL;
    new->next = next;
    new->prev = prev;
    new->to_entity = to;

    return new;
}

rel_to_t *find_to_node(entity_node_t *to, rel_to_t *rel_list)
{
    while (rel_list != NULL)
    {
        if (!strcmp(rel_list->to_entity->name, to->name)) return rel_list;
        rel_list = rel_list->next;
    }
    return NULL;
}

rel_name_t *add_new_rel_name(char name[REL_SIZE], rel_name_t *rel_list)
{
    // Inserimento in testa
    rel_list->prev_rel = create_new_rel_name(name, NULL, rel_list);
    rel_list = rel_list->prev_rel;
    return rel_list;
}

rel_to_t *add_new_to_node(entity_node_t *to, rel_to_t *to_list)
{
    if (to_list == NULL) return create_new_to_element(to, NULL, NULL);
    to_list->prev = create_new_to_element(to, NULL, to_list);
    return to_list->prev;
}

rel_element_t *add_new_element_node(entity_node_t *from, rel_element_t *from_list)
{
    if (from_list == NULL) return create_new_rel_element(from, NULL, NULL);
    from_list->prev = create_new_rel_element(from, NULL, from_list);
    return from_list->prev;
}

int is_rel_exist_from_entity(entity_node_t *from, entity_node_t *to, rel_to_t *to_list)
{
    rel_to_t *found_to_node = find_to_node(to, to_list);
    if (found_to_node == NULL) return 0;

    rel_element_t *from_list = found_to_node->from_list;
    while (from_list != NULL)
    {
        if (!strcmp(from_list->from_entity->name, from->name)) return 1;
        from_list = from_list->next;
    }
    return 0;
}

rel_name_t *addrel(char name[REL_SIZE], entity_node_t *from, entity_node_t *to, rel_name_t *list)
{


    /*
     *
     * Ad ogni aggiunta di una relazione in una lista to, bisogna ordinare SOLO quella lista
     * prima di terminare addrel si riordina la lista dei nomi SOLO se sono state fatte aggiunte alla lista stessa
     *
     */

    rel_name_t *head = list;
    // La lista è vuota quindi utilizzo il primo elemento
    if (!strcmp(list->name, FIRST_TRY))
    {
        strcpy(list->name, name);
        list->next_rel = NULL;
        list->prev_rel = NULL;
        list->rel_list = create_new_to_element(to, NULL, NULL);
        list->rel_list->from_list = create_new_rel_element(from, NULL, NULL);
        list->rel_list->counter++;
        return list;
    }

    // Controllo se la relazione è gia monitorata
    rel_name_t *rel_name_position = find_exis_rel(name, list);
    //rel_name_t *rel_name_head = list;
    if (rel_name_position == NULL)
    {
        // Non ho trovato la relazione, quindi la devo aggiungere
        head = add_new_rel_name(name, list);
        // Ho aggiunto la relazione in testa alla lista, ora aggiungo il nodo to;
        head->rel_list = add_new_to_node(to, head->rel_list);
        // Aggiunto il nodo to, devo aggiungere il nodo from
        head->rel_list->from_list = add_new_element_node(from, head->rel_list->from_list);
        head->rel_list->counter++;


        bubblesort_rel_name(head);

    } else
    {
        // La relazione è gia monitorata quindi devo controllare che non esista gia la relazione tra from e to
        if (is_rel_exist_from_entity(from, to, rel_name_position->rel_list) == 0)
        {
            // La relazione non esiste quindi devo crearla
            // Cerco il nodo to
            rel_to_t *to_position = find_to_node(to, rel_name_position->rel_list);
            if (to_position == NULL)
            {
                // Non ci sono relazioni che come destinatario hanno questa entità
                rel_name_position->rel_list = add_new_to_node(to, rel_name_position->rel_list);
                // Creato il nodo to aggiungo la relazione
                rel_name_position->rel_list->from_list = add_new_element_node(from, rel_name_position->rel_list->from_list);
                rel_name_position->rel_list->counter++;

                bubblesort_to_list(rel_name_position->rel_list);
            }
            else
            {
                // Ho trovato il nodo to quindi devo aggiungere la relazione che so per certo che gia non esiste
                // Ho gia il nodo to, aggiungo e incremento il counter
                to_position->from_list = add_new_element_node(from, to_position->from_list);
                to_position->counter++;
                //bubblesort_to_list(rel_name_position->rel_list);

                // TODO: Ordinare la lista appena aggiunta in questo modo non c'è da percorrere l'intero grafo
            }

        }
    }
    // Ordino entrambe le liste prima quella dei to, poi rel_name
    // TODO: Da implementare l'ordinamento con bubblesort prima e poi con qualcosa di più veloce

    return head;
}


// Cerco la corrispondenza alla relazione, se esiste ritorno l'indrizzo così da poterlo usare altrimenti torno NULL
rel_name_t *find_exis_rel(char name[REL_SIZE], rel_name_t *list)
{
    while (list != NULL)
    {
        if (!strcmp(list->name, name)) return list;
        list = list->next_rel;
    }

    return NULL;
}
/*
rel_element_t *find_exis_rel(char name[REL_SIZE], rel_element_t *list)
{
    while (list != NULL)
    {
        if (!strcmp(list->name, name)) return list;
        list = list->next_rel;
    }

    return NULL;
}
 */

// Report -----------------------------------------------------

// Ordinamento rapido

#define MAX 100
/*
void quicksort(rel_element_t array[MAX], int first, int last)
{
    int i, j, pivot;
    rel_element_t tmp;

    if (first < last)
    {
        pivot = first;
        i = first;
        j = last;

        while (i < j)
        {
            while ((strcmp(array[i].name, array[pivot].name) <= 0) && i < last)
            {
                i++;
            }
            while (strcmp(array[j].name, array[pivot].name) > 0)
                j--;

            if (i < j)
            {
                tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }

        tmp = array[pivot];
        array[pivot] = array[j];
        array[j] = tmp;

        quicksort(array, first, j-1);
        quicksort(array, j+1, last);
    }

}
*/

void quicksort(rel_name_t array[MAX], int first, int last)
{
    int i, j, pivot;
    rel_name_t tmp;

    if (first < last)
    {
        pivot = first;
        i = first;
        j = last;

        while (i < j)
        {
            while ((strcmp(array[i].name, array[pivot].name) <= 0) && i < last)
            {
                i++;
            }
            while (strcmp(array[j].name, array[pivot].name) > 0)
                j--;

            if (i < j)
            {
                tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }

        tmp = array[pivot];
        array[pivot] = array[j];
        array[j] = tmp;

        quicksort(array, first, j-1);
        quicksort(array, j+1, last);
    }

}
void quicksort2(entity_counter_t array[MAX], int first, int last)
{
    int i, j, pivot;
    entity_counter_t tmp;

    if (first < last)
    {
        pivot = first;
        i = first;
        j = last;

        while (i < j)
        {
            while ((strcmp(array[i].name, array[pivot].name) <= 0) && i < last)
            {
                i++;
            }
            while (strcmp(array[j].name, array[pivot].name) > 0)
                j--;

            if (i < j)
            {
                tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }

        tmp = array[pivot];
        array[pivot] = array[j];
        array[j] = tmp;

        quicksort2(array, first, j-1);
        quicksort2(array, j+1, last);
    }

}

// Fine ordinamento rapido





// delent  ----------------------------------------------------

entity_node_t *treeMinimum(entity_node_t *x)
{
    while (x->left != NULL)
    {
        x = x->left;
    }
    return x;
}

entity_node_t *tree_next(entity_node_t *z)
{
    if (z->right != NULL)
    {
        return treeMinimum(z->right);
    }

    entity_node_t *y = z->parent;
    while (y != NULL && z == y->right)
    {
        z = y;
        y = y->parent;
    }
    return y;
}


entity_node_t *delent(entity_node_t *z, entity_node_t *tree, rel_name_t *head)
{

    //del_entity_occ(z, head);

    entity_node_t *y = NULL;
    entity_node_t *x = NULL;
    if ((z->left == NULL) || (z->right == NULL))
    {
        y = z;
    } else
    {
        y = tree_next(z);
    }
    //if (y ==  NULL) return NULL; // Entità non presente nell'albero
    if (y->left != NULL)
    {
        x = y->left;
    } else
    {
        x = y->right;
    }

    if (x != NULL)
    {
        x->parent = y->parent;
    }
    if (y->parent == NULL)
    {
        tree = x;
    } else if (y == y->parent->left)
    {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }

    if (y != z)
    {
        strcpy(z->name, y->name);
    }

    // Elimino ogni occorrenza nella lista delle relazioni



    return tree;

}

// cancellazioni -----------------------------------------------------

rel_element_t *del_rel_from_element(rel_element_t *to_delete, rel_element_t *list)
{
    // Elimina una relazione tra un qualsiasi to e uno specifico from
    rel_element_t *head = list;
    if (to_delete->prev != NULL)
    {
        to_delete->prev->next = to_delete->next;
    } else
    {
        head = to_delete->next;
    }

    if (to_delete->next != NULL)
    {
        to_delete->next->prev = to_delete->prev;
    }

    free(to_delete);
    return head;
}

rel_to_t *del_rel_to_element(rel_to_t *to_delete, rel_to_t *list)
{
    // Elimina una relazione tra un qualsiasi to e tutti i for, è usata quando bisogna eliminare una entità tramite delent
    rel_to_t *head = list;
    if (to_delete->prev != NULL)
    {
        to_delete->prev->next = to_delete->next;
    } else
    {
        head = to_delete->next;
    }

    if (to_delete->next != NULL)
    {
        to_delete->next->prev = to_delete->prev;
    }

    free(to_delete);
    return head;
}

rel_name_t *del_rel_name(rel_name_t *to_delete, rel_name_t *list)
{
    // Elimina una relazione che non deve più essere monitorata, usato quando il suo puntatore *rel_list diventa nullo
    rel_name_t *head = list;
    if (to_delete->prev_rel != NULL)
    {
        to_delete->prev_rel->next_rel = to_delete->next_rel;
    } else
    {
        head = to_delete->next_rel;
    }

    if (to_delete->next_rel != NULL)
    {
        to_delete->next_rel->prev_rel = to_delete->prev_rel;
    }

    free(to_delete);
    return head;
}

rel_name_t *delrel(char name[REL_SIZE], entity_node_t *to, entity_node_t *from, rel_name_t *rel_list)
{


    rel_name_t *rel_name_pos = find_exis_rel(name, rel_list);

    // Se rel_name_pos = NULL allora la relazione non è monitorata quindi non è da eliminare!!

    if (rel_name_pos == NULL || is_rel_exist_from_entity(from, to, rel_name_pos->rel_list) == 0) return rel_list;
    rel_to_t *to_pos = find_to_node(to, rel_name_pos->rel_list);

    rel_element_t *from_pos = to_pos->from_list;

    while (from_pos != NULL)
    {
        if (!strcmp(from_pos->from_entity->name, from->name))
        {
            to_pos->from_list = del_rel_from_element(from_pos, to_pos->from_list);
            to_pos->counter = to_pos->counter - 1;
            if (to_pos->counter < 0) to_pos->counter = 0;
            break;
        }
        from_pos = from_pos->next;
    }

    if (to_pos->counter == 0)
    {
        rel_name_pos->rel_list = del_rel_to_element(to_pos, rel_name_pos->rel_list);
    }
    if (rel_name_pos->rel_list == NULL)
    {
        rel_list = del_rel_name(rel_name_pos, rel_list);
    }
    if (rel_list == NULL)
    {
        rel_list = create_new_rel_name(FIRST_TRY, NULL, NULL);
    }
    return rel_list;
}

// report
void find_max_entity(rel_to_t *rel_to_list)
{
    rel_to_t *rel_to_list_head = rel_to_list;
    int max = 0;
    while (rel_to_list != NULL)
    {
        if (rel_to_list->counter > max) max = rel_to_list->counter;
        rel_to_list = rel_to_list->next;
    }

    while (rel_to_list_head != NULL)
    {
        if (rel_to_list_head->counter == max)
        {
            //printf("%s ", rel_to_list_head->to_entity->name);
            fputs(rel_to_list_head->to_entity->name, stdout);
            fputs(SPACE, stdout);
        }
        rel_to_list_head = rel_to_list_head->next;
    }

    printf("%d; ", max);
}

// Cancello tutte le occorrenze di una entità in seguito a delent
rel_name_t *del_entity_occ(entity_node_t *entity, rel_name_t *rel_list)
{
    rel_name_t *head = rel_list;

    while (rel_list != NULL)
    {
        rel_to_t *to_list = rel_list->rel_list;
        while (to_list != NULL)
        {
            if (!strcmp(to_list->to_entity->name, entity->name))
            {
                // Devo eliminare tutto il nodo to e passare al prossimo
                rel_to_t *to_delete = to_list;
                to_list = to_list->next;
                rel_list->rel_list = del_rel_to_element(to_delete, rel_list->rel_list);
                if (rel_list->rel_list == NULL)
                {
                    head = del_rel_name(rel_list, head);
                }
            }
            else
            {
                rel_element_t *from_list = to_list->from_list;
                while (from_list != NULL)
                {
                    if (!strcmp(from_list->from_entity->name, entity->name))
                    {
                        // Devo eliminare questa relazione
                        head = delrel(rel_list->name, to_list->to_entity, entity, head);
                        break;
                    }
                    else
                    {
                        // è possibile avere un solo from per ogni to quindi se ho trovato l'entità esco e proseguo
                        from_list = from_list->next;
                    }
                }
                to_list = to_list->next;
            }
        }
        rel_list = rel_list->next_rel;
    }

    return head;
}

void report(rel_name_t *rel_list)
{
    if (rel_list == NULL || !strcmp(rel_list->name, FIRST_TRY))
    {
        fputs(NO_REL_DEFINED, stdout);
        return;
    }

    while (rel_list != NULL)
    {
        //fputs("%s ", rel_list->name);
        fputs(rel_list->name, stdout);
        fputs(SPACE, stdout);
        find_max_entity(rel_list->rel_list);
        rel_list = rel_list->next_rel;
    }
    printf("\n");
}



// Main program -----------------------------------------------
void main()
{

    FILE *fp;
    fp = stdin;
    // Variabili generali
    char cmd[15];
    char param1[MAX_NAME_SIZE];
    char param2[MAX_NAME_SIZE];
    char param3[MAX_NAME_SIZE];

    // Input
    char string_input[256];

    entity_node_t *entity_tree = create_new_node(FIRST_TRY, NULL);
    //entity_node_t *entity_tree = NULL;
    //rel_element_t *rel_list = create_new_rel_element(FIRST_TRY, NULL, NULL, NULL, NULL);
    rel_name_t *rel_list = create_new_rel_name(FIRST_TRY, NULL, NULL);

    while (1)
    {
        fgets(string_input, 256, fp);
        //fgetsets(string_input);
        sscanf(string_input, "%s %s %s %s", cmd, param1, param2, param3);

        if (!strcmp(cmd, "end")) return;
        if (!strcmp(cmd, "addent")) addent(param1, entity_tree);
        if (!strcmp(cmd, "addrel"))
        {
            entity_node_t *from = find_entity_in_tree(param1, entity_tree);
            entity_node_t *to = find_entity_in_tree(param2, entity_tree);

            if (from != NULL && to != NULL) rel_list = addrel(param3, from, to, rel_list); // Aggiungo la relazione
        }
        if (!strcmp(cmd, "report")) report(rel_list);
        if (!strcmp(cmd, "delrel"))
        {
            entity_node_t *from = find_entity_in_tree(param1, entity_tree);
            entity_node_t *to = find_entity_in_tree(param2, entity_tree);

            if (from != NULL && to != NULL)
            {
                rel_list = delrel(param3, to, from, rel_list);
            }
            //check_for_null_name_pointer(rel_list);
        }
        if (!strcmp(cmd, "delent"))
        {
            entity_node_t *trash = find_entity_in_tree(param1, entity_tree);
            if (trash != NULL)
            {
                rel_list = del_entity_occ(trash, rel_list);
                entity_tree = delent(trash, entity_tree, rel_list);
            }
        }

    }

}