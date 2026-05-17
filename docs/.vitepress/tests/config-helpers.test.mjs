import test from 'node:test'
import assert from 'node:assert/strict'

import { englishReferenceHub } from '../config-helpers.js'

test('English reference hub points to the reference section landing page', () => {
  assert.equal(englishReferenceHub, '/en/reference/')
})
