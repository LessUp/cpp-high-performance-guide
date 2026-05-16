import test from 'node:test'
import assert from 'node:assert/strict'

import { futureSidebar } from '../config-helpers.js'

test('futureSidebar scaffolds all top-level IA buckets for English', () => {
  assert.deepEqual(Object.keys(futureSidebar('/en/')).sort(), [
    '/en/academy/',
    '/en/architecture/',
    '/en/playbook/',
    '/en/reference/',
    '/en/research/',
  ])
})

test('futureSidebar scaffolds all top-level IA buckets for Chinese', () => {
  assert.deepEqual(Object.keys(futureSidebar('/zh/')).sort(), [
    '/zh/academy/',
    '/zh/architecture/',
    '/zh/playbook/',
    '/zh/reference/',
    '/zh/research/',
  ])
})
