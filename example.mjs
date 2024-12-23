import { LlamaModel } from './index.js'

// Create and initialize text generation model
const model = await LlamaModel.create({
  modelFilepath: './models/smollm/SmolLM-135M-Instruct.Q8_0.gguf',
})

// Generate text
const result = await model.generate("The quick brown fox", {
  temperature: 0.8,
  maxTokens: 5,
})

console.log(result)

// Clean up
await model.destroy()
