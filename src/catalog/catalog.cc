// Copyright 2025 Xiaochen Cui
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// =====================================================================
// third-party libraries
// =====================================================================

// spdlog
#include "spdlog/spdlog.h"

// =====================================================================
// self header
// =====================================================================

#include "src/catalog/catalog.h"

namespace small::catalog {

Catalog* Catalog::instancePtr = nullptr;

void Catalog::InitInstance() {
    if (instancePtr == nullptr) {
        instancePtr = new Catalog();
    } else {
        SPDLOG_ERROR("catalog instance already initialized");
    }
}

Catalog* Catalog::GetInstance() {
    if (instancePtr == nullptr) {
        SPDLOG_ERROR("catalog instance not initialized");
        return nullptr;
    }
    return instancePtr;
}

}  // namespace small::catalog
