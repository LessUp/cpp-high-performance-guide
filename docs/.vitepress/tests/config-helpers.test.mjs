import test from 'node:test'
import assert from 'node:assert/strict'

import { englishReferenceHub, futureSidebar } from '../config-helpers.js'

test('futureSidebar scaffolds the five peer IA buckets for English', () => {
  assert.deepEqual(Object.keys(futureSidebar('/en/')).sort(), [
    '/en/academy/',
    '/en/architecture/',
    '/en/playbook/',
    '/en/reference/',
    '/en/research/',
  ])
})

test('futureSidebar scaffolds the five peer IA buckets for Chinese', () => {
  assert.deepEqual(Object.keys(futureSidebar('/zh/')).sort(), [
    '/zh/academy/',
    '/zh/architecture/',
    '/zh/playbook/',
    '/zh/reference/',
    '/zh/research/',
  ])
})

test('English reference hub points to the reference section landing page', () => {
  assert.equal(englishReferenceHub, '/en/reference/')
})
