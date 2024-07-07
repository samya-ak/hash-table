#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

const int HT_PRIME_1 = 151;
const int HT_PRIME_2 = 177;

static ht_item *ht_new_item(const char *k, const char *v) {
  ht_item *i = malloc(sizeof(ht_item));
  i->key = strdup(k);
  i->value = strdup(v);
  return i;
}

ht_hash_table *ht_new() {
  ht_hash_table *ht = malloc(sizeof(ht_hash_table));

  ht->size = 53;
  ht->count = 0;
  ht->items = calloc((size_t)ht->size, sizeof(ht_item *));

  return ht;
}

static void ht_del_item(ht_item *i) {
  free(i->key);
  free(i->value);
  free(i);
}

void ht_del_hash_table(ht_hash_table *ht) {
  for (int i = 0; i < ht->size; i++) {
    ht_item *item = ht->items[i];
    if (item != NULL) {
      ht_del_item(item);
    }
  }
  free(ht->items);
  free(ht);
}

/*
 * s - key string
 * a - prime number > 128
 * m - number of buckets
 *
 * - When hashing strings, the goal is to map them to a fixed range of integers
 * (the number of buckets) as uniformly as possible.
 * - Prime numbers do not share factors with other numbers (other than 1 and
 * themselves), which helps in spreading out the hash values more uniformly.
 * - Choosing a prime number larger than the alphabet size ensures that the base
 * `𝑎` can cover the entire range of possible characters (in this case, 0 to
 * 127 for ASCII) without repeated cycles.
 *
 * - For any hash function, there is a 'pathological' set of inputs which all
 * hash to same value. Pathological inputs poses a security issue. If a hash
 * table is fed a set of colliding keys, the searches for those keys will take
 * linear time instead of constant time. This can be used as a DOS attack
 * against systems like DNS servers that depends on hash tables
 * */

static int ht_hash(const char *s, const int a, const int m) {
  long hash = 0;
  const int len_s = strlen(s);
  for (int i = 0; i < len_s; i++) {
    hash += (long)pow(a, len_s - (i + 1)) * s[i];
    hash = hash % m;
  }
  return (int)hash;
}

/*
 * Handling collisions using Double hashing
 *
 * Hash functions map an infinitely large number of input to finite number of
 * outputs. Different keys will map to the same array index causing bucket
 * collisions.
 *
 * Number of attempts will gradually increase after collisions. hash_b might be
 * 0 so 1 is added to make sure it's never 0
 * '
 * */

static int ht_get_hash(const char *s, const int num_buckets,
                       const int attempt) {
  const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
  const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);

  return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}
