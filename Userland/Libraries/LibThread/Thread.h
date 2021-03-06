/*
 * Copyright (c) 2019-2020, Sergey Bugaev <bugaevc@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <AK/DistinctNumeric.h>
#include <AK/Function.h>
#include <AK/Result.h>
#include <AK/String.h>
#include <LibCore/Object.h>
#include <pthread.h>

namespace LibThread {

TYPEDEF_DISTINCT_ORDERED_ID(int, ThreadError);

class Thread final : public Core::Object {
    C_OBJECT(Thread);

public:
    virtual ~Thread();

    void start();

    template<typename T = void>
    Result<T, ThreadError> join();

    String thread_name() const { return m_thread_name; }
    pthread_t tid() const { return m_tid; }

private:
    explicit Thread(Function<int()> action, StringView thread_name = nullptr);
    Function<int()> m_action;
    pthread_t m_tid { 0 };
    String m_thread_name;
};

template<typename T>
Result<T, ThreadError> Thread::join()
{
    void* thread_return = nullptr;
    int rc = pthread_join(m_tid, &thread_return);
    if (rc != 0) {
        return ThreadError { rc };
    }

    m_tid = 0;
    if constexpr (IsVoid<T>::value)
        return {};
    else
        return { static_cast<T>(thread_return) };
}

}
