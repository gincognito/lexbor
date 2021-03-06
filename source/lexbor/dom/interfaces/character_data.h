/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <lex.borisov@gmail.com>
 */

#ifndef LEXBOR_DOM_CHARACTER_DATA_H
#define LEXBOR_DOM_CHARACTER_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/str.h"

#include "lexbor/dom/interfaces/document.h"
#include "lexbor/dom/interfaces/node.h"


struct lxb_dom_character_data {
    lxb_dom_node_t node;

    lexbor_str_t   data;
};


LXB_API lxb_dom_character_data_t *
lxb_dom_character_data_interface_create(lxb_dom_document_t *document);

LXB_API lxb_dom_character_data_t *
lxb_dom_character_data_interface_destroy(lxb_dom_character_data_t *character_data);

LXB_API lxb_status_t
lxb_dom_character_data_replace(lxb_dom_character_data_t *ch_data,
                               const lxb_char_t *data, size_t len,
                               size_t offset, size_t count);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DOM_CHARACTER_DATA_H */
