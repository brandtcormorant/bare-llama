[**bare-llama**](../README.md)

---

[bare-llama](../README.md) / detokenize

# Function: detokenize()

> **detokenize**(`model`, `tokens`, `options`?): `Promise`\<`string`\>

Convert token IDs back into text

## Parameters

### model

`any`

The model instance

### tokens

`ArrayBuffer`

Array of token IDs to convert to text

### options?

Detokenization options

#### removeSpecial

`boolean`

Remove special tokens from output

#### unparseSpecial

`boolean`

Unparse special tokens in text

## Returns

`Promise`\<`string`\>

Decoded text

## Defined in

[index.js:82](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L82)
