/*
 * Copyright 2010-2017 JetBrains s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <locale.h>
#include <stdio.h>

#include "Runtime.h"

struct RuntimeState {
  MemoryState* memoryState;
};

namespace {

InitNode* initHeadNode = nullptr;
InitNode* initTailNode = nullptr;

void InitGlobalVariables() {
  InitNode *currNode = initHeadNode;
  while (currNode != nullptr) {
    currNode->init();
    currNode = currNode->next;
  }
}

}  // namespace

#ifdef __cplusplus
extern "C" {
#endif

void AppendToInitializersTail(struct InitNode *next) {
  // TODO: use RuntimeState.
  if (initHeadNode == nullptr) {
    initHeadNode = next;
  } else {
    initTailNode->next = next;
  }
  initTailNode = next;
}

// TODO: properly use RuntimeState.
RuntimeState* InitRuntime() {
   // Set Unicode locale, otherwise towlower() and friends do not work properly.
  if (setlocale(LC_CTYPE, "en_US.UTF-8") == nullptr) {
    fprintf(stderr, "Cannot set locale, string ops may suffer\n");
  }
  RuntimeState* result = new RuntimeState();
  result->memoryState = InitMemory();
  // Keep global variables in state as well.
  InitGlobalVariables();
  return result;
}

void DeinitRuntime(RuntimeState* state) {
  if (state != nullptr) {
    DeinitMemory(state->memoryState);
    delete state;
  }
}

#ifdef __cplusplus
}
#endif
