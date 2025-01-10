# Memory Allocator Implementation

## Overview

This project implements a custom memory allocator in C, providing functionality for `malloc`, `calloc`, `realloc`, and `free`. The allocator uses a multi-pool strategy for efficient memory allocation and a bulk allocator for larger allocations. 

## Features

- **`malloc`**: Allocates memory of the requested size using pool-based allocation for small sizes or bulk allocation for large sizes.
- **`calloc`**: Allocates memory and initializes it to zero.
- **`realloc`**: Resizes previously allocated memory, preserving existing data.
- **`free`**: Frees previously allocated memory and returns it to the appropriate pool or bulk allocation.

## Key Concepts

1. **Multi-Pool Allocation**:
   - Memory is divided into multiple pools based on block sizes.
   - Allocation uses `CHUNK_SIZE` (4 KB) as the basic unit for memory requests from the operating system via `sbrk()`.

2. **Bulk Allocation**:
   - Large memory allocations exceeding `CHUNK_SIZE` are handled separately using `mmap()`.

3. **Block Metadata**:
   - Each allocated block contains metadata such as its size and allocation status.

4. **Free List Management**:
   - Freed memory blocks are maintained in a free list table to allow reuse.

5. **Optimization**:
   - Efficient block selection and splitting to minimize fragmentation.
   - Memory requests to the OS are minimized by reusing free blocks.

## File Structure

- **Header File**: Defines the allocator's API and helper functions.
- **Implementation**: Includes logic for memory allocation, deallocation, and block management.
- **Bulk Memory Management**:
  - `bulk_alloc`: Allocates contiguous memory regions using `mmap()`.
  - `bulk_free`: Releases memory regions allocated by `bulk_alloc()`.

## Functions

### 1. Allocation Functions
- `void *malloc(size_t size)`: Allocates memory of the specified size.
- `void *calloc(size_t nmemb, size_t size)`: Allocates and initializes memory for an array of `nmemb` elements.
- `void *realloc(void *ptr, size_t size)`: Resizes previously allocated memory.

### 2. Deallocation Function
- `void free(void *ptr)`: Frees memory allocated by `malloc`, `calloc`, or `realloc`.

### 3. Bulk Memory Management
- `void *bulk_alloc(size_t size)`: Allocates a large contiguous memory region using `mmap`.
- `void bulk_free(void *ptr, size_t size)`: Frees a region allocated by `bulk_alloc`.

### 4. Helper Functions
- `block_index(size_t x)`: Computes the appropriate pool index for a given block size.
- `initialize_list()`: Initializes the free list table.
- `allocateblock_null` and `allocateblock_free`: Handles block allocation.
- `insert`: Finds a suitable free block in the free list.

## Constants
- `CHUNK_SIZE`: The basic unit of memory requests (4 KB).
- `LIST_SIZE`: Number of pools in the free list table.

## How to Use

1. Compile the project:
   ```bash
   gcc -o allocator allocator.c
