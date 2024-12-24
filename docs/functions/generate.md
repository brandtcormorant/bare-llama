[**bare-llama**](../README.md)

---

[bare-llama](../README.md) / generate

# Function: generate()

> **generate**(`context`, `prompt`, `options`?): `Promise`\<`string`\>

Generate text based on a prompt.
Must be used with a LlamaContextInstance that has been created with the `embedding` option set to `false`.

## Parameters

### context

`any`

The context instance to use for generation

### prompt

`string`

Text prompt to generate from

### options?

Generation options

#### addSpecial

`boolean`

Add special tokens to output

#### parseSpecial

`boolean`

Parse special tokens in text

## Returns

`Promise`\<`string`\>

Generated text

## Defined in

[index.js:134](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L134)
