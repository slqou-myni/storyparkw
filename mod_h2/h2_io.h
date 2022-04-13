/* Copyright 2015 greenbytes GmbH (https://www.greenbytes.de)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __mod_h2__h2_io__
#define __mod_h2__h2_io__

struct apr_thread_cond_t;
struct h2_bucket;
struct h2_response;
struct h2_task;

#include "h2_bucket_queue.h"

typedef struct h2_io h2_io;
struct h2_io {
    int id;                      /* stream identifier */
    apr_pool_t *pool;            /* pool to use for holding data */
    
    h2_bucket_queue input;       /* input data for stream */
    apr_size_t input_consumed;   /* how many bytes have been read */
    struct apr_thread_cond_t *input_arrived; /* block on reading */
    
    apr_bucket_brigade *bbout;   /* output data from stream */
    struct apr_thread_cond_t *output_drained; /* block on writing */
    
    struct h2_task *task;         /* the task connected to this io */
    struct h2_response *response; /* submittable response created */
};

/*******************************************************************************
 * Object lifecycle and information.
 ******************************************************************************/

/**
 * Creates a new h2_io for the given stream id. 
 */
h2_io *h2_io_create(int id, apr_pool_t *pool, apr_bucket_alloc_t *bucket_alloc);

/**
 * Frees any resources hold by the h2_io instance. 
 */
void h2_io_destroy(h2_io *io);

/*******************************************************************************
 * Input handling of streams.
 ******************************************************************************/
/**
 * Reads the next bucket from the input. Returns APR_EAGAIN if none
 * is currently available, APR_EOF if end of input has been reached.
 */
apr_status_t h2_io_in_read(h2_io *io, struct h2_bucket **pbucket);

/*******************************************************************************
 * Output handling of streams.
 ******************************************************************************/

struct h2_response *h2_io_extract_response(h2_io *io);

apr_status_t h2_io_out_write(h2_io *io, apr_bucket_brigade *bb, 
                             apr_size_t maxlen);

/**
 * Closes the input. After existing data has been read, APR_EOF will
 * be returned.
 */
apr_status_t h2_io_out_close(h2_io *io);

/**
 * Gives the overall length of the data that is currently queued for
 * output.
 */
apr_size_t h2_io_out_length(h2_io *io);


/*******************************************************************************
 * Synchronization to h2_session.
 ******************************************************************************/
apr_status_t h2_io_sync(h2_io *io, struct h2_bucket_queue *input, 
                        apr_bucket_brigade *output);


#endif /* defined(__mod_h2__h2_io__) */
