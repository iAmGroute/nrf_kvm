#pragma once

/*
 * Copyright (c) 2019, Sandro Magi
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the async.h library.
 *
 * Author: Sandro Magi <naasking@gmail.com>
 *
 */

// *** Some modifications made ***

/*
 * = Stackless Async Subroutines =
 *
 * Taking inspiration from protothreads and async/await as found in C#, Rust and JS,
 * this is an async/await implementation for C based on Duff's device.
 *
 * Features:
 *
 * 1. Subroutines can have persistent state that isn't just static state, because
 *    each async subroutine accepts its own struct it uses as a parameter, and
 *    the async state is stored there.
 * 2. Because of the more flexible state handling, async subroutines can be nested
 *    in tree-like fashion which permits fork/join concurrency patterns.
 *
 * Caveats:
 *
 * 1. Due to compile-time bug, MSVC requires changing: 
 *      Project Properties > Configuration Properties > C/C++ > General > Debug Information Format
 *    from "Program Database for Edit And Continue" to "Program Database".
 * 2. As with protothreads, you have to be very careful with switch statements within an async
 *    subroutine. Generally best to avoid them.
 * 3. As with protothreads, you can't make blocking system calls and preserve the async semantics.
 *    These must be changed into non-blocking calls that test a condition.
 */

/*
 * The async computation status
 */
typedef enum ASYNC_EVT { ASYNC_INIT = 0, ASYNC_CONT = ASYNC_INIT, ASYNC_DONE = 1 } async;

/*
 * Core async structure, optional to use.
 */
struct Async  { unsigned _async_k; };
struct Async2 { unsigned _async_k; Async  pt; };
struct Async3 { unsigned _async_k; Async2 pt2; };
struct Async4 { unsigned _async_k; Async3 pt3; };
struct Async5 { unsigned _async_k; Async4 pt4; };
struct Async6 { unsigned _async_k; Async5 pt5; };
struct Async7 { unsigned _async_k; Async6 pt6; };
struct Async8 { unsigned _async_k; Async7 pt7; };
struct Async9 { unsigned _async_k; Async8 pt8; };

/*
 * Mark the start of an async subroutine
 *
 * Unknown continuation values now restart the subroutine from the beginning.
 */
#define async_begin(k) unsigned *_async_k = &(k)->_async_k; switch(*_async_k) { default:

/*
 * Mark the end of a async subroutine
 */
#define async_end *_async_k=ASYNC_DONE; case ASYNC_DONE: return ASYNC_DONE; }

/*
 * Wait until the condition succeeds
 */
#define await(cond) await_while(!(cond))

/*
 * Wait while the condition succeeds (optional)
 *
 * Continuation state is now callee-saved like protothreads which avoids
 * duplicate writes from the caller-saved design.
 */
#define await_while(cond) *_async_k = __LINE__; case __LINE__: if (cond) return ASYNC_CONT

/*
 * Yield execution
 */
#define async_yield *_async_k = __LINE__; return ASYNC_CONT; case __LINE__:

/*
 * Exit the current async subroutine
 */
#define async_exit {*_async_k = ASYNC_DONE; return ASYNC_DONE;}

/*
 * Initialize/reset a new async computation
 */
#define async_init(state)  ((state)->_async_k=ASYNC_INIT)
#define async_reset(state) ((state)->_async_k=ASYNC_DONE)

/*
 * Check if async subroutine is done
 */
#define async_done(state) ((state)->_async_k==ASYNC_DONE)

/*
 * Like await, but for an async function
 *
 * Will initialize the state given, before awaiting the function.
 */
#define await_call(f, state, ...) async_init(state); await((f)(state, ##__VA_ARGS__))
