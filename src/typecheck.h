#pragma once
#include <vector>
#include "error.h"

struct Node;

std::vector<TypeError> TypeCheck(Node& ast);
