//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 16.
//

#include "map.h"

void map_init(map *m) {
    m->length = 0;
    m->size = 0;
    m->pairs = 0;
}

bool map_isFound(map map, char *key) {
    for (int i = 0; i < map.length; ++i) {
        if (strcmp(map.pairs[i].key, key) == 0)
            return true;
    }
    return false;
}

char *map_getValue(map m, char *key) {
    size_t min = 0;
    size_t max = m.length-1;
    size_t kp = (min+max)/2;
    while(min <= max && strcmp(m.pairs[kp].key,key) != 0){
        if(strcmp(m.pairs[kp].key,key) < 0)
            min = kp+1;
        else
            max = kp-1;
        kp = (min+max)/2;
    }
    return min <= max ? m.pairs[kp].value : NULL;
}

void map_dump(map m) {
    for (int i = 0; i < m.length; ++i) {
        printf("%s %s\n", m.pairs[i].key, m.pairs[i].value);
    }
}

void map_addPair(map *m, pair p) {
    if (m->length >= m->size) {
        assert(m->length == m->size);
        size_t new_size = (m->size + 2) * 2;
        pair *new_list = realloc(m->pairs, new_size * sizeof(pair));
        if (new_list == 0)
            printf("OUT OF MEMORY!");
        m->size = new_size;
        m->pairs = new_list;
    }
    m->pairs[m->length++] = p;
    map_sort(m);
}

pair map_make_pair(char *key, char *value) {
    pair result;
    strcpy(result.key, key);
    strcpy(result.value, value);
    return result;
}

void map_sort(map* m) {

    for (int i = m->length - 1; i > 0; --i) {
        for (int j = 0; j < i; ++j) {
            if (strcmp(m->pairs[j].key, m->pairs[j + 1].key) > 0) {
                pair tmp = m->pairs[j];
                m->pairs[j] = m->pairs[j + 1];
                m->pairs[j + 1] = tmp;
            }
        }
    }
}

