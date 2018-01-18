/*
* Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License").
* You may not use this file except in compliance with the License.
* A copy of the License is located at
*
*  http://aws.amazon.com/apache2.0
*
* or in the "license" file accompanying this file. This file is distributed
* on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied. See the License for the specific language governing
* permissions and limitations under the License.
*/

#include <aws/common/mutex.h>
#include <errno.h>

void aws_mutex_clean_up(struct aws_mutex *mutex) {
    aws_mutex_release(mutex);
    pthread_mutex_destroy(&mutex->mutex_handle);
}

int aws_mutex_init(struct aws_mutex *mutex, struct aws_allocator *allocator) {

    mutex->allocator = allocator;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&mutex->mutex_handle, &attr);
    pthread_mutexattr_destroy(&attr);

    return AWS_ERROR_SUCCESS;
}

static int convert_and_raise_error_code (int error_code) {
    switch (error_code) {
        case 0:
            return AWS_ERROR_SUCCESS;
        case EINVAL:
            return aws_raise_error(AWS_ERROR_MUTEX_NOT_INIT);
        case EBUSY:
            return aws_raise_error(AWS_ERROR_MUTEX_TIMEOUT);
        case EPERM:
            return aws_raise_error(AWS_ERROR_MUTEX_CALLER_NOT_OWNER);
        default:
            return aws_raise_error(AWS_ERROR_MUTEX_FAILED);
    }
}

int aws_mutex_acquire(struct aws_mutex *mutex) {

    return convert_and_raise_error_code(pthread_mutex_lock(&mutex->mutex_handle));
}

int aws_mutex_try_acquire(struct aws_mutex *mutex) {

    return convert_and_raise_error_code(pthread_mutex_trylock(&mutex->mutex_handle));
}

int aws_mutex_release(struct aws_mutex *mutex) {

    return convert_and_raise_error_code(pthread_mutex_unlock(&mutex->mutex_handle));
}
