#include <assert.h>
#include <bare.h>
#include <js.h>
#include <llama.h>
#include <stdatomic.h>
#include <stdio.h>

typedef struct {
  struct llama_model *model;
  atomic_int refs;
} bare_llama_model_t;

typedef struct {
  struct llama_context *ctx;
  atomic_int refs;
  struct llama_model *model;
  bool is_embedding;
} bare_llama_context_t;

typedef struct {
  bool add_special;
  bool parse_special;
} bare_llama_token_options_t;

static enum ggml_log_level global_log_level = GGML_LOG_LEVEL_NONE;

static void
bare_llama_log_callback(enum ggml_log_level level, const char *text, void *user_data) {
  if (level <= global_log_level) {
    fprintf(stderr, "%s", text);
  }
}

static void
initialize_logging(enum ggml_log_level log_level) {
  global_log_level = log_level;
  ggml_log_set(bare_llama_log_callback, NULL);
  llama_log_set(bare_llama_log_callback, NULL);
}

static void
get_token_options (js_env_t *env, js_value_t *options, bare_llama_token_options_t *token_opts) {
  int err;

  llama_log_set(bare_llama_log_callback, NULL);
  ggml_log_set(bare_llama_log_callback, NULL);

  // Set defaults
  token_opts->add_special = false;
  token_opts->parse_special = false;

  if (options == NULL) return;

  bool is_null;
  err = js_is_null(env, options, &is_null);
  assert(err == 0);

  bool is_undefined;
  err = js_is_undefined(env, options, &is_undefined);
  assert(err == 0);

  if (!is_null && !is_undefined) {
    js_value_t *add_special_val;
    if (js_get_named_property(env, options, "addSpecial", &add_special_val) == 0) {
      err = js_get_value_bool(env, add_special_val, &token_opts->add_special);
      assert(err == 0);
    }

    js_value_t *parse_special_val;
    if (js_get_named_property(env, options, "parseSpecial", &parse_special_val) == 0) {
      err = js_get_value_bool(env, parse_special_val, &token_opts->parse_special);
      assert(err == 0);
    }
  }
}

static void
bare_llama_context_teardown (void *data) {
  bare_llama_context_t *ctx = (bare_llama_context_t *) data;

  if (--ctx->refs == 0) {
    llama_free(ctx->ctx);
    free(ctx);
  }
}

static void
bare_llama_context_finalize (js_env_t *env, void *data, void *finalize_hint) {
  int err;

  bare_llama_context_teardown(data);

  err = js_remove_teardown_callback(env, bare_llama_context_teardown, data);
  assert(err == 0);
}

static js_value_t *
bare_llama_context_destroy (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 1; // context instance
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);
  assert(argc == 1);

  bare_llama_context_t *context;
  err = js_remove_wrap(env, argv[0], (void **) &context);
  assert(err == 0);

  err = js_remove_teardown_callback(env, bare_llama_context_teardown, (void *) context);
  assert(err == 0);

  bare_llama_context_teardown((void *) context);

  return NULL;
}

static js_value_t *
bare_llama_context_create (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3; // instance, model instance, and options object
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  bare_llama_model_t *model;
  err = js_unwrap(env, argv[1], (void **) &model);
  assert(err == 0);

  // Set up default context params
  struct llama_context_params params = llama_context_default_params();
  params.n_ctx = 2048;
  params.n_batch = 512;

  // Parse options
  bool is_embedding = false;
  if (argc > 2) {
    bool is_null;
    err = js_is_null(env, argv[2], &is_null);
    assert(err == 0);

    bool is_undefined;
    err = js_is_undefined(env, argv[2], &is_undefined);
    assert(err == 0);

    // TODO: helper function that checks for null/undefined?

    if (!is_null && !is_undefined) {
        js_value_t *embedding_val;
        if (js_get_named_property(env, argv[2], "embedding", &embedding_val) == 0) {
            err = js_get_value_bool(env, embedding_val, &is_embedding);
            assert(err == 0);
        }
    }

    js_value_t *context_size_val;
    if (js_get_named_property(env, argv[2], "contextSize", &context_size_val) == 0) {
      err = js_get_value_uint32(env, context_size_val, &params.n_ctx);
      assert(err == 0);
    } else {
      params.n_ctx = 2048; // default value
    }

    js_value_t *batch_size_val;
    if (js_get_named_property(env, argv[2], "batchSize", &batch_size_val) == 0) {
      err = js_get_value_uint32(env, batch_size_val, &params.n_batch);
      assert(err == 0);
    } else {
      params.n_batch = 512; // default value
    }
  }

  // Set mode-specific params
  if (is_embedding) {
    params.embeddings = true;
    params.logits_all = false;
  } else {
    params.embeddings = false;
    params.logits_all = false;
  }

  struct llama_context *llama_ctx = llama_new_context_with_model(model->model, params);

  if (llama_ctx == NULL) {
    err = js_throw_error(env, NULL, "Failed to create context");
    assert(err == 0);
    return NULL;
  }

  bare_llama_context_t *ctx = malloc(sizeof(bare_llama_context_t));
  ctx->ctx = llama_ctx;
  ctx->refs = 1;
  ctx->model = model->model;
  ctx->is_embedding = is_embedding;

  err = js_wrap(env, argv[0], ctx, bare_llama_context_finalize, NULL, NULL);
  assert(err == 0);

  err = js_add_teardown_callback(env, bare_llama_context_teardown, (void *) ctx);
  assert(err == 0);

  return NULL;
}

static js_value_t *
bare_llama_context_encode (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  bare_llama_context_t *ctx;
  err = js_unwrap(env, argv[0], (void **) &ctx);
  assert(err == 0);

  if (!ctx->is_embedding) {
    err = js_throw_error(env, NULL, "Context not configured for embeddings");
    assert(err == 0);
    return NULL;
  }

  bare_llama_token_options_t token_opts;
  get_token_options(env, argc > 2 ? argv[2] : NULL, &token_opts);

  // First tokenize the input
  size_t text_len;
  err = js_get_value_string_utf8(env, argv[1], NULL, 0, &text_len);
  assert(err == 0);

  utf8_t *text = malloc(text_len + 1);
  err = js_get_value_string_utf8(env, argv[1], text, text_len + 1, NULL);
  assert(err == 0);

  // Get tokens
  int n_tokens = llama_tokenize(ctx->model, (const char *) text, text_len, NULL, 0, token_opts.add_special, token_opts.parse_special);
  if (n_tokens < 0) {
    n_tokens = -n_tokens;
  }

  llama_token *tokens = malloc(n_tokens * sizeof(llama_token));
  int result_tokens = llama_tokenize(ctx->model, (const char *) text, text_len, tokens, n_tokens, token_opts.add_special, token_opts.parse_special);

  // Prepare batch
  struct llama_batch batch = llama_batch_init(n_tokens, 0, 1);

  batch.n_tokens = result_tokens;

  for (int i = 0; i < n_tokens; i++) {
    batch.token[i] = tokens[i];
    batch.pos[i] = i;
    batch.n_seq_id[i] = 1;
    batch.seq_id[i][0] = 0;
    batch.logits[i] = false;
  }
  // Ask for embeddings for the last token
  batch.logits[n_tokens - 1] = true;

  // Decode batch
  int ret = llama_decode(ctx->ctx, batch);

  if (ret != 0) {
    free(tokens);
    free(text);
    llama_batch_free(batch);
    err = js_throw_error(env, NULL, "Failed to process text");
    assert(err == 0);
    return NULL;
  }

  // Make sure processing is complete
  llama_synchronize(ctx->ctx);

  enum llama_pooling_type pooling_type = llama_pooling_type(ctx->ctx);

  const float *embeddings = NULL;

  if (pooling_type != LLAMA_POOLING_TYPE_NONE) {
    embeddings = llama_get_embeddings_seq(ctx->ctx, 0);
  }

  if (embeddings == NULL) {
    embeddings = llama_get_embeddings(ctx->ctx);
  }

  int n_embd = llama_n_embd(ctx->model);

  if (embeddings == NULL) {
    free(tokens);
    free(text);
    llama_batch_free(batch);
    err = js_throw_error(env, NULL, "Failed to get embeddings");
    assert(err == 0);
    return NULL;
  }

  // Create result Float64Array
  js_value_t *result;
  err = js_create_arraybuffer(env, n_embd * sizeof(double), NULL, &result);
  assert(err == 0);

  double *data;
  size_t length;
  err = js_get_arraybuffer_info(env, result, (void **) &data, &length);
  assert(err == 0);

  // Copy embeddings to result
  for (int i = 0; i < n_embd; i++) {
    data[i] = (double) embeddings[i];
  }

  free(tokens);
  free(text);
  llama_batch_free(batch);

  return result;
}

static int
resample_until_valid (
    struct llama_sampler *chain,
    struct llama_context *ctx,
    struct llama_model *model,
    llama_token *token_out,
    char *text_out,
    size_t text_max_len,
    int max_attempts
) {
  for (int attempt = 0; attempt < max_attempts; attempt++) {
    llama_token new_token = llama_sampler_sample(chain, ctx, 0);

    // Check for EOS
    if (llama_token_eos(model) == new_token) {
      return -1;
    }

    // Try to get text
    int token_len = llama_token_to_piece(model, new_token, text_out, text_max_len, 0, true);

    if (token_len > 0) {
      *token_out = new_token;
      return token_len;
    }
  }

  return -1;
}

static js_value_t *
bare_llama_context_generate (js_env_t *env, js_callback_info_t *info) {
    int err;

    size_t argc = 4; // context instance, text, and options
    js_value_t *argv[4];

    err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
    assert(err == 0);

    bare_llama_context_t *ctx;
    err = js_unwrap(env, argv[0], (void **) &ctx);
    assert(err == 0);

    if (ctx->is_embedding) {
    err = js_throw_error(env, NULL, "Context not configured for generation");
    assert(err == 0);
    return NULL;
    }

    bare_llama_token_options_t token_opts;
    get_token_options(env, argc > 3 ? argv[3] : NULL, &token_opts);

    // Parse generation options
    int max_tokens = 20;
    float temperature = 0.8f;
    int top_k = 40;

    if (argc > 2) {
    bool is_null;
    err = js_is_null(env, argv[2], &is_null);
    assert(err == 0);

    bool is_undefined;
    err = js_is_undefined(env, argv[2], &is_undefined);
    assert(err == 0);

    if (!is_null && !is_undefined) {
        js_value_t *max_tokens_val;
        if (js_get_named_property(env, argv[2], "maxTokens", &max_tokens_val) == 0) {
        err = js_get_value_int32(env, max_tokens_val, &max_tokens);
        assert(err == 0);
        }

        js_value_t *temp_val;
        if (js_get_named_property(env, argv[2], "temperature", &temp_val) == 0) {
        double temp;
        err = js_get_value_double(env, temp_val, &temp);
        assert(err == 0);
        temperature = (float) temp;
        }

        js_value_t *top_k_val;
        if (js_get_named_property(env, argv[2], "topK", &top_k_val) == 0) {
        err = js_get_value_int32(env, top_k_val, &top_k);
        assert(err == 0);
        }
    }
    }

    // Get input text
    size_t text_len;
    err = js_get_value_string_utf8(env, argv[1], NULL, 0, &text_len);
    assert(err == 0);

    utf8_t *text = malloc(text_len + 1);
    err = js_get_value_string_utf8(env, argv[1], text, text_len + 1, NULL);
    assert(err == 0);

    // Tokenize input
    int n_tokens = llama_tokenize(ctx->model, (const char *) text, text_len, NULL, 0, token_opts.add_special, token_opts.parse_special);
    if (n_tokens < 0) n_tokens = -n_tokens;

    llama_token *tokens = malloc(n_tokens * sizeof(llama_token));
    int result_tokens = llama_tokenize(ctx->model, (const char *) text, text_len, tokens, n_tokens, token_opts.add_special, token_opts.parse_special);

    // Initialize sampling chain
    struct llama_sampler_chain_params chain_params = llama_sampler_chain_default_params();
    struct llama_sampler *chain = llama_sampler_chain_init(chain_params);
    llama_sampler_chain_add(chain, llama_sampler_init_top_k(top_k));
    llama_sampler_chain_add(chain, llama_sampler_init_temp(temperature));
    llama_sampler_chain_add(chain, llama_sampler_init_dist(0));

    // Process initial prompt batch
    struct llama_batch batch = llama_batch_init(n_tokens, 0, 1);
    batch.n_tokens = result_tokens;

    // Set up initial batch with prompt
    for (int i = 0; i < result_tokens; i++) {
    batch.token[i] = tokens[i];
    batch.pos[i] = i;
    batch.n_seq_id[i] = 1;
    batch.seq_id[i][0] = 0;
    batch.logits[i] = (i == result_tokens - 1); // Only last token needs logits
    }

    // Process initial batch
    int ret = llama_decode(ctx->ctx, batch);

    if (ret != 0) {
    free(tokens);
    free(text);
    llama_batch_free(batch);
    llama_sampler_free(chain);
    err = js_throw_error(env, NULL, "Failed to process initial text");
    assert(err == 0);
    return NULL;
    }

    // Make sure processing is complete
    llama_synchronize(ctx->ctx);

    const float *logits = llama_get_logits(ctx->ctx);

    if (logits == NULL) {
    free(tokens);
    free(text);
    llama_batch_free(batch);
    llama_sampler_free(chain);
    err = js_throw_error(env, NULL, "No logits available");
    assert(err == 0);
    return NULL;
    }

    // Sample first token using the last position
    llama_token new_token = llama_sampler_sample(chain, ctx->ctx, batch.n_tokens - 1);

    // Buffer for collecting generated text
    size_t gen_buffer_size = 1024; // Start with reasonable size
    char *gen_buffer = malloc(gen_buffer_size);
    size_t gen_len = 0;

    // Get text for first token
    char token_text[8];
    int token_len = llama_token_to_piece(ctx->model, new_token, token_text, sizeof(token_text), 0, true);

    memcpy(gen_buffer + gen_len, token_text, token_len);
    gen_len += token_len;

    // Continue generating
    for (int i = 1; i < max_tokens; i++) {
    // Create new batch for single token
    struct llama_batch next_batch = llama_batch_init(1, 0, 1);
    next_batch.n_tokens = 1;
    next_batch.token[0] = new_token;
    next_batch.pos[0] = batch.n_tokens + i - 1; // Continue position sequence
    next_batch.n_seq_id[0] = 1;
    next_batch.seq_id[0][0] = 0;
    next_batch.logits[0] = true;

    ret = llama_decode(ctx->ctx, next_batch);
    if (ret != 0) {
        break;
    }

    llama_synchronize(ctx->ctx);

    // Sample next token
    new_token = llama_sampler_sample(chain, ctx->ctx, 0);

    // Check for special tokens
    if (llama_token_eos(ctx->model) == new_token) {
        break;
    }

    // Get token text
    token_len = llama_token_to_piece(ctx->model, new_token, token_text, sizeof(token_text), 0, true);

    // Check for valid token length
    if (token_len <= 0) {
        if (token_len <= 0) {
        token_len = resample_until_valid(
            chain,
            ctx->ctx,
            ctx->model,
            &new_token,
            token_text,
            sizeof(token_text),
            50
        );

        if (token_len < 0) {
            break;
        }
        }
    }

    // Check buffer size
    if (gen_len + token_len >= gen_buffer_size) {
        size_t old_size = gen_buffer_size;
        gen_buffer_size *= 2;
        gen_buffer = realloc(gen_buffer, gen_buffer_size);
    }

    // Add to buffer
    memcpy(gen_buffer + gen_len, token_text, token_len);
    gen_len += token_len;

    llama_batch_free(next_batch);
    }

    // Create final string
    js_value_t *result;
    err = js_create_string_utf8(env, (utf8_t *) gen_buffer, gen_len, &result);
    assert(err == 0);

    // Cleanup
    free(gen_buffer);
    free(tokens);
    free(text);
    llama_batch_free(batch);
    llama_sampler_free(chain);
    return result;
}

static void
bare_llama_model_teardown (void *data) {
  bare_llama_model_t *model = (bare_llama_model_t *) data;

  if (--model->refs == 0) {
    llama_free_model(model->model);
    free(model);
  }
}

static void
bare_llama_model_finalize (js_env_t *env, void *data, void *finalize_hint) {
  int err;

  bare_llama_model_teardown(data);

  err = js_remove_teardown_callback(env, bare_llama_model_teardown, data);
  assert(err == 0);
}

static js_value_t *
bare_llama_model_create (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 2; // model and options object
  js_value_t *argv[2];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);
  assert(argc == 2);

  enum ggml_log_level log_level = GGML_LOG_LEVEL_NONE;

  bool is_object;
  err = js_is_object(env, argv[1], &is_object);
  assert(err == 0);

  if (sizeof(argv) > 1 && is_object) {
    js_value_t *log_level_val;
    if (js_get_named_property(env, argv[1], "logLevel", &log_level_val) == 0) {
      int32_t log_level_int;
      err = js_get_value_int32(env, log_level_val, &log_level_int);
      assert(err == 0);
      log_level = (enum ggml_log_level)log_level_int;
    }
  }

  initialize_logging(log_level);

  bare_llama_model_t *model = malloc(sizeof(bare_llama_model_t));
  model->model = NULL;
  model->refs = 1;

  err = js_wrap(env, argv[0], model, bare_llama_model_finalize, NULL, NULL);
  assert(err == 0);

  err = js_add_teardown_callback(env, bare_llama_model_teardown, (void *) model);
  assert(err == 0);

  return NULL;
}

static js_value_t *
bare_llama_model_load (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 2;
  js_value_t *argv[2];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);
  assert(argc == 2);

  bare_llama_model_t *model;
  err = js_unwrap(env, argv[0], (void **) &model);
  assert(err == 0);

  size_t path_len;
  err = js_get_value_string_utf8(env, argv[1], NULL, 0, &path_len);
  assert(err == 0);

  path_len += 1;

  utf8_t *path = malloc(path_len);
  err = js_get_value_string_utf8(env, argv[1], path, path_len, NULL);
  assert(err == 0);

  struct llama_model_params params = llama_model_default_params();

  model->model = llama_load_model_from_file((const char *) path, params);

  free(path);

  if (model->model == NULL) {
    err = js_throw_error(env, NULL, "Failed to load model");
    assert(err == 0);
    return NULL;
  }

  return NULL;
}

static js_value_t *
bare_llama_model_destroy (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 1;
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);
  assert(argc == 1);

  bare_llama_model_t *model;
  err = js_remove_wrap(env, argv[0], (void **) &model);
  assert(err == 0);

  err = js_remove_teardown_callback(env, bare_llama_model_teardown, (void *) model);
  assert(err == 0);

  bare_llama_model_teardown((void *) model);

  return NULL;
}

static js_value_t *
bare_llama_model_get_metadata (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 1;
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);
  assert(argc == 1);

  bare_llama_model_t *model;
  err = js_unwrap(env, argv[0], (void **) &model);
  assert(err == 0);

  if (model->model == NULL) {
    err = js_throw_error(env, NULL, "Model not loaded");
    assert(err == 0);
    return NULL;
  }

  js_value_t *result;
  err = js_create_object(env, &result);
  assert(err == 0);

  js_value_t *n_params;
  err = js_create_int64(env, llama_model_n_params(model->model), &n_params);
  assert(err == 0);

  err = js_set_named_property(env, result, "parameters", n_params);
  assert(err == 0);

  js_value_t *n_ctx_train;
  err = js_create_int32(env, llama_n_ctx_train(model->model), &n_ctx_train);
  assert(err == 0);

  err = js_set_named_property(env, result, "contextWindow", n_ctx_train);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_llama_model_tokenize (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3; // model, text, options
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  bare_llama_model_t *model;
  err = js_unwrap(env, argv[0], (void **) &model);
  assert(err == 0);

  size_t text_len;
  err = js_get_value_string_utf8(env, argv[1], NULL, 0, &text_len);
  assert(err == 0);
  text_len += 1;

  utf8_t *text = malloc(text_len + 1);
  err = js_get_value_string_utf8(env, argv[1], text, text_len + 1, NULL);
  assert(err == 0);

  bool add_special = false;
  bool parse_special = false;

  if (argc > 2) {
    bool is_null;
    err = js_is_null(env, argv[2], &is_null);
    assert(err == 0);

    bool is_undefined;
    err = js_is_undefined(env, argv[2], &is_undefined);
    assert(err == 0);

    if (!is_null && !is_undefined) {
      js_value_t *add_special_val;
      if (js_get_named_property(env, argv[2], "addSpecial", &add_special_val)) {
        err = js_get_value_bool(env, add_special_val, &add_special);
        assert(err == 0);
      }

      js_value_t *parse_special_val;
      if (js_get_named_property(env, argv[2], "parseSpecial", &parse_special_val)) {
        err = js_get_value_bool(env, parse_special_val, &parse_special);
        assert(err == 0);
      }
    }
  }

  int n_tokens = llama_tokenize(model->model, (const char *) text, text_len, NULL, 0, add_special, parse_special);

  if (n_tokens < 0) {
    n_tokens = -n_tokens;
  }

  llama_token *tokens = malloc(n_tokens * sizeof(llama_token));
  int result_tokens = llama_tokenize(model->model, (const char *) text, text_len, tokens, n_tokens, add_special, parse_special);

  if (result_tokens < 0) {
    free(tokens);
    free(text);
    err = js_throw_error(env, NULL, "Failed to tokenize text");
    assert(err == 0);
    return NULL;
  }

  js_value_t *result;
  err = js_create_array_with_length(env, result_tokens, &result);
  assert(err == 0);

  for (int i = 0; i < result_tokens; i++) {
    js_value_t *token;
    err = js_create_int32(env, tokens[i], &token);
    assert(err == 0);
    err = js_set_element(env, result, i, token);
    assert(err == 0);
  }

  free(tokens);
  free(text);

  return result;
}

static js_value_t *
bare_llama_model_detokenize (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err = 0);

  bare_llama_model_t *model;
  err = js_unwrap(env, argv[0], (void **) &model);
  assert(err == 0);

  uint32_t n_tokens;
  err = js_get_array_length(env, argv[1], &n_tokens);
  assert(err == 0);

  llama_token *tokens = malloc(n_tokens * sizeof(llama_token));

  for (uint32_t i = 0; i < n_tokens; i++) {
    js_value_t *val;
    err = js_get_element(env, argv[1], i, &val);
    assert(err == 0);

    int32_t token;
    err = js_get_value_int32(env, val, &token);
    assert(err == 0);
    tokens[i] = token;
  }

  bool remove_special = false;
  bool unparse_special = false;

  if (argc > 2) {
    bool is_null;
    err = js_is_null(env, argv[2], &is_null);
    assert(err == 0);

    bool is_undefined;
    err = js_is_undefined(env, argv[2], &is_undefined);
    assert(err == 0);

    if (!is_null && !is_undefined) {
      js_value_t *remove_special_val;
      if (js_get_named_property(env, argv[2], "removeSpecial", &remove_special_val)) {
        err = js_get_value_bool(env, remove_special_val, &remove_special);
        assert(err == 0);
      }

      js_value_t *unparse_special_val;
      if (js_get_named_property(env, argv[2], "unparseSpecial", &unparse_special_val)) {
        err = js_get_value_bool(env, unparse_special_val, &unparse_special);
        assert(err == 0);
      }
    }
  }

  int text_size = llama_detokenize(model->model, tokens, n_tokens, NULL, 0, remove_special, unparse_special);

  if (text_size < 0) {
    text_size = -text_size;
  }

  char *text = malloc(text_size + 1);
  int result_len = llama_detokenize(model->model, tokens, n_tokens, text, text_size, remove_special, unparse_special);

  if (result_len < 0) {
    free(text);
    free(tokens);
    err = js_throw_error(env, NULL, "Failed to detokenize");
    assert(err == 0);
    return NULL;
  }

  if (text[result_len - 1] == '\0') {
    result_len -= 1;
  }

  js_value_t *result;
  err = js_create_string_utf8(env, (utf8_t *) text, result_len, &result);
  assert(err == 0);

  free(text);
  free(tokens);

  return result;
}

static js_value_t *
bare_llama_exports (js_env_t *env, js_value_t *exports) {
  int err;

#define V(name, fn) \
  { \
    js_value_t *val; \
    err = js_create_function(env, name, -1, fn, NULL, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("createModel", bare_llama_model_create)
  V("loadModel", bare_llama_model_load)
  V("destroyModel", bare_llama_model_destroy)
  V("getModelMetadata", bare_llama_model_get_metadata)
  V("tokenize", bare_llama_model_tokenize)
  V("detokenize", bare_llama_model_detokenize)
  V("createContext", bare_llama_context_create)
  V("encode", bare_llama_context_encode)
  V("generate", bare_llama_context_generate)
#undef V

  return exports;
}

BARE_MODULE(bare_llama, bare_llama_exports)
