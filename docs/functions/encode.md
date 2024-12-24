[**bare-llama**](../README.md)

---

[bare-llama](../README.md) / encode

# Function: encode()

> **encode**(`context`, `text`, `options`?): `Promise`\<`ArrayBuffer`\>

Encode text into an array of token embeddings
Must be used with a LlamaContextInstance that has been created with the `embedding` option set to `true`.

## Parameters

### context

`any`

The context instance to use for encoding

### text

`string`

Text to encode into embeddings

### options?

Encoding options

#### addSpecial

`boolean`

Add special tokens to output

#### parseSpecial

`boolean`

Parse special tokens in text

## Returns

`Promise`\<`ArrayBuffer`\>

Array of token embeddings

## Defined in

[index.js:120](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L120)
