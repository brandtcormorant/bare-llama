const test = require('brittle')
const { LlamaModel } = require('../index.js')

const modelFilepath = './models/smollm/SmolLM-135M-Instruct.Q8_0.gguf'

test('LlamaModel loads and initializes correctly', async function (t) {
  t.plan(4)

  const model = await LlamaModel.create({
    modelFilepath
  })

  // Verify model was created
  t.ok(model instanceof LlamaModel, 'Should create LlamaModel instance')

  // Get and verify metadata
  const metadata = await model.getMetadata()
  t.ok(metadata.parameters > 0, 'Should have valid parameter count')
  t.ok(metadata.contextWindow > 0, 'Should have valid context window')

  t.is(
    metadata.embedding,
    false,
    'Should initialize as generation model by default'
  )

  await model.destroy()
})

test('LlamaModel handles missing model file', async function (t) {
  await t.exception(
    async () => {
      await LlamaModel.create({
        modelFilepath: 'nope'
      })
    },
    /Failed to load model/,
    'Should throw on missing model file'
  )
})

test('LlamaModel handles basic tokenization and detokenization', async function (t) {
  t.plan(3)

  const model = await LlamaModel.create({ modelFilepath })

  t.teardown(async () => await model.destroy())

  // Test simple ASCII text
  const tokens = await model.tokenize('Hello')
  t.ok(tokens.length > 0, 'Should produce tokens for basic text')

  const decoded = await model.detokenize(tokens)
  t.ok(decoded.length === 5, 'Should produce decoded text')
  t.is(decoded, 'Hello', 'Should handle basic ASCII correctly')
})

test('LlamaModel handles special tokenization cases', async function (t) {
  const model = await LlamaModel.create({ modelFilepath, embedding: false })

  t.teardown(async () => await model.destroy())

  // Test punctuation
  const withPunct = await model.tokenize('Hello, world!')
  const decoded2 = await model.detokenize(withPunct)
  t.is(decoded2.trim(), 'Hello, world!', 'Should preserve punctuation')

  // Test case sensitivity
  const mixedCase = await model.tokenize('HeLLo WoRLD')
  const decoded3 = await model.detokenize(mixedCase)
  t.is(decoded3.trim(), 'HeLLo WoRLD', 'Should preserve case')
})

test('LlamaModel handles text generation', async function (t) {
  const model = await LlamaModel.create({
    modelFilepath,
    embedding: false
  })

  t.teardown(async () => await model.destroy())

  const prompt = 'The quick brown fox'
  const generated = await model.generate(prompt, {
    temperature: 0.9,
    maxTokens: 7
  })

  // A wild guess at what the model might generate 😏
  t.ok(
    generated.includes('jumps over the lazy dog'),
    'Should generate coherent text'
  )
})
