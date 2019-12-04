#include <deepstate/DeepState.hpp>
using namespace deepstate;

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include "blosc2.h"

#define NUM_RUNS 4
#define NUM_INTERNAL 4

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

TEST(CBlosc2, RoundTrip) {
  unsigned blosc_thread_count = 1;
  
  blosc_init();
  blosc_set_nthreads(blosc_thread_count);

  size_t alignments[] = {32, 64};
  /* snappy is not available */
  const char* compressors[] = {"blosclz", "lz4", "lz4hc", "zlib", "zstd"};
  
  for (int i = 0; i < NUM_RUNS; i++) {
    LOG(TRACE) << "Starting run #" << i;
    size_t type_size = DeepState_UIntInRange(1, 512);
    LOG(TRACE) << "Type size = " << type_size;
    size_t num_elements = DeepState_UIntInRange(1, 2048/type_size);
    LOG(TRACE) << "Number of elements = " << num_elements;
    size_t buffer_alignment = OneOf(alignments);
    LOG(TRACE) << "Buffer alignment = " << buffer_alignment;
    size_t compression_level = DeepState_IntInRange(0, 9);
    LOG(TRACE) << "Compression level = " << compression_level;
    size_t do_shuffle = DeepState_Bool();
    LOG(TRACE) << "Do shuffle = " << do_shuffle;

    const char* compressor = OneOf(compressors);
    LOG(TRACE) << "Setting compressor to " << compressor;
    ASSERT(blosc_set_compressor(compressor) != -1) << "setting compressor to " << compressor << " failed!";

    int new_delta = DeepState_UIntInRange(0, 1);
    LOG(TRACE) << "Setting delta to " << new_delta;
    blosc_set_delta(new_delta);    

    size_t buffer_size = type_size * num_elements;
    LOG(TRACE) << "Buffer size = " << buffer_size;
    
    /* Allocate memory for the test. */
    void* original = blosc_malloc(buffer_alignment, buffer_size);
    void* intermediate = blosc_malloc(buffer_alignment, buffer_size + BLOSC_MAX_OVERHEAD);
    void* items = blosc_malloc(buffer_alignment, buffer_size);
    void* result = blosc_malloc(buffer_alignment, buffer_size);
    
    /* Fill the input data buffer */
    DeepState_SymbolizeData(original, (char*)original + buffer_size);
    
    /* Compress the input data and store it in an intermediate buffer.
       Decompress the data from the intermediate buffer into a result buffer. */
    blosc_compress(compression_level, do_shuffle, type_size, buffer_size,
		   original, intermediate, buffer_size + BLOSC_MAX_OVERHEAD);

    size_t n_uncompressed;
    size_t n_compressed;
    size_t n_blocksize;
    blosc_cbuffer_sizes(intermediate, &n_uncompressed, &n_compressed, &n_blocksize);
    LOG(TRACE) << "# uncompressed bytes = " << n_uncompressed;
    ASSERT_EQ(n_uncompressed, buffer_size) << "buffer size = " << buffer_size << " but bytes = " << n_uncompressed;
    LOG(TRACE) << "# compressed bytes = " << n_compressed;
    LOG(TRACE) << "block size = " << n_blocksize;

    size_t typesize;
    int flags;
    blosc_cbuffer_metainfo(intermediate, &typesize, &flags);
    LOG(TRACE) << "type size = " << typesize;
    //ASSERT_EQ(typesize, type_size) << "type size = " << type_size << " but meta claims " << typesize;
    LOG(TRACE) << "DOSHUFFLE: " << (int)(flags & BLOSC_DOSHUFFLE);
    //ASSERT(((flags & BLOSC_DOSHUFFLE) && do_shuffle) || (!(flags & BLOSC_DOSHUFFLE) && !do_shuffle)) <<
    //"do shuffle = " << (int)(flags & BLOSC_DOSHUFFLE) << " but set to " << do_shuffle;
    LOG(TRACE) << "DOBITSHUFFLE: " << (int)(flags & BLOSC_DOBITSHUFFLE);
    LOG(TRACE) << "DODELTA: " << (int)(flags & BLOSC_DODELTA);
    LOG(TRACE) << "MEMCPYED: " << (int)(flags & BLOSC_MEMCPYED);

    const char* b_compressor = blosc_cbuffer_complib(intermediate);
    LOG(TRACE) << "compressor = " << b_compressor;
    if ((strcmp(compressor, "lz4hc") == 0) && (strcmp(b_compressor, "LZ4") == 0)) {
      LOG(TRACE) << "expected change from lz4hc to LZ4";
    } else { 
      char* b_compressor_lower = (char*)malloc(strlen(b_compressor) + 1);
      for (int bci = 0; bci < strlen(b_compressor); bci++) {
	b_compressor_lower[bci] = tolower(b_compressor[bci]);
      }
      b_compressor_lower[strlen(b_compressor)] = 0;
      ASSERT(strcmp(b_compressor_lower, compressor) == 0) << "Compressor changed from " << compressor << " to " << b_compressor;
    }

    for(int j = 0; j < NUM_INTERNAL; j++) {
      OneOf(
	    [&] {
	      const char* compressor = OneOf(compressors);
	      ASSERT(blosc_set_compressor(compressor) != -1) << "Setting compressor to " << compressor << " failed!";
	    },
	    [&] {
	      int new_delta = DeepState_UIntInRange(0, 1);
	      LOG(TRACE) << "Setting delta to " << new_delta;
	      blosc_set_delta(new_delta);
	    },
	    [&] {
	      unsigned start_item = DeepState_UIntInRange(0, num_elements-1);
	      unsigned num_items = DeepState_UIntInRange(0, num_elements-start_item);
	      LOG(TRACE) << "Getting " << num_items << " from " << start_item;
	      int get_result = blosc_getitem(intermediate, start_item, num_items, items);
	      //ASSERT_EQ(get_result, num_items * type_size) <<
	      //"Getting " << num_items << " from " << start_item << " expected: " << num_items * type_size << ": got " << get_result;
	    },
	    [&] {
	      LOG(TRACE) << "Freeing resources";
	      ASSERT(blosc_free_resources() == 0) << "Freeing resources failed!";
	    }
	    );
    }
    
    blosc_decompress(intermediate, result, buffer_size);
    
    /* The round-tripped data matches the original data when the
       result of memcmp is 0. */
    ASSERT(memcmp(original, result, buffer_size) == 0) << "Round trip failed!";
    
    /* Free allocated memory. */
    free(original);
    free(intermediate);
    free(items);
    free(result);
  }
}
