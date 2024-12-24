[**bare-llama**](../README.md)

---

[bare-llama](../README.md) / tokenize

# Function: tokenize()

> **tokenize**(`model`, `text`, `options`?): `Promise`\<`ArrayBuffer`\>

Convert text into tokens that the model can understand

## Parameters

### model

`any`

The model instance

### text

`string`

Input text to tokenize

### options?

Tokenization options

#### addSpecial

`boolean`

Add special tokens to output

#### parseSpecial

`boolean`

Parse special tokens in text

## Returns

`Promise`\<`ArrayBuffer`\>

Array of token IDs

## Defined in

[index.js:69](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L69)
