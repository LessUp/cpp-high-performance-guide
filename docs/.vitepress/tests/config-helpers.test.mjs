import test from 'node:test'
import assert from 'node:assert/strict'

import { englishReferenceHub, futureSidebar } from '../config-helpers.js'

test('futureSidebar scaffolds only safe empty IA buckets for English', () => {
  assert.deepEqual(Object.keys(futureSidebar('/en/')).sort(), [
    '/en/academy/',
    '/en/architecture/',
    '/en/playbook/',
    '/en/research/',
  ])
})

test('futureSidebar scaffolds only safe empty IA buckets for Chinese', () => {
  assert.deepEqual(Object.keys(futureSidebar('/zh/')).sort(), [
    '/zh/academy/',
    '/zh/architecture/',
    '/zh/playbook/',
    '/zh/research/',
  ])
})

test('English API hub points to the reference landing page', () => {
  assert.equal(englishReferenceHub, '/en/reference/api-reference')
})
