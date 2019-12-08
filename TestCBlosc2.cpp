#include <deepstate/DeepState.hpp>
using namespace deepstate;

#include "blosc2.h"

#define MAX_RUNS 4
#define MAX_INTERNAL 4

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
  const char *compressors[] = {"blosclz", "lz4", "lz4hc", "zlib", "zstd"};
  
  unsigned num_runs = DeepState_UIntInRange(1, MAX_RUNS);
  LOG(TRACE) << "Performing " << num_runs << " round trips.";
  
  for (int i = 0; i < num_runs; i++) {
    LOG(TRACE) << "*******************************   Starting run #" << i << "   *******************************";
    size_t type_size = DeepState_UIntInRange(1, 255);
    size_t num_elements = DeepState_UIntInRange(1, ((DEEPSTATE_SIZE-256)/type_size)/num_runs);
    size_t buffer_alignment = OneOf(alignments);
    int compression_level = DeepState_IntInRange(0, 9);
    int do_shuffle = DeepState_Bool();

    const char *compressor = OneOf(compressors);
    ASSERT(blosc_set_compressor(compressor) != -1) << "setting compressor to " << compressor << " failed!";

    int new_delta = DeepState_UIntInRange(0, 1);
    blosc_set_delta(new_delta);    

    size_t buffer_size = type_size * num_elements;

    LOG(TRACE) << "Compression task: type_size: " << type_size << " ; num_elements: " << num_elements << " ; buffer_alignment: " <<
      buffer_alignment << "; compression_level: " << compression_level << "; do_shuffle: " << do_shuffle << "; compressor: " <<
      compressor << "; delta: " << new_delta << "; buffer_size: " << buffer_size;
    
    /* Allocate memory for the test. */
    void* original = blosc_malloc(buffer_alignment, buffer_size);
    void* intermediate = blosc_malloc(buffer_alignment, buffer_size + BLOSC_MAX_OVERHEAD);
    void* items = blosc_malloc(buffer_alignment, buffer_size);
    void* result = blosc_malloc(buffer_alignment, buffer_size);
    
    /* Fill the input data buffer */
    DeepState_SymbolizeData(original, (char*)original + buffer_size);

    for (int j = 0; j < buffer_size; j++) {
      LOG(TRACE) << "original[" << j << "] = " << (int)(((char*)original)[j]);
    }
    
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
    ASSERT_EQ(typesize, type_size) << "typesize:" << type_size << " but meta claims " << typesize;
    LOG(TRACE) << "typesize: " << typesize << "; DOSHUFFLE: " << (int)(flags & BLOSC_DOSHUFFLE) <<
      "; DOBITSHUFFLE: " << (int)(flags & BLOSC_DOBITSHUFFLE) << "; DODELTA: " << (int)(flags & BLOSC_DODELTA) <<
      "; MEMCPYED: " << (int)(flags & BLOSC_MEMCPYED);
    ASSERT(((flags & BLOSC_DOSHUFFLE) && do_shuffle) || (!(flags & BLOSC_DOSHUFFLE) && !do_shuffle) ||
	   (!(flags & BLOSC_DOSHUFFLE) && do_shuffle && (type_size == 1))) <<
      "do shuffle = " << (int)(flags & BLOSC_DOSHUFFLE) << " but set to " << do_shuffle;

    const char *b_compressor = blosc_cbuffer_complib(intermediate);
    LOG(TRACE) << "compressor: " << b_compressor;
    if ((strcmp(compressor, "lz4hc") == 0) && (strcmp(b_compressor, "LZ4") == 0)) {
      LOG(TRACE) << "expected change from lz4hc to LZ4";
    } else {
      unsigned b_len = strlen(b_compressor);
      char *b_compressor_lower = (char*)malloc(b_len + 1);
      for (int bci = 0; bci < b_len; bci++) {
        b_compressor_lower[bci] = tolower(b_compressor[bci]);
      }
      b_compressor_lower[b_len] = 0;
      ASSERT(strcmp(b_compressor_lower, compressor) == 0) << "Compressor changed from " << compressor << " to " << b_compressor;
    }

    unsigned num_internal_actions = DeepState_UIntInRange(0, MAX_INTERNAL);
    LOG(TRACE) << "Performing " << num_internal_actions << " non-buffer-changing actions.";

    for (int j = 0; j < num_internal_actions; j++) {
      OneOf(
	    [&] {
	      const char *compressor = OneOf(compressors);
	      LOG(TRACE) << "Setting compressor to" << compressor;
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
	      ASSERT_EQ(get_result, num_items * type_size) <<
		"Getting " << num_items << " from " << start_item << " expected: " << num_items * type_size << ": got " << get_result;
	      for (int k = 0; k < num_items; k++) {
		for (int l = 0; l < type_size; l++) {
		  int original_value = *((char*)original + ((k + start_item) * type_size) + l);
		  int item_value = *((char*)items + (k * type_size) + l);
		  if (original_value != item_value) {
		    LOG(TRACE) << "!!! MISMATCH !!!:";
		  }
		  LOG(TRACE) << "[" << k << "][" << l << "]: original: " << original_value << "; items: " <<item_value;
		}
		ASSERT(memcmp((char*)items + (k * type_size), (char*)original + ((k + start_item) * type_size), type_size) == 0) <<
		  "Get returned wrong data for item " << k << " (getting " << num_items << " from " << start_item << ")";
	      }
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
  
    /* Cleanup blosc resources. */
    blosc_destroy();
  }
}
