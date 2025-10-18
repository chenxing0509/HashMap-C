//
// Created by CK on 2025/10/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "HashMap.h"

#define DEFAULT_CAPACITY 8
#define MAX_PREALLOC 4096
#define LOAD_FACTOR 0.75 //链表平均长度的最大值（用空间换时间）

// API
HashMap* createMap(void) {
    HashMap* map = malloc(sizeof(HashMap));
    if (map == NULL) {
        return NULL;
    }

    map->elements = calloc(DEFAULT_CAPACITY, sizeof(Node*));
    if (map->elements == NULL) {
        return NULL;
    }
    map->size = 0;
    map->length = DEFAULT_CAPACITY;
    map->hashseed = time(NULL);

    return map;
}

// 哈希表遍历
void destroyMap(HashMap* map) {
    for (int i = 0; i < map->length; i++) {
        Node* curr = map->elements[i];
        while (curr != NULL) {
            Node* next = curr->next;
            free(curr);
            curr = next;
        }// curr == NULL
    }
    // 释放动态数组
    free(map->elements);
    free(map);
}

// murmurhash2
// 可以对任意类型的key求hash值
// 返回的hash值和seed相关，引入不确定性，提高安全性
// 问题：DDOS攻击
unsigned int MurmurHash2(const void *key, int len, unsigned int seed) {
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.
    const unsigned int m = 0x5bd1e995;
    const int r = 24;
    // Initialize the hash to a 'random' value
    unsigned int h = seed ^ len;
    // Mix 4 bytes at a time into the hash
    const unsigned char *data = (const unsigned char *) key;
    while (len >= 4) {
        unsigned int k = *(unsigned int *) data;
        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
        data += 4;
        len -= 4;
    }
    // Handle the last few bytes of the input array
    switch (len) {
        case 3:
            h ^= data[2] << 16;
        case 2:
            h ^= data[1] << 8;
        case 1:
            h ^= data[0];
            h *= m;
    };
    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

void reHash(Node* node, Node** table, int length, uint32_t seed) {
    // 重新计算hash值
    int idx = MurmurHash2(node->key, strlen(node->key), seed) % length;
    // 头插法
    node->next = table[idx];
    table[idx] = node;
}

void growCapacity(HashMap* map) {
    int newCapacity = (map->length <= MAX_PREALLOC) ? (map->length << 1) : (map->length + MAX_PREALLOC);

    Node** newTable = calloc(newCapacity, sizeof(Node*));
    // rehash
    uint32_t seed = time(NULL);

    for (int i = 0; i < map->length; i++) {
        Node* curr = map->elements[i];
        while (curr != NULL) {
            Node* next = curr->next;
            reHash(curr, newTable, newCapacity, seed);
            curr = curr->next;
        } // curr == NULL
    }

    free(map->elements);
    map->elements = newTable;
    map->length = newCapacity;
    map->hashseed = seed;
}

/*
 * key存在，更新key关联的值，并把原来的值返回
 * key不存在，添加键值对，并返回特殊值（NULL）
 */
V putMap(HashMap* map, K key, V val) {
    // 1.计算hash找到数组的索引
    int idx = MurmurHash2(key, strlen(key), map->hashseed) % map->length;
    // 2.遍历链表判断key是否存在
    Node* curr = map->elements[idx];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            // 更新value，返回旧的值
            V oldVal = curr->val;
            curr->val = val;
            return oldVal;
        }
        curr = curr->next;
    }
    // 3.判断是否需要扩容
    if (map->size >= map->length * LOAD_FACTOR) {
        growCapacity(map);
        //重新计算索引位置
        idx = MurmurHash2(key, strlen(key), map->hashseed) % map->length;
    }

    // 4.创建键值对
    Node* newNode = malloc(sizeof(Node));
    newNode->key = key;
    newNode->val = val;
    newNode->next = map->elements[idx];
    map->elements[idx] = newNode;
    map->size++;

    return NULL;
}

void deleteMap(HashMap* map, K key) {
    int idx = MurmurHash2(key, strlen(key), map->hashseed) % map->length;
    Node* prev = NULL;
    Node* curr = map->elements[idx];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            if (prev == NULL) {
                map->elements[idx] = curr->next;
            }
            else {
                prev->next = curr->next;
            }
            free(curr);
            map->size--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

V getMap(HashMap* map, K key) {
    int idx = MurmurHash2(key, strlen(key), map->hashseed);
    Node* curr = map->elements[idx];

    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            return curr->val;
        }
        curr = curr->next;
    }

    return NULL;
}

