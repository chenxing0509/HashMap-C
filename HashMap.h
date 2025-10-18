//
// Created by CK on 2025/10/18.
//


#ifndef TEST_HASHMAP_H
#define TEST_HASHMAP_H

typedef char* K;
typedef char* V;

typedef struct node {
    K key;
    V val;
    struct node* next;
}Node;

typedef struct list {
    Node** elements;
    int size; // 键值对个数
    int length;
    uint32_t hashseed;
}HashMap;

HashMap* createMap(void);
void destroyMap(HashMap* map);
V putMap(HashMap* map, K key, V val);
void deleteMap(HashMap* map, K key);
V getMap(HashMap* map, K key);


#endif //TEST_HASHMAP_H