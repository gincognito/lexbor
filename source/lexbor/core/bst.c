/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <lex.borisov@gmail.com>
 */

#include "lexbor/core/bst.h"


lexbor_bst_t *
lexbor_bst_create(void)
{
    return lexbor_calloc(1, sizeof(lexbor_bst_t));
}

lxb_status_t
lexbor_bst_init(lexbor_bst_t *bst, size_t size)
{
    lxb_status_t status;

    if (bst == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    if (size == 0) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    bst->dobject = lexbor_dobject_create();
    status = lexbor_dobject_init(bst->dobject, size,
                                 sizeof(lexbor_bst_entry_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    bst->root = 0;
    bst->tree_length = 0;

    return LXB_STATUS_OK;
}

void
lexbor_bst_clean(lexbor_bst_t *bst)
{
    lexbor_dobject_clean(bst->dobject);

    bst->root = 0;
    bst->tree_length = 0;
}

lexbor_bst_t *
lexbor_bst_destroy(lexbor_bst_t *bst, bool self_destroy)
{
    if (bst == NULL) {
        return NULL;
    }

    bst->dobject = lexbor_dobject_destroy(bst->dobject, true);

    if (self_destroy) {
        return lexbor_free(bst);
    }

    return bst;
}

lexbor_bst_entry_t *
lexbor_bst_entry_make(lexbor_bst_t *bst, size_t size)
{
    lexbor_bst_entry_t *new_entry = lexbor_dobject_calloc(bst->dobject);
    if (new_entry == NULL) {
        return NULL;
    }

    new_entry->size = size;

    bst->tree_length++;

    return new_entry;
}

lexbor_bst_entry_t *
lexbor_bst_insert(lexbor_bst_t *bst, lexbor_bst_entry_t **scope,
                  size_t size, void *value)
{
    lexbor_bst_entry_t *new_entry, *entry;

    new_entry = lexbor_dobject_calloc(bst->dobject);
    if (new_entry == NULL) {
        return NULL;
    }

    new_entry->size  = size;
    new_entry->value = value;

    bst->tree_length++;

    if (*scope == NULL) {
        *scope = new_entry;
        return new_entry;
    }

    entry = *scope;

    while (entry != NULL) {
        if (size == entry->size) {
            if (entry->next) {
                new_entry->next = entry->next;
            }

            entry->next = new_entry;
            new_entry->parent = entry->parent;

            return new_entry;
        }
        else if (size > entry->size) {
            if (entry->right == NULL) {
                entry->right = new_entry;
                new_entry->parent = entry;

                return new_entry;
            }

            entry = entry->right;
        }
        else {
            if (entry->left == NULL) {
                entry->left = new_entry;
                new_entry->parent = entry;

                return new_entry;
            }

            entry = entry->left;
        }
    }

    return NULL;
}

lexbor_bst_entry_t *
lexbor_bst_insert_not_exists(lexbor_bst_t *bst, lexbor_bst_entry_t **scope,
                             size_t size)
{
    lexbor_bst_entry_t *entry;

    if (*scope == NULL) {
        *scope = lexbor_bst_entry_make(bst, size);

        return *scope;
    }

    entry = *scope;

    while (entry != NULL) {
        if (size == entry->size) {
            return entry;
        }
        else if (size > entry->size) {
            if (entry->right == NULL) {
                entry->right = lexbor_bst_entry_make(bst, size);
                entry->right->parent = entry;

                return entry->right;
            }

            entry = entry->right;
        }
        else {
            if (entry->left == NULL) {
                entry->left = lexbor_bst_entry_make(bst, size);
                entry->left->parent = entry;

                return entry->left;
            }

            entry = entry->left;
        }
    }

    return NULL;
}

lexbor_bst_entry_t *
lexbor_bst_search(lexbor_bst_t *bst, lexbor_bst_entry_t *scope, size_t size)
{
    while (scope != NULL) {
        if (scope->size == size) {
            return scope;
        }
        else if (size > scope->size) {
            scope = scope->right;
        }
        else {
            scope = scope->left;
        }
    }

    return NULL;
}

lexbor_bst_entry_t *
lexbor_bst_search_close(lexbor_bst_t *bst, lexbor_bst_entry_t *scope,
                        size_t size)
{
    lexbor_bst_entry_t *max = NULL;

    while (scope != NULL) {
        if (scope->size == size) {
            return scope;
        }
        else if (size > scope->size) {
            scope = scope->right;
        }
        else {
            max = scope;
            scope = scope->left;
        }
    }

    return max;
}

void *
lexbor_bst_remove(lexbor_bst_t *bst, lexbor_bst_entry_t **scope, size_t size)
{
    lexbor_bst_entry_t *entry = *scope;

    while (entry != NULL) {
        if (entry->size == size) {
            return lexbor_bst_remove_by_pointer(bst, entry,
                                                entry->parent, scope);
        }
        else if (size > entry->size) {
            entry = entry->right;
        }
        else {
            entry = entry->left;
        }
    }

    return NULL;
}

void *
lexbor_bst_remove_close(lexbor_bst_t *bst, lexbor_bst_entry_t **scope,
                        size_t size, size_t *found_size)
{
    lexbor_bst_entry_t *entry = *scope;
    lexbor_bst_entry_t *max = NULL;

    while (entry != NULL) {
        if (entry->size == size) {
            if (found_size) {
                *found_size = entry->size;
            }

            return lexbor_bst_remove_by_pointer(bst, entry,
                                                entry->parent, scope);
        }
        else if (size > entry->size) {
            entry = entry->right;
        }
        else {
            max = entry;
            entry = entry->left;
        }
    }

    if (max != NULL) {
        if (found_size != NULL) {
            *found_size = max->size;
        }

        return lexbor_bst_remove_by_pointer(bst, max, max->parent, scope);
    }

    if (found_size != NULL) {
        *found_size = 0;
    }

    return NULL;
}

void *
lexbor_bst_remove_by_pointer(lexbor_bst_t *bst, lexbor_bst_entry_t *entry,
                             lexbor_bst_entry_t *parent,
                             lexbor_bst_entry_t **root)
{
    void *value;
    lexbor_bst_entry_t *next, *right, *left;

    bst->tree_length--;

    if (entry->next != NULL) {
        next = entry->next;
        entry->next = entry->next->next;

        value = next->value;

        lexbor_dobject_free(bst->dobject, next);

        return value;
    }

    value = entry->value;

    if (entry->left == NULL && entry->right == NULL) {
        if (parent != NULL) {
            if (parent->left == entry)  parent->left  = NULL;
            if (parent->right == entry) parent->right = NULL;
        }
        else {
            *root = NULL;
        }

        lexbor_dobject_free(bst->dobject, entry);
    }
    else if (entry->left == NULL) {
        if (parent == NULL) {
            entry->right->parent = NULL;

            *root = entry->right;

            lexbor_dobject_free(bst->dobject, entry);
        }
        else {
            right = entry->right;

            memcpy(entry, right, sizeof(lexbor_bst_entry_t));

            entry->parent = parent;

            lexbor_dobject_free(bst->dobject, right);
        }
    }
    else if (entry->right == NULL) {
        if (parent == NULL) {
            entry->left->parent = NULL;

            *root = entry->left;

            lexbor_dobject_free(bst->dobject, entry);
        }
        else {
            left = entry->left;

            memcpy(entry, left, sizeof(lexbor_bst_entry_t));

            entry->parent = parent;

            lexbor_dobject_free(bst->dobject, left);
        }
    }
    else {
        lexbor_bst_entry_t *left_free = entry->right;
        lexbor_bst_entry_t *left_free_parent = entry;

        while (left_free->left != NULL) {
            left_free_parent = left_free;
            left_free = left_free->left;
        }

        /* Swap */
        entry->size  = left_free->size;
        entry->next  = left_free->next;
        entry->value = left_free->value;

        if (left_free_parent->left == left_free) {
            left_free_parent->left = left_free->right;
        }
        else {
            left_free_parent->right = left_free->right;
        }

        lexbor_dobject_free(bst->dobject, left_free);
    }

    return value;
}

void
lexbor_bst_serialize(lexbor_bst_t *bst, lexbor_callback_f callback, void *ctx)
{
    lexbor_bst_serialize_entry(bst->root, callback, ctx, 0);
}

void
lexbor_bst_serialize_entry(lexbor_bst_entry_t *entry,
                           lexbor_callback_f callback, void *ctx, size_t tabs)
{
    size_t buff_len;
    char buff[1024];

    if (entry == NULL) {
        return;
    }

    /* Left */
    for (size_t i = 0; i < tabs; i++) {
        callback((lxb_char_t *) "\t", 1, ctx);
    }
    callback((lxb_char_t *) "<left ", 6, ctx);

    if (entry->left) {
        buff_len = sprintf(buff, LEXBOR_FORMAT_Z, entry->left->size);
        callback((lxb_char_t *) buff, buff_len, ctx);

        callback((lxb_char_t *) ">\n", 2, ctx);
        lexbor_bst_serialize_entry(entry->left, callback, ctx, (tabs + 1));

        for (size_t i = 0; i < tabs; i++) {
            callback((lxb_char_t *) "\t", 1, ctx);
        }
    }
    else {
        callback((lxb_char_t *) "NULL>", 5, ctx);
    }

    callback((lxb_char_t *) "</left>\n", 8, ctx);

    /* Right */
    for (size_t i = 0; i < tabs; i++) {
        callback((lxb_char_t *) "\t", 1, ctx);
    }
    callback((lxb_char_t *) "<right ", 7, ctx);

    if (entry->right) {
        buff_len = sprintf(buff, LEXBOR_FORMAT_Z, entry->right->size);
        callback((lxb_char_t *) buff, buff_len, ctx);

        callback((lxb_char_t *) ">\n", 2, ctx);
        lexbor_bst_serialize_entry(entry->right, callback, ctx, (tabs + 1));

        for (size_t i = 0; i < tabs; i++) {
            callback((lxb_char_t *) "\t", 1, ctx);
        }
    }
    else {
        callback((lxb_char_t *) "NULL>", 5, ctx);
    }

    callback((lxb_char_t *) "</right>\n", 9, ctx);
}
