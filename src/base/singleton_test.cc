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

// gtest
#include "gtest/gtest.h"

// spdlog
#include "spdlog/spdlog.h"

// =====================================================================
// local libraries
// =====================================================================

#include "src/base/singleton.h"

class Person {
    friend class base::Singleton<Person>;

   private:
    Person() {}
    ~Person() {}

   public:
    int age;
    void print(void) { SPDLOG_INFO("person age: {}", age); }
};

TEST(SingletonTest, SingletonInstance) {
    auto p1 = base::Singleton<Person>::getInstance();
    auto p2 = base::Singleton<Person>::getInstance();

    p1->age = 10;
    p2->print();
}

TEST(SingletonTest, DeletedCopyConstructor) {
    auto p1 = base::Singleton<Person>::getInstance();

    // Uncommenting the following line should cause a compilation error
    // because the copy constructor is deleted.
    // auto p2 = *p1; // Copy constructor test
}

TEST(SingletonTest, DeletedAssignmentOperator) {
    auto p1 = base::Singleton<Person>::getInstance();
    auto p2 = base::Singleton<Person>::getInstance();

    // Uncommenting the following line should cause a compilation error
    // because the assignment operator is deleted.
    // *p2 = *p1; // Assignment operator test
}
