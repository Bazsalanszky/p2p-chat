//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 16.
//

#include "map.h"

void map_init(Map *m) {
    m->length = 0;
    m->size = 0;
    m->pairs = 0;
}

bool map_isFound(Map map, char *key) {
    char *res = map_getValue(map, key);
    return (res == NULL) ? false : true;
}

char *map_getValue(Map m, char *key) {
    int min = 0;
    int max = m.length - 1;
    int kp = (min + max) / 2;
    while (min <= max && strcmp(m.pairs[kp].key, key) != 0) {
        if (strcmp(m.pairs[kp].key, key) < 0)
            min = kp + 1;
        else
            max = kp - 1;
        kp = (min + max) / 2;
    }
    return min <= max ? m.pairs[kp].value : NULL;
}

void map_dump(Map m) {
    for (size_t i = 0; i < m.length; ++i) {
        printf("%s %s\n", m.pairs[i].key, m.pairs[i].value);
    }
}

void map_addPair(Map *m, Pair p) {
    if (m->length >= m->size) {
        assert(m->length == m->size);
        size_t new_size = (m->size + 2) * 2;
        m->pairs = realloc(m->pairs, new_size * sizeof(Pair));
        if (m->pairs == NULL) {
            printf("OUT OF MEMORY!");
            abort();
        }
        m->size = new_size;
    }
    m->pairs[m->length++] = p;
    map_sort(m);
}

Pair map_make_pair(char *key, char *value) {
    Pair result;
    strncpy(result.key, key,65);
    strncpy(result.value, value,1024);
    return result;
}

void map_sort(Map *m) {
    for (int i = m->length - 1; i > 0; --i) {
        for (int j = 0; j < i; ++j) {
            if (strcmp(m->pairs[j].key, m->pairs[j + 1].key) > 0) {
                Pair tmp = m->pairs[j];
                m->pairs[j] = m->pairs[j + 1];
                m->pairs[j + 1] = tmp;
            }
        }
    }
}

