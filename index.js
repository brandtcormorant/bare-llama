const binding = require("./binding.js");

/**
 * @typedef {Object} LlamaModelInstance
 */

/**
 * @typedef {Object} LlamaContextInstance
 */

/**
 * @typedef {Object} LlamaModelInstanceMetadata
 * @property {number} parameters - model parameters
 * @property {number} contextWindow - The context window size
 */

// TODO: set defaults at the function binding level

/**
 * Create a new model instance from a gguf file
 * @param {string} modelFilepath
 * @param {Object} options - Model configuration options
 * @param {number}[options.logLevel=0] - Log level for the model (0 to suppress logs)
 * @returns {Promise<LlamaModelInstance>}
 */
async function createModel(modelFilepath, options = { logLevel: 0 }) {
  const model = {};
  await binding.createModel(model, modelFilepath, options);
  return model;
}

/**
 * Load an existing model instance from its file
 * @param {LlamaModelInstance} model
 * @param {string} modelFilepath
 * @returns {Promise<void>}
 */
async function loadModel(model, modelFilepath) {
  await binding.loadModel(model, modelFilepath);
}

/**
 * Destroy a model instance
 * @param {LlamaModelInstance} model
 * @returns {Promise<void>}
 */
async function destroyModel(model) {
  await binding.destroyModel(model);
}

/**
 * Get metadata from a model
 * @param {LlamaModelInstance} model
 * @returns {Promise<LlamaModelInstanceMetadata>}
 */
async function getModelMetadata(model) {
  return binding.getModelMetadata(model);
}

/**
 * Convert text into tokens that the model can understand
 * @param {LlamaModelInstance} model - The model instance
 * @param {string} text - Input text to tokenize
 * @param {Object} [options={}] - Tokenization options
 * @param {boolean} [options.addSpecial=false] - Add special tokens to output
 * @param {boolean} [options.parseSpecial=false] - Parse special tokens in text
 * @returns {Promise<ArrayBuffer>} Array of token IDs
 */
async function tokenize(model, text, options = {}) {
  return binding.tokenize(model, text, options);
}

/**
 * Convert token IDs back into text
 * @param {LlamaModelInstance} model - The model instance
 * @param {ArrayBuffer} tokens - Array of token IDs to convert to text
 * @param {Object} [options={}] - Detokenization options
 * @param {boolean} [options.removeSpecial=false] - Remove special tokens from output
 * @param {boolean} [options.unparseSpecial=false] - Unparse special tokens in text
 * @returns {Promise<string>} Decoded text
 */
async function detokenize(model, tokens, options = {}) {
  return binding.detokenize(model, tokens, options);
}

/**
 * Create a new context instance for a model that can be used for text generation or embeddings
 * @param {LlamaModelInstance} model - The model instance to create a context for
 * @param {Object} [options={}] - Options for context creation
 * @param {boolean} [options.embedding=false] - Whether to create an embedding context (true) or generation context (false)
 * * @param {number} [options.n_ctx=2048] - Maximum tokens in context
 * @param {number} [options.n_batch=512] - Maximum tokens to process in parallel
 * @returns {Promise<LlamaContextInstance>} The created context instance
 */
async function createContext(model, options = {}) {
  const context = {};
  await binding.createContext(context, model, options);
  return context;
}

/**
 * Destroy a context instance
 * @param {LlamaContextInstance} context - The context instance to destroy
 * @returns {Promise<void>}
 */
async function destroyContext(context) {
  return binding.destroyContext(context);
}

/**
 * Encode text into an array of token embeddings
 * Must be used with a LlamaContextInstance that has been created with the `embedding` option set to `true`.
 * @param {LlamaContextInstance} context - The context instance to use for encoding
 * @param {string} text - Text to encode into embeddings
 * @param {Object} [options={}] - Encoding options
 * @param {boolean} [options.addSpecial=false] - Add special tokens to output
 * @param {boolean} [options.parseSpecial=false] - Parse special tokens in text
 * @returns {Promise<ArrayBuffer>} Array of token embeddings
 */
async function encode(context, text, options = {}) {
  return binding.encode(context, text);
}

/**
 * Generate text based on a prompt.
 * Must be used with a LlamaContextInstance that has been created with the `embedding` option set to `false`.
 * @param {LlamaContextInstance} context - The context instance to use for generation
 * @param {string} prompt - Text prompt to generate from
 * @param {Object} [options={}] - Generation options
 * @param {boolean} [options.addSpecial=false] - Add special tokens to output
 * @param {boolean} [options.parseSpecial=false] - Parse special tokens in text
 * @returns {Promise<string>} Generated text
 */
async function generate(context, prompt, options = {}) {
  return binding.generate(context, prompt, options);
}

class LlamaModel {
  /** @type {LlamaModelInstance} */
  #model;

  /** @type {LlamaModelContext} */
  #context;

  static async create(options = {}) {
    const model = new LlamaModel(options.modelFilepath, options);
    await model.init();
    await model.load();
    await model.context(options);
    return model;
  }

  /**
   * Creates a new LlamaModel instance for text generation and embeddings
   * @param {string} modelFilepath - Path to the model GGUF file
   * @param {Object} options - Model configuration options
   * @param {boolean} [options.embedding=false] - Whether to create an embedding model (true) or generation model (false)
   * @param {boolean} [options.addSpecial=false] - Whether to add special tokens to output
   * @param {boolean} [options.parseSpecial=false] - Whether to parse special tokens in text
   * @param {boolean} [options.removeSpecial=false] - Whether to remove special tokens from output
   * @param {boolean} [options.unparseSpecial=false] - Whether to unparse special tokens in text
   * @param {Object} [options.context] - Customize the initial context created for this model
   */
  constructor(modelFilepath, options = {}) {
    this.modelFilepath = modelFilepath;
    this.options = {
      embedding: false,
      addSpecial: false,
      parseSpecial: false,
      removeSpecial: false,
      unparseSpecial: false,
      context: {},
      ...options,
    };
  }

  /**
   * Initialize the LlamaModel instance by creating a new model from the model filepath
   * @returns {Promise<void>}
   * @private
   */
  async init() {
    this.#model = await createModel(this.modelFilepath);
  }

  /**
   * Load a model instance from a model file
   * @returns {Promise<void>}
   * @private
   */
  async load() {
    await loadModel(this.#model, this.modelFilepath);
  }

  /**
   * @typedef {Object} LlamaModelMetadata
   * @property {number} parameters - The number of parameters in the model
   * @property {number} contextWindow - The context window size of the model
   * @property {string} filepath - File path to the model
   * @property {boolean} embedding - Additional model metadata
   * @property {LlamaModelContextOptions} [context] - Metadata about the model context
   */

  /**
   * Get metadata from the model instance such as parameter count and name
   * @returns {Promise<LlamaModelMetadata>}
   */
  async getMetadata() {
    const sourceMetadata = await getModelMetadata(this.#model);
    console.log("this.options.embedding", this.options.embedding, this.options);
    /** @type {LlamaModelMetadata} */
    const metadata = {
      ...sourceMetadata,
      filepath: this.modelFilepath,
      embedding: this.options.embedding,
    };

    if (this.#context) {
      metadata.context = this.#context.options;
    }

    console.log("metadata?????", metadata);
    return metadata;
  }

  /**
   * Destroy both the model and context instances associated with this LlamaModel
   * @returns {Promise<void>}
   */
  async destroy() {
    await destroyModel(this.#model);
  }

  /**
   * Convert input text into token IDs that the model can understand
   * @param {string} text - Input text to tokenize
   * @param {Object} [options={}] - Tokenization options
   * @param {boolean} [options.addSpecial=false] - Add special tokens to output
   * @param {boolean} [options.parseSpecial=false] - Parse special tokens in text
   * @returns {Promise<ArrayBuffer>} Array of token IDs
   */
  async tokenize(text, options = {}) {
    const overridenOptions = {
      addSpecial: this.options.addSpecial,
      parseSpecial: this.options.parseSpecial,
      ...options,
    };

    return tokenize(this.#model, text, overridenOptions);
  }

  /**
   * Convert token IDs back into text
   * @param {ArrayBuffer} tokens - Array of token IDs to convert to text
   * @param {Object} [options={}] - Detokenization options
   * @param {boolean} [options.removeSpecial=false] - Remove special tokens from output
   * @param {boolean} [options.unparseSpecial=false] - Unparse special tokens in text
   * @returns {Promise<string>} Decoded text
   */
  async detokenize(tokens, options = {}) {
    const overridenOptions = {
      removeSpecial: this.options.removeSpecial,
      unparseSpecial: this.options.unparseSpecial,
      ...options,
    };

    return detokenize(this.#model, tokens, overridenOptions);
  }

  /**
   * Create a new context instance for this model that can be used for text generation or embeddings
   * @param {Object} [options={}] - Options for context creation
   * @param {boolean} [options.embedding=false] - Whether to create an embedding context (true) or generation context (false)
   * @param {boolean} [options.existing=true] - Return existing context if one exists rather than creating new
   * @param {number} [options.n_ctx=512] - Maximum tokens in context
   * @param {number} [options.n_batch=512] - Maximum tokens to process in parallel
   * @returns {Promise<LlamaModelContext>} The context instance
   */
  async context(options = {}) {
    if (this.#context && options.existing) {
      return this.#context;
    }

    this.#context = await LlamaModelContext.create(this.#model, options);
    return this.#context;
  }

  /**
   * Encode text into an array of token embeddings.
   * Must be used with a model created with the `embedding` option set to `true`.
   * @param {string} text - Text to encode into embeddings
   * @param {Object} [options={}] - Encoding options
   * @param {boolean} [options.addSpecial=false] - Add special tokens to output
   * @param {boolean} [options.parseSpecial=false] - Parse special tokens in text
   * @returns {Promise<ArrayBuffer>} Array of token embeddings
   */
  async encode(text, options = {}) {
    return this.#context.encode(text, options);
  }

  /**
   * Generate text based on a prompt.
   * Must be used with a model created with the `embedding` option set to `false`.
   * @param {string} prompt - Text prompt to generate from
   * @param {Object} [options={}] - Generation options
   * @param {boolean} [options.addSpecial=false] - Add special tokens to output
   * @param {boolean} [options.parseSpecial=false] - Parse special tokens in text
   * @returns {Promise<string>} Generated text
   */
  async generate(prompt, options = {}) {
    return this.#context.generate(prompt, options);
  }
}

/**
 * A context instance for LlamaModel that can be used for text generation or embeddings
 * @class
 */
class LlamaModelContext {
  /** @type {LlamaModel} */
  #model;

  /** @type {LlamaContextInstance} */
  #context;

  /**
   * Creates a new LlamaModelContext instance that is fully initialized and ready to use
   * @param {LlamaModel} model - The LlamaModel instance to create a context for
   * @param {LlamaModelContextOptions} options - Configuration options for the context
   */
  static async create(model, options = {}) {
    const context = new LlamaModelContext(model, options);
    await context.init();
    return context;
  }

  /**
   * @typedef {Object} LlamaModelContextOptions
   * @property {number} [contextSize=2048] - Maximum number of tokens that can be processed at once
   * @property {number} [batchSize=512] - Maximum number of tokens to process in parallel
   * @property {boolean} [embedding=false] - Whether to create an embedding context (true) or generation context (false)
   * @property {boolean} [options.addSpecial=false] - Add special tokens to output
   * @property {boolean} [options.parseSpecial=false] - Parse special tokens in text
   */

  /**
   * Creates a new LlamaModelContext instance that can be lazily initialized using the `init` method
   * @param {LlamaModel} model - The LlamaModel instance to create a context for
   * @param {LlamaModelContextOptions} options - Configuration options for the context
   */
  constructor(model, options = {}) {
    this.#model = model;
    this.options = {
      contextSize: 2048,
      batchSize: 512,
      embedding: false,
      addSpecial: false,
      parseSpecial: false,
      ...options,
    };
  }

  /**
   * Initializes a new context instance for this model. Can override options supplied in constructor.
   * @param {LlamaModelContextOptions} options - Configuration options for the context
   * @returns {Promise<void>}
   */
  async init(options = {}) {
    const overridenOptions = {
      ...this.options,
      ...options,
    };

    this.#context = await createContext(this.#model, overridenOptions);
  }

  /**
   * Destroy the context associated with this LlamaModelContext
   * @returns {Promise<void>}
   */
  async destroy() {
    await destroyContext(this.#context);
  }

  /**
   * Encode text into an array of token embeddings.
   * Must be used with a LlamaContextInstance created with the `embedding` option set to `true`.
   * @param {string} text - Text to encode into embeddings
   * @param {Object} [options={}] - Encoding options
   * @param {boolean} [options.addSpecial=false] - Add special tokens to output
   * @param {boolean} [options.parseSpecial=false] - Parse special tokens in text
   * @returns {Promise<ArrayBuffer>} Array of token embeddings
   */
  async encode(text, options = {}) {
    if (!this.options.embedding) {
      throw new Error(
        "Cannot encode text without an embedding context. Use `embedding: true` when creating the context.",
      );
    }

    const overridenOptions = {
      addSpecial: this.options.addSpecial,
      parseSpecial: this.options.parseSpecial,
      ...options,
    };

    return binding.encode(this.#context, text, overridenOptions);
  }

  /**
   * Generate text based on a prompt.
   * Must be used with a LlamaContextInstance created with the `embedding` option set to `false`.
   * @param {string} prompt - Text prompt to generate from
   * @param {Object} [options={}] - Generation options
   * @param {boolean} [options.addSpecial=false] - Add special tokens to output
   * @param {boolean} [options.parseSpecial=false] - Parse special tokens in text
   * @returns {Promise<string>} Generated text
   */
  async generate(prompt, options = {}) {
    if (this.options.embedding) {
      throw new Error(
        "Cannot generate text without a generation context. Use `embedding: false` when creating the context",
      );
    }

    const overridenOptions = {
      addSpecial: this.options.addSpecial,
      parseSpecial: this.options.parseSpecial,
      ...options,
    };

    return binding.generate(this.#context, prompt, overridenOptions);
  }
}

module.exports = {
  createModel,
  loadModel,
  destroyModel,
  tokenize,
  detokenize,
  getModelMetadata,
  createContext,
  destroyContext,
  encode,
  generate,
  LlamaModel,
  LlamaModelContext,
};
