#ifndef SL_GEN_H
#define SL_GEN_H

#include "bytecode.h"
#include "parser.h"

void SL_parser_node_to_bytecode(SL_bytecode *bc, SL_parser_node *root);

#endif // SL_GEN_H