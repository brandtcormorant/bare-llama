# bare-llama

Native llama.cpp bindings for the [bare](https://github.com/holepunchto/bare) runtime, enabling efficient large language model inference from JavaScript.

## You might want this, but you probably shouldn't

This is nowhere near ready to use.

If you want to use bare-llama be prepared to join the development team 😏

## Bare & Pear & nothing else just yet

This is the beginnings of an addon for the bare js runtime, which means we'll be able to use it with [Pear, a js runtime that enables p2p applications](https://github.com/holepunchto/pear).

Bare is the core runtime of Pear.

### Nope: Node.js, Deno, Bun, etc.

Node.js support is possible, probably not Deno or Bun. But maybe! Who knows?! If you know let me know. I have no idea.

## Quick Start

```javascript
import { LlamaModel } from 'bare-llama'

// Create and initialize text generation model
const model = await LlamaModel.create({
  modelFilepath: './models/model.gguf',
})

// Generate text
const result = await model.generate("The quick brown fox")
console.log(result)

// Clean up
await model.destroy()
```

## Usage

Generate text:

```javascript
// Create a new model instance
const model = await LlamaModel.create({
  modelFilepath: './path/to/model.gguf',
  embedding: false,  // true for embedding models, false by default for text generation models
})

// Generate text
const generated = await model.generate("Once upon a time", {
  temperature: 0.8,
  maxTokens: 100
})

await model.destroy()
```

Create embeddings:

```js
const model = await LlamaModel.create({
  modelFilepath: './path/to/embeddings-model.gguf',
  embedding: true,
})

// Get embeddings (requires embedding: true)
const embeddings = await model.encode("Hello world")

await model.destroy()
```

Additional methods:

```javascript
const model = await LlamaModel.create({
  modelFilepath: './path/to/embeddings-model.gguf',
  embedding: true,
})

// Get model metadata
const metadata = await model.getMetadata()

// Tokenize text & detokenize tokens
const tokens = await model.tokenize("Hello world")
const text = await model.detokenize(tokens)

await model.destroy()
```

# Models

You'll have to download models yourself!

The tests are currently set up to use a smollm gguf model: https://huggingface.co/mradermacher/SmolLM-135M-Instruct-GGUF

## Credits

Built on [llama.cpp](https://github.com/ggerganov/llama.cpp) and [bare](https://github.com/holepunchto/bare)

## License

MIT
