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

#pragma once

// =====================================================================
// c++ std
// =====================================================================

#include <string>

// =====================================================================
// local libraries
// =====================================================================

namespace base {

template <typename T>
class Singleton {
   private:
    // static pointer to the Singleton instance
    static Singleton* instancePtr;

    // mutex to ensure thread safety
    static std::mutex mtx;

   public:
    /**
     * Delete the assignment operator.
     */
    Singleton& operator=(const Singleton&) = delete;

    // /**
    //  * Delete the copy constructor.
    //  */
    // Singleton(const Singleton& obj) = delete;

    static T* getInstance() {
        static T instance;
        return &instance;
    }
};

}  // namespace base
