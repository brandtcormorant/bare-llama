[**bare-llama**](../README.md)

---

[bare-llama](../README.md) / createContext

# Function: createContext()

> **createContext**(`model`, `options`?): `Promise`\<`any`\>

Create a new context instance for a model that can be used for text generation or embeddings

## Parameters

### model

`any`

The model instance to create a context for

### options?

Options for context creation

#### embedding

`boolean`

Whether to create an embedding context (true) or generation context (false)

-

#### n_batch

`number`

Maximum tokens to process in parallel

#### n_ctx

`number`

Maximum tokens in context

## Returns

`Promise`\<`any`\>

The created context instance

## Defined in

[index.js:95](https://github.com/brandtcormorant/bare-llama/blob/9d915366231fdfe4c124d45c77627e653cecaf2d/index.js#L95)
