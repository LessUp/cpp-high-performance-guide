import test from 'node:test'
import assert from 'node:assert/strict'

import { resolveLanguageTarget } from '../theme/language-routing.js'

test('switching from no-trailing-slash Pages root to zh keeps a single base segment', () => {
  const targetPath = resolveLanguageTarget({
    routePath: '/cpp-high-performance-guide',
    siteBase: '/cpp-high-performance-guide/',
    currentLangPath: '/en/',
    targetLangPath: '/zh/',
  })

  assert.equal(targetPath, '/cpp-high-performance-guide/zh/')
})

test('switching from no-trailing-slash Pages root to en keeps a single base segment', () => {
  const targetPath = resolveLanguageTarget({
    routePath: '/cpp-high-performance-guide',
    siteBase: '/cpp-high-performance-guide/',
    currentLangPath: '/zh/',
    targetLangPath: '/en/',
  })

  assert.equal(targetPath, '/cpp-high-performance-guide/en/')
})
