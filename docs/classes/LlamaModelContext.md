[**bare-llama**](../README.md)

---

[bare-llama](../README.md) / LlamaModelContext

# Class: LlamaModelContext

A context instance for LlamaModel that can be used for text generation or embeddings

## Constructors

### new LlamaModelContext()

> **new LlamaModelContext**(`model`, `options`): [`LlamaModelContext`](LlamaModelContext.md)

Creates a new LlamaModelContext instance that can be lazily initialized using the `init` method

#### Parameters

##### model

[`LlamaModel`](LlamaModel.md)

The LlamaModel instance to create a context for

##### options

`LlamaModelContextOptions` = `...`

Configuration options for the context

#### Returns

[`LlamaModelContext`](LlamaModelContext.md)

#### Defined in

[index.js:349](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L349)

## Properties

### options

> **options**: `LlamaModelContextOptions`

#### Defined in

[index.js:357](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L357)

## Methods

### destroy()

> **destroy**(): `Promise`\<`void`\>

Destroy the context associated with this LlamaModelContext

#### Returns

`Promise`\<`void`\>

#### Defined in

[index.js:378](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L378)

---

### encode()

> **encode**(`text`, `options`?): `Promise`\<`ArrayBuffer`\>

Encode text into an array of token embeddings.
Must be used with a LlamaContextInstance created with the `embedding` option set to `true`.

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

[index.js:391](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L391)

---

### generate()

> **generate**(`prompt`, `options`?): `Promise`\<`string`\>

Generate text based on a prompt.
Must be used with a LlamaContextInstance created with the `embedding` option set to `false`.

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

[index.js:414](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L414)

---

### init()

> **init**(`options`): `Promise`\<`void`\>

Initializes a new context instance for this model. Can override options supplied in constructor.

#### Parameters

##### options

`LlamaModelContextOptions` = `{}`

Configuration options for the context

#### Returns

`Promise`\<`void`\>

#### Defined in

[index.js:365](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L365)

---

### create()

> `static` **create**(`model`, `options`): `Promise`\<[`LlamaModelContext`](LlamaModelContext.md)\>

Creates a new LlamaModelContext instance that is fully initialized and ready to use

#### Parameters

##### model

[`LlamaModel`](LlamaModel.md)

The LlamaModel instance to create a context for

##### options

`LlamaModelContextOptions` = `{}`

Configuration options for the context

#### Returns

`Promise`\<[`LlamaModelContext`](LlamaModelContext.md)\>

#### Defined in

[index.js:329](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L329)
