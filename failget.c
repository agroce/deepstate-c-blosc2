#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "blosc2.h"

inline static void* blosc_malloc(const size_t alignment, const size_t size) {
  const int32_t clean_value = 0x99;
  void* block = NULL;
  
  block = aligned_alloc(alignment, size);
  
  if (block == NULL) {
    fprintf(stderr, "Error allocating memory!");
    return NULL;
  }
  
  /* Clean the allocated memory before returning. */
  memset(block, clean_value, size);
  
  return block;
}

int main() {
  blosc_init();
  blosc_set_nthreads(1);

  size_t type_size = 131;
  size_t num_elements = 1;
  size_t buffer_alignment = 32;

  blosc_set_compressor("blosclz");
  blosc_set_delta(1);

  size_t buffer_size = type_size * num_elements;

  /* Allocate memory for the test. */
  void* original = blosc_malloc(buffer_alignment, buffer_size);
  void* intermediate = blosc_malloc(buffer_alignment, buffer_size + BLOSC_MAX_OVERHEAD);
  void* items = blosc_malloc(buffer_alignment, buffer_size);
  void* result = blosc_malloc(buffer_alignment, buffer_size);

  /* The test data */
  memset(original, 0, buffer_size);
  ((char*)original)[128] = 1;

  /* Compress the input data and store it in an intermediate buffer.
     Decompress the data from the intermediate buffer into a result buffer. */
  blosc_compress(6, 0, type_size, buffer_size,
		 original, intermediate, buffer_size + BLOSC_MAX_OVERHEAD);
    
  unsigned start_item = 0;
  unsigned num_items = 1;
  int get_result = blosc_getitem(intermediate, start_item, num_items, items);
  assert(get_result == (num_items * type_size));
  for (int i = 0; i < type_size; i++) {
    int original_value = *((char*)original + i);
    int item_value = *((char*)items + i);
    if (original_value != item_value) {
      printf("ERROR: mismatch at %u: %d (original) vs. %d (getitem)\n", i,
	     original_value, item_value);
    }
  }

  blosc_decompress(intermediate, result, buffer_size);
  
  /* The round-tripped data matches the original data when the
     result of memcmp is 0. */
  assert(memcmp(original, result, buffer_size) == 0);

  /* Now that we see the round-trip passed, check the items */
  assert(memcmp(intermediate, items, get_result) == 0);
  
  /* Free allocated memory. */
  free(original);
  free(intermediate);
  free(items);
  free(result);
  
  /* Cleanup blosc resources. */
  blosc_destroy();
}
