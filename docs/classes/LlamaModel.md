[**bare-llama**](../README.md)

---

[bare-llama](../README.md) / LlamaModel

# Class: LlamaModel

## Constructors

### new LlamaModel()

> **new LlamaModel**(`modelFilepath`, `options`): [`LlamaModel`](LlamaModel.md)

Creates a new LlamaModel instance for text generation and embeddings

#### Parameters

##### modelFilepath

`string`

Path to the model GGUF file

##### options

Model configuration options

###### addSpecial

`boolean`

Whether to add special tokens to output

###### context

`any`

Customize the initial context created for this model

###### embedding

`boolean`

Whether to create an embedding model (true) or generation model (false)

###### parseSpecial

`boolean`

Whether to parse special tokens in text

###### removeSpecial

`boolean`

Whether to remove special tokens from output

###### unparseSpecial

`boolean`

Whether to unparse special tokens in text

#### Returns

[`LlamaModel`](LlamaModel.md)

#### Defined in

[index.js:164](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L164)

## Properties

### modelFilepath

> **modelFilepath**: `string`

#### Defined in

[index.js:172](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L172)

---

### options

> **options**: `object`

#### addSpecial?

> `optional` **addSpecial**: `boolean`

#### context?

> `optional` **context**: `any`

#### embedding?

> `optional` **embedding**: `boolean`

#### parseSpecial?

> `optional` **parseSpecial**: `boolean`

#### removeSpecial?

> `optional` **removeSpecial**: `boolean`

#### unparseSpecial?

> `optional` **unparseSpecial**: `boolean`

#### Defined in

[index.js:173](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L173)

## Methods

### context()

> **context**(`options`?): `Promise`\<[`LlamaModelContext`](LlamaModelContext.md)\>

Create a new context instance for this model that can be used for text generation or embeddings

#### Parameters

##### options?

Options for context creation

###### embedding

`boolean`

Whether to create an embedding context (true) or generation context (false)

###### existing

`boolean`

Return existing context if one exists rather than creating new

###### n_batch

`number`

Maximum tokens to process in parallel

###### n_ctx

`number`

Maximum tokens in context

#### Returns

`Promise`\<[`LlamaModelContext`](LlamaModelContext.md)\>

The context instance

#### Defined in

[index.js:277](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L277)

---

### destroy()

> **destroy**(): `Promise`\<`void`\>

Destroy both the model and context instances associated with this LlamaModel

#### Returns

`Promise`\<`void`\>

#### Defined in

[index.js:228](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L228)

---

### detokenize()

> **detokenize**(`tokens`, `options`?): `Promise`\<`string`\>

Convert token IDs back into text

#### Parameters

##### tokens

`ArrayBuffer`

Array of token IDs to convert to text

##### options?

Detokenization options

###### removeSpecial

`boolean`

Remove special tokens from output

###### unparseSpecial

`boolean`

Unparse special tokens in text

#### Returns

`Promise`\<`string`\>

Decoded text

#### Defined in

[index.js:258](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L258)

---

### encode()

> **encode**(`text`, `options`?): `Promise`\<`ArrayBuffer`\>

Encode text into an array of token embeddings.
Must be used with a model created with the `embedding` option set to `true`.

#### Parameters

##### text

`string`

Text to encode into embeddings

##### options?

Encoding options

###### addSpecial

`boolean`

Add special tokens to output

###### parseSpecial

`boolean`

Parse special tokens in text

#### Returns

`Promise`\<`ArrayBuffer`\>

Array of token embeddings

#### Defined in

[index.js:295](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L295)

---

### generate()

> **generate**(`prompt`, `options`?): `Promise`\<`string`\>

Generate text based on a prompt.
Must be used with a model created with the `embedding` option set to `false`.

#### Parameters

##### prompt

`string`

Text prompt to generate from

##### options?

Generation options

###### addSpecial

`boolean`

Add special tokens to output

###### parseSpecial

`boolean`

Parse special tokens in text

#### Returns

`Promise`\<`string`\>

Generated text

#### Defined in

[index.js:308](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L308)

---

### getMetadata()

> **getMetadata**(): `Promise`\<`LlamaModelMetadata`\>

Get metadata from the model instance such as parameter count and name

#### Returns

`Promise`\<`LlamaModelMetadata`\>

#### Defined in

[index.js:207](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L207)

---

### tokenize()

> **tokenize**(`text`, `options`?): `Promise`\<`ArrayBuffer`\>

Convert input text into token IDs that the model can understand

#### Parameters

##### text

`string`

Input text to tokenize

##### options?

Tokenization options

###### addSpecial

`boolean`

Add special tokens to output

###### parseSpecial

`boolean`

Parse special tokens in text

#### Returns

`Promise`\<`ArrayBuffer`\>

Array of token IDs

#### Defined in

[index.js:240](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L240)

---

### create()

> `static` **create**(`options`): `Promise`\<[`LlamaModel`](LlamaModel.md)\>

#### Parameters

##### options

#### Returns

`Promise`\<[`LlamaModel`](LlamaModel.md)\>

#### Defined in

[index.js:145](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L145)
