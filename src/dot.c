#include "dot.h"

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>

#include "ptrmap.h"

#define ITOA_BUFFER_SZ 3

static const char *dot_header = "digraph finite_automaton { rankdir=LR;"
                           "start [shape=point, style=invis];"
                           "node [shape=doublecircle];";

struct state_id_map
{
    PTR_MAP map;
    int count;
};

typedef struct state_id_map * ID_MAP;

static ID_MAP id_map_new()
{
    ID_MAP map = malloc(sizeof(struct state_id_map));
    map->map = ptrmap_init();
    map->count = 0;
    return map;
}

static void id_map_free(ID_MAP map)
{
    PTR_MAP_ITERATOR iter = ptrmap_iterator_init(map->map);
    void *string;
    while (ptrmap_iterator_has_next(iter))
    {
        ptrmap_iterator_next(iter, NULL, &string);
        free(string);
    }
    ptrmap_iterator_fini(iter);

    ptrmap_fini(map->map);
    free(map);
}

static char * id_map_get(ID_MAP map, void *state)
{
    char *id = ptrmap_get(map->map, state);
    if (id) return id;

    id = calloc(2 + ITOA_BUFFER_SZ, sizeof(char));
    int index = map->count++;
    snprintf(id, 2 + ITOA_BUFFER_SZ, "q%d", index);

    ptrmap_set(map->map, state, id);
    return id;
}

static int gen_message(int fd, const char *buffer)
{
    size_t len = strlen(buffer);
    size_t offset = 0;
    ssize_t ret;

    while (offset != len)
    {
        ret = write(fd, buffer + offset, len - offset);
        if (ret < 0) return ret;
        offset += ret;
    }
    return 0;
}

static int gen_list(int fd, void **list, size_t sz, ID_MAP map)
{
    int ret;
    for (size_t i = 0; i < sz; ++i)
    {
        void *data = list[i];
        char *id = id_map_get(map, data);

        ret = gen_message(fd, id);
        if (ret) return ret;
        ret = gen_message(fd, ";");
    }
    return 0;
}

static void gen_transition(int fd, void *from, SYMBOL sym, void *to, ID_MAP map)
{
    gen_message(fd, id_map_get(map, from));
    gen_message(fd, " -> ");
    gen_message(fd, id_map_get(map, to));

    gen_message(fd, "[label=\"");
    if (sym == EPSILON)
    {
        gen_message(fd, "ε");
    }   
    else if (isprint(sym)) 
    {
        char sym_buf[] = { sym, 0 };
        gen_message(fd, sym_buf);
    }
    else
    {
        char buf[11] = { 0 };
        snprintf(buf, 11, "%d", sym);
        gen_message(fd, buf);
    }
    gen_message(fd, "\"];");

}

static void nstate_assign_id(NSTATE state, ID_MAP map)
{
    id_map_get(map, state);

    SYMBOL *symbols = nstate_get_transition_symbols(state);
    size_t symbols_sz = nstate_count_transition_symbols(state);

    for (size_t i = 0; i < symbols_sz; ++i)
    {
        NSTATE *states = nstate_get_transition_states(state, symbols[i]);
        size_t states_sz = nstate_count_transition_states(state, symbols[i]);
        for (size_t j = 0; j < states_sz; ++j)
        {
            NSTATE new_state = states[j];
            if (!ptrmap_contains_key(map->map, new_state))
                nstate_assign_id(new_state, map);
        }
        free(states);
    }
    free(symbols);
}

static int nfa_gen_dot_fd(NFA nfa, int fd)
{
    ID_MAP map = id_map_new();
    nstate_assign_id(nfa_get_starting_state(nfa), map);

    // generate the header
    gen_message(fd, dot_header);

    // generate the declarations for the accepting states
    NSTATE *accepting_states = nfa_get_accepting_states(nfa);
    size_t accepting_states_sz = nfa_count_accepting_states(nfa);
    gen_list(fd, (void**) accepting_states, accepting_states_sz, map);
    free(accepting_states);

    // generate the declaration for any following states
    gen_message(fd, "node [shape=circle];");

    // generate the transitions now
    // first generate the starting arrow to the start state

    NSTATE start_state = nfa_get_starting_state(nfa);
    gen_message(fd, "start -> ");
    gen_message(fd, id_map_get(map, start_state));
    gen_message(fd, "[label=\"start\"];");

    // loop through all the states
    NSTATE *all_states = nfa_get_states(nfa);
    size_t all_states_sz = nfa_count_states(nfa);
    for (size_t i = 0; i < all_states_sz; ++i)
    {
        NSTATE from = all_states[i];
        SYMBOL *transition_symbols = nstate_get_transition_symbols(from);
        size_t transition_symbols_sz = nstate_count_transition_symbols(from);
        for (size_t j = 0; j < transition_symbols_sz; ++j)
        {
            SYMBOL sym = transition_symbols[j];
            NSTATE *to_states = nstate_get_transition_states(from, sym);
            size_t to_states_sz = nstate_count_transition_states(from, sym);
            for (size_t k = 0; k < to_states_sz; ++k)
            {
                NSTATE to = to_states[k];
                gen_transition(fd, from, sym, to, map);
            }
            free(to_states);
        }
        free(transition_symbols);
    }
    free(all_states);

    // generate the ending
    gen_message(fd, "}");

    id_map_free(map);
    return 0;
}

int nfa_gen_dot(NFA nfa, char *filename)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0333);
    nfa_gen_dot_fd(nfa, fd);
    close(fd);
    return 0;
}
// 0 - READ, 1 - WRITE
int nfa_gen_img(NFA nfa, char *filename)
{
    // set up pipe
    int comm[2];
    int ret = pipe(comm);
    if (ret != 0) return ret;

    pid_t pid = fork();
    if (pid) // parent
    {
        close(comm[0]); // close the read end
        nfa_gen_dot_fd(nfa, comm[1]); // write the data
        close(comm[1]); // now close this end since we are done
        wait(NULL);
    }
    else // child 
    {
        close(comm[1]); // close the write end 
        dup2(comm[0], STDIN_FILENO); // redirect the read end of the pipe into STDIN
        
        char *command = "dot";
        char *args[] = {
            command,
            "-Tpng",
            "-o",
            filename,
            NULL
        };
        execvp(command, args);
    }
    return 0;
}

static void dstate_assign_id(DSTATE state, ID_MAP map)
{
    id_map_get(map, state);

    SYMBOL *symbols = dstate_get_transition_symbols(state);
    size_t symbols_sz = dstate_count_transition_symbols(state);

    for (size_t i = 0; i < symbols_sz; ++i)
    {
        DSTATE to = dstate_get_transition_state(state, symbols[i]);
        if (!ptrmap_contains_key(map->map, to))
            dstate_assign_id(to, map);
    }
}

static int dfa_gen_dot_fd(DFA dfa, int fd)
{
    ID_MAP map = id_map_new();
    dstate_assign_id(dfa_get_starting_state(dfa), map);

    // generate the header
    gen_message(fd, dot_header);

    // generate the declarations for the accepting states
    DSTATE *accepting_states = dfa_get_accepting_states(dfa);
    size_t accepting_states_sz = dfa_count_accepting_states(dfa);
    gen_list(fd, (void**) accepting_states, accepting_states_sz, map);
    free(accepting_states);

    // generate the declaration for any following states
    gen_message(fd, "node [shape=circle];");

    // generate the transitions now
    // first generate the starting arrow to the start state

    DSTATE start_state = dfa_get_starting_state(dfa);
    gen_message(fd, "start -> ");
    gen_message(fd, id_map_get(map, start_state));
    gen_message(fd, "[label=\"start\"];");

    // loop through all the states
    DSTATE *all_states = dfa_get_states(dfa);
    size_t all_states_sz = dfa_count_states(dfa);
    for (size_t i = 0; i < all_states_sz; ++i)
    {
        DSTATE from = all_states[i];
        SYMBOL *transition_symbols = dstate_get_transition_symbols(from);
        size_t transition_symbols_sz = dstate_count_transition_symbols(from);
        for (size_t j = 0; j < transition_symbols_sz; ++j)
        {
            SYMBOL sym = transition_symbols[j];
            DSTATE to = dstate_get_transition_state(from, sym);
            gen_transition(fd, from, sym, to, map);
        }
        free(transition_symbols);
    }
    free(all_states);

    // generate the ending
    gen_message(fd, "}");

    id_map_free(map);
    return 0;
}

int dfa_gen_dot(DFA dfa, char *filename)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0333);
    dfa_gen_dot_fd(dfa, fd);
    close(fd);
    return 0;
}

int dfa_gen_img(DFA dfa, char *filename)
{
    // set up pipe
    int comm[2];
    int ret = pipe(comm);
    if (ret != 0) return ret;

    pid_t pid = fork();
    if (pid) // parent
    {
        close(comm[0]); // close the read end
        dfa_gen_dot_fd(dfa, comm[1]); // write the data
        close(comm[1]); // now close this end since we are done
        wait(NULL);
    }
    else // child 
    {
        close(comm[1]); // close the write end 
        dup2(comm[0], STDIN_FILENO); // redirect the read end of the pipe into STDIN
        
        char *command = "dot";
        char *args[] = {
            command,
            "-Tpng",
            "-o",
            filename,
            NULL
        };
        execvp(command, args);
    }
    return 0;
}